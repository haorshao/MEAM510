#ifndef findPuck__
#define findPuck__

#include <avr/io.h>
#include "m_general.h"
#include "m_usb.h"
#include "communication.h"
#include "common_parm.h"

// Functions for finding the puck
void init_ADC(void);
void PID_Pho(float Kp, float Kd);
void PID_stalk(float Kp_stalk, float Kd_stalk);
void findPuck(int state);
void deterState(int *state);
void ifGotoPuck(int *state, float *dir);

#endif