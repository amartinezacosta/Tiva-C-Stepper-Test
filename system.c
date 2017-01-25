#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "stepper.h"
#include "system.h"
#include "parser.h"
#include "config.h"
#include "motion.h"
#include "planner.h"
#include "queue.h"
#include "spindle.h"

System system_t;

Modal* system_get_modal(){
	return &system_t.modal;
}

float* system_get_position(){
	return system_t.position;
}

void system_update(Block *New_Block){
	//ORDER OF EXECUTION
	//[1. comments] NOT SUPPORTED

	//[2. set feedrate mode]
	system_t.modal.feed_rate = New_Block->modal.feed_rate;

	//[3.set feedrate]
	if(WORD_EXIST(New_Block->flags.value_flags, F)){ system_t.feed_rate = New_Block->value.f; }

	//[4. set spindle speed]
	if(WORD_EXIST(New_Block->flags.value_flags, S)){ system_t.spindle_speed = New_Block->value.s; }

	//[5. select tool]
	if(system_t.modal.tool_select != New_Block->value.t){
		system_t.tool = New_Block->value.t;
	}

	//[6. change tool] TODO: implement later! DO NOT FORGET!!!!

	//[7. spindle on or off]
	system_t.modal.spindle = New_Block->modal.spindle;
	switch(system_t.modal.spindle){
	case TURN_SPINDLE_CW:
		spindle_run(system_t.modal.spindle, system_t.spindle_speed);
		break;
	case TURN_SPINDLE_CCW:
		spindle_run(system_t.modal.spindle, system_t.spindle_speed);
		break;
	case STOP_SPINDLE:
		spindle_run(system_t.modal.spindle, system_t.spindle_speed);
		break;
	}

	//[8. coolant on or off]
	if(system_t.modal.coolant != New_Block->modal.coolant){
		//coolant_run(New_Block->modal.coolant);
		system_t.modal.coolant = New_Block->modal.coolant;
	}

	//[9. enable disable overrides] NOT SUPPORTED

	//[10. dwell]
	if(New_Block->non_modal == DWELL){
		//dwell(New_Block->value.p);
	}

	//[11. set active plane]
	system_t.modal.plane_select = New_Block->modal.plane_select;
	switch(system_t.modal.plane_select){
	case XY_PLANE:
		New_Block->value.axis[0] = X_AXIS;
		New_Block->value.axis[1] = Y_AXIS;
		New_Block->value.axis[2] = Z_AXIS;
		break;
	case XZ_PLANE:
		New_Block->value.axis[0] = X_AXIS;
		New_Block->value.axis[1] = Z_AXIS;
		New_Block->value.axis[2] = X_AXIS;
		break;
	default:
		New_Block->value.axis[0] = Y_AXIS;
		New_Block->value.axis[1] = Z_AXIS;
		New_Block->value.axis[2] = X_AXIS;
		break;
	}

	//[12. set lenght units]
	system_t.modal.units = New_Block->modal.units;

	//[13. cutter radius compensation] NOT SUPPORTED

	//[14. cutter lenght compensation] NOT SUPPORTTED

	//[15. coordinate system selection] NOT SUPPORTED

	//[16. set path control] NOT SUPPORTED

	//[17. set distance mode]
	system_t.modal.distance = New_Block->modal.distance;
	if(system_t.modal.distance){
		New_Block->value.xyz[X] += system_t.position[X];
		New_Block->value.xyz[Y] += system_t.position[Y];
		New_Block->value.xyz[Z] += system_t.position[Z];
	}

	//[18. set retract mode] NOT SUPPORTED

	//[19. home, change coordinate data, set axis offsets] NOT SUPPORTED

	//[20. perform motion]
	if(!WORD_EXIST(New_Block->flags.value_flags, X)){ New_Block->value.xyz[X] = system_t.position[X]; }	//keep last coordinate if there is no axis input
	if(!WORD_EXIST(New_Block->flags.value_flags, Y)){ New_Block->value.xyz[Y] = system_t.position[Y]; }
	if(!WORD_EXIST(New_Block->flags.value_flags, Z)){ New_Block->value.xyz[Z] = system_t.position[Z]; }

	system_t.modal.motion = New_Block->modal.motion;
	if(system_t.modal.motion != CANCEL_MOTION_MODE){
		switch(system_t.modal.motion){
		case RAPID_POSITIONING:
			motion_linear(New_Block->value.xyz, -1);
			break;
		case LINEAR_INTERPOLATION:
			motion_linear(New_Block->value.xyz, system_t.feed_rate);
			break;
		case CIRCULAR_INTERPOLATION_CW:
			motion_arc(system_t.position, New_Block->value.xyz, New_Block->value.ijk, New_Block->value.r, system_t.feed_rate,
					system_t.modal.feed_rate, New_Block->value.axis, true);
			break;
		case CIRCULAR_INTERPOLATION_CCW:
			motion_arc(system_t.position, New_Block->value.xyz, New_Block->value.ijk, New_Block->value.r, system_t.feed_rate,
					system_t.modal.feed_rate, New_Block->value.axis, false);
			break;
		}

		system_t.position[X] = New_Block->value.xyz[X];	//update system position
		system_t.position[Y] = New_Block->value.xyz[Y];
		system_t.position[Z] = New_Block->value.xyz[Z];
	}

	//[21. stop]
	system_t.modal.program_flow = New_Block->modal.program_flow;
	switch(system_t.modal.program_flow){
	case PROGRAM_END:
		//report that the program has ended (AKA file streaming)
		break;
	case PROGRAM_RUNNING:
		//continue with normal operation
		break;
	case OPTTIONAL_PROGRAM_STOP:
		//stop program here
		break;
	}
}

void system_execute(void){
	if(system_check(EXECUTE)){
		//wake up stepper motors, start the stepper ISR
		if(system_check(IDLE)){
			//only execute motion if the system is idle
			stepper_wakeup();
			system_set(EXECUTE, false);
			system_set(IDLE, false);
		}
	}

	//check other flags
}


void system_update_position(int8_t step, uint8_t axis){
	switch(axis){
	case X:
		system_t.rt_position[X] += (float)(step * MM_PER_STEP);
		break;
	case Y:
		system_t.rt_position[Y] += (float)(step * MM_PER_STEP);
		break;
	case Z:
		system_t.rt_position[Z] += (float)(step * MM_PER_STEP);
		break;
	}
}

float *system_get_position_rt(void){
	return system_t.rt_position;
}

float *system_get_postion(void){
	return system_t.position;
}

bool system_check_same_position(float *target_position){
	uint8_t idx;
	for(idx = 0; idx < N_AXIS; idx++){
		if(target_position[idx] != system_t.position[idx]){ return false;}
	}
	return true;
}

void system_set(uint8_t status, bool set){
	switch(status){
	case EXECUTE:
		if(set){ system_t.status |= BIT(EXECUTE); }
		else{ system_t.status &= ~BIT(EXECUTE); }
		break;
	case IDLE:
		if(set){ system_t.status |= BIT(IDLE); }
		else{ system_t.status &= ~BIT(IDLE); }
		break;
	}
}

bool system_check(uint8_t status){
	switch(status){
	case EXECUTE:
		return system_t.status & BIT(EXECUTE);
	case IDLE:
		return system_t.status & BIT(IDLE);
	}
	return false;
}


void system_init(void){
	memset(&system_t, 0, sizeof(System));
	system_set(IDLE, true);
}

