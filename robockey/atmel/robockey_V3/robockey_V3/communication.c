/*
 * communication.c
 *
 */ 

#include "communication.h"


void send_message(int state, int num, char *buffer, float *c_wii){
	switch(state){ //buffer = {A9, No, X_L, X_H, Y_L, Y_H, ...}
		case 2:	
			buffer[0] = 0xB1;
			buffer[1] = num;
			buffer[2] = (unsigned int) c_wii[0] % 256;
			buffer[3] = (unsigned int) c_wii[0] / 256;
			buffer[4] = (unsigned int) c_wii[1] % 256;
			buffer[5] = (unsigned int) c_wii[1] / 256;
			m_rf_send(TXADDRESS1,buffer,PACKET_LENGTH);
			m_wait(10);
			m_rf_send(TXADDRESS2,buffer,PACKET_LENGTH);
			usb_tx_string("Done \n");
			break;
		case 3:  
			buffer[0] = 0xB2;
			buffer[1] = num;
			buffer[2] = (unsigned int) c_wii[0] % 256;
			buffer[3] = (unsigned int) c_wii[0] / 256;
			buffer[4] = (unsigned int) c_wii[1] % 256;
			buffer[5] = (unsigned int) c_wii[1] / 256;
			m_rf_send(TXADDRESS1,buffer,PACKET_LENGTH);
			m_wait(10);
			m_rf_send(TXADDRESS2,buffer,PACKET_LENGTH);
			usb_tx_string("Done \n");
			break;
		default:
			break;
	}
}