#ifndef BUTTON_H
#define BUTTON_H
#include "piodirect.h"

typedef enum State
{
	Pushed,
	Released
} State;

typedef enum FlancDirection
{
	Negative,
	Positive
} FlancDirection;

typedef struct Button
{
	int value, oldValue;
	enum Pin pin;
	GPIO pinObject;
} Button;

// Constructors
void buttonInit(Button* button, enum Pin pin);

// Methods
int buttonNegativeFlancDetection(Button* button);
int buttonPositiveFlancDetection(Button* button);
int buttonReadValue(Button* button);
void buttonDestroy(Button* button);
int button_flancDetection(Button* button, FlancDirection direction);
void button_create(Button* button);

#endif