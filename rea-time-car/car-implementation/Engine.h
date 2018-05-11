#ifndef ENGINE_H
#define ENGINE_H
#include "piodirect.h"
#include "pwm.h"
#include "Led.h"

typedef enum Direction
{
	Forward,
	Reverse
}Direction;

typedef struct Engine
{
	PWM speedPin;
	GPIO forwardPin;
	GPIO reversePin;
	Direction direction;
	int speed;
}Engine;

void engineInit(Engine* engine, enum Pin speedPin, enum Pin forwardPin, enum Pin reversePin);
void engineDestroy(Engine* engine);
void engineRun(Engine* engine);
void engineSetDirection(Engine* engine, Direction direction);
void engineSetSpeed(Engine* engine, int speed);

#endif // !ENGINE_H