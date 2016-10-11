#ifndef localization__
#define localization__

#include <avr/io.h>
#include "m_general.h"
#include "m_usb.h"
#include "m_bus.h"
#include "m_wii.h"
#include "common_parm.h"

// Functions for localization and finding goal
void localization(int state, float *goal, float *c_wii, float *ypdir);
int find_sign(float costheta, float sintheta, float *ref_vc, float *ref_vw);
void cal_angle(int if_pos_sign, float sintheta, float costheta, float *goal, float *c_wii, float *ypdir);
void PID_alpha(float Kp, float Kd);
void PID_p2p(float Kp, float Kd);
int find_if_reach(float x, float y, float *goal);

#endif