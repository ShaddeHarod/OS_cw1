#define MAX_NUMBER_OF_JOBS 50
#define MAX_BUFFER_SIZE 10

#include "osc_queue.c"
#include "coursework.c"
#include <pthread.h>
#include <semaphore.h>

sem_t sync;
sem_t empty;
sem_t full;

int countJobs;
int countJobsConsumed;

double responseTime[MAX_NUMBER_OF_JOBS];
double turnaroundTime[MAX_NUMBER_OF_JOBS];
int countResponse, countTurnaround;



void generatePQ(struct queue **PQ);
double average(double* sum);
void runPQ(struct queue **my_Arr);

void * producer(void * buffer){
	struct queue ** my_Arr = (struct queue**)buffer;
	while(1){
		
			sem_wait(&empty);
			sem_wait(&sync);
			countJobs++;
			generatePQ(my_Arr);
			sem_post(&sync);
			sem_post(&full);
			printf("Producer doesn't want to sleep.\n");
			if(countJobs == MAX_NUMBER_OF_JOBS) {break;}
	}
	return NULL;
}

void * consumer(void * buffer){
	struct queue ** my_Arr = (struct queue**)buffer;
	while(1){
		sem_wait(&full);
		sem_wait(&sync);
		runPQ(my_Arr);
		sem_post(&sync);
		sem_post(&empty);
		printf("Consumer doesn't want to sleep.\n");
		if(countJobsConsumed == MAX_NUMBER_OF_JOBS) {break;}
	}
	return NULL;
}
		


int main(){
	int i;
	pthread_t tProducer;
	pthread_t tConsumer[PRIORITY];
	sem_init(&sync, 0, 1);
	sem_init(&empty, 0, MAX_BUFFER_SIZE);
	sem_init(&full, 0, 0);
	
	countJobs = 0;
	countJobsConsumed = 0;

	countResponse = 0;
 	countTurnaround = 0;
	
	struct queue **my_Arr = (struct queue**)malloc(sizeof(struct queue *) * PRIORITY);
	for(i = 0; i < PRIORITY;i++){ 
		my_Arr[i] = (struct queue*)malloc(sizeof(struct queue));
	
		if(init(my_Arr[i], MAX_BUFFER_SIZE) == 1){
			exit(-1);
		}else {printf("Q #%d\n",i);}

		printAll(my_Arr[i]);
	}
	pthread_create(&tProducer, NULL, producer, (void *)my_Arr);
	for(i = 0; i < PRIORITY; i++) { pthread_create(&tConsumer[i], NULL, consumer, (void *)my_Arr);}

	pthread_join(tProducer, NULL);
	for(i = 0; i < PRIORITY; i++) { pthread_join(tConsumer[i], NULL); }


	printf("Average response time: %.2lf milliseconds.\nAverage turn around time: %.2lf milliseconds\n", average(responseTime), average(turnaroundTime));

	for(i = 0; i < PRIORITY;i++){freeAll(my_Arr[i]);};
	free(my_Arr);
	my_Arr = NULL;
	return 0;
}


void generatePQ(struct queue **PQ){

	struct element p = generateProcess();
	
	if(addFirst(PQ[p.pid_priority], &p) != 1){printf("P: job on buffer %d, job produced %d, job consumed %d\n", p.pid_priority,countJobs, countJobsConsumed);}
}

double average(double* sum){
	int i;
	double temp = 0;
	for(i = 0; i < MAX_NUMBER_OF_JOBS; i++) { temp = temp + sum[i];}
	return (temp / (double) MAX_NUMBER_OF_JOBS);
}

void runPQ(struct queue **my_Arr){
	//end_S and end_E are for response time and turn around time respectively
	//c:created-time s: response timestamp e:turn around time stamp r:response time t:turn around time	
	int i;
	long int r, t;
	//counter is for record which process should be executed, checkFinish is for checking if all processes in the queue finished, num is to count the number of processes in one queue
	int counter, checkFinish, num;
	struct timeval *start;
	struct timeval *end_S; 
	struct timeval *end_E;
		
	for(i = 0; i < PRIORITY;i++){
		num = getCount(my_Arr[i]);
		counter = 0;
		checkFinish = 0;
		start = (struct timeval *)malloc(sizeof(struct timeval) * num);
		end_S = (struct timeval *)malloc(sizeof(struct timeval) * num);
		end_E = (struct timeval *)malloc(sizeof(struct timeval) * num);
		
		while(checkFinish != num){
			
			//record start of each process, only once. start, c(start)
			if(start[counter].tv_sec == 0){
				//record response time of each process, only once. start, s(end_S), r(start, end_S)
				start[counter] = my_Arr[i] -> e[counter].created_time;
			}
			if(end_S[counter].tv_sec == 0){
				//record response time of each process, only once. start, s(end_S), r(start, end_S)
				gettimeofday(&end_S[counter],NULL);
				r= getDifferenceInMilliSeconds(start[counter], end_S[counter]);
				responseTime[countResponse++] =  (double)r;
			}
			//run the preemptive Job
			if(my_Arr[i] -> e[counter].pid_time != 0){
				runPreemptiveJob(my_Arr[i], counter);
			}

			if(my_Arr[i] -> e[counter].pid_time == 0 && end_E[counter].tv_sec == 0){
				//mark e(end_E), t(start, end_E)
				gettimeofday(&end_E[counter],NULL);
				//print queue, pid, its c(start), e(end_E), t(start, end_E) 
				t = getDifferenceInMilliSeconds(start[counter], end_E[counter]);
				turnaroundTime[countTurnaround++] = (double)t;
				countJobsConsumed++;
				printf("C%d: job on buffer %d, job produced %d, job consumed %d\n", i,i,countJobs, countJobsConsumed);
				checkFinish++;
			}
			counter = (counter + 1) % num;
			
			//if the process is finished, get end_E and then t, update response_Sum and turnaround_Sum
		}
		free(start);
		start = NULL;
		free(end_S);
		end_S = NULL;
		free(end_E);
		end_E = NULL;
		printf("This is the end of runPQ\n");
		
		
	}
}
