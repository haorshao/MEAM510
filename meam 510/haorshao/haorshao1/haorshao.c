/*
 * haorshao.c
 *
 * Created: 9/23/2015 8:32:11 PM
 *  Author: haorshao
 */ 


#include <avr/io.h>
#include "m_general.h"
#define PIN 6

int h;

int main()


{   
    while(1)
    { for(h=0;h<30000;){h++;}
        
		toggle(PORTE,PIN);//TODO:: Please write your application code 
    }
}


