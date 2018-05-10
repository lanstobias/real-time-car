#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include "Car.h"

pthread_t carThread;
pthread_t ledThread;
Car volvo;
void* carTask(void* args)
{
	while (1)
	{
		for (int i = 0; i < 50; i++)
		{
			engineRun(&(volvo.engineLeft), 50, Forward);
		}
		for (int i = 0; i < 40; i++)
		{
			engineRun(&(volvo.engineLeft), 30, Reverse);
		}
	}
}
void* ledTask(void* args)
{
	while (1)
	{
		for (int i = 0; i < 80; i++)
		{
			engineRun(&(volvo.engineRight), 80, Forward);
		}
		for (int i = 0; i < 20; i++)
		{
			engineRun(&(volvo.engineRight), 40, Reverse);
		}
	}
}

int main(int argc, char *argv[])
{
	//Init
	gpioSetup();
	carInit(&volvo);
	pthread_create(&carThread, NULL, carTask, NULL);
	pthread_create(&ledThread, NULL, ledTask, NULL);

	//Run
	sleep(5);
	carDestroy(&volvo);
	pthread_cancel(carThread);
	pthread_cancel(ledThread);
	pthread_join(carThread, NULL);
	pthread_join(ledThread, NULL);

	
	//Deinit
	carDestroy(&volvo);
	fflush(stdout); /* <============== Put a breakpoint here */
	return 0;
}