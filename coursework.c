#include "coursework.h"

int counter = 0;

int generateProcessTime()
{
	return ((rand() % 10) + 1);
}

int generatePriority()
{
	return (rand() % PRIORITY);
}

long int getDifferenceInMilliSeconds(struct timeval start, struct timeval end)
{
	long int iSeconds = end.tv_sec - start.tv_sec;
	long int iUSeconds = end.tv_usec - start.tv_usec;
 	long int mtime = (iSeconds * 1000 + iUSeconds / 1000.0);
	return mtime;
}

struct element generateProcess()
{
	struct element e;

	e.pid = counter;
	counter = counter + 1;

	e.pid_time = generateProcessTime();
	e.pid_priority = generatePriority();
	gettimeofday(&e.created_time, NULL);
	sleep(1);

	return e;	
}

void runProcess(int index, int t)
{
	printf("Running: #%d for %d sec ...\n", index, t);
	sleep(t);
	// use sleep to occupy the process
}

void runNonPreemptiveJob(struct queue *my_arr, int index)
{
	runProcess(index, my_arr->e[index].pid_time);
}

void runPreemptiveJob(struct queue *my_arr, int index)
{
	int t = my_arr->e[index].pid_time;

	int iBurstTime = t > TIME_SLICE ? TIME_SLICE : t;

	printf("Q #%d >>> pid: %d remain time %d, will be running for %d sec\n", 
		my_arr->e[index].pid_priority, my_arr->e[index].pid, t, iBurstTime);
	
	runProcess(my_arr->e[index].pid, iBurstTime);
	
	my_arr->e[index].pid_time = my_arr->e[index].pid_time - iBurstTime;
}

