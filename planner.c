#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "stepper.h"
#include "planner.h"
#include "config.h"

int32_t position[N_AXIS];

void motion_linear(float *target, float feed_rate){
	//STEP 1: Initialize new motion structure
	Linear_Motion motion = {0};

	motion.direction_bits = 0;
	motion.total_steps = 0;
	int32_t target_steps[N_AXIS];

	//STEP 2: Compute steps requiered to reach the desired position, compute max steps from the axis
	target_steps[X] = lround(target[X] * STEPS_PER_MM_X);
	target_steps[Y] = lround(target[Y] * STEPS_PER_MM_Y);
	target_steps[Z] = lround(target[Z] * STEPS_PER_MM_Z);

	if(target_steps[X] < position[X]){ motion.direction_bits |= X_STEPS; } //set direction bits, bits mapped in config.h
	if(target_steps[Y] < position[Y]){ motion.direction_bits |= Y_STEPS; }
	if(target_steps[Z] < position[Z]){ motion.direction_bits |= Z_STEPS; }

	motion.steps[X] = labs(target_steps[X]-position[X]);						//absolute amount of steps in each axis
	motion.steps[Y] = labs(target_steps[Y]-position[Y]);
	motion.steps[Z] = labs(target_steps[Z]-position[Z]);
	motion.total_steps = MAX(motion.steps[X], MAX(motion.steps[Y], motion.steps[Z]));	//total steps in this motion


	//STEP 3: Compute desired feedrate for this motion, steps/second
	uint32_t feed_steps;

	if(feed_rate <= 0){
		feed_rate = MAX_FEEDRATE;
		feed_steps = (feed_rate * STEPS_PER_MM) / 60;
		motion.min_delay = SysCtlClockGet()/feed_steps;
	}else{
		feed_steps = (feed_rate * STEPS_PER_MM) / 60;
		motion.min_delay = SysCtlClockGet()/feed_steps;
	}

	//initialize counters for bresenham line displacement
	motion.counter[X] = motion.counter[Y] = motion.counter[Z] = (motion.total_steps / 2);
	motion.step_count = 0;

	//set the position of the planner equal to the target steps, done for arc segmentation positioning
	position[X] = target_steps[X];
	position[Y] = target_steps[Y];
	position[Z] = target_steps[Z];


	stepper_prepare(motion);
}
