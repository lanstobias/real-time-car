#include "Engine.h"

void engineInit(Engine* engine, enum Pin speedPin, enum Pin forwardPin, enum Pin reversePin)
{
	engine->forwardPin = create(forwardPin, OUT_PIN);
	engine->reversePin = create(reversePin, OUT_PIN);
	engine->speedPin = pwmCreate(speedPin, 100);
}

void engineRun(Engine* engine)
{
	pwmPulse(engine->speedPin, engine->speed);

	if (engine->direction == Forward)
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

void engineSetDirection(Engine* engine, Direction direction)
{
	engine->direction = direction;
}
void engineSetSpeed(Engine* engine, int speed)
{
	if (speed>=0 && speed<=100)
		engine->speed = speed;
}