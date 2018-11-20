#define MAX_NUMBER_OF_JOBS 50
#define MAX_BUFFER_SIZE 10

#include "osc_queue.c"
#include "coursework.c"
#include <pthread.h>
#include <semaphore.h>

sem_t sync;
sem_t delay_consumer;

int tempConsumer;
int countJobs;
int countJobsConsumed;
double responseTime[MAX_NUMBER_OF_JOBS];
double turnaroundTime[MAX_NUMBER_OF_JOBS];
int countResponse, countTurnaround;

int sortArrayByRunTime(struct queue *SJF, struct element *e);
void generateSJF(struct queue *SJF);
void runSJF(struct queue *SJF);
double average(double* sum);

void *producer(void * arr);
void *consumer(void * arr);




int main(){
	pthread_t tProducer, tConsumer;
	countJobs = 0;
	countJobsConsumed = 0;
	sem_init(&sync,0,1);
	sem_init(&delay_consumer, 0, 0);
	countResponse = 0;
 	countTurnaround = 0;
	//these two are for calculating the response and turnaround average time


	struct queue *my_Arr = NULL;
	my_Arr = (struct queue*)malloc(sizeof(struct queue));
	if(init(my_Arr, MAX_BUFFER_SIZE) == 1){exit(-1);}
	else {printf("Max buffer: %d\nMAX_BUFFER_SIZE: %d\n", MAX_BUFFER_SIZE,MAX_NUMBER_OF_JOBS);}
	
	pthread_create(&tProducer, NULL, producer, (void *) my_Arr);
	pthread_create(&tConsumer, NULL, consumer, (void *) my_Arr);

	pthread_join(tProducer, NULL);
	pthread_join(tConsumer, NULL);
	
	printf("Average response time: %.2lf milliseconds.\nAverage turn around time: %.2lf milliseconds\n", average(responseTime), average(turnaroundTime));
	//free memory allocation
	freeAll(my_Arr);

	return 0;


}




int sortArrayByRunTime(struct queue *SJF, struct element *e){
	int pid_t = e -> pid_time;
	int pos = 0;
	int i;

	for(i = 0; i < SJF -> count; i++,pos++){
		if(pid_t >= SJF -> e[i].pid_time){break;}	
	}
	//check if it is legal to insert
	if(addHere(SJF, e, pos) == 1){exit(-1);}
	//if the element should be added in the first position, return 0
	if(pos == 0){return 0;}
	return 1;
}

void generateSJF(struct queue *SJF){
	//if(SJF -> count == 0){printf("SJF: add first\n");}
	
	struct element p = generateProcess();
	sortArrayByRunTime(SJF, &p);
	//if(sortArrayByRunTime(SJF, &p) == 0){printf("SJF: add new largest\n");}
}

void runSJF(struct queue *SJF){
	//end_S and end_E are for response time and turn around time respectively
	struct timeval start, end_S, end_E;
	long int r,t;
	start = SJF -> e[getCount(SJF) - 1].created_time;
	//response time
	gettimeofday(&end_S, NULL);
	runNonPreemptiveJob(SJF, getCount(SJF) - 1);
	//turn around time
	gettimeofday(&end_E, NULL);

	r = getDifferenceInMilliSeconds(start, end_S);
	t = getDifferenceInMilliSeconds(start, end_E);
	//calculate sum of response and turnaround
	responseTime[countResponse++] =  (double)r;
	turnaroundTime[countTurnaround++] = (double)t;
	removeLast(SJF);
}
double average(double* sum){
	int i;
	double temp = 0;
	for(i = 0; i < MAX_NUMBER_OF_JOBS; i++) { temp = temp + sum[i];}
	return (temp / (double) MAX_NUMBER_OF_JOBS);
}
void *producer(void * arr){
	struct queue *my_Arr = (struct queue*)arr;
	while(1){
		if(my_Arr -> count < MAX_BUFFER_SIZE) {
			sem_wait(&sync);
			generateSJF((struct queue*)arr);
			if(my_Arr-> count == 1) {sem_post(&delay_consumer);}
			countJobs++;
			printf("P: job produced %d, job consumed %d\n", countJobs, countJobsConsumed);
			sem_post(&sync);
		}	
		if(countJobs == MAX_NUMBER_OF_JOBS) {break;}
		
	}
	return NULL;
}
void *consumer(void * arr){
	struct queue *my_Arr = (struct queue*)arr;
	sem_wait(&delay_consumer);
	while(1){
		sem_wait(&sync);
		runSJF(my_Arr);
		tempConsumer = my_Arr -> count;
		countJobsConsumed++;
		printf("C: job produced %d, job consumed %d\n", countJobs, countJobsConsumed);
		sem_post(&sync);
		
		if (countJobsConsumed == MAX_NUMBER_OF_JOBS) {break;}
		if (tempConsumer == 0){ sem_wait(&delay_consumer);}
	}
	return NULL;
}
