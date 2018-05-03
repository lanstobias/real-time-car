#include "Engine.h"

void engineInit(Engine* engine, enum Pin speedPin, enum Pin forwardPin, enum Pin reversePin)
{
	
}

void engineRun(Engine* engine, int speed, Direction direction)
{
	pwmPulse(engine->pwm, speed);

	engine->forwardPin = 1;
	engine->reversePin = 0;
}

void engineDestroy(Engine* engine)
{
	
}