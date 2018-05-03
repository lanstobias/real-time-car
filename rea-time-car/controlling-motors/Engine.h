#ifndef ENGINE_H
#define ENGINE_H
#include "piodirect.h"
#include "pwm.h"
#include "Button.h"
#include "Led.h"

typedef enum Direction
{
	Forward,
	Reverse
}Direction;

typedef struct Engine
{
	enum Pin speedPin;
	enum Pin forwardPin;
	enum Pin reversePin;
	Direction direction;
	PWM pwm;
	GPIO* basePio;
}Engine;

void engineInit(Engine* engine, enum Pin speedPin, enum Pin forwardPin, enum Pin reversePin);
void engineDestroy(Engine* engine);
void engineRun(Engine* engine, int speed, Direction direction);

#endif // !ENGINE_H