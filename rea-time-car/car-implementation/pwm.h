#ifndef _PWM_H
#define _PWM_H

#include "piodirect.h"

typedef struct PWM_
{
	GPIO pio;
	unsigned int frequency;
} PWM;

PWM pwmCreate(enum Pin pin, unsigned int frequency);

int pwmPulse(PWM pwm, float dutyPercent);

void pwmDestroy(PWM pwm);

#endif  