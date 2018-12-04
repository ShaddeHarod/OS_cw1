#define MAX_NUMBER_OF_JOBS 50
#define MAX_BUFFER_SIZE 10
#define NUMBER_OF_CONSUMER 4

#include "coursework.h"
#include <pthread.h>
#include <semaphore.h>

sem_t sync,empty,full;


int countJobs,countJobsConsumed;

double responseTime[MAX_NUMBER_OF_JOBS];
double turnaroundTime[MAX_NUMBER_OF_JOBS];

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
	sem_init(&empty, 0, MAX_BUFFER_SIZE);
	sem_init(&full, 0, 0);



	
	_Arr = (struct queue**)malloc(sizeof(struct queue *) * PRIORITY);
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



void * producer(void * i){
	while(countJobs < MAX_NUMBER_OF_JOBS){
			sem_wait(&empty);
			struct element p = generateProcess();
			sem_wait(&sync);
			countJobs++;
			if(addFirst(_Arr[p.pid_priority], &p) != 1){
				printf("Producer   is operating on buffer %d, job produced %d, job consumed %d, , there are %d elements remained in the whole\n", p.pid_priority,countJobs, countJobsConsumed,(countJobs - countJobsConsumed));
			}
			sem_post(&sync);
			sem_post(&full);
	}
	return NULL;
}
void * consumer(void * index) {
	int i = *(int *)index;
	int j;
	long int r, t;
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
		for(j = 0; j < PRIORITY; j++) {
			if(getCount(_Arr[j]) != 0){
				my_Arr = _Arr[j];
				break;			
			}
		}
		e = my_Arr -> e[getCount(my_Arr) - 1];
		
		removeLast(my_Arr);
		sem_post(&sync);
		
		gettimeofday(&end_S,NULL);
		runPreemptiveJobv2(&e); 
		if(e.pid_time == 0){
			gettimeofday(&end_E,NULL);
			sem_wait(&sync);
			countJobsConsumed++;
			printf("ConsumerID:%d is operating on buffer %d , job produced %d, job consumed %d, there are %d elements remained in the whole\n",i,e.pid_priority,countJobs, countJobsConsumed, (countJobs - countJobsConsumed));
			if(countJobsConsumed >= MAX_NUMBER_OF_JOBS) {sem_post(&full);}
			sem_post(&sync);
			sem_post(&empty);
			t= getDifferenceInMilliSeconds(e.created_time, end_E);
			turnaroundTime[e.pid] = (double)t;
		}else {
			sem_wait(&sync);
			addLast(my_Arr, &e);
			sem_post(&sync);
			sem_post(&full);
		}
		
		if(responseTime[e.pid] == 0){
			r= getDifferenceInMilliSeconds(e.created_time, end_S);
			responseTime[e.pid] =  (double)r;
		}
		
	}
	return NULL;
}























