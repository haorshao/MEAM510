#ifndef common_parm__
#define common_parm__

#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include "m_general.h"
#include "m_wii.h"	
#include "m_usb.h"
#include "m_rf.h"
#include "m_bus.h"

#define No 3	// Robot Number

#define CHANNEL 1
#define RXADDRESS 0x0E			// No.1 RX address
#define TXADDRESS1 0x0C		// No.2 Rx address
#define TXADDRESS2 0x0D			// No.3 Rx addre

#define PACKET_LENGTH 10		// packet length

#define beta 0.8	// Filter parameter

#define full_speed 650 // full speed for wheels

#define GateA 400
#define GateB -400

#define center_x 20	 // rink center x-axis
#define xline 200	// rink 80-cm line x-axis
#define yline 50	// goal gate width



#endif