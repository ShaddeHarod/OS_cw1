#include "osc_queue.h"


//if the queue is empty, return -1
int getCount(struct queue *my_arr){
	return my_arr -> count;
}

int init(struct queue *my_arr, int arr_size){
	my_arr -> max = arr_size;
	my_arr -> e = (struct element*) malloc (sizeof(struct element) * my_arr -> max);
	my_arr -> count = 0;
}

int addHere(struct queue *my_arr, struct element *new_e, int index){
	int count = getCount(my_arr);
	int i;

	if(index > count || index < 0){
		printf("The index is illegal or the queue is full\n");
		return 1;
	}else if(count == my_arr -> max){
		printf("The queue is full.\n");
		return 1;
	}
	
	for(i = count; i >= index + 1; i--){
		my_arr -> e[i] = my_arr -> e[i - 1]; 
	}
	my_arr -> e[index] = (*new_e);
	my_arr -> count++;	
	return 0;
	
	
}

int addFirst(struct queue *my_arr, struct element *new_e){
	return addHere(my_arr, new_e, 0);
}


int addLast(struct queue *my_arr, struct element *new_e){
	return addHere(my_arr, new_e, getCount(my_arr));
}

void freeAll(struct queue *my_arr){
	int i;
	for(i = 0; i < getCount(my_arr); i++){
		free(&(my_arr -> e[i]));	
	}
	free(my_arr -> e);
	//free(my_arr);
}



void printAll(struct queue *my_arr){
	int i;
	for(i = 0; i < getCount(my_arr); i++){
		printf("for the %d element in the queue\n\tits pid is%d\n\tits pid_time is %d\n\tits priority is %d\n\n",i,my_arr -> e[i].pid, my_arr -> e[i].pid_time,my_arr -> e[i].pid_priority);
	}
	
}	

void removeLast(struct queue *my_arr){
	free(&(my_arr -> e[getCount(my_arr) - 1]));
	my_arr -> count--;
}

void main(){}

