#include <string.h>
#include "lcd12864.h"

//TODO: lcd is working with ROM generated characters, characters are way too big to display anything useful
//graphic display should be used so that every space of the lcd can be used to display useful information
//the addressing is rather odd for the graphics ram (and for the cursor), to be finished


static void LCD_command(uint8_t command);
static void LCD_data(uint8_t data);
static void SSI_Send(uint8_t byte);
static void LCD_graphics(uint8_t mode);

#define LCD_DELAY		SysCtlDelay(DELAY);		\

#define LCD_DELAY_MS	SysCtlDelay(DELAY * 100);	\

#define COMMAND_MODE 	SSI_Send(COMMAND); LCD_DELAY;	\

#define DATA_MODE		SSI_Send(DATA); LCD_DELAY;	\

#define SEND_UPPER(B)	SSI_Send(UPPER_8BIT(B)); LCD_DELAY; \

#define SEND_LOWER(B)	SSI_Send(LOWER_8BIT(B)); LCD_DELAY;	\


void LCD_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);		//pin PA3 cs

	GPIOPinConfigure(GPIO_PA2_SSI0CLK);						//pin PA2 sclk
	GPIOPinConfigure(GPIO_PA5_SSI0TX);						//pin PA5 TX
	GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_2);
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 550000, 8);  // frequency at 550Khz, SSI Master, 8 bit frame format
	SSIEnable(SSI0_BASE);
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);

	LCD_command(FUNCTION_3);
	LCD_command(FUNCTION_3);
	LCD_command(DISPLAY_2);
	LCD_command(CLEAR);
	LCD_command(ENTRY_MODE_RIGHT);
	LCD_command(FUNCTION_3); 		//basic function set, graphics display off

	LCD_DELAY_MS;
}


static void LCD_command(uint8_t command){
	COMMAND_MODE;			// Command mode set
	SEND_UPPER(command);	// Upper first
	SEND_LOWER(command);	// then lower
}

static void LCD_data(uint8_t data){
	DATA_MODE;
	SEND_UPPER(data);
	SEND_LOWER(data);
}

static void SSI_Send(uint8_t byte){
	while(SSIBusy(SSI0_BASE));	//wait for data to be sent
	SSIDataPut(SSI0_BASE, byte);
}


static void LCD_GDRAM_address(uint8_t x, uint8_t y){
	LCD_command(GRAM_ADDRESS(y));
	LCD_command(GRAM_ADDRESS(x));
}



void LCD_clear(void){
	LCD_command(CLEAR);
}

void LCD_draw(uint8_t pattern){
	uint8_t rows, columns;

	for(rows = 0; rows < 64; rows++){
		if(rows < 32){
			for(columns = 0; columns < 8; columns++){
				LCD_GDRAM_address(columns, rows);
				LCD_data(pattern);
				LCD_data(pattern);
			}
		}
		else{
			for(columns = 0; columns < 8; columns++){
				LCD_GDRAM_address(0x08|columns, rows - 32);
				LCD_data(pattern);
				LCD_data(pattern);
			}
		}
	}

}

void LCD_pixel(uint8_t x, uint8_t y){
	uint8_t y1,x1, mask;

	y1 = y;
	x1 = x >> 4;
	mask = 0x8000 >> (x % 16);
	LCD_GDRAM_address(x1, y1);
	LCD_data(mask);
	LCD_data(mask);
}

static void LCD_graphics(uint8_t mode){
	switch(mode){
		case GRAPHICS_ON:
			LCD_command(EXTENDED_FUNCTION_4); 		//extended function set
			LCD_command(EXTENDED_FUNCTION_3);		//graphics on
			break;
		case GRAPHICS_OFF:
			LCD_command(FUNCTION_3);				//Basic Instruction Set
			LCD_command(FUNCTION_3);				//graphics off
			break;
	}
}

void LCD_set_cursor(uint8_t x, uint8_t y){
	switch(y){
	case 0:
		y = 0x80;			//First line AC range is 80H..8FH
		x = y|x;
		break;
	case 1:
		y = 0x90;			//Second line AC range is 90H..9FH
		x = y|x;
		break;
	case 2:
		y = 0x88;			//Third line AC range is A0H..AFH
		x = y|x;
		break;
	case 3:
		y = 0x98;			//Fourth line AC range is B0H..BFH
		x = y|x;
		break;
	}
	LCD_command(x);
}

void LCD_ROM_string(const char *string){
	while(*string){
		LCD_data(*string++);
	}
}

