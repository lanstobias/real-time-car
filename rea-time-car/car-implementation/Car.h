#ifndef CAR_H
#define CAR_H
#include "Engine.h"
#include "piodirect.h"
#include "pwm.h"

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
}Car;

void carInit(Car* car);
void implementEngines(Car* car);
void carRun(Car* car, int speed, Direction direction);
void carDestroy(Car* car);
void carStop(Car* car);
#endif

