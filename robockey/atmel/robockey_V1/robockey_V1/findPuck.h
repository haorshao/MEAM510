#ifndef findPuck__
#define findPuck__

#include <avr/io.h>
#include "m_general.h"
#include "m_usb.h"
#include "communication.h"
#include "common_parm.h"

// Functions for finding the puck
void init_ADC(void);
void PID_Pho(float Kp, float Ki, float Kd);
void findPuck(int state);
int deterState(void);

#endif