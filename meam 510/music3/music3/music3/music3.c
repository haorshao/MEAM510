/*
 * music3.c
 *
 * Created: 10/10/2015 1:26:14 PM
 *  Author: haorshao
 */ 


#include <avr/io.h>
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include <math.h>
double sin(double __x);

#define channel  1
#define RXADDRESS  0x47
#define PACKET_LENGTH 3
#define pi 3.1415926

volatile char buffer[3] = {0, 0, 0};
volatile int frequency;
volatile int realFrequency;
volatile int countsine=0;
volatile int realDuration;
volatile int count=0;
volatile unsigned int value;

int main(void)
{
	m_clockdivide(3);//set the system clock frequency as 500KHz
	m_usb_init();
	
	m_bus_init(); // enable mBUS
	m_rf_open(channel,RXADDRESS,PACKET_LENGTH); // configure mRF
	
	clear(TCCR1B,CS12);//set prescaler to /64
	set(TCCR1B,CS11);
	set(TCCR1B,CS10);
	
	//OCR1A=2000000/(64*frequency);//set the value of OCR1A as 100
	//OCR1B=OCR1A/2

	set(TCCR1B,WGM13);//(mode 15) UP to OCR1A, PWM mode
	set(TCCR1B,WGM12);
	set(TCCR1A,WGM11);
	set(TCCR1A,WGM10);
	set(TCCR1A,COM1B1);//set at OCR1B, clear at rollover
	set(TCCR1A,COM1B0);
	
	set(TIMSK1,OCIE1A);//call an interrupt whenever (TCNT1 matches OCR1A)
	sei();//enable global interrupt
	
	set(TCCR0B,WGM02); // MODE: up to OCR0A
	set(TCCR0A,WGM01); // ^
	set(TCCR0A,WGM00); // ^
	
	set(TCCR0A,COM0B1); // clear at OCR0B, set at OCR0A
	set(TCCR0A,COM0B0); // ^
	
	set(TCCR0B,CS02); // set prescaler to /1024
	clear(TCCR0B,CS01); // ^
	set(TCCR0B,CS00); // ^
	
	set(TIMSK0,OCIE0A);
	
	OCR0B=10;
	
	set(DDRB,6);
	
	
	
	while(1){
		value=m_usb_tx_char(frequency);
		m_usb_tx_uint(value);
		
		
		if(count>195){
			OCR1A=0;
			count=0;
			}
		if(countsine<OCR1A){
			OCR1B=OCR1A/2.0+sin(countsine/OCR1A*2*pi)*OCR1A/2.0;//set the value of OCR1B
			}else{
			countsine=0;
		}
		

		
		
		
		
		
		//        OCR1A=520;//set the value of OCR1A as 100
		// 	   OCR1B=OCR1A/2;

	}
	
	
}


ISR(INT2_vect){
	m_rf_read(buffer,PACKET_LENGTH);
	frequency= buffer[0]+256*buffer[1];
	realFrequency=frequency/10;
	OCR1A=2000000/(64*realFrequency);
	realDuration=buffer[2]/100;



	
	
	
	
	m_green(TOGGLE);
}
ISR(TIMER1_COMPA_vect){
    countsine++;
}

ISR(TIMER0_COMPA_vect){
	count++;
}