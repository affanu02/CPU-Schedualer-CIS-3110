/*
* Assignment 2, CIS*3110, due feb 26th, 2021. CPU simulation.
* last date modified: 2021-03-04
* Author: Affan Khan 1095729
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simcpu.h"

/*main function that gets the command line arguments*/
int main(int argc, char *argv[]){
    //Loop to read all command Line arguments 
    for(int i = 0; i < argc; i++){
        if(!strcmp(argv[i], "-d")){//if d flag is inputted
            dFlag = 1;
        }
        else if(!strcmp(argv[i], "-v")){//if v flag is inputted
            vFlag = 1;
        }
        else if(!strcmp(argv[i], "-r")){//if r flag is inputted
            quant = atoi(argv[i+1]);
            rFlag = 1;
        }
    }

    //if rFlag is true, then do RR
    if(rFlag){
        RR();
    }
    else{//else do FCFS
        FCFS();
    }
}

/*first come first served scheduling code. extracts info from the textfile and puts it into a list of structures*/
void FCFS(){
    //variables used
    int procNum = 0;
    int tswitch = 0;
    int procswitch = 0;
    int totalBursts = 0;
    

    //get number of processes, number of time units requeird to swith to new thread, time units requeird to switch to different process
    fscanf(stdin, "%d %d %d", &procNum, &tswitch, &procswitch);
    Process readyQ[procNum];
    threadSwitch = tswitch;
    processSwitch = procswitch;

    //loop through processes
    for(int i = 0; i < procNum; i++){
        fscanf(stdin, "%d %d", &readyQ[i].procNumInside, &readyQ[i].threadNum);
        readyQ[i].pState = 0;

        //loop through threads in process
        for(int j = 0; j < readyQ[i].threadNum; j ++){
            //variables used in here
            readyQ[i].thread[j].IOtotalTime = 0;
            readyQ[i].thread[j].serviceTime = 0;
            readyQ[i].thread[j].finishTime = 0;
            readyQ[i].thread[j].turnaroundTime = 0;
            readyQ[i].thread[j].tState = 0;
            fscanf(stdin, "%d %d %d", &readyQ[i].thread[j].threadNumInside, &readyQ[i].thread[j].arrival, &readyQ[i].thread[j].CPUbursts);

            //loop through CPU bursts except the last one
            int k = 0;
            for(k = 0; k < readyQ[i].thread[j].CPUbursts - 1; k++){
                fscanf(stdin, "%d %d %d", &readyQ[i].thread[j].burst[k].CPUburstsInside, &readyQ[i].thread[j].burst[k].CPUtime, &readyQ[i].thread[j].burst[k].IOtime);
                //calculations
                totalBursts++;
                readyQ[i].thread[j].burst[k].bState = 0;
                readyQ[i].thread[j].IOtotalTime += readyQ[i].thread[j].burst[k].IOtime;
                readyQ[i].thread[j].serviceTime += readyQ[i].thread[j].burst[k].CPUtime;
            }

            fscanf(stdin, "%d %d", &readyQ[i].thread[j].burst[k].CPUburstsInside, &readyQ[i].thread[j].burst[k].CPUtime);
            //calculations
            totalBursts++;
            readyQ[i].thread[j].burst[k].bState = 0;
            readyQ[i].thread[j].burst[k].IOtime = 0;
            readyQ[i].thread[j].serviceTime += readyQ[i].thread[j].burst[k].CPUtime;
        }
    }

    //printOutput(readyQ, procNum);
    readyQueueCretor(readyQ, procNum, totalBursts);
    if(dFlag){
        dFlagPrint(readyQ, procNum);
    } 
    return;
}//end of FCFS

/*Creates the Ready queue list with an algorithm I created*/
void readyQueueCretor(Process info[], int x, int totalBursts){
    //variables
    int checker = 1;
    int switchProc = 0;
    ReadyQueue list[totalBursts];
    int listElement = 0;

    while(checker){
        for(int i = 0; i < x; i++){//process loop
            //if P state flag is ready (0)
            if(info[i].pState == 0){
                for(int j = 0; j < info[i].threadNum; j ++){//thread looper
                    int k = 0;
                    for(k = 0; k < info[i].thread[j].CPUbursts; k++){//burst looper
                        if(info[i].thread[j].burst[k].bState != 2){
                            //printf("%d %d %d\n", info[i].thread[j].burst[k].CPUburstsInside, info[i].thread[j].burst[k].CPUtime, info[i].thread[j].burst[k].IOtime);
                            
                            {
                            list[listElement].processNum = i;
                            list[listElement].threadNum = j;
                            list[listElement].timeCpu = info[i].thread[j].burst[k].CPUtime;
                            list[listElement].timeIO = info[i].thread[j].burst[k].IOtime;
                            listElement++;
                            }
                            info[i].thread[j].burst[k].bState = 2;


                            //round robin calculations
                            if(info[i].thread[j].burst[k].quantFlag && (info[i].thread[j].burst[k].CPUtime > quant)){
                                info[i].thread[j].burst[k].bState = 1;//set to running if the quant is higher than CPUtime
                                list[listElement].timeCpu = quant;
                                info[i].thread[j].burst[k].CPUtime -= quant;
                            }


                            switchProc = 1;
                            break;
                        }
                    }

                    //checks to see if all bursts are finished inside of a thread
                    info[i].pState = 1;
                    info[i].thread[j].tState = 2;
                    for(k = 0; k < info[i].thread[j].CPUbursts; k++){
                        if(info[i].thread[j].burst[k].bState != 2){
                            info[i].thread[j].tState = 1;
                            break;
                        }
                    }

                    //check if it needs to break to go back to other process
                    if(switchProc){
                        switchProc = 0;
                        break;
                    }
                }
            }
            else if(info[i].pState == 1){
                //finds any threads in ready phase
                int avoiderElem = -1;
                for(int j = 0; j < info[i].threadNum; j ++){//thread looper
                    if(info[i].thread[j].tState == 0){//do its first burst
                        avoiderElem = j;
                        int k = 0;
                        for(k = 0; k < info[i].thread[j].CPUbursts; k++){//burst looper
                            if(info[i].thread[j].burst[k].bState != 2){
                                //printf("%d %d %d\n", info[i].thread[j].burst[k].CPUburstsInside, info[i].thread[j].burst[k].CPUtime, info[i].thread[j].burst[k].IOtime);
                                
                                {
                                list[listElement].processNum = i;
                                list[listElement].threadNum = j;
                                list[listElement].timeCpu = info[i].thread[j].burst[k].CPUtime;
                                list[listElement].timeIO = info[i].thread[j].burst[k].IOtime;
                                listElement++;
                                }                               
                                info[i].thread[j].burst[k].bState = 2;

                                //round robin calculations
                                if(info[i].thread[j].burst[k].quantFlag && (info[i].thread[j].burst[k].CPUtime > quant)){
                                    info[i].thread[j].burst[k].bState = 1;//set to running if the quant is higher than CPUtime
                                    list[listElement].timeCpu = quant;
                                    info[i].thread[j].burst[k].CPUtime -= quant;
                                }

                                switchProc = 1;
                                break;
                            }
                        }

                        //check to see if all bursts are finished in thread
                        info[i].thread[j].tState = 2;
                        for(k = 0; k < info[i].thread[j].CPUbursts; k++){
                            if(info[i].thread[j].burst[k].bState != 2){
                                info[i].thread[j].tState = 1;
                                break;
                            }
                        }
                    }
                }

                //now loop any threads that are running and execute its next burst
                if(avoiderElem == -1){
                    //means there are no ready threads, all running or finished
                    for(int j = (info[i].threadNum - 1); j > -1; j--){
                        if(info[i].thread[j].tState != 2){
                            int k = 0;
                            for(k = 0; k < info[i].thread[j].CPUbursts; k++){//burst looper
                                if(info[i].thread[j].burst[k].bState != 2){
                                    //printf("%d %d %d\n", info[i].thread[j].burst[k].CPUburstsInside, info[i].thread[j].burst[k].CPUtime, info[i].thread[j].burst[k].IOtime);
                                    
                                    {
                                    list[listElement].processNum = i;
                                    list[listElement].threadNum = j;
                                    list[listElement].timeCpu = info[i].thread[j].burst[k].CPUtime;
                                    list[listElement].timeIO = info[i].thread[j].burst[k].IOtime;
                                    listElement++;
                                    }


                                    //round robin calculations
                                    if(info[i].thread[j].burst[k].quantFlag && (info[i].thread[j].burst[k].CPUtime > quant)){
                                        info[i].thread[j].burst[k].bState = 1;//set to running if the quant is higher than CPUtime
                                        list[listElement].timeCpu = quant;
                                        info[i].thread[j].burst[k].CPUtime -= quant;
                                    }
                                    
                                    info[i].thread[j].burst[k].bState = 2;
                                    break;
                                }
                            }

                            //check to see if all bursts are finished in thread
                            info[i].thread[j].tState = 2;
                            for(k = 0; k < info[i].thread[j].CPUbursts; k++){
                                if(info[i].thread[j].burst[k].bState != 2){
                                    info[i].thread[j].tState = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
                else{
                    //means there is one less ready thread
                    for(int j = 0; j < avoiderElem; j++){
                        if(info[i].thread[j].tState != 2){
                            int k = 0;
                            for(k = 0; k < info[i].thread[j].CPUbursts; k++){//burst looper
                                if(info[i].thread[j].burst[k].bState != 2){
                                    //printf("%d %d %d\n", info[i].thread[j].burst[k].CPUburstsInside, info[i].thread[j].burst[k].CPUtime, info[i].thread[j].burst[k].IOtime);
                                    
                                    {
                                    list[listElement].processNum = i;
                                    list[listElement].threadNum = j;
                                    list[listElement].timeCpu = info[i].thread[j].burst[k].CPUtime;
                                    list[listElement].timeIO = info[i].thread[j].burst[k].IOtime;
                                    listElement++;
                                    }


                                    //round robin calculations
                                    if(info[i].thread[j].burst[k].quantFlag && (info[i].thread[j].burst[k].CPUtime > quant)){
                                        info[i].thread[j].burst[k].bState = 1;//set to running if the quant is higher than CPUtime
                                        list[listElement].timeCpu = quant;
                                        info[i].thread[j].burst[k].CPUtime -= quant;
                                    }
                                    
                                    info[i].thread[j].burst[k].bState = 2;
                                    break;
                                }
                            }

                            //check to see if all bursts are finished in thread
                            info[i].thread[j].tState = 2;
                            for(k = 0; k < info[i].thread[j].CPUbursts; k++){
                                if(info[i].thread[j].burst[k].bState != 2){
                                    info[i].thread[j].tState = 1;
                                    break;
                                }
                            }
                        }
                        else if(info[i].thread[j].tState == 0){
                            break;
                        }
                    }
                }

            }

            //loop through all threads and check if all are finished
            if(info[i].pState == 1 || info[i].pState == 0){
                info[i].pState = 2;
                for(int j = 0; j < info[i].threadNum; j++){
                    if(info[i].thread[j].tState != 2){
                        info[i].pState = 1;
                    }
                }
            }
        }

        //checks if process are all finished (2)
        checker = 0;
        for(int i = 0; i < x; i++){
            //checks if all processes are finished
            if(info[i].pState != 2){
                checker = 1;
            }
        }
    }

    CpuScheduler(info, list, totalBursts, x);
    //printReadyQueue(list, totalBursts);
}

/*
* CPU scheduling function that calculates the total time required, average turnaround time and CPU utilization
* Also calculates turnaround time and finish time of each thread
*/
void CpuScheduler(Process info[], ReadyQueue list[], int totalBursts, int x){
    //variables
    int time = 0;
    double totalCpu = 0;
    int totalContextSwitch = 0;
    int contextSwitch = 0;
    double CPUU = 0;
    double averageTurnaroundTime = 0;
    double totalTime = 0;

    //loops through the queue and performs calculations
    for(int i = 0; i < totalBursts; i++){
        if(vFlag){
            printf("At time %d: Thread %d of Process %d moves from %s to %s\n", time ,list[i].processNum + 1, list[i].threadNum + 1, "new", "ready");
        }

        //check to see what the context switch added on would be for future queue value
        if(i+1 == totalBursts){
            contextSwitch = 0;
        }
        else{
            if(list[i+1].processNum == list[i].processNum){
                contextSwitch = threadSwitch;
            }
            else{
                contextSwitch = processSwitch;
            }
        }

        //do calculations
            totalCpu += list[i].timeCpu;
            totalContextSwitch += contextSwitch;
            time += contextSwitch;
            time += list[i].timeCpu;
        if(vFlag){
            printf("At time %d: Thread %d of Process %d moves from %s to %s\n", time,list[i].processNum + 1, list[i].threadNum + 1, "ready", "running");
        }


        //checks to see if the thread is finished or not
        if(list[i].timeIO == 0){
            //calculations
            int m = list[i].processNum;
            int n = list[i].threadNum;
            int temp = time - contextSwitch;
            info[m].thread[n].finishTime = temp;
            info[m].thread[n].turnaroundTime = temp - info[m].thread[n].arrival;
        }


        if(vFlag){
            printf("At time %d: Thread %d of Process %d moves from %s to %s\n", time, list[i].processNum + 1, list[i].threadNum + 1, "running", "terminated");
        }
    }

    //get highest turnaround times for each process
    for(int i = 0; i < x; i++){
        int highest = 0;
        for(int j = 0; j < info[i].threadNum; j++){
            if(info[i].thread[j].finishTime > highest){
                highest = info[i].thread[j].finishTime;
                
            }
        }
        averageTurnaroundTime += highest;
    }

    //calculations
    totalTime = totalCpu + totalContextSwitch;
    CPUU = totalCpu/totalTime;
    averageTurnaroundTime = averageTurnaroundTime/x;

    //print output to STDOUT
    printf("Total Time Required = %0.f units\n", totalTime);
    printf("Average Turnaround Time is %0.1f units\n", averageTurnaroundTime);//averageturnaroundtime 417.5?????
    printf("CPU Utilization is %0.1f\n", CPUU*100);//CPU utilizatrion % 91???
}

/*If the dFlag is true, it prints the dFlag output to the STDOUT*/
void dFlagPrint(Process readyQ[], int x){
    int procNum = x;

    //loop through processes
    for(int i = 0; i < procNum; i++){

        //loop through threads in process
        for(int j = 0; j < readyQ[i].threadNum; j ++){
            printf("\nThread %d of Process %d: \n",(j + 1), (i + 1));  
                printf("  Arrival time: %d \n",  readyQ[i].thread[j].arrival); 
                printf("  Service time: %d units, ",  readyQ[i].thread[j].serviceTime); 
                printf("I/O time: %d units, ",  readyQ[i].thread[j].IOtotalTime);
                printf("Turnaround time: %d units, ", readyQ[i].thread[j].turnaroundTime) ;
                printf("Finish time: %d units", readyQ[i].thread[j].finishTime);
        }
    }

    printf("\n");
}

/*Function for debugging purposes, prints readyQueue to STDOUT*/
void printReadyQueue(ReadyQueue list[], int x){
    printf("--------------------------readyQueue print order------------------------------------\n");
    for(int i = 0; i < x; i++){
        printf("%d %d   %d %d\n", list[i].processNum, list[i].threadNum, list[i].timeCpu, list[i].timeIO);
    }
}

/*Function for debugging purposes, prints the textfile to STDOUT*/
void printOutput(Process readyQ[], int x){
    printf("--------------------------------------\n");
    int procNum = x;

    //loop through processes
    for(int i = 0; i < procNum; i++){
        printf("%d %d\n", readyQ[i].procNumInside, readyQ[i].threadNum);

        //loop through threads in process
        for(int j = 0; j < readyQ[i].threadNum; j ++){
            printf("\t%d %d %d\n", readyQ[i].thread[j].threadNumInside, readyQ[i].thread[j].arrival, readyQ[i].thread[j].CPUbursts);

            //loop through CPU bursts except the last one
            int k = 0;
            for(k = 0; k < readyQ[i].thread[j].CPUbursts - 1; k++){
                printf("\t\t%d %d %d\n", readyQ[i].thread[j].burst[k].CPUburstsInside, readyQ[i].thread[j].burst[k].CPUtime, readyQ[i].thread[j].burst[k].IOtime);
            }

            printf("\t\t%d %d\n", readyQ[i].thread[j].burst[k].CPUburstsInside, readyQ[i].thread[j].burst[k].CPUtime);
        }
    }
}

/*robin scheduling code*/
void RR(){
    //variables used
    int procNum = 0;
    int tswitch = 0;
    int procswitch = 0;
    int totalBursts = 0;

    //get number of processes, number of time units requeird to swith to new thread, time units requeird to switch to different process
    fscanf(stdin, "%d %d %d", &procNum, &tswitch, &procswitch);
    Process readyQ[procNum];
    threadSwitch = tswitch;
    processSwitch = procswitch;

    //loop through processes
    for(int i = 0; i < procNum; i++){
        fscanf(stdin, "%d %d", &readyQ[i].procNumInside, &readyQ[i].threadNum);
        readyQ[i].pState = 0;

        //loop through threads in process
        for(int j = 0; j < readyQ[i].threadNum; j ++){
            //variables used in here
            readyQ[i].thread[j].IOtotalTime = 0;
            readyQ[i].thread[j].serviceTime = 0;
            readyQ[i].thread[j].finishTime = 0;
            readyQ[i].thread[j].turnaroundTime = 0;
            readyQ[i].thread[j].tState = 0;
            fscanf(stdin, "%d %d %d", &readyQ[i].thread[j].threadNumInside, &readyQ[i].thread[j].arrival, &readyQ[i].thread[j].CPUbursts);

            //loop through CPU bursts except the last one
            int k = 0;
            for(k = 0; k < readyQ[i].thread[j].CPUbursts - 1; k++){
                fscanf(stdin, "%d %d %d", &readyQ[i].thread[j].burst[k].CPUburstsInside, &readyQ[i].thread[j].burst[k].CPUtime, &readyQ[i].thread[j].burst[k].IOtime);
                //calculations
                totalBursts++;
                readyQ[i].thread[j].burst[k].bState = 0;
                readyQ[i].thread[j].IOtotalTime += readyQ[i].thread[j].burst[k].IOtime;
                readyQ[i].thread[j].serviceTime += readyQ[i].thread[j].burst[k].CPUtime;

                //checks if the CPU time is greater than the quant time
                if(quant < readyQ[i].thread[j].burst[k].CPUtime){
                    readyQ[i].thread[j].burst[k].quantFlag = 1;
                    totalBursts++;
                }
                else{
                    readyQ[i].thread[j].burst[k].quantFlag = 0;
                }
            }

            fscanf(stdin, "%d %d", &readyQ[i].thread[j].burst[k].CPUburstsInside, &readyQ[i].thread[j].burst[k].CPUtime);
            //calculations
            totalBursts++;
            readyQ[i].thread[j].burst[k].bState = 0;
            readyQ[i].thread[j].burst[k].IOtime = 0;
            readyQ[i].thread[j].serviceTime += readyQ[i].thread[j].burst[k].CPUtime;

            //checks if the CPU time is greater than the quant time
            if(quant < readyQ[i].thread[j].burst[k].CPUtime){
                readyQ[i].thread[j].burst[k].quantFlag = 1;
                totalBursts++;
            }
            else{
                readyQ[i].thread[j].burst[k].quantFlag = 0;
            }
        }
    }

    //printOutput(readyQ, procNum);
    readyQueueCretor(readyQ, procNum, totalBursts);

    if(dFlag){
        dFlagPrint(readyQ, procNum);
    } 
    return;
}