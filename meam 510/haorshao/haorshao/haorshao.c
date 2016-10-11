/*
 * haorshao.c
 *
 * Created: 9/23/2015 8:32:11 PM
 *  Author: haorshao
 */ 


#include <avr/io.h>
#include "m_general.h"


int h;

int main()


{   
    while(1)
    { for(h=0;h<30000;){h++;}
        
		m_red(TOGGLE);//TODO:: Please write your application code 
    }
}


