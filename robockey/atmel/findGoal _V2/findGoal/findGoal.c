/*
 * findGoal.c
 *
 * Created: 2015/11/18 10:56:16
 *  Author: Liu Yilin
 */ 


#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include "m_general.h"
#include "m_wii.h"
#include "m_usb.h"



unsigned int blobs[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
float c_wii[2]={0,0};
float ypdir[2]={0,0};
float c_wii_last[2]={0,0};
float ypdir_last[2]={0,0};
float theta = 0;
float threshold = 2;
float pix2act = 29.0/85;
float actmax[3] = {29, 25.99, 23.08};
float camera_center[2] = {545, 320};
char dummy = 0;
void localization(void);
void calibration(void);
int find_sign(float costheta, float sintheta, float *ref_vc, float *ref_vw);

float goal[2] = {300, 0};  // right gate
float alpha = 0;
float alpha_last = 0;
float sumpid = 0;
float diff = 0;
float gain = 0;
void init_timer(void);
void cal_angle(int if_pos_sign, float sintheta, float costheta);
void PID_alpha(float Kp, float Ki, float Kd);
int find_if_reach(float x, float y);

int main(void){
	m_clockdivide(0);
	
	//m_red(ON);
 	m_usb_init();
	//while(!m_usb_isconnected());
	//m_red(OFF);
	
	m_green(ON);
	while(!m_wii_open());
	m_green(OFF);
		
	/*
	while(!m_usb_rx_available()){
		usb_tx_string("Press any key  \n");
	}
	dummy = m_usb_rx_char();
	*/
	//calibration();
	init_timer();
	m_wii_read(&blobs[0]);
	m_wii_read(&blobs[0]);
	m_wii_read(&blobs[0]);
	while(1){
		m_wii_read(&blobs[0]);
		
		localization();
		/*
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
		*/
		//m_wait(100);
		
	}
	
}

void init_timer(void){
	// Direction pin B0 for motor 1, B1 for motor 2
	set(DDRB, 0);
	set(PORTB, 0);
	
	set(DDRB, 1);
	clear(PORTB, 1);
	
	// Initialize timer 1
	OCR1A = 250;
	OCR1B = 250;
	OCR1C = 250;
	
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
	
	// Set prescaler as /1
	clear(TCCR1B, CS12);
	clear(TCCR1B, CS11);
	set(TCCR1B, CS10);
	
}

/*
void calibration(void){
	m_green(ON);
	m_usb_tx_string("start calibration...\n");
	int sum = 0;
	int k=0, i=0,j=0,ind=0;
	for (k = 0; k < 100; k++){
		m_wii_read(&blobs[0]);
		unsigned int x[4] = {blobs[0], blobs[3], blobs[6], blobs[9]};
		unsigned int y[4] = {blobs[1], blobs[4], blobs[7], blobs[10]};

		double d[6];
		
		for (i=0; i<3; i++){
			for (j=i+1; j<4; j++){
				if(x[i] != 1023 && y[i] != 1023 && x[j] != 1023 && y[j] != 1023)
				d[ind] = sqrt((x[i] - x[j])*(x[i] - x[j]) + (y[i] - y[j])*(y[i] - y[j]));
				ind += 1;
			}
		}
		
		// find maximum
		int dmax = 0;
		for (i=0; i<ind; i++){
			if(d[i] > dmax){
				dmax = d[i];
			}
		}
		m_usb_tx_string("dmax: ");
		m_usb_tx_int(dmax);
		m_usb_tx_string("\n");
		sum = sum + dmax;
	}
	pix2act = 29.0/(sum/100);
	m_usb_tx_string("Done, dmax mean:");
	m_usb_tx_int(sum/100);
	m_usb_tx_string("\n");
	m_green(OFF);
}
*/

void localization(void){
	m_green(ON);
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
	/*
	m_usb_tx_string("dmin: ");
	m_usb_tx_int(dmin);
	m_usb_tx_string("\n");
	*/
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
		
		int if_reach = find_if_reach(c_wii[0], c_wii[1]);
		if (!if_reach){
			cal_angle(if_pos_sin, sintheta, costheta);
			PID_alpha(150, 0, 100);
		}
		else{ // reach goal, stop
			OCR1B = 0;
			OCR1C = 0;
		}
	}
	else{
		c_wii[0] = c_wii_last[0];
		c_wii[1] = c_wii_last[1];
		ypdir[0] = ypdir_last[0];
		ypdir[1] = ypdir_last[1];
	}
	m_green(OFF);
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
	/*
	m_usb_tx_string("alpha: ");
	m_usb_tx_int(alpha*10); // multiply X10
	m_usb_tx_string("\n");
	*/
	
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
	diff = alpha - alpha_last;
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
	I = Ki * sumpid;
	D = Kd * diff;   
	/*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
	m_usb_tx_string("P, I, D: ");
	m_usb_tx_int(P);
	m_usb_tx_string(", ");
	m_usb_tx_int(I);
	m_usb_tx_string(", ");
	m_usb_tx_int(D*10);
	m_usb_tx_string("\n");
	*/
	gain= P+D;
	
	alpha_last = alpha;
	/*
	m_usb_tx_string("alpha*10: ");
	m_usb_tx_int(alpha*10); // multiply X10
	m_usb_tx_string("\n");
	*/
	m_usb_tx_string("gain*10: ");
	m_usb_tx_int((int) (gain*10)); // multiply X10
	m_usb_tx_string("\n");
	
	if(gain<0){  // turn right
		//m_usb_tx_string("turn right\n");
		OCR1B=250-abs(gain);
		if (abs(gain) > 200){
			OCR1B = 50;
		}
		OCR1C=250;
	}
	else{		// turn left
		//m_usb_tx_string("turn left\n");
		OCR1C=250-abs(gain);
		if (abs(gain) > 200){
			OCR1C = 50;
		}
		OCR1B=250;
	}
		m_usb_tx_string("1B, 1C: ");
		m_usb_tx_int(OCR1B);
		m_usb_tx_string(", ");
		m_usb_tx_int(OCR1C);
		m_usb_tx_string("\n");
	
}

int find_if_reach(float x, float y){
	int t = 100;
	float dist = (x - goal[0]) * (x - goal[0]) + (y - goal[1]) * (y - goal[1]);
	if (dist < t){
		return 1;
	}
	else
		return 0;
}