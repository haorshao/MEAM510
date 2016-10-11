/*
 * findPuck.c
 *
 */ 

#include "findPuck.h"

#define Kp_Pho 0.5
#define Kd_Pho 15

#define Kp_stalk 0.7
#define Kd_stalk 0

#define strThres 50
#define revThres 1200
#define gotThres 55
#define loseThres 50
#define backThres 500

#define full_speed_stalk 650
int atkThres = 2200;


// Variables for find puck (ADC)
int Pho[7]={0, 0, 0, 0, 0, 0, 0};
int i=0;
int input;
int input_last=0;
int output;
int sum_Pho = 0;
int sum_stalk = 0;
int diff_Pho = 0;
int diff_stalk = 0;
int speed_diff_Pho = full_speed;	// speed different for wheels (in order to have a smoother turn)

void init_ADC(void){
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
	set(DIDR2,ADC8D);
	
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


void findPuck(state){
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
	m_usb_tx_string("Pho[6]");
	m_usb_tx_int(Pho[6]);
	m_usb_tx_string("\r\n");
	m_usb_tx_string("\r\n");
	
	int mean024 = (Pho[0] + Pho[2] + Pho[4])/3;
	int mean135 = (Pho[1] + Pho[3] + Pho[5])/3;
	
	m_usb_tx_string("mean024, mean135: ");
	m_usb_tx_int(mean024);
	m_usb_tx_string(", ");
	m_usb_tx_int(mean135);
	m_usb_tx_string("\n");
	
	switch(state){
		case 2:
			if(mean024 - mean135> strThres || (mean024 > mean135 && Pho[4] > Pho[2]) ){
				m_usb_tx_string("turn left\n");
				if(mean024 > revThres){
					set(PORTB, 0);
					clear(PORTB, 1);
					OCR1C = full_speed;
					OCR1B = full_speed;
				}
				else{
					set(PORTB, 0);
					set(PORTB, 1);
					OCR1C = full_speed - speed_diff_Pho;
					OCR1B = full_speed;
					if(Pho[2]>Pho[4]){
						m_usb_tx_string("PID\n");
						PID_Pho(Kp_Pho, Kd_Pho);
					}
				}
			}
		
			else {
				if (mean135 - mean024> strThres || (mean135 > mean024 && Pho[5] > Pho[3])){
					m_usb_tx_string("turn right\n");
					if(mean135 > revThres){
						set(PORTB, 1);
						clear(PORTB, 0);
						OCR1B = full_speed;
						OCR1C = full_speed;
					}
					else{
						set(PORTB, 0);
						set(PORTB, 1);
						OCR1B = full_speed - speed_diff_Pho;
						OCR1C = full_speed;
						if(Pho[3]>Pho[5]){
							m_usb_tx_string("PID\n");
							PID_Pho(Kp_Pho, Kd_Pho);
						}
					}
				}
				else{
					PID_Pho(2*Kp_Pho, Kd_Pho);
				}
			}
			break;
		case 5:
			if(mean024 - mean135 > strThres){
				m_usb_tx_string("turn left\n");
				set(PORTB, 0);
				clear(PORTB, 1);
				if (Pho[2] > Pho[4]){
					PID_stalk(Kp_stalk, Kd_stalk); 
				}
				else{
					OCR1B = full_speed_stalk;
					OCR1C = full_speed_stalk;
				}
			}
			else{
				if(mean135 - mean024 > strThres){
					m_usb_tx_string("turn right\n");
					clear(PORTB, 0);
					set(PORTB, 1);
					if (Pho[3] > Pho[5]){
						PID_stalk(Kp_stalk, Kd_stalk);
					}
					else{
						OCR1B = full_speed_stalk;
						OCR1C = full_speed_stalk;
					}
				}
			}
			break;
		default:
			break;
	}
	
}

void PID_Pho(float Kp, float Kd) {
	int sum02 = Pho[0] + Pho[2];
	int sum13 = Pho[1] + Pho[3];
	input = sum02 - sum13;
	diff_Pho = input - input_last;
	sum_Pho = sum_Pho + input;
	output = Kp * input + Kd * diff_Pho;
	
	input_last = input;
	
	if(output>0){
		OCR1C = full_speed - abs(output);
		if (abs(output) > speed_diff_Pho){
			OCR1C = full_speed - speed_diff_Pho;
		}
		OCR1B = full_speed;
		
	}
	else{
		OCR1B = full_speed - abs(output);
		if (abs(output) > speed_diff_Pho){
			OCR1B = full_speed - speed_diff_Pho;
		}
		OCR1C = full_speed;
	}
}

void PID_stalk(float Kp, float Kd){
	int sum02 = Pho[0] + Pho[2];
	int sum13 = Pho[1] + Pho[3];
	input = sum02 - sum13;
	diff_stalk = input - input_last;
	sum_stalk = sum_stalk + input;
	output = Kp * input + Kd * diff_stalk;
	
	input_last = input;
	
	if(abs(output) >= full_speed_stalk){
		OCR1C = full_speed_stalk;
		OCR1B = full_speed_stalk;
	}
	else{
		OCR1C = abs(output);
		OCR1B = abs(output);
	}
}

ISR(ADC_vect){
	Pho[i] = beta * Pho[i] + (1- beta) * ADC;
	
	i = i + 1;
	if(i > 7){
		i = 0;
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
		case 6:
			set(ADCSRB,MUX5);//select D4 as analog input
			clear(ADMUX,MUX2);
			clear(ADMUX,MUX1);
			clear(ADMUX,MUX0);
			break;
	}
	set(ADCSRA, ADEN);
	set(ADCSRA,ADSC);
	
}

void deterState(int *state){
	if (*state == 2 || *state == 3){
		if (Pho[6] >gotThres){
			*state = 3;
			m_usb_tx_string("got the puck!\n");
			
		}
		else{
			if(Pho[6] < loseThres){
				*state = 2;
				m_usb_tx_string("lost the puck!\n");
			}
		}
	}
}

void ifGotoPuck(int *state, float *dir){
	if(Pho[0] + Pho[1] + Pho[2] + Pho[3] > atkThres){
		*state = 2;
	}
}