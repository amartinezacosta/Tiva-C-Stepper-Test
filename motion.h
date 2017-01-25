#ifndef MOTION_H_
#define MOTION_H_
#include "config.h"
#include "queue.h"

void motion_linear(float *position, float feed_rate);
void motion_arc(float *position, float *target, float *offset, float radius, float feed_rate,
				uint8_t invert_feedrate,uint8_t *axis, uint8_t is_clockwise);

#endif /* MOTION_H_ */
