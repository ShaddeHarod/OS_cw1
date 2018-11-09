#ifndef __COURSEWORK_H__
#define __COURSEWORK_H__

#include "osc_queue.h"

// Duration of the time slice for the round robin algorithm
#define TIME_SLICE 2

#define PRIORITY 3

int generateProcessTime();
long int getDifferenceInMilliSeconds(struct timeval start, struct timeval end);

struct element generateProcess();

void runProcess(int index, int t, FILE *fpWrite);

void runNonPreemptiveJob(struct queue *my_arr, int index, FILE *fpWrite);

void runPreemptiveJob(struct queue *my_arr, int index, FILE *fpWrite);

int counter = 0;

#endif
