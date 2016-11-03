/*
 * planner.h
 *
 *  Created on: May 7, 2016
 *      Author: amart
 */

#ifndef PLANNER_H_
#define PLANNER_H_

#include "config.h"

#define MAX(a, b)						( ((a) > (b)) ? (a) : (b) )

#define X	0
#define Y	1
#define Z	2

typedef struct{
	volatile uint8_t direction_bits;
	volatile uint8_t step_outbits;
	volatile uint32_t steps[N_AXIS];
	volatile uint32_t counter[N_AXIS];
	volatile uint32_t total_steps;
	volatile uint32_t step_count;
	volatile uint32_t min_delay;
}Linear_Motion;

void motion_linear(float *target, float feed_rate);

#endif /* PLANNER_H_ */
