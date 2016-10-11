/*
 * Motor_drive.c
 *
 * Created: 11/14/2015 7:25:09 PM
 *  Author: haorshao
 */ 


#include <avr/io.h>
#include "m_general.h"

int main(void)
{
	m_clockdivide(0);
	clear(TCCR0B,CS02);
    clear(TCCR0B,CS01);
	set(TCCR0B,CS00);
	
	set(TCCR0B,WGM02);
	set(TCCR0A,WGM01);
	set(TCCR0A,WGM00);
	
	set(TCCR0A,COM0B1);
	clear(TCCR0A,COM0B0);
	
	OCR0A=250;
	
	OCR0B=125;
	
	set(DDRD,0);
	
	
	
	
    while(1)
    {
		m_green(ON);
		
        //TODO:: Please write your application code 
    }
}