#include "Car.h"
#include "Engine.h"

void carInit(Car* car)
{
	implementEngines(car);
	carSetSpeed(car, 70);
}

void implementEngines(Car* car)
{
	engineInit(&(car->engineLeft), _23, _24, _25);
	engineInit(&(car->engineRight), _6, _16, _20);
}

void carRun(Car* car)
{
	engineRun(&(car->engineLeft));
	engineRun(&(car->engineRight));
}

void carSetForward(Car* car)
{
	car->carDirection = CarForward;
	carSetEnginesToCarDirection(car);
}
void carSetReverse(Car* car)
{
	car->carDirection = CarReverse;
	car->gearState = ReverseGear;
	carSetEnginesToCarDirection(car);
	carSetSpeed(car, 25);
}

void carSetEnginesToCarDirection(Car* car)
{
	switch (car->carDirection)
	{
	case(CarForward):
		engineSetDirection(&(car->engineLeft), Forward);
		engineSetDirection(&(car->engineRight), Forward);
		break;
	case(CarReverse):
		engineSetDirection(&(car->engineLeft), Reverse);
		engineSetDirection(&(car->engineRight), Reverse);
		break;
	default:
		break;
	}
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

void carSetSpeed(Car* car, int speed)
{
	car->speed = speed;
	carSetEnginesSpeed(car);
}

void carSetEnginesSpeed(Car* car)
{
	engineSetSpeed(&(car->engineLeft), car->speed);
	engineSetSpeed(&(car->engineRight), car->speed);
}

void incrementGear(Car* car)
{
	switch (car->gearState)
	{
	case (Neutral):
		car->gearState = gs25;
		carSetSpeed(car, 25);
		carSetForward(car);
		break;

	case (gs25):
		car->gearState = gs50;
		carSetSpeed(car, 50);
		break;

	case (gs50):
		car->gearState = gs75;
		carSetSpeed(car, 75);
		break;

	case (gs75):
		car->gearState = gs100;
		carSetSpeed(car, 100);
		break;

	case (gs100):
		car->gearState = Neutral;
		carSetSpeed(car, 0);
		break;
		
	case (ReverseGear):
		car->gearState = gs25;
		carSetSpeed(car, 25);
		carSetForward(car);
		break;

	default:
		car->gearState = Neutral;
		carSetSpeed(car, 0);
		carSetForward(car);
		break;
	}
}