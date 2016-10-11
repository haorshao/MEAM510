/*
 * localization.c
 *
 */ 

#include "localization.h"


// -----parameters to be tuned-----
#define Kp_loc 30//30
#define Kd_loc 400

#define Kp_p2p 100
#define Kd_p2p 400

float camera_center[2] = {521, 381};
int dist_threshold = 1000;
int speed_diff_loc = full_speed;	// speed different for wheels (in order to have a smoother turn)
int speed_diff_p2p = full_speed;	
//----------------------------------

// Variables for localization
unsigned int blobs[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

float c_wii_last[2]={0,0};
float dir_last[2]={0,0};
float theta = 0;
float threshold = 2;
float pix2act = 29.0/95;
float actmax[3] = {29, 25.99, 23.08};


// Variables for find goal
float alpha = 0;
float alpha_last = 0;
float sum_loc = 0;
float diff_loc = 0;
float gain_loc = 0;


void localization(int *state, float *goal, float *c_wii, float *dir){
	m_wii_read(&blobs[0]);
// 	m_usb_tx_int(blobs[0]);
// 	m_usb_tx_string(" ");
// 	m_usb_tx_int(blobs[1]);
// 	m_usb_tx_string(" ");
// 	m_usb_tx_int(blobs[3]);
// 	m_usb_tx_string(" ");
// 	m_usb_tx_int(blobs[4]);
// 	m_usb_tx_string(" ");
// 	m_usb_tx_int(blobs[6]);
// 	m_usb_tx_string(" ");
// 	m_usb_tx_int(blobs[7]);
// 	m_usb_tx_string(" ");
// 	m_usb_tx_int(blobs[9]);
// 	m_usb_tx_string(" ");
// 	m_usb_tx_int(blobs[10]);
// 	m_usb_tx_string("\n");
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
	m_usb_tx_string("dmax:");
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
			dir[0] = costheta;
			dir[1] = sintheta;
		}
		else{
			c_wii[0] = ref_pw[0] - costheta*(ref_pc[0] - camera_center[0]) - sintheta*(ref_pc[1] - camera_center[1]);
			c_wii[1] = ref_pw[1] - costheta*(ref_pc[1] - camera_center[1]) + sintheta*(ref_pc[0] - camera_center[0]);
			dir[0] = costheta;
			dir[1] = -sintheta;
		}
		c_wii_last[0] = c_wii[0];
		c_wii_last[1] = c_wii[1];
		dir_last[0] = dir[0];
		dir_last[1] = dir[1];
		
		if (*state == 4){
			int if_reach = find_if_reach(c_wii[0], c_wii[1], &goal[0]);
			if (!if_reach){
				cal_angle(if_pos_sin, sintheta, costheta, &goal[0], &c_wii[0], &dir[0]);
				PID_p2p(Kp_p2p, Kd_p2p);
			}
			else{ // reach goal, stop
				OCR1B = 0;
				OCR1C = 0;
				//if (*state == 4){
					//*state = 5;
				//}
			}
		}
		if (*state == 3){
			cal_angle(if_pos_sin, sintheta, costheta, &goal[0], &c_wii[0], &dir[0]);
			PID_alpha(Kp_loc, Kd_loc);
		}
		
	}
	else{
		c_wii[0] = c_wii_last[0];
		c_wii[1] = c_wii_last[1];
		dir[0] = dir_last[0];
		dir[1] = dir_last[1];
	}
	m_usb_tx_string("Position x:");
	m_usb_tx_int(c_wii[0]);
	m_usb_tx_string("		y:");
	m_usb_tx_int(c_wii[1]);
	m_usb_tx_string("\n");
	
	m_usb_tx_string("Direction vx: 0.");
	m_usb_tx_int(dir[0]*1000);
	m_usb_tx_string("	vy: 0.");
	m_usb_tx_int(dir[1]*1000);
	m_usb_tx_string("\n");
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

void cal_angle(int if_pos_sign, float sintheta, float costheta, float *goal ,float *c_wii, float *dir){
	float c2g[2] = {0, 0};
	c2g[0] = goal[0] - c_wii[0];
	c2g[1] = goal[1] - c_wii[1];

	float norm = c2g[0]*c2g[0] + c2g[1]*c2g[1];
	c2g[0] = c2g[0] / sqrt(norm);
	c2g[1] = c2g[1] / sqrt(norm);

	float cos_alpha = c2g[0] * dir[0] + c2g[1] * dir[1];
	alpha = acos(cos_alpha);
	float c2g_wx;
	if(if_pos_sign == 1){
		c2g_wx = -costheta*c2g[1] + sintheta*c2g[0];
	}
	else{
		c2g_wx = -costheta*c2g[1] - sintheta*c2g[0];
	}
	if (c2g_wx < 0){
		alpha = - alpha;
	};   

	m_usb_tx_string("alpha: ");
	m_usb_tx_int(alpha*10);
	m_usb_tx_string("\n");
	
}
void PID_alpha(float Kp, float Kd){
	diff_loc = alpha - alpha_last;
	float P =0, D = 0;
	P = Kp * alpha;
	D = Kd * diff_loc;	gain_loc = P+D;
	
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
		if (abs(gain_loc) > speed_diff_loc){
			OCR1B = full_speed - speed_diff_loc;
		}
		OCR1C = full_speed;
	}
	else{		// turn left
		m_usb_tx_string("turn left\n");
		OCR1C = full_speed-abs(gain_loc);
		if (abs(gain_loc) > speed_diff_loc){
			OCR1C = full_speed - speed_diff_loc;
		}
		OCR1B = full_speed;
	}	
}

void PID_p2p(float Kp, float Kd){
	diff_loc = alpha - alpha_last;
	float P =0, D = 0;
	P = Kp * alpha;
	D = Kd * diff_loc;   
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
		if (abs(gain_loc) > speed_diff_p2p){
			OCR1B = full_speed - speed_diff_p2p;
		}
		OCR1C = full_speed;
	}
	else{		// turn left
		m_usb_tx_string("turn left\n");
		OCR1C = full_speed-abs(gain_loc);
		if (abs(gain_loc) > speed_diff_p2p){
			OCR1C = full_speed - speed_diff_p2p;
		}
		OCR1B = full_speed;
	}
	
}


int find_if_reach(float x, float y, float *goal){
	//unsigned int dist = abs(x - goal[0])* abs(x - goal[0]);
	/*m_usb_tx_string("dist: ");
	m_usb_tx_int(dist); 
	m_usb_tx_string("\n");*/
	if (goal[0]<0){
		if(x<0){
			return 1;
			
		}else{
			return 0;
		}
		
	}
	else{
		if(x>0){
			return 1;
		}else{
			return 0;
		}
		
	}
}