/*
 * haorshao2.c
 *
 * Created: 9/23/2015 9:41:59 PM
 *  Author: haorshao
 */ 


#include <avr/io.h>


#include "m_general.h"
#define PIN 6

int h;

int main()//Frequency is 5.5555HZ


{
	while(1)
	{ for(h=0;h<335;){h++;}
	
	toggle(PORTB,PIN);
	m_red(1);
	//TODO:: Please write your application code
}
}