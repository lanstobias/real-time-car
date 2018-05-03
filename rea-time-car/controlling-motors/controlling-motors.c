#include <stdio.h>
#include "Engine.h"

int main(int argc, char *argv[])
{
	gpioSetup();

	Engine engineLeft, engineRight;
	engineInit(&engineRight, _6, _16, _20);
	engineInit(&engineLeft, _23, _24, _25);

	for (int i = 0; i < 180; i++)
	{
		engineRun(&engineLeft, 100, Forward);
		engineRun(&engineRight, 100, Forward);
	}

	engineDestroy(&engineLeft);
	engineDestroy(&engineRight);
	
	fflush(stdout); /* <============== Put a breakpoint here */
	return 0;
}