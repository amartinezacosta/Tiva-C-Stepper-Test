#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include "planner.h"
#include "queue.h"
#include "stepper.h"

int32_t position[N_AXIS];
Queue Motion_Queue;
QueueEntry new_entry;
QueueEntry Motion_Buffer[64];
uint32_t ID;

void buffer_motion(float *target, float feed_rate){
	//STEP 1: Initialize new motion structure
	memset(&new_entry, 0, sizeof(QueueEntry));

	new_entry.ID = ID++;
	new_entry.motion.direction_bits = 0;
	new_entry.motion.total_steps = 0;
	int32_t target_steps[N_AXIS];

	//STEP 2: Compute steps requiered to reach the desired position, compute max steps from the axis
	target_steps[X] = lround(target[X] * STEPS_PER_MM_X);
	target_steps[Y] = lround(target[Y] * STEPS_PER_MM_Y);
	target_steps[Z] = lround(target[Z] * STEPS_PER_MM_Z);

	if(target_steps[X] < position[X]){ new_entry.motion.direction_bits |= X_STEPS; } //set direction bits, bits mapped in config.h
	if(target_steps[Y] < position[Y]){ new_entry.motion.direction_bits |= Y_STEPS; }
	if(target_steps[Z] < position[Z]){ new_entry.motion.direction_bits |= Z_STEPS; }

	new_entry.motion.steps[X] = labs(target_steps[X]-position[X]);						//absolute amount of steps in each axis
	new_entry.motion.steps[Y] = labs(target_steps[Y]-position[Y]);
	new_entry.motion.steps[Z] = labs(target_steps[Z]-position[Z]);
	new_entry.motion.total_steps = MAX(new_entry.motion.steps[X], MAX(new_entry.motion.steps[Y], new_entry.motion.steps[Z]));	//total steps in this motion


	//STEP 3: Compute desired feedrate for this motion, steps/second
	uint32_t feed_steps;

	if(feed_rate <= 0){
		feed_rate = MAX_FEEDRATE;
		feed_steps = (feed_rate * STEPS_PER_MM) / 60;
		new_entry.motion.min_delay = SysCtlClockGet()/feed_steps;
	}else{
		feed_steps = (feed_rate * STEPS_PER_MM) / 60;
		new_entry.motion.min_delay = SysCtlClockGet()/feed_steps;
	}

	//initialize counters for bresenham line displacement
	new_entry.motion.counter[X] = new_entry.motion.counter[Y] = new_entry.motion.counter[Z] = (new_entry.motion.total_steps / 2);
	new_entry.motion.step_count = 0;

	//set the position of the planner equal to the target steps, done for arc segmentation positioning
	position[X] = target_steps[X];
	position[Y] = target_steps[Y];
	position[Z] = target_steps[Z];

	#ifdef DEBUG
		UARTprintf("Appending Motion ID: %i with %i total steps\n", new_entry.ID, new_entry.motion.total_steps);
	#endif
	Append(new_entry, &Motion_Queue);
}

void buffer_init(void){
	CreateQueue(&Motion_Queue, Motion_Buffer);
	ID = 0;
}

bool buffer_empty(void){
	return QueueEmpty(&Motion_Queue);
}

bool buffer_full(void){
	return QueueFull(&Motion_Queue);
}

void buffer_reset(void){
	ClearQueue(&Motion_Queue);
}

Linear_Motion *motion_serve(void){
	static QueueEntry entry;
	Serve(&entry, &Motion_Queue);
	#ifdef DEBUG
		UARTprintf("Serving Motion ID: %i with %i total steps\n", entry.ID, entry.motion.total_steps);
	#endif
	return &entry.motion;
}
