#include "Engine.h"

void engineInit(Engine* engine, enum Pin speedPin, enum Pin forwardPin, enum Pin reversePin)
{
	engine->forwardPin = create(forwardPin, OUT_PIN);
	engine->reversePin = create(reversePin, OUT_PIN);
	engine->speedPin = pwmCreate(speedPin, 100);
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