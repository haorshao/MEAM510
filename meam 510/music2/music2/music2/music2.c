/*
 * music2.c
 *
 * Created: 10/9/2015 5:47:47 PM
 *  Author: haorshao
 */ 


#include <avr/io.h>
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"

#define channel  1
#define RXADDRESS  0x47
#define PACKET_LENGTH 3
#define pi 3.1415926

char buffer[3] = {0, 0, 0};
volatile int frequency;
volatile int realFrequency;
volatile float i;
volatile int realDuration;
volatile float dutyCycle;
int main(void)
{
  m_clockdivide(3);//set the system clock frequency as 2MHz
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
  
  set(DDRB,6);
  
  
  
  
  
	
	
    while(1){
		
	   
//        OCR1A=520;//set the value of OCR1A as 100
// 	   OCR1B=OCR1A/2;

	    }
		
        
    }


ISR(INT2_vect){
	m_rf_read(buffer,PACKET_LENGTH); // pull the packet
	
	frequency= buffer[0]+256*buffer[1];
	realFrequency=frequency/10;
	OCR1A=2000000/(64*realFrequency);
// 	realDuration=buffer[2]/100;
// 	dutyCycle=realDuration*realFrequency;
    OCR1B=OCR1A/2;
	m_green(TOGGLE);
}
// ISR(TIMER1_COMPA_vect){
// 	i++;
// }
