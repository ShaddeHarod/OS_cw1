#define MAX_NUMBER_OF_JOBS 50
#define MAX_BUFFER_SIZE 10
#define NUMBER_OF_CONSUMER 4

#include "coursework.h"
#include <pthread.h>
#include <semaphore.h>
//delay_producer is for making producer to sleep when one of the queues is full
sem_t sync,full,empty;


int countJobs,countJobsConsumed;

double responseTime[MAX_NUMBER_OF_JOBS];
double turnaroundTime[MAX_NUMBER_OF_JOBS];

//for recording how many processes in the queue(including running)
int queueProcessNumber[PRIORITY];

struct queue **_Arr;

void * producer(void * i);
void * consumer(void * index);




int main(){
	int i = 0;
	double temp,averageResponseTime, averageTurnAroundTime;
	int producerID[NUMBER_OF_CONSUMER];
	for(i = 0; i < NUMBER_OF_CONSUMER; i++){ producerID[i] = i;}
	pthread_t tProducer;
	pthread_t tConsumer[NUMBER_OF_CONSUMER];

	sem_init(&sync, 0, 1);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, MAX_BUFFER_SIZE);




	_Arr = (struct queue**)malloc(sizeof(struct queue *) * PRIORITY);
	printf("Max buffer size: %d\nMax number of jobs: %d\n", MAX_BUFFER_SIZE,MAX_NUMBER_OF_JOBS);
	for(i = 0; i < PRIORITY;i++){ 
		_Arr[i] = (struct queue*)malloc(sizeof(struct queue));
		if(init(_Arr[i], MAX_BUFFER_SIZE) == 1){
			exit(-1);
		}
	}

	
	
	pthread_create(&tProducer, NULL, producer, (void *)&i);
	for(i = 0; i < NUMBER_OF_CONSUMER; i++) { pthread_create(&tConsumer[i], NULL, consumer, (void *)&producerID[i]);}

	pthread_join(tProducer, NULL);
	for(i = 0; i < NUMBER_OF_CONSUMER; i++) { pthread_join(tConsumer[i], NULL); }


	for(i = 0, temp = 0; i < MAX_NUMBER_OF_JOBS; i++) { temp = temp + responseTime[i];}
	averageResponseTime = temp / (double) MAX_NUMBER_OF_JOBS;

	for(i = 0, temp = 0; i < MAX_NUMBER_OF_JOBS; i++) { temp = temp + turnaroundTime[i];}
	averageTurnAroundTime = temp / (double) MAX_NUMBER_OF_JOBS;
	printf("Average response time: %.2lf milliseconds.\nAverage turn around time: %.2lf milliseconds\n", averageResponseTime, averageTurnAroundTime);

	for(i = 0; i < PRIORITY;i++){freeAll(_Arr[i]);};
	free(_Arr); _Arr = NULL;
	return 0;
}


//Last is the header, First is the trailer
void * producer(void * i){
	while(countJobs < MAX_NUMBER_OF_JOBS){
			struct element p = generateProcess();			
			sem_wait(&empty);
			sem_wait(&sync);
			countJobs++;
			queueProcessNumber[p.pid_priority]++;
			if(addFirst(_Arr[p.pid_priority], &p) != 1){
				printf("\nProducer is operating on buffer %d, processID:%d\nJob produced %d in total, job consumed %d in total\n", p.pid_priority,p.pid,countJobs, countJobsConsumed);
			}
			sem_post(&sync);
			sem_post(&full);
	}
	return NULL;
}
void * consumer(void * index) {
	//the consumer implements FIFO in each queue.
	int i = *(int *)index;
	int j;
	long int r, t;
	int processIndex;
	struct element e;
	struct timeval end_S,end_E;
	struct queue *my_Arr;
	while(countJobsConsumed < MAX_NUMBER_OF_JOBS){
		sem_wait(&full);
		if(countJobsConsumed >= MAX_NUMBER_OF_JOBS) {
			sem_post(&full);
			break;
		}
		sem_wait(&sync);
		for(j = 0; j < PRIORITY && getCount(_Arr[j]) == 0; j++);
		if(j >= PRIORITY) { 
			sem_post(&full);
			sem_post(&sync);
			continue;
		}else{my_Arr = _Arr[j];}
		processIndex = getCount(my_Arr) - 1;
		e = my_Arr -> e[processIndex];
		removeLast(my_Arr);
		printf("\nConsumerID:%d on buffer %d. Process #%d, index%d taken to run.\n",i, e.pid_priority, e.pid,processIndex);
		sem_post(&sync);
		
		gettimeofday(&end_S,NULL);
		runPreemptiveJobv2(&e); 
		if(e.pid_time == 0){
			gettimeofday(&end_E,NULL);
			sem_wait(&sync);
			countJobsConsumed++;
			queueProcessNumber[j]--;
			printf("\nConsumerID:%d on buffer %d. Process#%d has finished.\nJob produced %d in total, job consumed %d in total\n",i,e.pid_priority,e.pid,countJobs, countJobsConsumed);
			printf("\nProcesses Distribution(including running):\nqueue0:%d processes, queue1:%d processes, queue2: %d processes\n",queueProcessNumber[0], queueProcessNumber[1],queueProcessNumber[2]); 
			if(countJobsConsumed >= MAX_NUMBER_OF_JOBS) {sem_post(&full);}
			
			sem_post(&sync);
			sem_post(&empty);
			t= getDifferenceInMilliSeconds(e.created_time, end_E);
			turnaroundTime[e.pid] = (double)t;
		}else {
			sem_wait(&sync);
			addFirst(my_Arr, &e);
			printf("\nConsumerID:%d on buffer %d. Process#%d has been put back to the end of the queue.\n", i, e.pid_priority, e.pid); 
			printf("\nProcesses Distribution(including running):\nqueue0:%d processes, queue1:%d processes, queue2: %d processes\n",queueProcessNumber[0],queueProcessNumber[1],queueProcessNumber[2]);
			sem_post(&sync);
			sem_post(&full);
		}
		//if the corresponding turnaround time for the process has not been recorded.
		if(responseTime[e.pid] == 0){
			r= getDifferenceInMilliSeconds(e.created_time, end_S);
			responseTime[e.pid] =  (double)r;
		}
		
	}
	return NULL;
}























