#include "coursework.h"
#include "coursework.c"
#include "osc_queue.c"

#define MAX_PROCESSES 5




void fifo(struct queue *my_arr){
	int i;
	printf("Performing FIFO ...\n");
	for (i = 0; i < my_arr -> max; i++){
		struct element p = generateProcess();
		struct element *e = &p;
		if(addFirst(my_arr,e) == 1){
			exit(-1);
		}
		printf("Add: pid %d into the queue with time %d\n",e -> pid, e -> pid_time);
	}
}

void lifo(struct queue *my_arr){
	int i;
	printf("Performing LIFO ...\n");
	for (i = 0; i < my_arr ->max; i++){
		struct element p = generateProcess();
		struct element *e = &p;
		if(addLast(my_arr, e) == 1){
			exit(-1);
		}
		printf("Add: pid %d into the queue with time %d\n",e -> pid, e -> pid_time);
	}
}



int main(){
	
	int i;
	struct queue *my_Arr = NULL;
	my_Arr = (struct queue*)malloc(sizeof(struct queue));
	if(init(my_Arr, MAX_PROCESSES) == 1){exit(-1);}
	printAll(my_Arr);
	//start fifo first
	fifo(my_Arr);
	for(i = 0; i < MAX_PROCESSES; i++){
		printAll(my_Arr);
		//removeLast print message
		printf("Remove: %d from the queue index %d\n", my_Arr -> e[getCount(my_Arr) - 1].pid, getCount(my_Arr) - 1);
		removeLast(my_Arr);
	}
	printAll(my_Arr);
	//start lifo
	lifo(my_Arr);
	for(i = 0; i < MAX_PROCESSES; i++){
		printAll(my_Arr);
		//removeLast print message
		printf("Remove: %d from the queue index %d\n", my_Arr -> e[getCount(my_Arr) - 1].pid, getCount(my_Arr) - 1);
		removeLast(my_Arr);
	}
	printAll(my_Arr);
	freeAll(my_Arr);

}
