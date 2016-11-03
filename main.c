#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "stepper.h"
#include "console.h"
#include "tokenizer.h"
#include "planner.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"

#define G00	0
#define G01	1

#define X_FLAG	0
#define Y_FLAG	1
#define Z_FLAG	2
#define F_FLAG	3

#define BIT(n)							(1 << n)
#define FLAG_SET(A, mask)				(A |= 1 << mask)
#define FLAG_CHECK(A, mask)				((A & BIT(mask)) != 0)

float Global_Position[3];

void enter_command(void);
void print_position(void);

int main(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	stepper_init();
	InitConsole();
	UARTprintf("%c", 12);


	while(1){
		enter_command();

		while(busy()){ //check for busy condition on stepper motors
			if(update()){ //check for update request on stepper ISR
				//lcd_update();
				//system_check();
				print_position();
				update_done();
			}
		}

	}
}



void enter_command(void){
	char str[64];
	unsigned char *pstr;
	const unsigned char *delimiters = " GXYZF";
	uint8_t count = 0;
	unsigned char token[10];
	float value_f;
	uint32_t value_i;
	char command;
	float xyz[3];
	float f = 0;
	uint8_t value_flag = 0;
	uint8_t command_flag = 0;

	UARTprintf("----Stepper test-----\n\r");
	UARTprintf("Enter G00 or G01 Command\n\r");
	UARTgets(str, sizeof(str));
	pstr = (unsigned char*)str;

	while(*pstr){
		count = Token_get(pstr, delimiters, token);
		if(!count){
			UARTprintf("Wrong Input\n\r");
			return;
		}
		UARTprintf("%s\n\r", token);

		command = *token;
		value_f = atof((const char*)token + 1);
		value_i = trunc(value_f);

		switch(command){
		case 'G':
			switch(value_i){
			case 0:
				FLAG_SET(command_flag, G00);
				break;
			case 1:
				FLAG_SET(command_flag, G01);
				break;
			default:
				UARTprintf("G command not supported\n");
				return;
			}
			break;
		case 'X':
			FLAG_SET(value_flag, X_FLAG);
			xyz[X] = value_f;
			break;
		case 'Y':
			FLAG_SET(value_flag, Y_FLAG);
			xyz[Y] = value_f;
			break;
		case 'Z':
			FLAG_SET(value_flag, Z_FLAG);
			xyz[Z] = value_f;
			break;
		case 'F':
			FLAG_SET(value_flag, F_FLAG);
			f = value_f;
			break;
		default:
			UARTprintf("Command not supported\n");
			return;
		}

		pstr += count;
	}

	if(FLAG_CHECK(command_flag, G01) && !FLAG_CHECK(value_flag, F_FLAG)){
		UARTprintf("Feed rate missing from G01 command\n");
		return;
	}

	if(!FLAG_CHECK(value_flag, X)){
		xyz[X] = Global_Position[X];
	}

	if(!FLAG_CHECK(value_flag, Y)){
		xyz[Y] = Global_Position[Y];
	}

	if(!FLAG_CHECK(value_flag, Z)){
		xyz[Z] = Global_Position[Z];
	}

	motion_linear(xyz, f);
	//send x, y, z and f to motion planner

}

void print_position(){
	float *pos = Global_Position;
	uint32_t x_int,y_int,z_int;
	uint32_t x_man, y_man, z_man;

	stepper_position(pos);
	x_int = (uint32_t)pos[X];
	y_int = (uint32_t)pos[Y];
	z_int = (uint32_t)pos[Z];

	x_man = (uint32_t)(100*(pos[X] - x_int));
	y_man = (uint32_t)(100*(pos[Y] - y_int));
	z_man = (uint32_t)(100*(pos[Z] - z_int));

	UARTprintf("X: %i.%i, Y: %i.%i Z: %i.%i\n", x_int, x_man, y_int, y_man, z_int, z_man);
}


