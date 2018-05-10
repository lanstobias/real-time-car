#include <stdlib.h>
#include <stdbool.h>
#include "Button.h"

// Constructors
void buttonInit(Button* button, enum Pin pin)
{
	button->pin = pin;
	button_create(button);
}

// Methods
int buttonNegativeFlancDetection(Button* button)
{
	return button_flancDetection(button, Negative);
}

int buttonPositiveFlancDetection(Button* button)
{
	return button_flancDetection(button, Positive);
}

int button_flancDetection(Button* button, FlancDirection direction)
{
	int flancDetection = false;

	button->value = readIn(button->pinObject);

	if (direction == Negative)
	{
		flancDetection = (button->value && !button->oldValue) ? true : false;
	}
	else if (direction == Positive)
	{
		flancDetection = (!button->value && button->oldValue) ? true : false;
	}

	button->oldValue = button->value;

	return flancDetection;
}

int buttonReadValue(Button* button)
{
	return readIn(button->pinObject);
}

void buttonDestroy(Button* button)
{
	destroy(button->pinObject);
}

void button_create(Button* button)
{
	button->pinObject = createWithPullUpDown(button->pin, PUD_UP);
}