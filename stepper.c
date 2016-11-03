#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

#include "config.h"
#include "stepper.h"
#include "planner.h"

typedef struct{
	Linear_Motion motion;
	float xyz[3];
	volatile bool update;
	volatile bool busy;
}Stepper_t;

Stepper_t stepper;

void stepper_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);												//Steps port initialization
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, STEPS_MASK);											//Steps port initialization

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);												//Direction port initialization
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, DIRECTION_MASK);										//Direction port initialization

	GPIOPadConfigSet(GPIO_PORTF_BASE, STEPS_MASK, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD); 	//8mA drive with slew rate control, Push-pull, Both steps and direction ports
	GPIOPadConfigSet(GPIO_PORTE_BASE, DIRECTION_MASK, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);	//8mA drive with slew rate control, Push-pull, Both steps and direction ports

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);												//Timer 0 initialization, Full-width periodic timer
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	IntEnable(INT_TIMER0A);																		//Enable Interrupts for Timer 0
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);															//Enbale Timer0
}

void Stepper_ISR(void){
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);									//clear interrupt
	GPIOPinWrite(GPIO_PORTE_BASE, DIRECTION_MASK, stepper.motion.direction_bits);	//set steppers direction first
	GPIOPinWrite(GPIO_PORTF_BASE, STEPS_MASK, stepper.motion.step_outbits);			//pulse steppers according to the bresenham line displacement algorithm
	stepper.motion.step_outbits = 0;												//reset stepper pulse bits

	TimerLoadSet(TIMER0_BASE, TIMER_A, stepper.motion.min_delay);					//load next timer TODO: not an ideal implementation of delays, Fix this


	 //Perform Bresenham Line algorithm displacement
	if(stepper.motion.total_steps != stepper.motion.step_count){

		stepper.motion.counter[X] += stepper.motion.steps[X];
		if (stepper.motion.counter[X] > stepper.motion.total_steps) {
			stepper.motion.step_outbits |= X_STEPS;
			stepper.motion.counter[X] -= stepper.motion.total_steps;
			if (stepper.motion.direction_bits & X_DIRECTION_BIT) { stepper.xyz[X] -= 0.0125; }
			else { stepper.xyz[X] += 0.0125; }
		}

		stepper.motion.counter[Y] += stepper.motion.steps[Y];
		if (stepper.motion.counter[Y] > stepper.motion.total_steps) {
			stepper.motion.step_outbits |= Y_STEPS;
			stepper.motion.counter[Y] -= stepper.motion.total_steps;
			if (stepper.motion.direction_bits & Y_DIRECTION_BIT) { stepper.xyz[Y] -= 0.0125; }
			else { stepper.xyz[Y] += 0.0125; }
		}

		stepper.motion.counter[Z] += stepper.motion.steps[Z];
		if (stepper.motion.counter[Z] > stepper.motion.total_steps) {
			stepper.motion.step_outbits |= Z_STEPS;
			stepper.motion.counter[Z] -= stepper.motion.total_steps;
			if (stepper.motion.direction_bits & Z_DIRECTION_BIT) { stepper.xyz[Z] -= 0.00039; }
			else { stepper.xyz[Z] += 0.00039; }
		 }

		  GPIOPinWrite(GPIO_PORTF_BASE, STEPS_MASK, 0);
		  stepper.update = true;					//request update
		  stepper.motion.step_count++;

	}else if(stepper.motion.total_steps == stepper.motion.step_count){
		stepper.busy = false;
		GPIOPinWrite(GPIO_PORTF_BASE, STEPS_MASK, 0);
		TimerDisable(TIMER0_BASE, TIMER_A);
	}
}

void stepper_prepare(Linear_Motion Motion){
	stepper.motion = Motion;
	stepper.busy = true;
	stepper.update = false;
	//set counter load to pulse after 1 clock cycles, enable interrupts for timerA
 	TimerEnable(TIMER0_BASE, TIMER_A);
	TimerLoadSet(TIMER0_BASE, TIMER_A, 1);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

bool busy(void){
	return stepper.busy;
}

bool update(void){
	return stepper.update;
}

void update_done(void){
	stepper.update = false;
}

void stepper_position(float *position){
	position[X] = (float)stepper.xyz[X];
	position[Y] = (float)stepper.xyz[Y];
	position[Z] = (float)stepper.xyz[Z];
}
