/*
 * config.h
 *
 *  Created on: Apr 14, 2016
 *      Author: amart
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#include "planner.h"

void stepper_end(void);
void stepper_init(void);
void stepper_prepare(Linear_Motion New_Motion);
bool busy(void);
bool update(void);
void update_done(void);
void stepper_position(float *position);

#endif /* CONFIG_H_ */
