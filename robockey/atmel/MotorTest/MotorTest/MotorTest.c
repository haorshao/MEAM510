/*
 * MotorTest.c
 *
 * Created: 2015/11/14 19:32:55
 *  Author: Liu Yilin
 */ 
#include <avr/io.h>
#include "m_general.h"
#include "m_usb.h"

float speed;
char ichar;
void init(void);
char dummy;
int main(void)
{
	m_clockdivide(0);
	m_green(ON);
	m_red(ON);
	m_usb_init();
	while(!m_usb_isconnected());
	m_red(OFF);
	
	m_usb_tx_string("Press any key to keep reading data \n");
	while(!m_usb_rx_available());
	m_usb_rx_flush();
	
	init();

	while(1){
		m_usb_tx_string("M1:type '1' as increase 10%, '2' as decrease 10%, '3' as reverse direction \n");
		//m_usb_tx_string("M2:type '4' as increase 10%, '5' as decrease 10%, '6' as reverse direction \n");
		while(!m_usb_rx_available());
		ichar = m_usb_rx_char();
		switch(ichar){
			case '1':
				OCR1B = OCR1B + 25;
				break;
			case '2':
				OCR1B = OCR1B - 25;
				break;
			case '3':
				toggle(PORTB, 0);
				break;
			case 'a':
				OCR0B = OCR0B + 25;
				break;
			case 's':
				OCR0B = OCR0B - 25;
				break;
			case 'd':
				toggle(PORTB, 1);
				break;
			default:
				break;
		}
		m_usb_tx_string("M1 OCR1B:");
		m_usb_tx_int(OCR1B);
		m_usb_tx_string("	  M2 OCR0B:");
		m_usb_tx_int(OCR0B);
		m_usb_tx_string("\n");
		m_usb_tx_int(PORTB);
		m_usb_tx_string("	Direction M1: ");
		if(check(PORTB, 0)){
			m_usb_tx_string(" Forward ");
			
		}
		else{
			m_usb_tx_string("Inverse ");
		}
		
		m_usb_tx_string("	  Direction M2:");
		if(check(PORTB,1)){
			m_usb_tx_string("Forward \n");
		}
		else{
			m_usb_tx_string("Inverse \n");
		}
		
	}
}

void init(void){
	// Direction pin B0 for motor 1, B1 for motor 2
	set(DDRB, 0);
	set(PORTB, 0);
	
	set(DDRB, 1);
	set(PORTB, 1);
	
	// Initialize timer 1
	OCR1A = 250;
	OCR1B = 125;	
	
	// Set timer 1 modes as up to OCR1A, PWM mode (mode 15)
	set(TCCR1B, WGM13);
	set(TCCR1B, WGM12);
	set(TCCR1A, WGM11);
	set(TCCR1A, WGM10);
	
	// Set timer modes as up to OCR3A, PWM mode (mode 15)
	set(TCCR1B, WGM13);
	set(TCCR1B, WGM12);
	set(TCCR1A, WGM11);
	set(TCCR1A, WGM10);

	// Set to clear at OCR1B, set at OCR1A
	set(TCCR1A, COM1B1);
	clear(TCCR1A, COM1B0);

	// Set B6 output
	set(DDRB, 6);
	
	// Set prescaler as /1
	clear(TCCR1B, CS12);
	clear(TCCR1B, CS11);
	set(TCCR1B, CS10);
	
	
	// Initialize timer 0
	OCR0A = 250;
	OCR0B = 125;
	
	// Set timer 0 modes as up to OCR0A, PWM mode 
	set(TCCR0B, WGM02);
	set(TCCR0A, WGM01);
	set(TCCR0A, WGM00);

	// Set to clear at OCR0B, set at OCR0A
	set(TCCR0A, COM0B1);
	clear(TCCR0A, COM0B0);

	// Set D0 output
	set(DDRD, 0);
	
	// Set prescaler as /1
	clear(TCCR0B, CS02);
	clear(TCCR0B, CS01);
	set(TCCR0B, CS00);
	
}