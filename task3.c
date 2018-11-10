#ifndef MAX_PROCESSES
#define MAX_PROCESSES 5
#endif
#include "osc_queue.c"
#include "coursework.c"
#define true 1
#define false 0

void generatePQ(struct queue **PQ){

	struct element p = generateProcess();
	printf("New process has priority %d\n",p.pid_priority);
	if(addFirst(PQ[p.pid_priority], &p) != 1){printf("PQ %d: add new\n", p.pid_priority);}
}

double average(double sum){return (sum / (double) MAX_PROCESSES);}

int checkAllFinished(struct queue *arr){
	int i;
	int flag = true;
	for(i = 0; i < getCount(arr); i++){
		if(arr -> e[i].pid_time == 0){ flag = flag && true;}	
		else {flag = flag && false;}
	}
	return flag;
}

long int getMilliSeconds(struct timeval time){ return time.tv_sec;}

void runPQ(struct queue **my_Arr,double *response_Sum, double *turnaround_Sum){
	//end_S and end_E are for response time and turn around time respectively
	//c:created-time s: response timestamp e:turn around time stamp r:response time t:turn around time	
	int i;
	long int r, t;
	int counter;
	struct timeval *start;
	struct timeval *end_S; 
	struct timeval *end_E;
		
	for(i = 0; i < PRIORITY;i++){

		int j = getCount(my_Arr[i]);
		counter = 0;
		start = (struct timeval *)malloc(sizeof(struct timeval) * j);
		end_S = (struct timeval *)malloc(sizeof(struct timeval) * j);
		end_E = (struct timeval *)malloc(sizeof(struct timeval) * j);
		
		while(!checkAllFinished(my_Arr[i])){
			
			//record start of each process, only once. start, c(start)
			if(start[counter].tv_sec == 0){
				//record response time of each process, only once. start, s(end_S), r(start, end_S)
				start[counter] = my_Arr[i] -> e[counter].created_time;
			}
			if(end_S[counter].tv_sec == 0){
				//record response time of each process, only once. start, s(end_S), r(start, end_S)
				gettimeofday(&end_S[counter],NULL);
				r= getDifferenceInMilliSeconds(start[counter], end_S[counter]);
				*response_Sum = *response_Sum + (double)r;
				printf("Q: %d P: %d C: %ld S: %ld R: %ld\n", i, my_Arr[i] -> e[counter].pid, getMilliSeconds(start[counter]), getMilliSeconds(end_S[counter]), r);
				
			}
			//run the preemptive Job
			if(my_Arr[i] -> e[counter].pid_time != 0){runPreemptiveJob(my_Arr[i], counter);}
			if(my_Arr[i] -> e[counter].pid_time == 0 && end_E[counter].tv_sec == 0){
				//mark e(end_E), t(start, end_E)
				//print queue, pid, its c(start), e(end_E), t(start, end_E) 
				gettimeofday(&end_E[counter],NULL);
				t = getDifferenceInMilliSeconds(start[counter], end_E[counter]);
				*turnaround_Sum = *turnaround_Sum + (double)t;
				printf("Q: %d P: %d C: %ld E: %ld T: %ld\n", i, my_Arr[i] -> e[counter].pid, getMilliSeconds(start[counter]), getMilliSeconds(end_E[counter]), t);
			}
			counter = (counter + 1) % getCount(my_Arr[i]);
			
			//if the process is finished, get end_E and then t, update response_Sum and turnaround_Sum
		}
		free(start);
		start = NULL;
		free(end_S);
		end_S = NULL;
		free(end_E);
		end_E = NULL;
		
		
		
	}
}


int main(){
	
	int i,j;
	//these two are for calculating the response and turnaround average time
	double response_Sum = 0;
	double turnaround_Sum = 0;
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
	runPQ(my_Arr,&response_Sum, &turnaround_Sum);

	//printf average result
	printf("Average response time: %.2lf milliseconds.\nAverage turn around time: %.2lf milliseconds\n", average(response_Sum), average(turnaround_Sum));
	for(i = 0; i < PRIORITY;i++){freeAll(my_Arr[i]);};
	free(my_Arr);
	my_Arr = NULL;
}
