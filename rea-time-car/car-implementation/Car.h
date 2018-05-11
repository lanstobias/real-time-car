#ifndef CAR_H
#define CAR_H
#include "Engine.h"
#include "piodirect.h"
#include "pwm.h"

typedef enum CarDirection
{
	CarForward,
	CarReverse,
	CarLeft,
	CarRight
}CarDirection;
typedef enum GearState
{
	Neutral, 
	gs25, 
	gs50,
	gs75,
	gs100,
	Stop,
	ReverseGear
} GearState;

typedef struct Car
{
	Engine engineLeft;
	Engine engineRight;
	GearState gearState;
	int speed;
	CarDirection carDirection;
	
}Car;

void carInit(Car* car);
void implementEngines(Car* car);
void carRun(Car* car);
void carDestroy(Car* car);
void carSetReverse(Car* car);
void carSetForward(Car* car);
void carStop(Car* car);
void carSetSpeed(Car* car, int speed);
void carSetEnginesToCarDirection(Car* car);
void carSetEnginesSpeed(Car* car);
void incrementGear(Car* car);
#endif

