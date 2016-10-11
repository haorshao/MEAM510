/*
 * phototransistor_ADC.c
 *
 * Created: 11/8/2015 2:14:04 PM
 *  Author: haorshao
 */ 


#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"

int Pho[8]={0, 0, 0, 0, 0, 0, 0, 0};
int timer1flag=0;
int i=0;
//int k=0;
int n;


int main(void)
{ 
	m_usb_init();
	 
	m_clockdivide(0);
 
		
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
	
	 
    while(1)
    {  m_red(TOGGLE);

		
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
	   m_usb_tx_string("Pho[7]");
	   m_usb_tx_int(Pho[7]);
	   m_usb_tx_string("\r\n");
	   
	  m_wait(500);
	   
	  
    }
}

ISR(ADC_vect){
	m_green(TOGGLE);
	
	Pho[i]=ADC;
	
	i=i+1;
	if(i>7){
		i=0;
	}
	
	clear(ADCSRA, ADEN);
	switch(i){
		
		case(0):
			clear(ADCSRB,MUX5);//select F0 as analog input
			clear(ADMUX,MUX2);
			clear(ADMUX,MUX1);
			clear(ADMUX,MUX0);
			
			break;
		case(1):
			clear(ADCSRB,MUX5);//select F1 as analog input
			clear(ADMUX,MUX2);
			clear(ADMUX,MUX1);
			set(ADMUX,MUX0);
			break;
			
		case(2):
		clear(ADCSRB,MUX5);//select F4 as analog input
		set(ADMUX,MUX2);
		clear(ADMUX,MUX1);
		clear(ADMUX,MUX0);
		
		break;
		
		case(3):
		clear(ADCSRB,MUX5);//select F5 as analog input
		set(ADMUX,MUX2);
		clear(ADMUX,MUX1);
		set(ADMUX,MUX0);
		break;
		
		case(4):
		clear(ADCSRB,MUX5);//select F6 as analog input
		set(ADMUX,MUX2);
		set(ADMUX,MUX1);
		clear(ADMUX,MUX0);
		break;
		
		case(5):
		clear(ADCSRB,MUX5);//select F7 as analog input
		set(ADMUX,MUX2);
		set(ADMUX,MUX1);
		set(ADMUX,MUX0);
		break;
		
		case(6):
		set(ADCSRB,MUX5);//select D4 as analog input
		clear(ADMUX,MUX2);
		clear(ADMUX,MUX1);
		clear(ADMUX,MUX0);
		
		break;
		
		case(7):
		
		set(ADCSRB,MUX5);//select D6 as analog input
		clear(ADMUX,MUX2);
		clear(ADMUX,MUX1);
		set(ADMUX,MUX0);
		
		break;
		}
		
	set(ADCSRA, ADEN);
	set(ADCSRA,ADSC);
	 
  	
}

