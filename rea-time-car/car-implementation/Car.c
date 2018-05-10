#include "Car.h"
#include "Engine.h"

void carInit(Car* car)
{
	implementEngines(car);
}

void implementEngines(Car* car)
{
	engineInit(&(car->engineLeft), _23, _24, _25);
	engineInit(&(car->engineRight), _6, _16, _20);
}

void carRun(Car* car, int speed, Direction direction)
{
	engineRun(&(car->engineLeft), speed, direction);
	engineRun(&(car->engineRight), speed, direction);
}

void carReverse(Car* car)
{
	carRun(car, 25, Reverse);
}

void carDestroy(Car* car)
{
	engineDestroy(&(car->engineLeft));
	engineDestroy(&(car->engineRight));
}

void carStop(Car* car)
{
	onOff(car->engineLeft.speedPin.pio, OFF);
	onOff(car->engineRight.speedPin.pio, OFF);
}