#define MAX_NUMBER_OF_JOBS 50
#define MAX_BUFFER_SIZE 10

#include "coursework.h"
#include <pthread.h>
#include <semaphore.h>

sem_t sync,empty,full;
sem_t semConsumer1,semConsumer2,semConsumer3;

sem_t **consumersSemArr;
int countJobs,countJobsConsumed;

double responseTime[MAX_NUMBER_OF_JOBS];
double turnaroundTime[MAX_NUMBER_OF_JOBS];

struct queue **_Arr;

void generatePQ(struct queue **PQ);
double average(double* sum);
void runPQ(struct queue *my_Arr);
void swap(struct queue *my_Arr, int pos);

void * producer(void * i);
void * consumer(void * index);



int main(){
	int i = 0;
	int arg[PRIORITY] = {0, 1, 2};
	pthread_t tProducer;
	//pthread_t consumer1, consumer2, consumer3;
	pthread_t tConsumer[PRIORITY]; //= {consumer1, consumer2, consumer3};

	sem_init(&sync, 0, 1);
	sem_init(&empty, 0, MAX_BUFFER_SIZE);
	sem_init(&full, 0, 0);

	sem_init(&semConsumer1, 0, 0);
	sem_init(&semConsumer2, 0, 0);
	sem_init(&semConsumer3, 0, 0);
	sem_t *c[PRIORITY] = {&semConsumer1, &semConsumer2, &semConsumer3};
	consumersSemArr = c;

	
	_Arr = (struct queue**)malloc(sizeof(struct queue *) * PRIORITY);
	for(i = 0; i < PRIORITY;i++){ 
		_Arr[i] = (struct queue*)malloc(sizeof(struct queue));
		if(init(_Arr[i], MAX_BUFFER_SIZE) == 1){
			exit(-1);
		}
	}
	
	pthread_create(&tProducer, NULL, producer, (void *)&i);
	for(i = 0; i < PRIORITY; i++) { pthread_create(&tConsumer[i], NULL, consumer, (void *)&arg[i]);}

	pthread_join(tProducer, NULL);
	for(i = 0; i < PRIORITY; i++) { pthread_join(tConsumer[i], NULL); }


	printf("Average response time: %.2lf milliseconds.\nAverage turn around time: %.2lf milliseconds\n", average(responseTime), average(turnaroundTime));

	for(i = 0; i < PRIORITY;i++){freeAll(_Arr[i]);};
	free(_Arr); _Arr = NULL;
	return 0;
}


void generatePQ(struct queue **PQ){
	struct element p = generateProcess();
	countJobs++;
	if(addFirst(PQ[p.pid_priority], &p) != 1){
		printf("P: job on buffer %d, job produced %d, job consumed %d\n", p.pid_priority,countJobs, countJobsConsumed);
		if(PQ[p.pid_priority]->count == 1) {sem_post(consumersSemArr[p.pid_priority]);}
	}
}

double average(double* sum){
	int i;
	double temp = 0;
	for(i = 0; i < MAX_NUMBER_OF_JOBS; i++) { temp = temp + sum[i];}
	return (temp / (double) MAX_NUMBER_OF_JOBS);
}
void swap(struct queue *my_Arr, int pos){
	struct element temp;
	temp = my_Arr ->e[pos];
	my_Arr -> e[pos] = my_Arr -> e[getCount(my_Arr) - 1];
	my_Arr -> e[getCount(my_Arr) - 1] = temp;
}
void runPQ(struct queue *my_Arr){
	//end_S and end_E are for response time and turn around time respectively
	//c:created-time s: response timestamp e:turn around time stamp r:response time t:turn around time	
	long int r, t;
	//counter is for record which process should be executed, checkFinish is for checking if all processes in the queue finished, num is to count the number of processes in one queue
	int counter = 0;
	struct timeval end_S,end_E;

	while(getCount(my_Arr) != 0){
		if(responseTime[my_Arr -> e[counter].pid] == 0){
			gettimeofday(&end_S,NULL);
			r= getDifferenceInMilliSeconds(my_Arr -> e[counter].created_time, end_S);
			responseTime[my_Arr -> e[counter].pid] =  (double)r;
		}
		//run the preemptive Job
		if(my_Arr -> e[counter].pid_time != 0) { 
			runPreemptiveJob(my_Arr, counter); 
			if(my_Arr -> e[counter].pid_time == 0){
				//mark e(end_E), t(start, end_E)
				gettimeofday(&end_E,NULL);
				//print queue, pid, its c(start), e(end_E), t(start, end_E) 
				t = getDifferenceInMilliSeconds(my_Arr -> e[counter].created_time, end_E);
				turnaroundTime[my_Arr -> e[counter].pid] = (double)t;
				countJobsConsumed++;
				printf("C: job on buffer %d, job produced %d, job consumed %d\n",my_Arr -> e[counter].pid_priority,countJobs, countJobsConsumed);
				swap(my_Arr, counter);
				removeLast(my_Arr);
				break;
			}
		}
		counter = (counter + 1) % getCount(my_Arr);
		//if the process is finished, get end_E and then t, update response_Sum and turnaround_Sum
	}
}
void * producer(void * i){
	while(countJobs < MAX_NUMBER_OF_JOBS){
			sem_wait(&empty);
			sem_wait(&sync);
			generatePQ(_Arr);
			sem_post(&sync);
			sem_post(&full);
	}
	return NULL;
}

void * consumer(void * index){
	int i = *(int *)index;
	int j;
	sem_wait(consumersSemArr[i]);

	while(countJobsConsumed < MAX_NUMBER_OF_JOBS){
		while((i==0 && (_Arr[i] == 0))||(i==1 && (_Arr[i - 1] -> count != 0 || _Arr[i] -> count == 0 ))||(i == 2  && (_Arr[i - 2] -> count != 0 || _Arr[i - 1] -> count != 0 || _Arr[i] -> count == 0))) {sem_wait(consumersSemArr[i]);}
		sem_wait(&full);
		sem_wait(&sync);
		runPQ(_Arr[i]);
		int tempConsumer = _Arr[i] -> count;
		sem_post(&sync);
		sem_post(&empty);
		if (tempConsumer == 0){ 	
			if(countJobsConsumed < MAX_NUMBER_OF_JOBS) {
				switch(i){
					case 0:	if(_Arr[i+1] -> count != 0) 		{ sem_post(consumersSemArr[i+1]); sem_post(consumersSemArr[i+2]);}
							else if(_Arr[i+2] -> count!= 0) 	{ sem_post(consumersSemArr[i+2]);}
							break;
						
					case 1:	if(_Arr[i-1] ->count!= 0) 		{ break;}
							else if(_Arr[i+1]->count != 0) 	{ sem_post(consumersSemArr[i+1]);}
							break;
						
					case 2:	if(_Arr[i-2]-> count != 0) 		{ sem_post(consumersSemArr[i-2]); sem_post(consumersSemArr[i-1]);}
							else if(_Arr[i-1]->count != 0) 	{ sem_post(consumersSemArr[i-1]);}
							break;	
					default: break;
				}	
				sem_wait(consumersSemArr[i]);
			}
			else { for(j = 0; j < PRIORITY; j++) { sem_post(consumersSemArr[j]);}}
    	} 
	}
	return NULL;
}
