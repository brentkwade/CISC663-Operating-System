#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
//#include "utils.h"
int warnloadflag = 0; 						//flag 0 says no warn load thread is created.
double warnload = 0.2;  
pthread_t tid1;

static void *mythread(void *param)
{

	double load[1]; 
    	// if second argument to the function is "0.0" then exit the thread

	printf("Thread created\n");

	while (getloadavg(load, 1) != -1)
   	{
		if (warnload == 0.0)
		{
			warnloadflag = 0;
			pthread_exit(NULL);	  			
		}
		if (load[0] >= warnload)
		{
   			printf("Warning: Load level is %.2f \n", load[0]);
		}
		//shellPrompt();
		sleep(5);
   	}
	
}

void warnLoadThreadCreate(double x)//to start a thread for warnload i call this
{
	warnload = x;
  	pthread_create(&tid1, NULL, mythread, "Thread 1");
	warnloadflag = 1; 					// indicates a warnload thread is created.

}

void updateThread(double x)//to update the warnload variable, i call this
{
	warnload = x;
}




