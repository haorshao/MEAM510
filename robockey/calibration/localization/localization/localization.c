/*
 * localization.c
 *
 * Created: 2015/11/9 21:50:24
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
float theta;
float threshold = 2;
float pix2act = 29.0/88;
float actmax[3] = {29, 25.99, 23.08};
float camera_center[2] = {552, 360};
char dummy;
void localization(void);
void calibration(void);
int find_sign(float costheta, float sintheta, float *ref_vc, float *ref_vw);

int main(void){
	m_clockdivide(0);
	
	m_red(ON);
	m_usb_init();
	while(!m_usb_isconnected());
	m_red(OFF);
	
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
	
	
	while(1){
		
		m_wii_read(&blobs[0]);
		
		m_usb_tx_int(blobs[0]);
		m_usb_tx_string(" ");
		m_usb_tx_int(blobs[1]);
		m_usb_tx_string(" ");
		m_usb_tx_int(blobs[3]);
		m_usb_tx_string(" ");
		m_usb_tx_int(blobs[4]);
		m_usb_tx_string(" ");
		m_usb_tx_int(blobs[6]);
		m_usb_tx_string(" ");
		m_usb_tx_int(blobs[7]);
		m_usb_tx_string(" ");
		m_usb_tx_int(blobs[9]);
		m_usb_tx_string(" ");
		m_usb_tx_int(blobs[10]);
		m_usb_tx_string("\n");
		
		/*
		m_usb_tx_string("p1:");
		m_usb_tx_int(blobs[0]);
		m_usb_tx_string(",");
		m_usb_tx_int(blobs[1]);
		m_usb_tx_string("	p2:");
		m_usb_tx_int(blobs[3]);
		m_usb_tx_string(",");
		m_usb_tx_int(blobs[4]);
		m_usb_tx_string("	p3:");
		m_usb_tx_int(blobs[6]);
		m_usb_tx_string(",");
		m_usb_tx_int(blobs[7]);
		m_usb_tx_string("	p4:");
		m_usb_tx_int(blobs[9]);
		m_usb_tx_string(",");
		m_usb_tx_int(blobs[10]);
		m_usb_tx_string("\n");
		
		localization();
		
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
		
		m_wait(500);
		
	}
	
}

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
	m_usb_tx_string("dmax: ");
	m_usb_tx_int(dmax);
	m_usb_tx_string("\n");
	// find minimum
	int dmin = 10000;
	int indmin = 0;
	for (i=0; i<ind; i++){
		if(d[i] < dmin){
			indmin = i;
			dmin = d[i];
		}
	}
	m_usb_tx_string("dmin: ");
	m_usb_tx_int(dmin);
	m_usb_tx_string("\n");
	
	// find miss which point
	double actdmax = dmax * pix2act;
	m_usb_tx_string("actual max:");
	m_usb_tx_int(actdmax);
	m_usb_tx_string("\n");
	
	int ca = 0;
	for(i = 1; i<4; i++){
		if(abs(actdmax - actmax[i-1])<threshold){
			ca = i;
		}
	}
	
	float ref_pw[2] = {0, 0};
	float ref_vw[2] = {0, 1};
	
	m_usb_tx_string("case :");
	m_usb_tx_int(ca);
	m_usb_tx_string("\n");
	
	switch(ca){
		case 1: // not miss point 1 or point 4
			// ref_pw[0] = 0; ref_pw[1] = -14.5 / pix2act;
			ref_pw[0] = 0; ref_pw[1] = 0;
			ref_vw[0] = 0; ref_vw[1] = 1;
			
			usb_tx_string("not miss\n");
			break;
		case 2: // miss point 4
			ref_pw[0] = 0; ref_pw[1] = 14.5 / pix2act;
			ref_vw[0] = 0.4483; ref_vw[1] = -0.8939;
			usb_tx_string("miss 4\n");
			break;
		case 3: // miss point 1
			ref_pw[0] = 11.655 / pix2act; ref_pw[1] = -8.741 / pix2act;
			ref_vw[0] = -0.9625; ref_vw[1] = 0.2711;
			usb_tx_string("miss 1\n");
			break;
		default:
			m_usb_tx_string("cannot calculate\n");
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
		
		m_usb_tx_string("coin_ind, other_ind:");
		m_usb_tx_int(coin_ind);
		m_usb_tx_string("	");
		m_usb_tx_int(other_ind);
		m_usb_tx_string("\n");
		
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
		m_usb_tx_string("ref_pc0, ref_pc1: ");
		m_usb_tx_int(ref_pc[0]);
		m_usb_tx_string("	");
		m_usb_tx_int(ref_pc[1]);
		m_usb_tx_string("\n");
		
		float ref_vc[2] = {x[other_ind] - ref_pc[0], y[other_ind] - ref_pc[1]};
			
		m_usb_tx_string("ref_vc0, ref_vc1 (unnormalize): ");
		m_usb_tx_int(ref_vc[0]);
		m_usb_tx_string("	");
		m_usb_tx_int(ref_vc[1]);
		m_usb_tx_string("\n");
		
		float norm = sqrt(ref_vc[0]*ref_vc[0] + ref_vc[1]*ref_vc[1]);
		ref_vc[0] = ref_vc[0] / norm;
		ref_vc[1] = ref_vc[1] / norm;
		
		m_usb_tx_string("ref_vc0, ref_vc1 (normalize): ");
		m_usb_tx_int(ref_vc[0]*1000);
		m_usb_tx_string("	");
		m_usb_tx_int(ref_vc[1]*1000);
		m_usb_tx_string("\n");
		
		// find rotation matrix
		float costheta = ref_vc[0]*ref_vw[0] + ref_vc[1]*ref_vw[1];
		float sintheta = sqrt(1 - costheta*costheta);
		
		m_usb_tx_string("cos, sin:");
		m_usb_tx_int(costheta*1000);
		m_usb_tx_string("	");
		m_usb_tx_int(sintheta*1000);
		m_usb_tx_string("\n");
		
		
		int if_pos_sin = find_sign(costheta, sintheta, &ref_vc[0], &ref_vw[0]);
		m_usb_tx_string("sign: ");
		m_usb_tx_int(if_pos_sin);
		m_usb_tx_string("\n");
		
		
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