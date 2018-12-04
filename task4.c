#define MAX_NUMBER_OF_JOBS 50
#define MAX_BUFFER_SIZE 10
#define MAX_RUN_BUFFER_SIZE 1

#include "coursework.h"
#include <pthread.h>
#include <semaphore.h>

sem_t sync;
sem_t delay_consumer;

int countJobs;
int countJobsConsumed;
double responseTime[MAX_NUMBER_OF_JOBS];
double turnaroundTime[MAX_NUMBER_OF_JOBS];

void *producer(void * arr);
void *consumer(void * arr);




int main(){
	int i;
	double temp,averageResponseTime, averageTurnAroundTime;
	pthread_t tProducer, tConsumer;
	countJobs = 0;
	countJobsConsumed = 0;
	sem_init(&sync,0,1);
	sem_init(&delay_consumer, 0, 0);
	//these two are for calculating the response and turnaround average time


	struct queue *my_Arr  = (struct queue*)malloc(sizeof(struct queue));
	
	if(init(my_Arr, MAX_BUFFER_SIZE) == 1){exit(-1);}
	else {printf("Max buffer: %d\nMAX_BUFFER_SIZE: %d\n", MAX_BUFFER_SIZE,MAX_NUMBER_OF_JOBS);}
	
	pthread_create(&tConsumer, NULL, consumer, (void *) my_Arr);
	pthread_create(&tProducer, NULL, producer, (void *) my_Arr);

	pthread_join(tProducer, NULL);
	pthread_join(tConsumer, NULL);

	for(i = 0, temp = 0; i < MAX_NUMBER_OF_JOBS; i++) { temp = temp + responseTime[i];}
	averageResponseTime = temp / (double) MAX_NUMBER_OF_JOBS;

	for(i = 0, temp = 0; i < MAX_NUMBER_OF_JOBS; i++) { temp = temp + turnaroundTime[i];}
	averageTurnAroundTime = temp / (double) MAX_NUMBER_OF_JOBS;
	printf("Average response time: %.2lf milliseconds.\nAverage turn around time: %.2lf milliseconds\n", averageResponseTime, averageTurnAroundTime);
	//free memory allocation
	freeAll(my_Arr);

	return 0;


}





void *producer(void * arr){
	int pos, i;
	struct queue *my_Arr = (struct queue*)arr;
	while(countJobs < MAX_NUMBER_OF_JOBS){
		if(my_Arr -> count < MAX_BUFFER_SIZE) {
			struct element p = generateProcess();
			sem_wait(&sync);
			for(i = 0, pos = 0; i < my_Arr -> count; i++,pos++) {if(p.pid_time >= my_Arr -> e[i].pid_time){break;}}
			//check if it is legal to insert
			if(addHere(my_Arr, &p, pos) == 1){exit(-1);}
			int tempProducer = my_Arr -> count;
			countJobs++;
			printf("P: buffer has %d elements, job produced %d, job consumed %d\n", tempProducer, countJobs, countJobsConsumed);
			sem_post(&sync);
			if(tempProducer == 1) {sem_post(&delay_consumer);}
		}
	}
	return NULL;
}
void *consumer(void * arr){
	struct queue *my_Arr = (struct queue*)arr;
	struct timeval end_S, end_E;
	long int r,t;
	struct element e;
	sem_wait(&delay_consumer);
	while(countJobsConsumed < MAX_NUMBER_OF_JOBS){
		
		sem_wait(&sync);
		e = my_Arr ->e[getCount(my_Arr) - 1];
		removeLast(my_Arr);
		countJobsConsumed++;
		int tempConsumer = getCount(my_Arr);
		printf("C: buffer has %d elements,job produced %d, job consumed %d\n", tempConsumer,countJobs, countJobsConsumed);
		sem_post(&sync);
		//response time
		gettimeofday(&end_S, NULL);
		runNonPreemptiveJobv2(&e);
		//turn around time
		gettimeofday(&end_E, NULL);
		r = getDifferenceInMilliSeconds(e.created_time, end_S);
		t = getDifferenceInMilliSeconds(e.created_time, end_E);
		//calculate sum of response and turnaround
		responseTime[e.pid] =  (double)r;
		turnaroundTime[e.pid] = (double)t;
		if (tempConsumer == 0 && countJobsConsumed < MAX_NUMBER_OF_JOBS){ sem_wait(&delay_consumer);}
	}
	return NULL;
} 
