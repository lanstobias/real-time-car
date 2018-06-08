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

// Variables
Car volvo;
Button stopButton, turnLeftButton, turnRightButton, gearButton;
Led stopLed, turnLeftLed, turnRightLed, greenLed;

void* carTask(void* args)
{
	while (1)
	{
		carRun(&volvo);
	}
}

void* readButtonTask(void* args)
{
	struct timespec waitTimeSpec, remainingTime;
	waitTimeSpec.tv_nsec = MS_IN_NANO * 100;
	
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
			if (mq_send(btnQueue, buffer, MAX_MSG_SIZE, 0) < 0)
			{
				printf("Buttontask: stop button couldn't send the message, Error: %s", strerror(errno));
				mq_close(btnQueue);
				exit(1);
			}
		}
		if (buttonPositiveFlancDetection(&turnLeftButton))
		{

		}
		if (buttonPositiveFlancDetection(&turnRightButton))
		{

		}
		if (buttonPositiveFlancDetection(&gearButton))
		{
			sprintf(buffer, "%s", "gear");
			if (mq_send(btnQueue, buffer, MAX_MSG_SIZE, 0) < 0)
			{
				printf("Buttontask: gear button couldn't send the message, Error: %s", strerror(errno));
				mq_close(btnQueue);
				exit(1);
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
	
	double degree = 0;
	double sinValue;

	while (1)
	{
		switch (volvo.gearState)
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
			ledToggle(&turnLeftLed);
			ledToggle(&turnRightLed);
			clock_nanosleep(CLOCK_REALTIME, 0, &blinkTimeSpec, &remainingTime);
			break;
		}

		clock_nanosleep(CLOCK_REALTIME, 0, &waitTimeSpec, &remainingWaitTime);
	}
}

void* gearTask(void* args)
{
	struct timespec waitTimeSpec, remainingTime;
	waitTimeSpec.tv_nsec = MS_IN_NANO * 100;
	mqd_t gearQueue;
	struct mq_attr gearAttr;
	char buffer[MAX_MSG_SIZE + 1];
	unsigned int prio;
	
	gearAttr.mq_flags = 0;
	gearAttr.mq_maxmsg = MAX_MSG_NUM;
	gearAttr.mq_msgsize = MAX_MSG_SIZE;
	gearAttr.mq_curmsgs = 0;
	
	gearQueue = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0777, &gearAttr);
	
	if (gearQueue == -1)
	{
		printf("gearTask couldn't create the queue,  Error: %s", strerror(errno));
		exit(1);
	}
	while (1)
	{
		if (mq_receive(gearQueue, buffer, MAX_MSG_SIZE, &prio) < 0)
		{
			printf("Geartask: couldn't recieve the message, Error: %s", strerror(errno));
			mq_close(gearQueue);
			exit(1);
		}
		if (strcmp(buffer, "gear") == 0)
		{
			incrementGear(&volvo);
		}
		if (strcmp(buffer, "stop") == 0)
		{
			carStop(&volvo);
		}
		clock_nanosleep(CLOCK_REALTIME, 0, &waitTimeSpec, &remainingTime);
	}
}

int main(int argc, char *argv[])
{
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
	
	carThreadParams.__sched_priority = 50;
	readButtonThreadParams.__sched_priority = 50;
	ledThreadParams.__sched_priority = 50;
	gearThreadParams.__sched_priority = 40;
	pthread_setschedparam(carThread, SCHED_FIFO, &carThreadParams);
	pthread_setschedparam(readButtonThread, SCHED_FIFO, &readButtonThreadParams);
	pthread_setschedparam(ledThread, SCHED_FIFO, &ledThreadParams);
	pthread_setschedparam(gearThread, SCHED_FIFO, &ledThreadParams);

	// Run
	sleep(20);
	carDestroy(&volvo);
	
	//Join
	pthread_join(carThread, NULL);
	pthread_join(readButtonThread, NULL);
	pthread_join(ledThread, NULL);
	pthread_join(gearThread, NULL);
	
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