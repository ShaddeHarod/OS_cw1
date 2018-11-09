#ifndef MAX_PROCESSES
#define MAX_PROCESSES 5
#endif
#include "osc_queue.c"
#include "coursework.c"

int sortArrayByRunTime(struct queue *SJF, struct element *e){
	int pid_t = e -> pid_time;
	int pos = 0;
	int i;

	for(i = 0; i < SJF -> count; i++,pos++){
		if(pid_t >= SJF -> e[i].pid_time){break;}	
	}
	//check if it is legal to insert
	if(addHere(SJF, e, pos) == 1){
		exit(-1);
	}
	//if the element should be added in the first position, return 0
	if(pos == 0){return 0;}
	return 1;
}

void generateSJF(struct queue *SJF, FILE *fpWrite){
	if(SJF -> count == 0){
		printf("SJF: add first\n");
		fprintf(fpWrite, "SJF: add first:\n");
	}
	
	struct element p = generateProcess();
	struct element *e = &p;
	
	if(sortArrayByRunTime(SJF, e) == 0){
		printf("SJF: add new largest\n");
		fprintf(fpWrite, "SJF: add new largest\n");
	}
}

double average(double sum){return (sum / (double) MAX_PROCESSES);}

void runSJF(struct queue *SJF, FILE *fpWrite,double *response_Sum,double *turnaround_Sum){
	//end_S and end_E are for response time and turn around time respectively
	struct timeval start, end_S, end_E;
	//c:created-time s: response timestamp e:turn around time stamp r:response time t:turn around time
	long int c, s, e, r, t;
	start = SJF -> e[getCount(SJF) - 1].created_time;
	//response time
	gettimeofday(&end_S, NULL);
	runNonPreemptiveJob(SJF, getCount(SJF) - 1, fpWrite);
	gettimeofday(&end_E, NULL);
	//calculate above time
	c = start.tv_sec * 1000 + start.tv_usec / 1000.0;
	s = end_S.tv_sec * 1000 + end_S.tv_usec / 1000.0;
	e = end_E.tv_sec * 1000 + end_E.tv_usec / 1000.0;
	r = getDifferenceInMilliSeconds(start, end_S);
	t = getDifferenceInMilliSeconds(start, end_E);
	//calculate sum of response and turnaround
	*response_Sum = *response_Sum + (double)r;
	*turnaround_Sum = *turnaround_Sum + (double)t;
	//printf message
	printf("C: %ld S: %ld E: %ld R: %ld T: %ld\n",c, s, e, r, t);
	fprintf(fpWrite,"C: %ld S: %ld E: %ld R: %ld T: %ld\n",c, s, e, r, t);
	removeLast(SJF);
}


void printToFile(struct queue *my_arr,FILE *fpWrite){
	int i;
	fprintf(fpWrite,"There are %d elements in total\n", getCount(my_arr));
	for(i = 0; i < my_arr -> max; i++){
		fprintf(fpWrite,"#[%d]: %d running-time %d created-time %ld sec %ld usec %d priority\n",i,my_arr -> e[i].pid, my_arr -> e[i].pid_time, my_arr -> e[i].created_time.tv_sec,  my_arr -> e[i].created_time.tv_usec,my_arr -> e[i].pid_priority);
	}
	
}

int main(){
	FILE *fpWrite = fopen("task2.txt","w");
	if(fpWrite == NULL){
		printf("Open file failed!\n");
		return -1;
	}
	int i;
	//these two are for calculating the response and turnaround average time
	double response_Sum = 0;
	double turnaround_Sum = 0;

	struct queue *my_Arr = NULL;
	my_Arr = (struct queue*)malloc(sizeof(struct queue));
	if(init(my_Arr, MAX_PROCESSES) == 1){
		exit(-1);
	}else{
		fprintf(fpWrite,"Initialising ...\n");
		fprintf(fpWrite,"Init: successfully malloc element with size of %d ...\n", my_Arr -> max);
	}
	printAll(my_Arr);
	printToFile(my_Arr,fpWrite);
	//Generating stage
	printf("Generating processes for SJF ...\n");
	fprintf(fpWrite,"Generating processes for SJF ...\n");

	for(i = 0; i < MAX_PROCESSES; i++){
		generateSJF(my_Arr, fpWrite);
		printAll(my_Arr);
		printToFile(my_Arr,fpWrite);
	}
	//run stage
	printf("Running the processes using SJF ...\n");
	fprintf(fpWrite,"Running the processes using SJF ...\n");
	for(i = 0; i < MAX_PROCESSES; i++){
		runSJF(my_Arr, fpWrite,&response_Sum, &turnaround_Sum);
	}
	//printf average result
	printf("Average response time: %.2lf milliseconds.\nAverage turn around time: %.2lf milliseconds\n", average(response_Sum), average(turnaround_Sum));
	fprintf(fpWrite,"Average response time: %.2lf milliseconds.\nAverage turn around time: %.2lf milliseconds\n", average(response_Sum), average(turnaround_Sum));
	fclose(fpWrite);
	freeAll(my_Arr);
}
