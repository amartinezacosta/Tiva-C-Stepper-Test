#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "driverlib/sysctl.h"
#include "config.h"
#include "planner.h"
#include "system.h"

void motion_linear(float *position, float feed_rate){
	do {
	//system check
	system_execute();
	 if (buffer_full()) {
		 //set flag to dequeue linear motion, start execution
		 system_set(EXECUTE, true);
	 }
	 else { break; }
	} while (1);

	//Enter new motion into the motion buffer
	buffer_motion(position, feed_rate);
}


void motion_arc(float *position, float *target, float *offset, float radius, float feed_rate,
				uint8_t invert_feedrate,uint8_t *axis, uint8_t is_clockwise){
	uint8_t axis_0 = axis[0];
	uint8_t axis_1 = axis[1];
	uint8_t axis_linear = axis[2];

	float x = target[axis_0] - position[axis_0];
	float y = target[axis_1] - position[axis_1];

	if(fabs(radius)){
		//convert radius to ijk values if absolute value of radius is greater than 0
		float h = 4.0 * radius*radius - x*x -y*y;
		h = -sqrt(h)/hypot(x,y);
		//if(h < 0){return ERROR_ARC_TARGET; }
		if(!is_clockwise){ h = -h; }
		if(radius < 0){
			h = -h;
			radius = -radius;
		}
		offset[axis_0] = 0.5 * (x - (y*h));
		offset[axis_1] = 0.5 * (y + (x*h));
	}else{
		//set radius with given offsets
		radius = hypot(offset[axis_0], offset[axis_1]);
	}

	float center_axis0 = position[axis_0] + offset[axis_0];
	float center_axis1 = position[axis_1] + offset[axis_1];
	float r_axis0 = -offset[axis_0];
	float r_axis1 = -offset[axis_1];
	float rt_axis0 = target[axis_0] - center_axis0;
	float rt_axis1 = target[axis_1] - center_axis1;

	float angular_travel = atan2(r_axis0*rt_axis1-r_axis1*rt_axis0, r_axis0*rt_axis0+r_axis1*rt_axis1);
	if(is_clockwise){
		if(angular_travel >= -ARC_ANGULAR_TRAVEL_EPSILON) { angular_travel -= 2*M_PI; }
	}else{
		if(angular_travel <= ARC_ANGULAR_TRAVEL_EPSILON){ angular_travel += 2*M_PI; }
	}

	uint16_t segments = floor(fabs(0.5*angular_travel*radius)/
							sqrt(ARC_TOLERANCE*(2*radius - ARC_TOLERANCE)) );

	if(segments){
		if(invert_feedrate){ feed_rate *= segments; }

		float theta_per_segment = angular_travel/segments;
		float linear_per_segment = (target[axis_linear] - position[axis_linear]) / segments;

		float cos_T = 2.0 - theta_per_segment*theta_per_segment;
		float sin_T = theta_per_segment*0.16666667*(cos_T + 4.0);
		cos_T *= 0.5;


		float sin_Ti;
		float cos_Ti;
		float r_axisi;
		uint16_t i;
		uint8_t count = 0;

		for(i = 1; i < segments; i++){
			if(count < N_ARC_CORRECTION){
				r_axisi = r_axis0*sin_T + r_axis1*cos_T;
				r_axis0 = r_axis0*cos_T - r_axis1*sin_T;
				r_axis1 = r_axisi;
				count++;
			}else{
				cos_Ti = cos(i*theta_per_segment);
				sin_Ti = sin(i*theta_per_segment);
				r_axis0 = -offset[axis_0]*cos_Ti + offset[axis_1]*sin_Ti;
				r_axis1 = -offset[axis_0]*sin_Ti - offset[axis_1]*cos_Ti;
				count = 0;
			}

			position[axis_0] = center_axis0 + r_axis0;
			position[axis_1] = center_axis1 + r_axis1;
			position[axis_linear] += linear_per_segment;

			motion_linear(position, feed_rate);
		}

		motion_linear(position, feed_rate);
	}

}
