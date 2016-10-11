/*
 * communication.c
 *
 * Created: 2015/11/13 18:39:55
 *  Author: Liu Yilin
 */ 


#include <avr/io.h>
#include "m_general.h"
#include "m_wii.h"
#include "m_usb.h"

#define CHANNEL 1
#define RXADDRESS1 0x0C
#define TXADDRESS1 0x0D
#define TXADDRESS2 0x0E
#define PACKET_LENGTH 10

char buffer[PACKET_LENGTH] = {0,0,0,0,0,0,0,0,0,0};
int scoreR = 0;
int scoreB = 0;
int state = 0;
int color = 0; // 0 = red; 1 = blue


int main(void)
{
	m_clockdivide(0);
	
	m_red(ON);
	m_usb_init();
	while(!m_usb_isconnected());
	m_red(OFF);
	
	m_green(ON);
	m_bus_init(); // enable mBUS
	m_rf_open(CHANNEL,RXADDRESS,PACKET_LENGTH); // configure mRF
	m_green(OFF);
	
	
	
    while(1)
    {
		switch(state){
			case 0:
						// stop (shut down PWM)
				break;
			case 1:		// comm test, flash a LED
				toggle(PORTB, 6);	// Toggle B6 
				m_wait(16);	// wait for 1ms
				break;
			case 2:		// play mode
				set(PORTB, 6);
				// do other thing
				break;
		}
    }
}

ISR(INT2_vect){
	m_rf_read(buffer,PACKET_LENGTH);	// pull the packet
	m_green(TOGGLE);
	switch(buffer[0]){
		case 0xA0:
			usb_tx_string("flah LED \n");
			state = 1;
			break;
		case 0xA1:
			usb_tx_string("Play \n");
			state = 2;
			break;
		case 0xA2:
			usb_tx_string("Goal R\n");
			scoreR = buffer[1];
			scoreB = buffer[2];
			break;
		case 0xA3:
			usb_tx_string("Goal B\n");
			scoreR = buffer[1];
			scoreB = buffer[2];
			break;
		case 0xA4:
			usb_tx_string("Pause\n");
			state = 0;
			break;
		case 0xA6:
			usb_tx_string("Halftime\n");
			state = 0;
			break;
		case 0xA7:
			usb_tx_string("Game over\n");
			state = 0;
			break;
		default:
			break;
	}
}