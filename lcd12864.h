/*
 * 12864LCD.h
 *
 *  Created on: Feb 6, 2016
 *      Author: amart
 */
#ifndef lcd12864_H_
#define lcd12864_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#define DELAY						2600
#define UPPER_8BIT(B)				(B & 0xF0)
#define LOWER_8BIT(B)				((B << 4) & 0xF0)
#define LOWER_16BIT(B)				(B & 0xFF)
#define UPPER_16BIT(B)				((B >> 8) & 0xFF)


#define CGRAM_ADDRESS(B)		(0x40|B)			//make CGRAM Address, AC range is 00H..3FH
#define DDRAM_ADDRESS(B)		(0x80|B)			//make DDRAM Address  First line AC range is 80H..8FH, Second line AC range is 90H..9FH, Third line AC range is A0H..AFH
													//Fourth line AC range is B0H..BFH
#define SCROLL_ADDRESS(B)		(0x40|B)			//make scroll address
#define GRAM_ADDRESS(B)			(0x80|B)			//make Graphic RAM address

#define	COMMAND					0xF8
#define	DATA					0xFA

//Basic Instruction Set
#define CLEAR					0x01			//Fill DDRAM with "20H"(space code). And set DDRAM address counter AC to"00H". Set entry mode I/D bit to be "1".
#define HOME					0x02			//Set DDRAM address counter AC to "00H". Cursor moves to origin. Then content of DDRAM is not changed.
#define ENTRY_MODE_RIGHT		0x06			//increment AC by one shift to the right
#define ENTRY_MODE_LEFT			0x07			//decrement AC by one shift to the left
#define DISPLAY_1				0x0F			//Display ON, Cursor ON, Blink ON
#define DISPLAY_2				0x0C			//Display ON, Cursor OFF, Blink OFF
#define CURSOR_RIGHT			0x10			//Cursor moves right by 1
#define CURSOR_LEFT				0x14			//Cursor moves left by 1
#define DISPLAY_CURSOR_RIGHT	0x18			//Display shift left by 1, cursor also follows to shift.
#define DISPLAY_CURSOR_LEFT		0x1C			//Display shift right by 1, cursor also follows to shift.
#define FUNCTION_1				0x30			//8-Bit parallel, Basic Instruction set
#define FUNCTION_2				0x34			//8-Bit parallel, Extended Instruction set
#define FUNCTION_3				0x20			//4-Bit parallel or Serial, Basic Instruction set
#define FUNCTION_4				0x24			//4-Bit parallel or Serial, Extended Instruction set

//Extended Instruction Set
#define STAND BY 				0x01
#define	VERTICAL_SCROLL			0x03
#define	RAM_SELECT				0x02			//the IRAM address set (extended instruction) and CGRAM address set(basic instruction) is enabled
#define REVERSE_1				0x04			//First line normal or reverse
#define REVERSE_2				0x05			//Second line normal or reverse
#define REVERSE_3				0x06			//Third line normal or reverse
#define REVERSE_4				0x07			//Fourth line normal or reverse
#define EXTENDED_FUNCTION_1		0x36			//8-Bit parallel, Extended Instruction, Graphics ON
#define EXTENDED_FUNCTION_2		0x34			//8-Bit parallel, Extended Instruction, Graphics OFF
#define EXTENDED_FUNCTION_3		0x26			//4-Bit parallel or Serial, Extended Instruction, Graphics ON
#define EXTENDED_FUNCTION_4		0x24			//4-Bit parallel or Serial, Extended Instruction, Graphics OFF


#define GRAPHICS_ON				1
#define GRAPHICS_OFF			0

#define RIGHT					1
#define LEFT					0

#define LCD_CLK 				GPIO_PIN_2
#define LCD_DATA 				GPIO_PIN_5
#define LCD_CE 					GPIO_PIN_3

#define LCD_WIDTH 				128
#define LCD_HEIGHT 				64


void LCD_init(void);
void LCD_draw(uint8_t pattern);
void LCD_pixel(uint8_t x, uint8_t y);
void LCD_clear(void);
void LCD_set_cursor(uint8_t x, uint8_t y);
void LCD_ROM_string(const char *string);

#endif /* 12864LCD_H_ */
