/*
 * findPuck.c
 *
 */ 

#include "findPuck.h"

#define Kp_Pho 0.3
#define Ki_Pho 0
#define Kd_Pho 0

#define strThres 50
#define revThres 1200
#define gotThres 300
#define loseThres 100
#define backThres 500

// Variables for find puck (ADC)
int Pho[7]={0, 0, 0, 0, 0, 0, 0};
int i=0;
int input;
int input_last=0;
int output;
int sum_Pho = 0;
int diff_Pho = 0;
int speed_diff_Pho = 500;	// speed different for wheels (in order to have a smoother turn)

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
				PID_Pho(Kp_Pho, Ki_Pho, Kd_Pho);
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
					PID_Pho(Kp_Pho, Ki_Pho, Kd_Pho);
				}
			}
		}
		else{
			PID_Pho(2*Kp_Pho, Ki_Pho, Kd_Pho);
		}
	}
}

void PID_Pho(float Kp, float Ki, float Kd) {
	int sum02 = Pho[0] + Pho[2];
	int sum13 = Pho[1] + Pho[3];
	input = sum02 - sum13;
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
	if (Pho[6] >gotThres){
		*state = 3;
		OCR1B = full_speed;
		OCR1C = full_speed;
		m_usb_tx_string("got the puck!\n");
		
	}
	else{
		if(Pho[6] < loseThres){
			*state = 2;
			m_usb_tx_string("lost the puck!\n");
		}
		/*
		if(Pho[0] + Pho[1] + Pho[2] + Pho[3] + Pho[4] < backThres){
			*state = 4;
			m_usb_tx_string("can't detect the puck, back for defense\n");
		}
		*/
	}
}