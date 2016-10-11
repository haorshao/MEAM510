/*
 * robockey_V2.c
 *
 * Created: 2015/11/30 9:45:46
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


char buffer[PACKET_LENGTH] = {0,0,0,0,0,0,0,0,0,0};


int scoreR = 0;
int scoreB = 0;
int state;

float c_wii[2] = {0, 0};
float dir[2] = {0, 0};
float goal[2] = {GateA, 0};  // default right gate, Goal A
float desPos[2] = {0,0};
	
int puck_holder = 0;
int puck_loc[2] = {0, 0};

void init_timer(void);

int main(void)
{
	m_clockdivide(0);
	
	m_usb_init();
	
	//m_wii_open();
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
	set(DDRB, 2);
	set(DDRB, 3);
	state = 2;
	while(1)
	{	
		if(check(PINC, 6)){
			set(PORTB, 2); // blue Red
			clear(PORTB, 3);
			goal[0] = GateB;  // left gate
		}
		else{
			set(PORTB, 3); // red blue
			clear(PORTB, 2);
			goal[0] = GateA;  // right gate
		}
		
		m_usb_tx_string("state: ");
		m_usb_tx_int(state);
		m_usb_tx_string("\n");
		switch(state){
			case 0:
				// stop
				OCR1B = 0;
				OCR1C = 0;
				break;
			case 1:		// comm test, flash a LED
				if(check(PINC, 6)){
					toggle(PORTB, 2); // blue Red
					clear(PORTB, 3);
				}
				else{
					toggle(PORTB, 3); // red blue
					clear(PORTB, 2);
				}
				m_red(TOGGLE);
				m_wait(1000);	// wait
				break;
			case 2:		// play mode, find puck
				m_red(TOGGLE);
				m_green(OFF);
				findPuck(state);
				localization(state, &goal[0], &c_wii[0], &dir[0]);
				deterState(&state);
				break;
			case 3:		// got the puck, find goal
				m_green(TOGGLE);
				m_red(OFF);
				if(c_wii[1] < - yline){
					goal[1] = -yline;
				}
				else{
					if(c_wii[1] > yline){
						goal[1] = yline;
					}
					else{
						goal[1] = c_wii[1];
					}
				}
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
			/*
			case 4:		//can't detect the puck, back for defense
				m_green(TOGGLE);
				m_red(TOGGLE);
				localization(state, &desPos[0], &c_wii[0], &dir[0]);
				m_usb_tx_string("Heading back\n");;
				deterState(&state);
				m_usb_tx_string("desPos = ");
				m_usb_tx_int(desPos[0]);
				m_usb_tx_string(", ");
				m_usb_tx_int(desPos[1]);
				m_usb_tx_string("\n");
			*/
 			default:
 				break;
		}
		
		m_usb_tx_string("1B, 1C: ");
		m_usb_tx_int(OCR1B);
		m_usb_tx_string(", ");
		m_usb_tx_int(OCR1C);
		m_usb_tx_string("\n");
		m_wait(15);		// 500ms for read putty, 50ms for actual running
		
	}
}

ISR(INT2_vect){
	m_rf_read(buffer,PACKET_LENGTH);	// pull the packet
	switch(buffer[0]){
		case 0xA0:
			usb_tx_string("flash LED \n");
			state = 1;
			break;
		case 0xA1:
			m_usb_tx_string("Play \n");
			state = 2; 
			break;
		case 0xA2:
			m_usb_tx_string("Goal A\n");
			scoreR = buffer[1];
			scoreB = buffer[2];
			break;
		case 0xA3:
			m_usb_tx_string("Goal B\n");
			scoreR = buffer[1];
			scoreB = buffer[2];
			break;
		case 0xA4:
			m_usb_tx_string("Pause\n");
			state = 0;
			break;
		case 0xA6:
			m_usb_tx_string("Halftime\n");
			state = 0;
			break;
		case 0xA7:
			m_usb_tx_string("Game over\n");
			state = 0;
			break;
		case 0xA9: // teammates got the puck, extract No. and location
			puck_holder = buffer[1]; // buffer = {A9, No, X_L, X_H, Y_L, Y_H, ...}
			puck_loc[0] = *(int*)&buffer[2];
			puck_loc[1] = *(int*)&buffer[4];
			m_usb_tx_string("No. ");		
			m_usb_tx_int(puck_holder);
			m_usb_tx_string("got the puck!\n");
			m_usb_tx_string("Its location is: ");
			m_usb_tx_int(puck_loc[0]);
			m_usb_tx_string("		y:");
			m_usb_tx_int(puck_loc[1]);
			m_usb_tx_string("\n");
			break;
		default:
			break;
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

	