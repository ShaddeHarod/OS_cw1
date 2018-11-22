
#define MAX_PROCESSES 5
#include "coursework.h"
#define true 1
#define false 0


double responseTime[MAX_PROCESSES];
double turnaroundTime[MAX_PROCESSES];

void generatePQ(struct queue **PQ){

	struct element p = generateProcess();
	printf("New process has priority %d\n",p.pid_priority);
	if(addFirst(PQ[p.pid_priority], &p) != 1){printf("PQ %d: add new\n", p.pid_priority);}
}

double average(double* sum){
	int i;
	double temp = 0;
	for(i = 0; i < MAX_PROCESSES; i++) { temp = temp + sum[i];}
	return (temp / (double) MAX_PROCESSES);
}

void runPQ(struct queue **my_Arr){
	//end_S and end_E are for response time and turn around time respectively
	//c:created-time s: response timestamp e:turn around time stamp r:response time t:turn around time	
	int i;
	long int r, t;
		
	for(i = 0; i < PRIORITY;i++){
		int numOfEnd = getCount(my_Arr[i]);
		int counter = 0;
		
		while(numOfEnd != 0){
			if(responseTime[my_Arr[i] -> e[counter].pid] == 0){
				struct timeval end_S;
				gettimeofday(&end_S,NULL);
				r= getDifferenceInMilliSeconds(my_Arr[i] -> e[counter].created_time, end_S);
				responseTime[my_Arr[i] -> e[counter].pid] =  (double)r;
				printf("Q: %d P: %d C: %ld S: %ld R: %ld\n", i, my_Arr[i] -> e[counter].pid, my_Arr[i] -> e[counter].created_time.tv_sec, end_S.tv_sec, r);
			}
			//run the preemptive Job
			if(my_Arr[i] -> e[counter].pid_time != 0){
				runPreemptiveJob(my_Arr[i], counter);
				if(my_Arr[i] -> e[counter].pid_time == 0){
					struct timeval end_E;
					gettimeofday(&end_E,NULL);
					t = getDifferenceInMilliSeconds(my_Arr[i] -> e[counter].created_time, end_E);
					turnaroundTime[my_Arr[i] -> e[counter].pid] = (double)t;
					printf("Q: %d P: %d C: %ld E: %ld T: %ld\n", i, my_Arr[i] -> e[counter].pid, my_Arr[i] -> e[counter].created_time.tv_sec, end_E.tv_sec, t);
					numOfEnd--;
				}
			}
			counter = (counter + 1) % getCount(my_Arr[i]);
			
		}

	}
}


int main(){
	
	int i,j;
	//these two are for calculating the response and turnaround average time
	//my_Arr in this file represents a struct queue pointer array.
	struct queue **my_Arr = (struct queue**)malloc(sizeof(struct queue *) * PRIORITY);

	for(i = 0; i < PRIORITY;i++){ 
		my_Arr[i] = (struct queue*)malloc(sizeof(struct queue));	
		if(init(my_Arr[i], MAX_PROCESSES) == 1){
			exit(-1);
		}else {printf("Q #%d\n",i);}
		printAll(my_Arr[i]);
	}
	//Generating stage
	printf("Generating processes for PQ ...\n");
	for(j = 0; j < MAX_PROCESSES; j++){
		generatePQ(my_Arr);
		for(i = 0; i < PRIORITY;i++){
			printf("Q #%d\n",i);
			printAll(my_Arr[i]);
		}
	}	
	//run stage
	printf("Running the processes using PQ ...\n");
	runPQ(my_Arr);

	//printf average result
	printf("Average response time: %.2lf milliseconds.\nAverage turn around time: %.2lf milliseconds\n", average(responseTime), average(turnaroundTime));
	for(i = 0; i < PRIORITY;i++){freeAll(my_Arr[i]);}
	free(my_Arr); my_Arr = NULL;
}
