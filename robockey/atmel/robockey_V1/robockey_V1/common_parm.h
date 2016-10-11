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

#define No 1	// Robot Number

#define CHANNEL 1
#define RXADDRESS 0x0C		// No.1 RX address
#define TXADDRESS1 0x0D			// No.2 Rx address
#define TXADDRESS2 0x0E			// No.3 Rx address
#define PACKET_LENGTH 10		// packet length

#define full_speed 245	// full speed for wheels
#define speed_diff 50	// speed different for wheels (in order to have a smoothier turn)


#define GateA 350
#define GateB -350

#define center_x 20	 // rink center x-axis
#define line 200	// rink 80-cm line x-axis



#endif