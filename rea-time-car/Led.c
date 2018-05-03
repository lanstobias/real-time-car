#include "Led.h"

// Constructors
void ledInit(Led* led, enum Pin pin)
{
	led->pin = pin;
	led->brightness = 0;
	led->frequency = 100;
	led->state = OFF;
	led->brightnessState = Default;
	led_create(led);
}	

// Methods
void ledToggle(Led* led)
{
	if (led->state == ON)
	{
		ledTurnOff(led);
	}
	else if (led->state == OFF)
	{
		ledTurnOn(led);
	}

	led->state = !led->state;
}

void ledTurnOff(Led* led)
{
	onOff(*led->basePio, OFF);
}

void ledTurnOn(Led* led)
{
	onOff(*led->basePio, ON);
}

void ledDestroy(Led* led)
{
	pwmDestroy(led->pwm);
}

void ledSetBrightness(Led* led, float brightnessLevel)
{
	led->brightness = brightnessLevel;
}

void ledIncrementBrightnessStep(Led* led)
{
	switch (led->brightnessState)
	{
		case (Default):
			led->brightnessState = One;
			ledSetBrightness(led, 0);
		break;

		case (One):
			led->brightnessState = Two;
			ledSetBrightness(led, 25);
		break;

		case (Two):
			led->brightnessState = Three;
			ledSetBrightness(led, 50);
		break;

		case (Three):
			led->brightnessState = Four;
			ledSetBrightness(led, 75);
		break;

		case (Four):
			led->brightnessState = Five;
			ledSetBrightness(led, 100);
		break;

		case (Five):
			led->brightnessState = One;
			ledSetBrightness(led, 0);
		break;

		default:
			led->brightnessState = Default;
			ledSetBrightness(led, 0);
		break;
	}
}

void ledPulse(Led* led)
{
	pwmPulse(led->pwm, led->brightness);
}

void led_create(Led* led)
{
	led->pwm = pwmCreate(led->pin, led->frequency);
	led->basePio = &led->pwm.pio;
}