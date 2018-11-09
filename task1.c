#include "coursework.h"
#include "coursework.c"
#include "osc_queue.c"

#define MAX_PROCESSES 5




void fifo(struct queue *my_arr,FILE *fpWrite){
	int i;
	printf("Performing FIFO ...\n");
	fprintf(fpWrite,"Performing FIFO ...\n");
	for (i = 0; i < my_arr -> max; i++){
		struct element p = generateProcess();
		struct element *e = &p;
		if(addFirst(my_arr,e) == 1){
			exit(-1);
		}
		printf("Add: pid %d into the queue with time %d\n",e -> pid, e -> pid_time);
		fprintf(fpWrite,"Add: pid %d into the queue with time %d\n",e -> pid, e -> pid_time);
	}
}

void lifo(struct queue *my_arr,FILE *fpWrite){
	int i;
	printf("Performing LIFO ...\n");
	fprintf(fpWrite,"Performing LIFO ...\n");
	for (i = 0; i < my_arr ->max; i++){
		struct element p = generateProcess();
		struct element *e = &p;
		if(addLast(my_arr, e) == 1){
			exit(-1);
		}
		printf("Add: pid %d into the queue with time %d\n",e -> pid, e -> pid_time);
		fprintf(fpWrite,"Add: pid %d into the queue with time %d\n",e -> pid, e -> pid_time);
	}
}


void printToFile(struct queue *my_arr,FILE *fpWrite){
	int i;
	fprintf(fpWrite,"There are %d elements in total\n", getCount(my_arr));
	for(i = 0; i < my_arr -> max; i++){
		fprintf(fpWrite,"#[%d]: %d running-time %d created-time %ld sec %ld usec %d priority\n",i,my_arr -> e[i].pid, my_arr -> e[i].pid_time, my_arr -> e[i].created_time.tv_sec,  my_arr -> e[i].created_time.tv_usec,my_arr -> e[i].pid_priority);
	}
	
}

int main(){
	FILE *fpWrite = fopen("task1.txt","w");
	if(fpWrite == NULL){
		printf("Open file failed!\n");
		return -1;
	}
	int i;
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
	//start fifo first
	fifo(my_Arr,fpWrite);
	for(i = 0; i < MAX_PROCESSES; i++){
		printAll(my_Arr);
		printToFile(my_Arr,fpWrite);
		//removeLast print message
		printf("Remove: %d from the queue index %d\n", my_Arr -> e[getCount(my_Arr) - 1].pid, getCount(my_Arr) - 1);
		fprintf(fpWrite,"Remove: %d from the queue index %d\n", my_Arr -> e[getCount(my_Arr) - 1].pid, getCount(my_Arr) - 1);
		removeLast(my_Arr);
		
	}
	printAll(my_Arr);
	printToFile(my_Arr,fpWrite);
	//start lifo
	lifo(my_Arr,fpWrite);
	for(i = 0; i < MAX_PROCESSES; i++){
		printAll(my_Arr);
		printToFile(my_Arr,fpWrite);
		//removeLast print message
		printf("Remove: %d from the queue index %d\n", my_Arr -> e[getCount(my_Arr) - 1].pid, getCount(my_Arr) - 1);
		fprintf(fpWrite,"Remove: %d from the queue index %d\n", my_Arr -> e[getCount(my_Arr) - 1].pid, getCount(my_Arr) - 1);
		removeLast(my_Arr);
	}
	printAll(my_Arr);
	printToFile(my_Arr,fpWrite);
	fclose(fpWrite);
	freeAll(my_Arr);

}
