#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <mqueue.h>
#include "Car.h"
#include "Button.h"
#include "Led.h"
#include "Car.h"

// Macros
#define SECOND_IN_NANO 1000000000
#define MS_IN_NANO (SECOND_IN_NANO/1000)
#define QUEUE_NAME "/gear_queue"
#define MAX_MSG_SIZE 10
#define MAX_MSG_NUM 30

// Function prototypes
double degreeToRadian(double degreeValue);
double sinValueToPercent(double sinValue);

// Threads
pthread_t carThread;
pthread_t ledThread;
pthread_t readButtonThread;
pthread_t gearThread;

//Mutex
pthread_mutex_t volvoMutex = PTHREAD_MUTEX_INITIALIZER;

// Variables
Car volvo;
Button stopButton, turnLeftButton, turnRightButton, gearButton;
Led stopLed, turnLeftLed, turnRightLed, greenLed;
volatile int runningFlag = 1;

void* carTask(void* args)
{
	struct timespec waitTimeSpec,waitTime2, remainingTime, remainingTime2;
	waitTimeSpec.tv_nsec = MS_IN_NANO * 100;
	waitTime2.tv_nsec = MS_IN_NANO * 1;
	
	while (1)
	{
		pthread_mutex_lock(&volvoMutex);
		carRun(&volvo);
		//clock_nanosleep(CLOCK_REALTIME, 0, &waitTime2, &remainingTime2);
		pthread_mutex_unlock(&volvoMutex);
		//clock_nanosleep(CLOCK_REALTIME, 0, &waitTimeSpec, &remainingTime);
	}
}

void* readButtonTask(void* args)
{
	struct timespec waitTimeSpec, remainingTime;
	waitTimeSpec.tv_nsec = MS_IN_NANO * 10;
	
	mqd_t btnQueue;
	struct mq_attr btnAttr;
	char buffer[MAX_MSG_SIZE];
	
	btnAttr.mq_flags = 0;
	btnAttr.mq_maxmsg = MAX_MSG_NUM;
	btnAttr.mq_msgsize = MAX_MSG_SIZE;
	btnAttr.mq_curmsgs = 0;
	
	btnQueue = mq_open(QUEUE_NAME, O_WRONLY);
	if (btnQueue == -1)
	{
		printf("Button task couldn't open the queue,  Error: %s", strerror(errno));
		exit(1);
	}

	while (1)
	{
		if (buttonPositiveFlancDetection(&stopButton))
		{
			sprintf(buffer, "%s", "stop");
			if (mq_send(btnQueue, buffer, MAX_MSG_SIZE, 10) < 0)
			{
				printf("Buttontask: stop button couldn't send the message, Error: %s", strerror(errno));
				mq_close(btnQueue);
				exit(1);
			}
		}
		if (buttonPositiveFlancDetection(&turnLeftButton))
		{
			sprintf(buffer, "%s", "left");
			if (mq_send(btnQueue, buffer, MAX_MSG_SIZE, 0) < 0)
			{
				printf("Buttontask: left button couldn't send the message, Error: %s", strerror(errno));
				mq_close(btnQueue);
				exit(1);
			}
		}
		if (buttonPositiveFlancDetection(&turnRightButton))
		{
			sprintf(buffer, "%s", "right");
			if (mq_send(btnQueue, buffer, MAX_MSG_SIZE, 0) < 0)
			{
				printf("Buttontask: right button couldn't send the message, Error: %s", strerror(errno));
				mq_close(btnQueue);
				exit(1);
			}
		}
		if (buttonPositiveFlancDetection(&gearButton))
		{
			clock_nanosleep(CLOCK_REALTIME, 0, &waitTimeSpec, &remainingTime);
			volatile int i = 1;
			while (!buttonReadValue(&gearButton))
			{
				if (buttonNegativeFlancDetection(&gearButton))
				{
					break;
				}
				sleep(1);
				if (i++ >= 3)
				{
					sprintf(buffer, "%s", "reverse");
					if (mq_send(btnQueue, buffer, MAX_MSG_SIZE, 0) < 0)
					{
						printf("Buttontask: gear button(reverse) couldn't send the message, Error: %s", strerror(errno));
						mq_close(btnQueue);
						exit(1);
					}
					break;
				}
			}
			if (i < 3)
			{
				sprintf(buffer, "%s", "gear");
				if (mq_send(btnQueue, buffer, MAX_MSG_SIZE, 0) < 0)
				{
					printf("Buttontask: gear button couldn't send the message, Error: %s", strerror(errno));
					mq_close(btnQueue);
					exit(1);
				}
			}
		}

		clock_nanosleep(CLOCK_REALTIME, 0, &waitTimeSpec, &remainingTime);
	}
}

void* ledTask(void* args)
{
	struct timespec blinkTimeSpec, remainingTime;
	struct timespec waitTimeSpec, remainingWaitTime;
	waitTimeSpec.tv_nsec = MS_IN_NANO * 10;
	blinkTimeSpec.tv_nsec = SECOND_IN_NANO/2;
	GearState ledGearState;
	double degree = 0;
	double sinValue;
	size_t reverseLedFlag = 1;

	while (1)
	{
		pthread_mutex_lock(&volvoMutex);
		ledGearState = volvo.gearState;
		pthread_mutex_unlock(&volvoMutex);
		
		switch (ledGearState)
		{
		case (Neutral):
			ledTurnOff(&stopLed);
			ledTurnOff(&turnLeftLed);
			ledTurnOff(&turnRightLed);
				
			sinValue = sin(degreeToRadian(degree++));
			pwmPulse(greenLed.pwm, sinValueToPercent(sinValue));

			if (degree == 360)
			{
				degree = 0;
			}
			break;

		case (gs25):
			ledTurnOff(&stopLed);
			ledTurnOff(&turnLeftLed);
			ledTurnOff(&turnRightLed);
			blinkTimeSpec.tv_nsec = 250000000;
			ledToggle(&greenLed);
			clock_nanosleep(CLOCK_REALTIME, 0, &blinkTimeSpec, &remainingTime);
			break;

		case (gs50):
			blinkTimeSpec.tv_nsec = 125000000;
			ledToggle(&greenLed);
			clock_nanosleep(CLOCK_REALTIME, 0, &blinkTimeSpec, &remainingTime);
			break;

		case (gs75):
			blinkTimeSpec.tv_nsec = 50000000;
			ledToggle(&greenLed);
			clock_nanosleep(CLOCK_REALTIME, 0, &blinkTimeSpec, &remainingTime);
			break;

		case (gs100):
			blinkTimeSpec.tv_nsec = 25000000;
			ledToggle(&greenLed);
			clock_nanosleep(CLOCK_REALTIME, 0, &blinkTimeSpec, &remainingTime);
			break;

		case (Stop):
			ledTurnOff(&turnLeftLed);
			ledTurnOff(&turnRightLed);
			ledTurnOff(&greenLed);
			blinkTimeSpec.tv_nsec = 500000000;
			ledToggle(&stopLed);
			clock_nanosleep(CLOCK_REALTIME, 0, &blinkTimeSpec, &remainingTime);
			break;

		case (ReverseGear):
			ledTurnOff(&greenLed);	
			ledTurnOff(&stopLed);
			blinkTimeSpec.tv_nsec = 500000000;
			if (reverseLedFlag)
			{
				ledTurnOn(&turnLeftLed);
				ledTurnOn(&turnRightLed);
				reverseLedFlag = 0;
			}
			else
			{
				ledTurnOff(&turnLeftLed);
				ledTurnOff(&turnRightLed);
				reverseLedFlag = 1;
			}
				
			
			clock_nanosleep(CLOCK_REALTIME, 0, &blinkTimeSpec, &remainingTime);
			break;
			
		case (LeftTurn):
			ledTurnOff(&greenLed);	
			ledTurnOff(&stopLed);
			ledTurnOff(&turnRightLed);
			blinkTimeSpec.tv_nsec = 250000000;
			ledToggle(&turnLeftLed);
			clock_nanosleep(CLOCK_REALTIME, 0, &blinkTimeSpec, &remainingTime);
			break;
			
		case (RightTurn):
			ledTurnOff(&greenLed);	
			ledTurnOff(&stopLed);
			ledTurnOff(&turnLeftLed);
			blinkTimeSpec.tv_nsec = 250000000;
			ledToggle(&turnRightLed);
			clock_nanosleep(CLOCK_REALTIME, 0, &blinkTimeSpec, &remainingTime);
			break;
		}
		
		

		clock_nanosleep(CLOCK_REALTIME, 0, &waitTimeSpec, &remainingWaitTime);
	}
}

void* gearTask(void* args)
{
	printf("Geartask: innan nagot\n");
	struct timespec waitTimeSpec, remainingTime;
	waitTimeSpec.tv_nsec = MS_IN_NANO * 100;
	mqd_t gearQueue;
	struct mq_attr gearAttr, gearAttr2;
	char spam[MAX_MSG_SIZE+1];
	char buffer[MAX_MSG_SIZE + 1];
	unsigned int prio;
	
	gearAttr.mq_flags = 0;
	gearAttr.mq_maxmsg = MAX_MSG_NUM;
	gearAttr.mq_msgsize = MAX_MSG_SIZE;
	//gearAttr.mq_curmsgs = 0;
	
	gearQueue = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0777, &gearAttr);
	//printf("Geartask: gearqueue skapad\n");
	if (gearQueue == -1)
	{
		printf("gearTask couldn't create the queue,  Error: %s", strerror(errno));
		exit(1);
	}
	while (1)
	{
		//printf("Geartask: innan mq_recieve\n");
		if (mq_receive(gearQueue, buffer, MAX_MSG_SIZE, &prio) < 0)
		{
			printf("Geartask: couldn't recieve the message, Error: %s", strerror(errno));
			mq_close(gearQueue);
			exit(1);
		}
		if (strcmp(buffer, "stop") == 0)
		{
//			if (volvo.gearState == Stop)
//			{
//				runningFlag = 0;
//				break;
//			}
			pthread_mutex_lock(&volvoMutex);
			carStop(&volvo);
			pthread_mutex_unlock(&volvoMutex);
//			mq_getattr(gearQueue, &gearAttr2);
//			printf("CurrentMessages: %ld\n", gearAttr2.mq_curmsgs);
//			while (gearAttr2.mq_curmsgs > 0) 
//			{
//				mq_receive(gearQueue, spam, MAX_MSG_SIZE, &prio);
//				printf("CurrentMessages: %ld\n", gearAttr2.mq_curmsgs);
//				mq_getattr(gearQueue, &gearAttr2);
//			}
		}
		else if (strcmp(buffer, "gear") == 0)
		{
			pthread_mutex_lock(&volvoMutex);
			incrementGear(&volvo);
			pthread_mutex_unlock(&volvoMutex);
		}
		
		else if (strcmp(buffer, "reverse") == 0)
		{
			pthread_mutex_lock(&volvoMutex);
			carSetReverse(&volvo);
			pthread_mutex_unlock(&volvoMutex);
		}
		
		else if (strcmp(buffer, "left") == 0)
		{
			pthread_mutex_lock(&volvoMutex);
			carTurnLeft(&volvo);
			pthread_mutex_unlock(&volvoMutex);
		}
		
		else if (strcmp(buffer, "right") == 0)
		{
			pthread_mutex_lock(&volvoMutex);
			carTurnRight(&volvo);
			pthread_mutex_unlock(&volvoMutex);
		}
		
		//printf("Geartask: vid sleep\n");
		clock_nanosleep(CLOCK_REALTIME, 0, &waitTimeSpec, &remainingTime);
	}
}

int main(int argc, char *argv[])
{
	printf("Start");
	// Init
	gpioSetup();
	carInit(&volvo);

	buttonInit(&stopButton, _4);
	buttonInit(&turnLeftButton, _22);
	buttonInit(&turnRightButton, _17);
	buttonInit(&gearButton, _27);
	
	ledInit(&stopLed, _19);
	ledInit(&turnLeftLed, _21);
	ledInit(&turnRightLed, _13);
	ledInit(&greenLed, _26);

	// Create threads
	
	if (pthread_create(&carThread, NULL, carTask, NULL) < 0)
	{
		printf("Could not create car thread. Error: %s", strerror(errno));
		exit(1);
	}
	if (pthread_create(&gearThread, NULL, gearTask, NULL) < 0)
	{
		printf("Could not create gear thread. Error: %s", strerror(errno));
		exit(1);
	}

	if (pthread_create(&readButtonThread, NULL, readButtonTask, NULL) < 0)
	{
		printf("Could not create read button thread. Error: %s", strerror(errno));
		exit(1);
	}

	if 	(pthread_create(&ledThread, NULL, ledTask, NULL)<0)
	{
		printf("Could not create led thread. Error: %s", strerror(errno));
		exit(1);
	}
	

	
	
	//Thread Params
	struct sched_param carThreadParams, readButtonThreadParams, ledThreadParams, gearThreadParams;
	
	carThreadParams.__sched_priority = 20;
	readButtonThreadParams.__sched_priority = 60;
	ledThreadParams.__sched_priority = 30;
	gearThreadParams.__sched_priority = 70;
	pthread_setschedparam(carThread, SCHED_FIFO, &carThreadParams);
	pthread_setschedparam(readButtonThread, SCHED_FIFO, &readButtonThreadParams);
	pthread_setschedparam(ledThread, SCHED_FIFO, &ledThreadParams);
	pthread_setschedparam(gearThread, SCHED_FIFO, &ledThreadParams);

	// Run
	sleep(20);
	runningFlag = 0;
	
	//Join
	pthread_join(carThread, NULL);
	pthread_join(readButtonThread, NULL);
	pthread_join(ledThread, NULL);
	pthread_join(gearThread, NULL);
	
	carDestroy(&volvo);
	
	buttonDestroy(&stopButton);
	buttonDestroy(&turnLeftButton);
	buttonDestroy(&turnRightButton);
	buttonDestroy(&gearButton);
	
	ledDestroy(&stopLed);
	ledDestroy(&turnLeftLed);
	ledDestroy(&turnRightLed);
	ledDestroy(&greenLed);
	


	
	mq_unlink(QUEUE_NAME);
	
	pthread_mutex_destroy(&volvoMutex);
	
	fflush(stdout); /* <============== Put a breakpoint here */
	return 0;
}

double degreeToRadian(double degreeValue)
{
	return (degreeValue * (M_PI / 180));
}

double sinValueToPercent(double sinValue)
{
	return (fabs(sinValue * 100));
}