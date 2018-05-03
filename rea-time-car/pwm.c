#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "piodirect.h"
#include "pwm.h"

PWM pwmCreate(enum Pin pin, unsigned int frequency)
{
	PWM pwm;
	pwm.pio = create(pin, OUT_PIN);
	pwm.frequency = frequency;

	return pwm;
}

int pwmPulse(PWM pwm, float dutyPercent)
{
	if (dutyPercent < 0.0 || dutyPercent > 100.0)
	{
		return EXIT_FAILURE;
	}

	struct timespec onTimeSpec, offTimeSpec, remainingTime;
	double pulseLength = ((1.0 / pwm.frequency) * 1000000000.0);

	double onTime = (pulseLength * (dutyPercent / 100.0));
	double offTime = (pulseLength * (100.0 - dutyPercent) / 100.0);

	onTimeSpec.tv_nsec = onTime;
	onTimeSpec.tv_sec = 0;
	offTimeSpec.tv_nsec = offTime;
	offTimeSpec.tv_sec = 0;
	
	onOff(pwm.pio, ON);
	clock_nanosleep(CLOCK_REALTIME, 0, &onTimeSpec, &remainingTime);

	onOff(pwm.pio, OFF);
	clock_nanosleep(CLOCK_REALTIME, 0, &offTimeSpec, &remainingTime);

	return EXIT_SUCCESS;
}

void pwmDestroy(PWM pwm)
{
	destroy(pwm.pio);
}