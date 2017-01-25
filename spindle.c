#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"

#include "spindle.h"
#include "planner.h"
#include "parser.h"
#include "config.h"

uint32_t period;
uint16_t duty_cycle;

void spindle_init(void){
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD_WPU);
	GPIOPinConfigure(GPIO_PD0_M1PWM0);

	uint32_t PWMClock = SysCtlClockGet() / 64;
	period = (PWMClock / MAX_SPINDLE_FREQUENCY) - 1;

	PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, period);				//1 KHz frequency
	PWMGenEnable(PWM1_BASE, PWM_GEN_0);
}


//Speed in RPM
void spindle_run(uint8_t mode, float speed){
	float adjust = speed/MAX_SPINDLE_FREQUENCY;

	duty_cycle = period * adjust;
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, duty_cycle);

	switch(mode){
	case TURN_SPINDLE_CW:
		//turn on MOSFET here
		PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
		break;
	case TURN_SPINDLE_CCW:
		//only if I can implement an H_Bridge
		break;
	case STOP_SPINDLE:
		while(!buffer_empty());							//wait for queue to empty to continue shut down spindle
		PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, false);
		//turn off MOSFET
		break;
	}
}

