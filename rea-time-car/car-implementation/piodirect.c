/*
* File        : piodirect.h
* Description : Access GPIOs on Raspberry Pi model B+ with 
* direct register access. Function setup_io() from Dom and Gert is reused.
* Author      : Farhang Nemati   March 2016
* 
*/

#include <time.h>
#include "piodirect.h"

extern int nanosleep(const struct timespec *, struct timespec *);

static void setup_io();
 
/* Set up a memory regions to access GPIO*/
static void setup_io()
{
   /* open /dev/mem */
	if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		printf("can't open /dev/mem \n");
		exit(-1);
	}
 
    /* mmap GPIO */
	gpio_map = mmap(
	    NULL,             //Any adddress in our space will do
		BLOCK_SIZE,       //Map length
		PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
		MAP_SHARED,       //Shared with other processes
		mem_fd,           //File to map
		GPIO_BASE         //Offset to GPIO peripheral
	);
	
	close(mem_fd); //No need to keep mem_fd open after mmap
 
	if (gpio_map == MAP_FAILED) {
		printf("mmap error %d\n", (int)gpio_map);//errno also set!
		exit(-1);
	}
 
	   // Always use volatile pointer!
	gpio = (volatile unsigned *)gpio_map;
 
} // setup_io
	
void gpioSetup()
{
	setup_io();
}

GPIO create(enum Pin pin, int direction)
{
	GPIO gpio_;
	
	gpio_.pin = pin;
	gpio_.direction = direction;
	
	INP_GPIO(pin);
	
	if (direction == OUT_PIN)
	{
		OUT_GPIO(pin);
	}
	
	return gpio_;
}

GPIO createWithPullUpDown(enum Pin pin, int pud)
{
	GPIO gpio_ = create(pin, IN_PIN);
	
	pullUpDown(gpio_, pud);
	
	return gpio_;
}

int onOff(GPIO gpio_, int onOff)
{
	if (gpio_.direction == OUT_PIN)
	{
		if (onOff == ON)
		{
			GPIO_SET = 1 << gpio_.pin;
		}
		else
		{
			GPIO_CLR = 1 << gpio_.pin;
		}
		return 0 ;
	}
	
	return -1 ;
}

int readIn(GPIO gpio_)
{
	if (gpio_.direction == IN_PIN)
	{
		if (GET_GPIO(gpio_.pin) == 0)
		{
			return 0;
		}
		
		/*if the value == 1*/
		return 1;
	}
	
	return -1 ;
}

int pullUpDown(GPIO gpio_, int pud)
{
	if (gpio_.direction == IN_PIN)
	{
		struct timespec ts1, ts2;
		ts1.tv_sec = 0;
		ts1.tv_nsec = 5000;
		
		GPIO_PULL = pud & 3; /*0 = off, 1 down, 2 up*/
		nanosleep(&ts1, &ts2);
		GPIO_PULLCLK0 = 1 << (gpio_.pin & 31);
		nanosleep(&ts1, &ts2);
		GPIO_PULL = 0;
		GPIO_PULLCLK0 = 0;
		
		return 0;
	}
		
	return -1;
}

void destroy(GPIO gpio_)
{
	onOff(gpio_, 0);
	INP_GPIO(gpio_.pin);
}
	                