#include "coursework.h"
#define MAX_PROCESSES 5

struct queue *my_arr;


void fifo(struct queue *my_arr){
	int i;
	printf("Performing FIFO ...\n");
	for (i = 0; i < my_arr -> max; i++){
		struct element p = generateProcess();
		struct element *e = &p;
		if(addFirst(my_arr,e) == 1){
			exit(-1);
		}
		printf("Add: pid %d into the queue with time %d\n",my_arr -> e[i].pid, my_arr -> e[i].pid_time);
	}
}

void lifo(struct queue *my_arr){
	int i;
	printf("Performing FIFO ...\n");
	for (i = 0; i < my_arr ->max; i++){
		struct element p = generateProcess();
		struct element *e = &p;
		if(addLast(my_arr, e) == 1){
			exit(-1);
		}
		printf("Add: pid %d into the queue with time %d\n",my_arr -> e[i].pid, my_arr -> e[i].pid_time);
	}
}




int main(){
	int i;
	if(init(my_arr, MAX_PROCESSES) == 1){
		exit(-1);
	}

	fifo(my_arr);
	for(i = 0; i < MAX_PROCESSES; i++){
		removeLast(my_arr);
		printAll(my_arr);
	}
	
	lifo(my_arr);
	for(i = 0; i < MAX_PROCESSES; i++){
		removeLast(my_arr);
		printAll(my_arr);
	}



}