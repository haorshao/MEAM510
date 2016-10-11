/*
 * findPuck.c
 *
 */ 

#include "findPuck.h"

#define Kp_Pho 0.1
#define Ki_Pho 0
#define Kd_Pho 0

// Variables for find puck (ADC)
int Pho[7]={0, 0, 0, 0, 0, 0, 0};
int i=0;
int input;
int input_last=0;
int output;
int sum_Pho = 0;
int diff_Pho = 0;

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
	
	int mean024 = (Pho[0] + Pho[2] + Pho[4])/3;
	int mean135 = (Pho[1] + Pho[3] + Pho[5])/3;
	
	if(mean024 - mean135> 50 || (mean024 > mean135 && Pho[4] > Pho[2]) ){
		m_usb_tx_string("turn left\n");
		OCR1C = 50;
		OCR1B = full_speed;
		if(Pho[2]>Pho[4]){
			m_usb_tx_string("PID\n");
			PID_Pho(Kp_Pho, Ki_Pho, Kd_Pho);
		}
	}
	
	else {
		if (mean135 - mean024> 50 || (mean135 > mean024 && Pho[5] > Pho[3])){
			m_usb_tx_string("turn right\n");
			OCR1C = full_speed;
			OCR1B = 50;
			if(Pho[3]>Pho[5]){
				m_usb_tx_string("PID\n");
				PID_Pho(Kp_Pho, Ki_Pho, Kd_Pho);
			}
		}
		else{
			m_usb_tx_string("straight\n");
			OCR1B = full_speed;
			OCR1C = full_speed;
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
		OCR1C=full_speed-abs(output);
		if (abs(output) > (full_speed-50)){
			OCR1C = 50;
		}
		OCR1B=full_speed;
		
	}
	else{
		OCR1B=full_speed-abs(output);
		if (abs(output) > (full_speed-50)){
			OCR1B = 50;
		}
		OCR1C=full_speed;
	}
}

ISR(ADC_vect){
	Pho[i]=ADC;
	
	i=i+1;
	if(i>7){
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

int deterState(void){
	if (Pho[6] > 300){
		
		m_usb_tx_string("get the puck \n");
		return 3;
	}
	else{
		
		return 2;
	}
}