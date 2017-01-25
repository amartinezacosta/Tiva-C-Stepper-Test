#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "utils/uartstdio.h"
#include "parser.h"
#include "tokenizer.h"

int8_t parse_gcode(const char *data, Block *block){
	char token[MAX_TOKEN_SIZE];											//token buffer
	uint8_t count = 0;													//count necessary to traverse the string (AKA block)
	uint16_t global_flags = 0;											//bitwise compare of flags

	while(*data){														//traverse the token
		count = Token_get(data, GCODEDELIMITERS, token); 				//get next token (aka Gcode command)
		if(!count){ return NO_COMMANDS_FOUND; }							//if count == 0 no tokens found
		char letter = *token;					  						//Point to first letter of the token
		if(letter < 'A' || letter > 'Z'){ return NO_COMMANDS_FOUND; } 	//invalid first character, error out (I dont't think it will ever hit it but just in case)
		float value = atof(token + 1);									//Extract float from the token
		uint8_t command = trunc(value);									//truncate float for command parsing


		switch(letter){

		case 'G':

		switch(command){
		case 0:
			block->modal.motion = RAPID_POSITIONING; global_flags = GROUP_1; break;
		case 1:
			block->modal.motion = LINEAR_INTERPOLATION; global_flags = GROUP_1; break;
		case 2:
			block->modal.motion = CIRCULAR_INTERPOLATION_CW; global_flags = GROUP_1; break;
		case 3:
			block->modal.motion = CIRCULAR_INTERPOLATION_CCW; global_flags = GROUP_1; break;
		case 21:
			block->modal.units = MILIMITERS; global_flags = GROUP_6; break;
		case 90:
			block->modal.distance = ABSOLUTE_DISTANCE; global_flags = GROUP_3; break;
		case 91:
			block->modal.distance = RELATIVE_DISTANCE; global_flags = GROUP_3; break;
		default:
			//ERROR: Command is not supported by this parser
			return COMMAND_NOT_SUPPORTED;
		}
		if(WORD_EXIST(block->flags.command_flags, global_flags)){ return REPEATED_COMMAND; } //ERROR: Repeated Modal command
		block->flags.command_flags |= BIT(global_flags);
		break;

		case 'M':

		switch(command){
		case 3:
			block->modal.spindle = TURN_SPINDLE_CW; global_flags = GROUP_7; break;
		case 5:
			block->modal.spindle = STOP_SPINDLE; global_flags = GROUP_7; break;
		case 2:
			block->modal.program_flow = PROGRAM_END; global_flags = GROUP_4; break;
		}
		if(WORD_EXIST(block->flags.command_flags, global_flags)){ return REPEATED_COMMAND; } //ERROR: Repeated Modal command
		block->flags.command_flags |= BIT(global_flags);
		break;


		default:

		switch(letter){
		case 'X':
			global_flags = X;
			block->value.xyz[0] = value;
			break;
		case 'Y':
			global_flags = Y;
			block->value.xyz[1] = value;
			break;
		case 'Z':
			global_flags = Z;
			block->value.xyz[2] = value;
			break;
		case 'F':
			global_flags = F;
			block->value.f = value;
			break;
		case 'I':
			global_flags = I;
			block->value.ijk[0] = value;
			break;
		case 'J':
			global_flags = J;
			block->value.ijk[1] = value;
			break;
		case 'S':
			global_flags = S;
			block->value.s = value;
			break;
		case '%': //file beginning optional just acknowledge
			return FILE_BEGIN;
		default:
				//ERROR: Letter value not supported by this parser
			return COMMAND_NOT_SUPPORTED;
		}
		if(WORD_EXIST(block->flags.value_flags, global_flags)){ return REPEATED_COMMAND; } //ERROR: Repeated Value word
		if(BIT(global_flags) & (BIT(F) | BIT(P) | BIT(T) | BIT(S))){
			if(value < 0.0){ return NEGATIVE_VALUE_ERROR; }//ERROR: negative value for non negative word
		}
		block->flags.value_flags |= BIT(global_flags);

		}

		data += count; //point to the next token
	}

#ifdef DEBUG
	uint32_t x_int,y_int,z_int,f_int;
	uint32_t x_man, y_man, z_man,f_man;

	x_int = (uint32_t)block->value.xyz[X];
	y_int = (uint32_t)block->value.xyz[Y];
	z_int = (uint32_t)block->value.xyz[Z];
	f_int = (uint32_t)block->value.f;

	x_man = (uint32_t)(100*(block->value.xyz[X] - x_int));
	y_man = (uint32_t)(100*(block->value.xyz[Y] - y_int));
	z_man = (uint32_t)(100*(block->value.xyz[Z] - z_int));
	f_man = (uint32_t)(100*(block->value.f - f_int));


	UARTprintf("-------------Parsed Block-----------------\n");
	UARTprintf("Motion: %i\n", (uint32_t)block->modal.motion);
	//UARTprintf("Feed Rate: %i\n", (uint32_t)block->modal.feed_rate);
	//UARTprintf("Units: %i\n", (uint32_t)block->modal.units);
	//UARTprintf("Distance: %i\n", (uint32_t)block->modal.distance);
	//UARTprintf("Plane Select: %i\n", (uint32_t)block->modal.plane_select);
	//UARTprintf("Tool Lenght: %i\n", (uint32_t)block->modal.tool_length);
	//UARTprintf("Coordinate Select: %i\n", (uint32_t)block->modal.coord_select);
	//UARTprintf("Program Flow: %i\n", (uint32_t)block->modal.program_flow);
	//UARTprintf("Coolant: %i\n", (uint32_t)block->modal.coolant);
	//UARTprintf("Spindle: %i\n\n", (uint32_t)block->modal.spindle);

	//UARTprintf("-------------Values-----------------\n");
	UARTprintf("X: %i.%i\n", x_int, x_man);
	UARTprintf("Y: %i.%i\n", y_int, y_man);
	UARTprintf("Z: %i.%i\n", z_int, z_man);
	UARTprintf("F: %i.%i\n\n", f_int, f_man);
#endif
	return PARSER_OK;
}
