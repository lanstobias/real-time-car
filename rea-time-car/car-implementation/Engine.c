#include "Engine.h"

void engineInit(Engine* engine, enum Pin speedPin, enum Pin forwardPin, enum Pin reversePin)
{
	engine->forwardPin = create(forwardPin, OUT_PIN);
	engine->reversePin = create(reversePin, OUT_PIN);
	engine->speedPin = pwmCreate(speedPin, 100);
}

void engineRun(Engine* engine, int speed, Direction direction)
{
	pwmPulse(engine->speedPin, speed);

	if (direction == Forward)
	{
		onOff(engine->forwardPin, ON);
		onOff(engine->reversePin, OFF);
	}
	else
	{
		onOff(engine->forwardPin, OFF);
		onOff(engine->reversePin, ON);
	}
}

void engineDestroy(Engine* engine)
{
	destroy(engine->forwardPin);
	destroy(engine->reversePin);
	pwmDestroy(engine->speedPin);
}