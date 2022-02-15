/*
* Assignment 2, CIS*3110, due feb 26th, 2021. CPU simulation.
* last date modified: 2021-03-04
* Author: Affan Khan 1095729
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Burst{
	int CPUburstsInside;
	int CPUtime;
	int IOtime;
    int bState;
    int quantFlag;
}Burst;
typedef struct Thread{
	Burst burst[32];
	int serviceTime;
	int IOtotalTime;
    int turnaroundTime;
	int finishTime;
	int arrival;
	int CPUbursts;
	int threadNumInside;
    int tState;
}Thread;
typedef struct Process{
    Thread thread[32];
    int threadNum;
    int procNumInside;
    int pState;
}Process;
typedef struct ReadyQueue{
    int processNum;
    int threadNum;
    int timeCpu;
    int timeIO;
    int quantFlag;
}ReadyQueue;

//variables
static int threadSwitch = 0;
static int processSwitch = 0;
static int dFlag = 0;
static int vFlag = 0;
static int rFlag = 0;
static int quant = 0;

void dFlagPrint(Process readyQ[], int x);
void printOutput(Process readyQ[], int x);
void FCFS();
void RR();
void readyQueueCretor(Process info[], int x, int totalBursts);
void printReadyQueue(ReadyQueue list[], int x);
void CpuScheduler(Process info[], ReadyQueue list[], int totalBursts, int x);
