#ifndef communication__
#define communication__

#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include "m_general.h"
#include "m_usb.h"
#include "m_bus.h"
#include "m_rf.h"
#include "common_parm.h"

void send_message(int state, int num, char *buffer, float *c_wii);

#endif