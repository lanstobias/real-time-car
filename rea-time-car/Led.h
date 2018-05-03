#ifndef LED_H
#define LED_H
#include "piodirect.h"
#include "pwm.h"

#define ON 1
#define OFF 0

typedef enum BrightnessState
{
	Default,
	One,
	Two,
	Three,
	Four,
	Five
} BrightnessState;

typedef struct Led
{
	float brightness;	
	int state;
	int frequency;
	enum Pin pin;
	BrightnessState brightnessState;
	PWM pwm;
	GPIO* basePio;
} Led;

// Constructors
void ledInit(Led* led, enum Pin pin);

// Methods
void ledToggle(Led* led);
void ledTurnOff(Led* led);
void ledTurnOn(Led* led);
void ledDestroy(Led* led);
void ledSetBrightness(Led* led, float brightnessLevel);
void ledIncrementBrightnessStep(Led* led);
void ledPulse(Led* led);

void led_create(Led* led);

#endif