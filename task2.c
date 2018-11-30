#define MAX_PROCESSES 5
#include "coursework.h"


double responseTime[MAX_PROCESSES];
double turnaroundTime[MAX_PROCESSES];
int countResponse, countTurnaround;




void generateSJF(struct queue *SJF){
	int pos,i;
	struct element p = generateProcess();
	if(SJF -> count == 0){printf("SJF: add first\n");}
	for(i = 0, pos = 0; i < SJF -> count; i++,pos++) {if(p.pid_time >= SJF -> e[i].pid_time){break;}}
	//check if it is legal to insert
	if(addHere(SJF, &p, pos) == 1){exit(-1);}
	if(pos == 0){printf("SJF: add new largest\n");}
}



void runSJF(struct queue *SJF){
	//end_S and end_E are for response time and turn around time respectively
	struct timeval end_S, end_E;
	//c:created-time s: response timestamp e:turn around time stamp r:response time t:turn around time
	long int r, t;
	//response time
	gettimeofday(&end_S, NULL);
	runNonPreemptiveJob(SJF, getCount(SJF) - 1);
	gettimeofday(&end_E, NULL);
	//calculate above time
	r = getDifferenceInMilliSeconds(SJF -> e[getCount(SJF) - 1].created_time, end_S);
	t = getDifferenceInMilliSeconds(SJF -> e[getCount(SJF) - 1].created_time, end_E);
	//calculate sum of response and turnaround
	responseTime[countResponse++] =  (double)r;
	turnaroundTime[countTurnaround++] = (double)t;
	//printf message
	printf("C: %ld S: %ld E: %ld R: %ld T: %ld\n",SJF -> e[getCount(SJF) - 1].created_time.tv_sec, end_S.tv_sec, end_E.tv_sec, r, t);
	removeLast(SJF);
}




int main(){

	int i;
	double temp,averageResponseTime, averageTurnAroundTime;
	//these two are for calculating the response and turnaround average time
	countResponse = 0;
 	countTurnaround = 0;

	struct queue *my_Arr = (struct queue*)malloc(sizeof(struct queue));
	if(init(my_Arr, MAX_PROCESSES) == 1){exit(-1);}
	printAll(my_Arr);
	//Generating stage
	printf("Generating processes for SJF ...\n");

	for(i = 0; i < MAX_PROCESSES; i++){
		generateSJF(my_Arr);
		printAll(my_Arr);
	}
	//run stage
	printf("Running the processes using SJF ...\n");
	for(i = 0; i < MAX_PROCESSES; i++){runSJF(my_Arr);}
	//printf average result
	for(i = 0, temp = 0; i < MAX_PROCESSES; i++) { temp = temp + responseTime[i];}
	averageResponseTime = temp / (double) MAX_PROCESSES;

	for(i = 0, temp = 0; i < MAX_PROCESSES; i++) { temp = temp + turnaroundTime[i];}
	averageTurnAroundTime = temp / (double) MAX_PROCESSES;

	printf("Average response time: %.2lf milliseconds.\nAverage turn around time: %.2lf milliseconds\n", averageResponseTime, averageTurnAroundTime);
	//free memory allocation
	freeAll(my_Arr);
}
