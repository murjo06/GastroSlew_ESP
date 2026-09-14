#pragma once

#include "driver/uart.h"


#define FIRMWARE_VERSION_MAJOR	0
#define FIRMWARE_VARSION_MINOR	1

#define TRACKING_RATE			0.00417807462230767		// deg/s

#define GUIDE_SPEED				0.75f					// sidersko

#define LOOP_PERIOD				5e-3f					// s,  perioda glavnega loopa


//* RA config

#define STEPS_PER_DEGREE_RA		4101097.2444444444444
#define UART_RA					UART_NUM_1


//* DEC config

#define STEPS_PER_DEGREE_DEC	4101097.2444444444444
#define UART_DEC				UART_NUM_2