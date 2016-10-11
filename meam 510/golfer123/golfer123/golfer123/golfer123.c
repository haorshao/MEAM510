#include "m_general.h"
#include "m_usb.h"

volatile float dutyCycle;

int main(void)
{
	m_usb_init();

	m_clockdivide(0);//set the system clock frequency as 16MHz

	clear(ADMUX,REFS1);//set the voltage reference
	set(ADMUX,REFS0);

	set(ADCSRA,ADPS2);// set the ADC clock prescaler/128
	set(ADCSRA,ADPS1);
	set(ADCSRA,ADPS0);

	set(DIDR0,ADC0D);// disable ADC0D digital inputs
	
	set(ADCSRA,ADATE);//set up the triggering as "free-running" mode.

	clear(ADCSRB,MUX5);//select F0 as analog input
	clear(ADMUX,MUX2);
	clear(ADMUX,MUX1);
	clear(ADMUX,MUX0);

	set(ADCSRA,ADEN);// enable conversions
	set(ADCSRA,ADSC);
	
	clear(TCCR1B,CS12);//set prescaler to /64
	set(TCCR1B,CS11);
	set(TCCR1B,CS10);
	
	set(TCCR1B,WGM13);//(mode 15) UP to OCR1A, PWM mode
	set(TCCR1B,WGM12);
	set(TCCR1A,WGM11);
	set(TCCR1A,WGM10);
	
	set(TCCR1A,COM1B1);//clear at OCR1B, set at rollover
	clear(TCCR1A,COM1B0);
	
	OCR1A=500;
	
	clear(DDRD,2);
	clear(PORTD,2);
    set(DDRB,6);
	
	while(1){
		OCR1B=-0.0848*(float)ADC+511.7;
			//OCR1B=((float)ADC/1024)*(float)100;
			m_usb_tx_int(OCR1B);
			//dutyCycle=(1-(float)OCR1B/(float)OCR1A)*(float)100;
// 			m_usb_tx_int(dutyCycle);
            //m_usb_tx_int(ADC);
			m_usb_tx_string("\r\n");
			m_red(ON);
			
		}
		
	}



