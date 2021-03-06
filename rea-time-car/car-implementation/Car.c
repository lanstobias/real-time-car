#include "Car.h"
#include "Engine.h"

void carInit(Car* car)
{
	implementEngines(car);
	car->gearState = Neutral;
	carSetForward(car);
	carSetSpeed(car, 0);
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
	car->gearState = Stop;
	carSetSpeed(car, 0);
	carSetForward(car);
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
		carSetForward(car);
		break;

	case (gs50):
		car->gearState = gs75;
		carSetSpeed(car, 75);
		carSetForward(car);
		break;

	case (gs75):
		car->gearState = gs100;
		carSetSpeed(car, 100);
		carSetForward(car);
		break;

	case (gs100):
		car->gearState = Neutral;
		carSetSpeed(car, 0);
		carSetForward(car);
		break;
		
	case (ReverseGear):
		car->gearState = Neutral;
		carSetSpeed(car, 0);
		carSetForward(car);
		break;
		
	case (Stop):
		car->gearState = Neutral;
		carSetSpeed(car, 0);
		carSetForward(car);
		break;
		
	case (LeftTurn):
		car->gearState = gs25;
		carSetSpeed(car, 25);
		carSetForward(car);
		break;
		
	case (RightTurn):
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

void carTurnLeft(Car* car)
{
	carSetForward(car);
	car->gearState = LeftTurn;
	car->speed = 25;
	engineSetSpeed(&(car->engineLeft), car->speed);
	engineSetSpeed(&(car->engineRight), 0);
}
void carTurnRight(Car* car)
{
	carSetForward(car);
	car->gearState = RightTurn;
	car->speed = 25;
	engineSetSpeed(&(car->engineLeft), 0);
	engineSetSpeed(&(car->engineRight), car->speed);
}