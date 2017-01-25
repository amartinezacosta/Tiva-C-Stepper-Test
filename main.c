#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "parser.h"
#include "gcode.h"
#include "console.h"
#include "planner.h"
#include "system.h"
#include "lcd12864.h"
#include "stepper.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include "spindle.h"

#define G00	0
#define G01	1
#define G02	2
#define G03	3

#define X_FLAG	0
#define Y_FLAG	1
#define Z_FLAG	2
#define F_FLAG	3

#define BIT(n)							(1 << n)
#define FLAG_SET(A, mask)				(A |= 1 << mask)
#define FLAG_CHECK(A, mask)				((A & BIT(mask)) != 0)

float Global_Position[3];

void select_option(void);
void enter_command(void);
void enter_gcode_block(void);
void enter_serial_stream(void);
void parse_command(const char *data);
void print_position(void);
void lcd_update(void);


int main(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	LCD_init();
	stepper_init();
	spindle_init();
	InitConsole();
	system_init();
	buffer_init();
	UARTprintf("%c", 12);

	while(1){
		select_option();
	}
}


void select_option(void){
	UARTprintf("------Stepper test-------\n");
	UARTprintf("What do you want to do?\n");
	UARTprintf("[1.] Execute Linear Command\n");
	UARTprintf("[2.] Execute Block of Gcode\n");
	UARTprintf("[3.] Execute from Serial Stream\n");

	char str[64];
	UARTgets(str, sizeof(str));
	switch(*str){
	case '1':
		enter_command();
		break;
	case '2':
		enter_gcode_block();
		break;
	case '3':
		enter_serial_stream();
		break;
	default:
		UARTprintf("Wrong input, select [1] or [2]\n");
		return;
	}
}


void enter_command(void){
	char str[64];
	Block block_t = {0};
	int8_t status = 0;

	UARTprintf("----Stepper test-----\n\r");
	UARTprintf("Enter G00 or G01 Command\n\r");
	UARTgets(str, sizeof(str));

	status = parse_gcode(str, &block_t);
	if(status > 0){
		system_update(&block_t);				//update system
		system_set(EXECUTE, true);				//set system execute motion TODO: check that block has motion to execute
	}else{
		UARTprintf("Error %i\n", status);
	}
	system_execute();
}

void enter_gcode_block(void){
	UARTprintf("-------Stepper test---------\n\r");
	UARTprintf("Select block of gcode to be executed\n\r");
	UARTprintf("[1.] GrblGcodeSample.ngc\n");
	UARTprintf("[2.] utep.ngc\n");

	char str[64];
	uint16_t i = 0;
	int8_t status = 0;
	Block block_t;

	UARTgets(str, sizeof(str));

	switch(*str){
	case '1':
		while(GrblGcodeSample[i] != 0){
			memset(&block_t, 0, sizeof(Block));
			status = parse_gcode(GrblGcodeSample[i], &block_t);
			if(status > 0){
				system_update(&block_t);
			}else{
				UARTprintf("Error %i\n", status);
			}
			i++;
		}
		//system_set(EXECUTE, true);
		//system_execute();
		break;
	//add more options here
	case '2':
		while(utep[i] != 0){
			memset(&block_t, 0, sizeof(Block));
			status = parse_gcode(utep[i], &block_t);
			if(status > 0){
				system_update(&block_t);
			}else{
				UARTprintf("Error: %i\n", status);
			}
			i++;
		}
		//system_set(EXECUTE, true);
		//system_execute();
		break;
	default:
		UARTprintf("Wrong input select [1] through [9]\n");
		return;
	}
}

void enter_serial_stream(void){
	UARTprintf("-------Stepper test---------\n\r");
	UARTprintf("Waiting for Serial Stream to begin\n\r");

	char str[128];
	int8_t status = 0;
	Block block_t;

	while(1){
		UARTgets(str, sizeof(str));

		memset(&block_t, 0, sizeof(Block));
		status = parse_gcode(str, &block_t);
		if(status > 0){
			system_update(&block_t);
			UARTprintf("ok\n");
		}else{
			UARTprintf("Error %i\n", status);
		}
	}
}



void print_position(){
	float *pos = Global_Position;
	uint32_t x_int,y_int,z_int;
	uint32_t x_man, y_man, z_man;

	pos = system_get_position_rt();
	x_int = (uint32_t)pos[X];
	y_int = (uint32_t)pos[Y];
	z_int = (uint32_t)pos[Z];

	x_man = (uint32_t)(100*(pos[X] - x_int));
	y_man = (uint32_t)(100*(pos[Y] - y_int));
	z_man = (uint32_t)(100*(pos[Z] - z_int));

	UARTprintf("X: %i.%i, Y: %i.%i Z: %i.%i\n", x_int, x_man, y_int, y_man, z_int, z_man);
}

void lcd_update(){
	float *pos = Global_Position;
	uint32_t x_int,y_int,z_int;
	uint32_t x_man, y_man, z_man;
	char x_i[10], y_i[10], z_i[10];
	char x_m[10], y_m[10], z_m[10];

	pos = system_get_position_rt();
	x_int = (uint32_t)pos[X];
	y_int = (uint32_t)pos[Y];
	z_int = (uint32_t)pos[Z];

	x_man = (uint32_t)(100*(pos[X] - x_int));
	y_man = (uint32_t)(100*(pos[Y] - y_int));
	z_man = (uint32_t)(100*(pos[Z] - z_int));

	ltoa(x_int, x_i);
	ltoa(x_man, x_m);
	ltoa(y_int, y_i);
	ltoa(y_man, y_m);
	ltoa(z_int, z_i);
	ltoa(z_man, z_m);

	LCD_set_cursor(0,0);
	LCD_ROM_string("X:");
	LCD_ROM_string(x_i);
	LCD_ROM_string(".");
	LCD_ROM_string(x_m);

	LCD_set_cursor(0,1);
	LCD_ROM_string("Y:");
	LCD_ROM_string(y_i);
	LCD_ROM_string(".");
	LCD_ROM_string(y_m);

	LCD_set_cursor(0,2);
	LCD_ROM_string("Z:");
	LCD_ROM_string(z_i);
	LCD_ROM_string(".");
	LCD_ROM_string(z_m);
}


