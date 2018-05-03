/*
* File        : piodirect.h
* Description : Access GPIOs on Raspberry Pi model B+ with 
* direct register access. A few parts from Dom and Gert are reused.
* Author      : Farhang Nemati   March 2016
* 
*/

#ifndef _PIO_DIRECT_H
#define _PIO_DIRECT_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define IN_PIN 0
#define OUT_PIN 1

#define ON 1
#define OFF 0

#define PUD_OFF 0
#define PUD_DOWN 1
#define PUD_UP 2

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
 
#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)
 
int  mem_fd;
void *gpio_map;
 
// I/O access
volatile unsigned *gpio;
 
// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
 
#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
 
#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH
 
#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock
 
enum Pin
{			     //On Board
	_17   = 17,  //11
	_18   = 18,  //12
	_27   = 27,  //13
	_22   = 22,  //15
	_23   = 23,  //16
	_24   = 24,  //18
	_25   = 25,  //22
	_4   =  4,   //7
	_CE1  = 7,   //26
	_CE0  = 8,   //24
	_SCLK = 11,  //23
	_MISO = 9,   //21
	_MOSI = 10,  //19
	_RXD  = 15,  //10 
	_TXD  = 14,  //8
	_SCL  = 3,   //5
	_SDA  = 2,   //3
//	_EED  =  ,   //27 Advanced use!
//	_EEC  =  ,   //28 Advanced use!
	_5  =   5,   //29
	_6  =   6,   //31
	_12  =  12,  //32
	_13  =  13,  //33
	_19  =  19,  //35
	_16  =  16,  //36
	_26  =  26,  //37
	_20  =  20,  //38
	_21  =  21   //40
};

typedef struct GPIO_
{
	enum Pin pin;
	int direction;
} GPIO;

void gpioSetup();

GPIO create(enum Pin pin, int direction);

GPIO createWithPullUpDown(enum Pin pin, int up);

void destroy(GPIO gpio_);

int onOff(GPIO gpio_, int onOff);

int readIn(GPIO gpio_);

int pullUpDown(GPIO gpio_, int pud);

#endif   
