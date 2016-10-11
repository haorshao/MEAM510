/*
 * Direction.c
 *
 * Created: 11/14/2015 10:19:29 PM
 *  Author: haorshao
 */ 

#include <avr/io.h>
#include "m_general.h"
#include "m_usb.h"

int Pho[6]={0, 0, 0, 0, 0, 0};
int i=0;
int Minimum02;
int Maximum02;
int Minimum13;
int Maximum13;
int Mean02;
int Mean13;

int Input;
int Olderinput=0;
int Output;
int Sum=0;
int difference;

int Kp=1;
int Ki=0;
int Kd=1;


void init_timer(void);
void init_ADC(void);
void PID();

int main(void)
{
	m_clockdivide(0);
	m_green(ON);
	m_red(ON);
	m_usb_init();
	while(!m_usb_isconnected());
	m_red(OFF);
	
	init_timer();
	init_ADC();
	
	set(DDRB,2);
	
	
	
	while(1)
	{	
// 		if (Pho[0] > Pho[2]){
// 			Minimum02 = Pho[2];
// 			Maximum02 = Pho[0];
// 		}
// 		else{
// 			Minimum02 = Pho[0];
// 			Maximum02 = Pho[2];
// 		}
// 			
// 		if (Pho[1] > Pho[3]){
// 			Minimum13 = Pho[3];
// 			Maximum13 = Pho[1];
// 		}
// 		else{
// 			Minimum13 = Pho[1];
// 			Maximum13 = Pho[3];
// 		}
// 		
		Mean02 = (Pho[0] + Pho[2])/2;
		Mean13 = (Pho[1] + Pho[3])/2;
		
		if(Mean02 - Mean13> 50){
			m_usb_tx_string("turn left\n");
			OCR1B = 100;
			OCR1C = 250;
			if(Pho[0]>Pho[2]){
			m_usb_tx_string("PID\n"); 
			
			PID();
			}
		}
		 
		else {
			if (Mean13 - Mean02> 50){
				m_usb_tx_string("turn right\n");
				OCR1B = 250;
				OCR1C = 100;
				if(Pho[1]>Pho[3]){
					m_usb_tx_string("PID\n");
					PID();
					
				}
			}
			else{
				OCR1B = 250;
				OCR1C = 250;
			}
		}
		
	m_red(TOGGLE);

		
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
		
	m_wait(500);
		
		
	}
}

ISR(ADC_vect){
	m_green(TOGGLE);
	toggle(PORTB,2);
	
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

void init_timer(void){
	// Direction pin B0 for motor 1, B1 for motor 2
	set(DDRB, 0);
	set(PORTB, 0);
	
	set(DDRB, 1);
	set(PORTB, 1);
	
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
	
	
void init_ADC(){
	
	
	clear(ADMUX,REFS1);//set the voltage reference
	set(ADMUX,REFS0);

	set(ADCSRA,ADPS2);// set the ADC clock prescaler/128
	set(ADCSRA,ADPS1);
	set(ADCSRA,ADPS0);

	set(DIDR0,ADC0D);
	set(DIDR0,ADC1D);
	set(DIDR0,ADC4D);// F4, phototransistor 3
	set(DIDR0,ADC5D);// F5, phototransistor 4
	set(DIDR0,ADC6D);// F6, phototransistor 5
	set(DIDR0,ADC7D);// F7, phototransistor 6
	set(DIDR2,ADC8D);// D4, phototransistor 7
	set(DIDR2,ADC9D);// D6, phototransistor 8
		
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


void PID() {
	
	Input=Pho[0]-Pho[1];
	m_usb_tx_string("Input=");
	m_usb_tx_int(Input);
	m_usb_tx_string("\r\n");
	difference=Input-Olderinput;
	Sum=Sum+Input;
	m_usb_tx_string("Sum=");
	m_usb_tx_int(Sum);
	m_usb_tx_string("\r\n");
	
	//if(Sum>)
	Output=Kp*Input+Ki*Sum+Kd*difference;
	m_usb_tx_string("Output=");
	m_usb_tx_int(Output);
	m_usb_tx_string("\r\n");
	
	Olderinput=Input;
	
	if(Output>0){
		OCR1B=250-abs(Output);
		OCR1C=250;
		
		
	}else{
		OCR1C=250-abs(Output);
		OCR1B=250;
	}

}