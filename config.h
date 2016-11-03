/*
 * config.h
 *
 *  Created on: May 7, 2016
 *      Author: amart
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/gpio.h"

//#define DEBUG

//-----------------SERIAL CONFIGURATION---------//
#define TX_BUFFER_SIZE				256
#define RX_BUFFER_SIZE				256
#define GCODEDELIMITERS		"GMXYZFIJKLNPRST"
#define MAX_TOKEN_SIZE				10

#define MAX_LINE_SIZE				(128)
#define REALTIME
//#define ECHO

//-----------AXIS CONFIGURATION-----------------//
#define X_AXIS						(0)
#define Y_AXIS						(1)
#define Z_AXIS						(2)
#define N_AXIS 						(3)
#define INCH_TO_MM					(2.54)
#define X_DIRECTION_BIT				GPIO_PIN_1
#define Y_DIRECTION_BIT				GPIO_PIN_2
#define Z_DIRECTION_BIT				GPIO_PIN_3
#define X_STEPS						GPIO_PIN_1
#define Y_STEPS						GPIO_PIN_2
#define Z_STEPS						GPIO_PIN_3
#define STEPS_MASK					X_STEPS | Y_STEPS | Z_STEPS
#define DIRECTION_MASK				X_DIRECTION_BIT | Y_DIRECTION_BIT | Z_DIRECTION_BIT

//----------FEEDRATE CONFIGURATION-------------//
#define MAX_FEEDRATE 				(6000)
#define MIN_FEEDRATE				(1)

//----------SPINDLE CONFIGURATION--------------//
#define MAX_SPINDLE_SPEED 			(1200)
#define MIN_SPINDLE_SPEED 			(0)
#define MAX_SPINDLE_FREQUENCY		(10000)


//------------TOOL CONFIGURATION----------------//
#define NUMBER_TOOLS				(3)

//----------STEPPER DRIVER CONFIGURATION-------//
#define STEPS_PER_MM_X				80
#define STEPS_PER_MM_Y				80
#define STEPS_PER_MM_Z				2560
#define STEPS_PER_MM				80
#define MM_PER_STEP					0.0125
#define USTEP16
//#define USTEP8
//#define HALFSTEP
//#define FULLSTEP


//-----------ARC DEFINITION CONFIGURATION--------//
#define ARC_ANGULAR_TRAVEL_EPSILON 	5E-7
#define ARC_TOLERANCE				0.002
#define N_ARC_CORRECTION 			12
#define M_PI						3.14159265359

//-----------PLANNER CONFIGURATION--------------//
#define MOTION_BUFFER_SIZE 			18
#define MINIMUM_JUNCTION_SPEED 		0.0
#define ACCELERATION				32000
#define DECELERATION				32000

#endif /* CONFIG_H_ */
