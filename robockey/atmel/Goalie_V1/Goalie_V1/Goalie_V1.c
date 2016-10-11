/*
 * Goalie_V1.c
 *
 * Created: 2015/12/5 23:08:44
 *  Author: Liu Yilin
 */ 


#include <avr/io.h>
#include "m_general.h"
#include "m_usb.h"
#include "m_bus.h"
#include "m_rf.h"

#include "common_parm.h"
#include "localization.h"
#include "findPuck.h"
#include "communication.h"

#define wait 15

char buffer[PACKET_LENGTH] = {0,0,0,0,0,0,0,0,0,0};


int scoreR = 0;
int scoreB = 0;
int state;

float c_wii[2] = {0, 0};
float dir[2] = {0, 0};
float goal[2] = {GateA, 0};  // default right gate, Goal A
float desPos[2] = {xline,0};	// default right gate defense line
	
int puck_holder = 0;
int puck_loc[2] = {0, 0};

void init_timer(void);

int main(void)
{
	m_clockdivide(0);
	
	m_usb_init();
	
	m_red(ON);
	while(!m_wii_open());
	m_red(OFF);
	
	m_red(ON);
	m_bus_init(); // enable mBUS
	m_rf_open(CHANNEL,RXADDRESS,PACKET_LENGTH); // configure mRF
	m_red(OFF);
	
	init_timer();
	init_ADC();
	
	clear(DDRC, 6); // Read a switch value to decide which team
	clear(DDRC, 7);
	clear(PORTC,7);
	set(DDRB, 2);
	set(DDRB, 3);
	state = 5;
	
	if(check(PINC, 6)){
		set(PORTB, 2); // red light
		clear(PORTB, 3);
		goal[0] = GateB;  // left gate
	}
	else{
		set(PORTB, 3); // blue light
		clear(PORTB, 2);
		goal[0] = GateA;  // right gate
	}
	while(1)
	{	
		m_usb_tx_string("state: ");
		m_usb_tx_int(state);
		m_usb_tx_string("\n");
		switch(state){
			case 2:		// attack mode, find puck
				m_red(TOGGLE);
				m_green(OFF);
				findPuck(state);
				localization(state, &goal[0], &c_wii[0], &dir[0]);
				deterState(&state);
				break;
			case 3:		// got the puck, find goal
				m_green(TOGGLE);
				m_red(OFF);
				localization(state, &goal[0], &c_wii[0], &dir[0]);
				send_message(state, No, &buffer[0], &c_wii[0]);
				m_usb_tx_string("sent messsage.\n");
				deterState(&state);
				m_usb_tx_string("goal = ");
				m_usb_tx_int(goal[0]);
				m_usb_tx_string(", ");
				m_usb_tx_int(goal[1]);
				m_usb_tx_string("\n");
				break;
			case 5:		// defense mode
				m_red(TOGGLE);
				m_green(OFF);
				findPuck(state);
				localization(state, &goal[0], &c_wii[0], &dir[0]);
				ifGotoPuck(&state, &dir[0]);
				break;
 			default:
 				break;
		}
		
		m_usb_tx_string("1B, 1C: ");
		m_usb_tx_int(OCR1B);
		m_usb_tx_string(", ");
		m_usb_tx_int(OCR1C);
		m_usb_tx_string("\n");
		m_wait(wait);		
		
	}
}

void init_timer(void){
	// Direction pin B0 for motor 1, B1 for motor 2
	set(DDRB, 0);
	set(PORTB, 0);
	
	set(DDRB, 1);
	set(PORTB, 1);
	
	// Initialize timer 1
	OCR1A = 800;
	OCR1B = 0;
	OCR1C = 0;
	
	// Set timer 1 modes as up to OCR1A, PWM mode (mode 15)
	set(TCCR1B, WGM13);
	set(TCCR1B, WGM12);
	set(TCCR1A, WGM11);
	set(TCCR1A, WGM10);

	// Set to clear at OCR1B, set at OCR1A
	set(TCCR1A, COM1B1);
	clear(TCCR1A, COM1B0);
	
	// Set to clear at OCR1C, set at OCR1A
	set(TCCR1A, COM1C1);
	clear(TCCR1A, COM1C0);

	// Set B6, B7 output
	set(DDRB, 6);
	set(DDRB, 7);
	
	// Turn off timer first
	clear(TCCR1B, CS12);
	clear(TCCR1B, CS11);
	set(TCCR1B, CS10);
}

	