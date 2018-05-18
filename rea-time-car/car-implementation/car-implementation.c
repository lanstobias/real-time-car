#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include "Car.h"
#include "Button.h"
#include "Led.h"

// Function prototypes
double degreeToRadian(double degreeValue);
double sinValueToPercent(double sinValue);

// Threads
pthread_t carThread;
pthread_t ledThread;
pthread_t readButtonThread;

// Variables
Car volvo;
Button stopButton, turnLeftButton, turnRightButton, gearButton;
Led stopLed, turnLeftLed, turnRightLed, greenLed;

void* carTask(void* args)
{
	while (1)
	{
		if (volvo.gearState == Neutral || volvo.gearState == Stop)
		{
			carStop(&volvo);
		}
		else
		{
			carRun(&volvo);
		}
	}
}

void* readButtonTask(void* args)
{
	struct timespec blinkTimeSpec, remainingTime;
	blinkTimeSpec.tv_nsec = 100000000;
	while (1)
	{
		if (buttonPositiveFlancDetection(&stopButton))
		{
			//if (volvo.gearState == Stop)
				//terminate();
			volvo.gearState = Stop;
		}
		if (buttonPositiveFlancDetection(&turnLeftButton))
		{
			carSetReverse(&volvo);
		}
		if (buttonPositiveFlancDetection(&turnRightButton))
		{
			//sendEventToQueue("turn right button pressed");
		}
		if (buttonPositiveFlancDetection(&gearButton))
		{
			incrementGear(&volvo);
		}
		clock_nanosleep(CLOCK_REALTIME, 0, &blinkTimeSpec, &remainingTime);
	}
}

void* ledTask(void* args)
{
	struct timespec blinkTimeSpec, remainingTime;
	struct timespec waitTimeSpec, remainingWaitTime;
	waitTimeSpec.tv_nsec = 1000000;
	blinkTimeSpec.tv_nsec = 500000000;
	
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
				degree = 0;

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


int main(int argc, char *argv[])
{
	//Init
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

	pthread_create(&carThread, NULL, carTask, NULL);
	pthread_create(&readButtonThread, NULL, readButtonTask, NULL);
	pthread_create(&ledThread, NULL, ledTask, NULL);


	//Run
	sleep(20);
	carDestroy(&volvo);
	
	//Deinit
	pthread_cancel(carThread);
	pthread_cancel(readButtonThread);
	pthread_cancel(ledThread);
	
	pthread_join(carThread, NULL);
	pthread_join(readButtonThread, NULL);
	pthread_join(ledThread, NULL);
	
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