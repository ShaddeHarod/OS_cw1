#include "osc_queue.h"

int getCount(struct queue *my_arr){
	return my_arr -> count;
}

int init(struct queue *my_arr, int arr_size){
	printf("Initialising ...\n");

	
	if((my_arr -> e = (struct element*) malloc (sizeof(struct element) * arr_size)) == NULL){
		printf("Init: malloc failed!\n");
		return 1;
	}

	
	my_arr -> max = arr_size;
	printf("Init: successfully malloc element with size of %d ...\n", my_arr -> max);
	my_arr -> count = 0;
	return 0;
}

int addHere(struct queue *my_arr, struct element *new_e, int index){
	int i;

	if(getCount(my_arr) == my_arr -> max){
		printf("The queue is full.\n");
		return 1;
	}else if(index >= my_arr -> max || index < 0){
		printf("The index is illegal\n");
		return 1;
	}else if(index < my_arr -> max && index > getCount(my_arr)){	
		printf("The element will be put at continuous position.\n");
		index = getCount(my_arr);
	}
	
	for(i = getCount(my_arr); i >= index + 1; i--){
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
	
	free(my_arr -> e);
	free(my_arr);
	my_arr -> e = NULL;
	my_arr = NULL;
}



void printAll(struct queue *my_arr){
	int i;
	printf("There are %d elements in total\n", getCount(my_arr));
	for(i = 0; i < my_arr -> max; i++){
		printf("#[%d]: %d running-time %d created-time %ld sec %ld usec %d priority\n",i,my_arr -> e[i].pid, my_arr -> e[i].pid_time, my_arr -> e[i].created_time.tv_sec,  my_arr -> e[i].created_time.tv_usec,my_arr -> e[i].pid_priority);
	}
	
}	

void removeLast(struct queue *my_arr){
	if(getCount(my_arr) == 0) {
		printf("the queue is empty!\n");
		exit(-1);
	}
	my_arr -> e[getCount(my_arr) - 1].pid = 0;
	my_arr -> e[getCount(my_arr) - 1].pid_time = 0;
	my_arr -> e[getCount(my_arr) - 1].created_time.tv_sec = 0;
	my_arr -> e[getCount(my_arr) - 1].created_time.tv_usec = 0;
	my_arr -> e[getCount(my_arr) - 1].pid_priority = 0;
	my_arr -> count--;
}


