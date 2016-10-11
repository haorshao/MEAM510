/*
 * all_in_one.c
 *
 * Created: 2015/11/21 13:59:55
 *  Author: Liu Yilin
 */ 

#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include "m_general.h"
#include "m_wii.h"s
#include "m_usb.h"
#include "m_rf.h"
#include "m_bus.h"

#define CHANNEL 1
#define RXADDRESS 0x0C
#define TXADDRESS1 0x0D
#define TXADDRESS2 0x0E
#define PACKET_LENGTH 10

// -------------------------------
// parameter that you should modify
#define full_speed 150

#define Kp_loc 150
#define Ki_loc 0
#define Kd_loc 100

#define Kp_Pho 1
#define Ki_Pho 0
#define Kd_Pho 1

#define GateA 350
#define GateB -350

float camera_center[2] = {525, 410};
int dist_threshold = 200;
// -------------------------------


char buffer[PACKET_LENGTH] = {0,0,0,0,0,0,0,0,0,0};

int scoreR = 0;
int scoreB = 0;
int state;
// Variables for find puck (ADC)
int Pho[6]={0, 0, 0, 0, 0, 0};
int i=0;
int input;
int input_last=0;
int output;
int sum_Pho = 0;
int diff_Pho = 0;

// Variables for localization
unsigned int blobs[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
float c_wii[2]={0,0};
float ypdir[2]={0,0};
float c_wii_last[2]={0,0};
float ypdir_last[2]={0,0};
float goal[2] = {GateA, 0};  // default right gate, Goal A
float theta = 0;
float threshold = 2;
float pix2act = 29.0/85;
float actmax[3] = {29, 25.99, 23.08};


// Variables for find goal
float alpha = 0;
float alpha_last = 0;
float sum_loc = 0;
float diff_loc = 0;
float gain_loc = 0;

void init_timer(void);
// Functions for finding the puck
void init_ADC(void);
void PID_Pho(float Kp, float Ki, float Kd);
void findPuck(void);

// Functions for localization and finding goal
void localization(void);
int find_sign(float costheta, float sintheta, float *ref_vc, float *ref_vw);
void cal_angle(int if_pos_sign, float sintheta, float costheta);
void PID_alpha(float Kp, float Ki, float Kd);
int find_if_reach(float x, float y);

int main(void)
{	
	m_clockdivide(0);
	
	m_usb_init();
	
	m_green(ON);
	while(!m_wii_open());
	m_green(OFF);

	m_bus_init(); // enable mBUS
	m_rf_open(CHANNEL,RXADDRESS,PACKET_LENGTH); // configure mRF
	
	init_timer();
	init_ADC();
	
	// clear(DDRC, 6); // Read a switch value to decide which team
	// set(DDRF, 4);
	// ser(DDRF, 5);
	state = 0;
	while(1)
    {	
		/*
		if(check(DDRC, 6)){
			set(DDRF, 4); // light Red
			clear(DDRF, 5); 
			goal[0] = 300;  // right gate
			goal[1] = 0;
		}
		else{
			set(DDRF, 4); // light Red
			clear(DDRF, 5);		
			goal[1] = -300;  // left gate
			goal[0] = 0;
		}
		*/
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
				//toggle(PORTF, 4);	// Toggle F4
				//toggle(PORTF, 5);	// Toggle F5
				m_red(TOGGLE);	// Toggle red led for qualify
				m_wait(1000);	// wait
				break;
			case 2:		// play mode, find puck
				m_red(TOGGLE);
				//findPuck(); //comment out for qualify test
				//localization();
				//if (Pho[4] + Pho[5] > 800){
				//	state = 3;
				//}
				state = 3;
				break;
			case 3:		// got the puck, find goal
				m_green(TOGGLE);
				m_wii_read(&blobs[0]);
				localization();
				/* comment out for qualify, ignore ADC value
				if (Pho[4] <= 300 && Pho[4] <= 300){
					state = 2;
					m_wii_read(&blobs[0]);
					m_wii_read(&blobs[0]);
					m_wii_read(&blobs[0]);
				}
				*/
				break;
		}
		
		m_usb_tx_string("Position x:");
		m_usb_tx_int(c_wii[0]);
		m_usb_tx_string("		y:");
		m_usb_tx_int(c_wii[1]);
		m_usb_tx_string("\n");
		
		m_usb_tx_string("Direction vx: 0.");
		m_usb_tx_int(ypdir[0]*1000);
		m_usb_tx_string("	vy: 0.");
		m_usb_tx_int(ypdir[1]*1000);
		m_usb_tx_string("\n");
		
		m_usb_tx_string("1B, 1C: ");
		m_usb_tx_int(OCR1B);
		m_usb_tx_string(", ");
		m_usb_tx_int(OCR1C);
		m_usb_tx_string("\n");
		m_wait(500);
		

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
			usb_tx_string("Play \n");
			state = 3; // should be 2, but set to 3 for qualify
			// Turn on timer, set prescaler as /1
			m_wii_read(&blobs[0]);
			m_wii_read(&blobs[0]);
			m_wii_read(&blobs[0]);
			break;
		case 0xA2:
			usb_tx_string("Goal A\n");
			scoreR = buffer[1];
			scoreB = buffer[2];
			goal[0] = GateA;
			goal[1] = 0;
			break;
		case 0xA3:
			usb_tx_string("Goal B\n");
			scoreR = buffer[1];
			scoreB = buffer[2];
			goal[0] = GateB;
			goal[1] = 0;
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

void init_timer(void){
	// Direction pin B0 for motor 1, B1 for motor 2
	set(DDRB, 0);
	set(PORTB, 0);
	
	set(DDRB, 1);
	set(PORTB, 1);
	
	// Initialize timer 1
	OCR1A = 250;
	OCR1B = full_speed;
	OCR1C = full_speed;
	
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

void init_ADC(){
	clear(ADMUX,REFS1);//set the voltage reference
	set(ADMUX,REFS0);

	set(ADCSRA,ADPS2);// set the ADC clock prescaler/128
	set(ADCSRA,ADPS1);
	set(ADCSRA,ADPS0);

	set(DIDR0,ADC0D);
	set(DIDR0,ADC1D);
	set(DIDR2,ADC9D);
	set(DIDR2,ADC10D);
	set(DIDR2,ADC11D);
	set(DIDR2,ADC12D);
	
	clear(ADCSRB,MUX5);//select F0 as analog input
	clear(ADMUX,MUX2);
	clear(ADMUX,MUX1);
	clear(ADMUX,MUX0);
	
	
	set(ADCSRA,ADATE); //triggering
	
	set(ADCSRA, ADIE);//interrupt
	sei();
	
	set(ADCSRA,ADEN);// enable conversions
	set(ADCSRA,ADSC);
}


void findPuck(void){
	m_usb_tx_string("\r\n");
	m_usb_tx_string("Pho[0]");
	m_usb_tx_int(Pho[0]);
	m_usb_tx_string("\r\n");
	m_usb_tx_string("Pho[1]");
	m_usb_tx_int(Pho[1]);
	m_usb_tx_string("\r\n");
	m_usb_tx_string("Pho[2]");
	m_usb_tx_int(Pho[2]);
	m_usb_tx_string("\r\n");
	m_usb_tx_string("Pho[3]");
	m_usb_tx_int(Pho[3]);
	m_usb_tx_string("\r\n");
	m_usb_tx_string("Pho[4]");
	m_usb_tx_int(Pho[4]);
	m_usb_tx_string("\r\n");
	m_usb_tx_string("Pho[5]");
	m_usb_tx_int(Pho[5]);
	m_usb_tx_string("\r\n");
	int mean02 = (Pho[0] + Pho[2])/2;
	int mean13 = (Pho[1] + Pho[3])/2;
	
	if(mean02 - mean13> 50 || (mean02 > mean13 && Pho[2] > Pho[0]) ){
		m_usb_tx_string("turn left\n");
		OCR1C = 50;
		OCR1B = full_speed;
		if(Pho[0]>Pho[2]){
			m_usb_tx_string("PID\n");
			PID_Pho(Kp_Pho, Ki_Pho, Kd_Pho);
		}
	}
	
	else {
		if (mean13 - mean02> 50 || (mean13 > mean02 && Pho[3] > Pho[1])){
			m_usb_tx_string("turn right\n");
			OCR1C = full_speed;
			OCR1B = 50;
			if(Pho[1]>Pho[3]){
				m_usb_tx_string("PID\n");
				PID_Pho(Kp_Pho, Ki_Pho, Kd_Pho);
			}
		}
		else{
			OCR1B = full_speed;
			OCR1C = full_speed;
		}
	}
}

void PID_Pho(float Kp, float Ki, float Kd) {
	input = Pho[0]-Pho[1];
	m_usb_tx_string("Input=");
	m_usb_tx_int(input);
	m_usb_tx_string("\r\n");
	diff_Pho = input - input_last;
	sum_Pho = sum_Pho + input;
	m_usb_tx_string("sum=");
	m_usb_tx_int(sum_Pho);
	m_usb_tx_string("\r\n");

	output = Kp * input + Ki*sum_Pho + Kd * diff_Pho;
	m_usb_tx_string("output=");
	m_usb_tx_int(output);
	m_usb_tx_string("\r\n");
	
	input_last = input;
	
	if(output>0){
		OCR1C=full_speed-abs(output);
		if (abs(output) > 100){
			OCR1C = 50;
		}
		OCR1B=full_speed;
		
	}
	else{
		OCR1B=full_speed-abs(output);
		if (abs(output) > 100){
			OCR1B = 50;
		}
		OCR1C=full_speed;
	}
}

ISR(ADC_vect){	
	Pho[i]=ADC;
	
	i=i+1;
	if(i>6){
		i=0;
	}
	
	clear(ADCSRA, ADEN);
	switch(i){
		case 0:
			clear(ADCSRB,MUX5);//select F0 as analog input
			clear(ADMUX,MUX2);
			clear(ADMUX,MUX1);
			clear(ADMUX,MUX0);
			break;
		case 1:
			clear(ADCSRB,MUX5);//select F1 as analog input
			clear(ADMUX,MUX2);
			clear(ADMUX,MUX1);
			set(ADMUX,MUX0);
			break;
		case 2:
			set(ADCSRB,MUX5);//select D6 as analog input
			clear(ADMUX,MUX2);
			clear(ADMUX,MUX1);
			set(ADMUX,MUX0);
			break;
		case 3:
			set(ADCSRB,MUX5);//select D7 as analog input
			clear(ADMUX,MUX2);
			set(ADMUX,MUX1);
			clear(ADMUX,MUX0);
			break;
		case 4:
			set(ADCSRB,MUX5);//select B4 as analog input
			clear(ADMUX,MUX2);
			set(ADMUX,MUX1);
			set(ADMUX,MUX0);
			break;
		case 5:
			set(ADCSRB,MUX5);//select B5 as analog input
			set(ADMUX,MUX2);
			clear(ADMUX,MUX1);
			clear(ADMUX,MUX0);
			break;
	}
	set(ADCSRA, ADEN);
	set(ADCSRA,ADSC);
	
}

void localization(void){
	//unsigned int data[8] = {740, 272, 777, 282, 715, 311, 755, 356};
	//float x[4] = {data[0], data[2], data[4], data[6]};
	//float y[4] = {data[1], data[3], data[5], data[7]};
	float x[4] = {blobs[0], blobs[3], blobs[6], blobs[9]};
	float y[4] = {blobs[1], blobs[4], blobs[7], blobs[10]};
	float d[6];
	int from[6], to[6];
	int ind = 0;
	int i, j;
	
	for (i=0; i<3; i++){
		for (j=i+1; j<4; j++){
			if(x[i] != 1023 && y[i] != 1023 && x[j] != 1023 && y[j] != 1023)
			d[ind] = sqrt((x[i] - x[j])*(x[i] - x[j]) + (y[i] - y[j])*(y[i] - y[j]));
			from[ind] = i;
			to[ind] = j;
			ind += 1;
		}
	}
	
	// find maximum
	int dmax = 0;
	int indmax = 0;
	for (i=0; i<ind; i++){
		if(d[i] > dmax){
			indmax = i;
			dmax = d[i];
		}
	}
	
	// find minimum
	int dmin = 10000;
	int indmin = 0;
	for (i=0; i<ind; i++){
		if(d[i] < dmin){
			indmin = i;
			dmin = d[i];
		}
	}

	// find miss which point
	double actdmax = dmax * pix2act;
	int ca = 0;
	for(i = 1; i<4; i++){
		if(abs(actdmax - actmax[i-1])<threshold){
			ca = i;
		}
	}
	
	float ref_pw[2] = {0, 0};
	float ref_vw[2] = {0, 1};
	
	
	switch(ca){
		case 1: // not miss point 1 or point 4
			// ref_pw[0] = 0; ref_pw[1] = -14.5 / pix2act;
			ref_pw[0] = 0; ref_pw[1] = 0;
			ref_vw[0] = 0; ref_vw[1] = 1;
			break;	
		case 2: // miss point 4
			ref_pw[0] = 0; ref_pw[1] = 14.5 / pix2act;
			ref_vw[0] = 0.4483; ref_vw[1] = -0.8939;
			break;		
		case 3: // miss point 1
			ref_pw[0] = 11.655 / pix2act; ref_pw[1] = -8.741 / pix2act;
			ref_vw[0] = -0.9625; ref_vw[1] = 0.2711;
			break;
		default:
			break;
	}
	
	if (ca > 0){
		// find coincide point and the other point of longest edge
		int coin_ind;
		int other_ind;
		if( from[indmin] == from[indmax] || to[indmin] == from[indmax]){
			coin_ind = from[indmax];
			other_ind = to[indmax];
		}
		else{
			coin_ind = to[indmax];
			other_ind = from[indmax];
		}
		
		
		// calculate reference point and vector in camera frame
		float ref_pc[2] = {0,0};
		if (ca == 1){
			ref_pc[0] = 0.5*(x[coin_ind] + x[other_ind]);
			ref_pc[1] = 0.5*(y[coin_ind] + y[other_ind]);
		}
		else{
			ref_pc[0] = x[coin_ind];
			ref_pc[1] = y[coin_ind];
		}
		
		float ref_vc[2] = {x[other_ind] - ref_pc[0], y[other_ind] - ref_pc[1]};
		
		
		float norm = sqrt(ref_vc[0]*ref_vc[0] + ref_vc[1]*ref_vc[1]);
		ref_vc[0] = ref_vc[0] / norm;
		ref_vc[1] = ref_vc[1] / norm;
		
		
		// find rotation matrix
		float costheta = ref_vc[0]*ref_vw[0] + ref_vc[1]*ref_vw[1];
		float sintheta = sqrt(1 - costheta*costheta);
		
		
		int if_pos_sin = find_sign(costheta, sintheta, &ref_vc[0], &ref_vw[0]);
		
		
		if(if_pos_sin == 1){
			c_wii[0] = ref_pw[0] - costheta*(ref_pc[0] - camera_center[0]) + sintheta*(ref_pc[1] - camera_center[1]);
			c_wii[1] = ref_pw[1] - costheta*(ref_pc[1] - camera_center[1]) - sintheta*(ref_pc[0] - camera_center[0]);
			ypdir[0] = -sintheta;
			ypdir[1] = costheta;
		}
		else{
			c_wii[0] = ref_pw[0] - costheta*(ref_pc[0] - camera_center[0]) - sintheta*(ref_pc[1] - camera_center[1]);
			c_wii[1] = ref_pw[1] - costheta*(ref_pc[1] - camera_center[1]) + sintheta*(ref_pc[0] - camera_center[0]);
			ypdir[0] = sintheta;
			ypdir[1] = costheta;
		}
		c_wii_last[0] = c_wii[0];
		c_wii_last[1] = c_wii[1];
		ypdir_last[0] = ypdir[0];
		ypdir_last[1] = ypdir[1];
		
		if (state == 3)
		{
			int if_reach = find_if_reach(c_wii[0], c_wii[1]);
			if (!if_reach){
				cal_angle(if_pos_sin, sintheta, costheta);
				PID_alpha(Kp_loc, Ki_loc, Kd_loc);
			}
			else{ // reach goal, stop
				OCR1B = 0;
				OCR1C = 0;
			}
		}
	}
	else{
		c_wii[0] = c_wii_last[0];
		c_wii[1] = c_wii_last[1];
		ypdir[0] = ypdir_last[0];
		ypdir[1] = ypdir_last[1];
	}
}

int find_sign(float costheta, float sintheta, float *ref_vc, float *ref_vw){
	float temp1[2] = {0, 0};
	float temp2[2] = {0, 0};
	
	temp1[0] = costheta*ref_vc[0] - ref_vw[0] - ref_vc[1]*sintheta;
	temp1[1] = costheta*ref_vc[1] - ref_vw[1] + ref_vc[0]*sintheta;
	
	temp2[0] = costheta*ref_vc[0] - ref_vw[0] + ref_vc[1]*sintheta;
	temp2[1] = costheta*ref_vc[1] - ref_vw[1] - ref_vc[0]*sintheta;
	
	float error1 = temp1[0]*temp1[0] +  temp1[1]*temp1[1];
	float error2 = temp2[0]*temp2[0] +  temp2[1]*temp2[1];
	
	if (error1 < error2)
		return 1;
	else
		return 0;	
}

void cal_angle(int if_pos_sign, float sintheta, float costheta){
	float c2g[2] = {0, 0};
	c2g[0] = goal[0] - c_wii[0];
	c2g[1] = goal[1] - c_wii[1];
	
	float norm = c2g[0]*c2g[0] + c2g[1]*c2g[1];
	c2g[0] = c2g[0] / sqrt(norm);
	c2g[1] = c2g[1] / sqrt(norm);
	
	float cos_alpha = c2g[0] * ypdir[0] + c2g[1] * ypdir[1];
	alpha = acos(cos_alpha);
	float c2g_wx;
	if(if_pos_sign == 1){
		c2g_wx = costheta*c2g[0] + sintheta*c2g[1];
	}
	else{
		c2g_wx = costheta*c2g[0] - sintheta*c2g[1];		
	}
	if (c2g_wx < 0){
		alpha = - alpha;
	}
	m_usb_tx_string("alpha: ");
	m_usb_tx_int(alpha*10);
	m_usb_tx_string("\n");
	
}
void PID_alpha(float Kp, float Ki, float Kd){
	/*
	m_usb_tx_string("Kp, Ki, Kd: ");
	m_usb_tx_int(Kp); 
	m_usb_tx_string(", ");
	m_usb_tx_int(Ki);
	m_usb_tx_string(", "); 
	m_usb_tx_int(Kd); 
	m_usb_tx_string("\n");
	*/
	diff_loc = alpha - alpha_last;
	/*
	m_usb_tx_string("diff: ");
	m_usb_tx_int(diff*10);
	m_usb_tx_string("\n");
	*/
	/*
	sumpid = sumpid + alpha;
	m_red(TOGGLE);
	m_usb_tx_string("sumpid: ");
	m_usb_tx_int(sumpid*10);
	m_usb_tx_string("\n");
	*/
	float P =0, I = 0, D = 0;
	P = Kp * alpha;
	I = Ki * sum_loc;
	D = Kd * diff_loc;   
	/*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
	m_usb_tx_string("P, I, D: ");
	m_usb_tx_int(P);
	m_usb_tx_string(", ");
	m_usb_tx_int(I);
	m_usb_tx_string(", ");
	m_usb_tx_int(D*10);
	m_usb_tx_string("\n");
	*/
	gain_loc = P+D;
	
	alpha_last = alpha;
	
	m_usb_tx_string("alpha*10: ");
	m_usb_tx_int(alpha*10); // multiply X10
	m_usb_tx_string("\n");
	
	m_usb_tx_string("gain*10: ");
	m_usb_tx_int((int) (gain_loc*10)); // multiply X10
	m_usb_tx_string("\n");
	
	if(gain_loc<0){  // turn right
		m_usb_tx_string("turn right\n");
		OCR1B = full_speed-abs(gain_loc);
		if (abs(gain_loc) > 100){
			OCR1B = 50;
		}
		OCR1C = full_speed;
	}
	else{		// turn left
		m_usb_tx_string("turn left\n");
		OCR1C = full_speed-abs(gain_loc);
		if (abs(gain_loc) > 100){
			OCR1C = 50;
		}
		OCR1B = full_speed;
	}
	
}



int find_if_reach(float x, float y){
	float dist = (x - goal[0]) * (x - goal[0]) + (y - goal[1]) * (y - goal[1]);
	if (dist < dist_threshold){
		return 1;
	}
	else{
		return 0;
	}
}