#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_gpio.h"
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
#include "system.h"

Linear_Motion *stepper;

void stepper_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);												//steps port initialization
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, STEPS_MASK);											//steps port initialization

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);												//direction port initialization
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, DIRECTION_MASK);										//direction port initialization

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);												//enable port initialization
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, SLEEP | RESET);										//enable port initialization

	GPIOPadConfigSet(GPIO_PORTF_BASE, STEPS_MASK, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD); 		//8mA drive with slew rate control, Push-pull, Both steps and direction ports
	GPIOPadConfigSet(GPIO_PORTE_BASE, DIRECTION_MASK, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);	//8mA drive with slew rate control, Push-pull, Both steps and direction ports
	GPIOPadConfigSet(GPIO_PORTB_BASE, RESET | SLEEP, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);		//8mA drive with slew rate control, Push-pull, Both steps and direction ports

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);												//Timer 0 initialization, Full-width periodic timer
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	IntEnable(INT_TIMER0A);																		//Enable Interrupts for Timer 0
	IntMasterEnable();
	stepper = NULL;
}

void Stepper_ISR(void){
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);									//clear interrupt

	if(stepper == NULL){
		//check if queue empty
		if(buffer_empty()){
			//we are done no more motions to execute
			TimerDisable(TIMER0_BASE, TIMER_A);
			TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
			system_set(IDLE, true);
			return;
		}
		//serve next motion
		stepper = motion_serve();
	}

	GPIOPinWrite(GPIO_PORTE_BASE, DIRECTION_MASK, stepper->direction_bits);	//set steppers direction first
	GPIOPinWrite(GPIO_PORTF_BASE, STEPS_MASK, stepper->step_outbits);		//pulse steppers according to the bresenham line displacement algorithm
	stepper->step_outbits = 0;												//reset stepper pulse bits

	TimerLoadSet(TIMER0_BASE, TIMER_A, stepper->min_delay);


	 //Perform Bresenham Line algorithm displacement
	if(stepper->total_steps != stepper->step_count){

		stepper->counter[X] += stepper->steps[X];
		if (stepper->counter[X] > stepper->total_steps) {
			stepper->step_outbits |= X_STEPS;
			stepper->counter[X] -= stepper->total_steps;
			if (stepper->direction_bits & X_DIRECTION_BIT) { /*update*/ }
			else { /*update*/ }
		}

		stepper->counter[Y] += stepper->steps[Y];
		if (stepper->counter[Y] > stepper->total_steps) {
			stepper->step_outbits |= Y_STEPS;
			stepper->counter[Y] -= stepper->total_steps;
			if (stepper->direction_bits & Y_DIRECTION_BIT) { /*update*/ }
			else {/*update*/ }
		}

		stepper->counter[Z] += stepper->steps[Z];
		if (stepper->counter[Z] > stepper->total_steps) {
			stepper->step_outbits |= Z_STEPS;
			stepper->counter[Z] -= stepper->total_steps;
			if (stepper->direction_bits & Z_DIRECTION_BIT) { /*update*/ }
			else { /*update*/ }
		 }

		  GPIOPinWrite(GPIO_PORTF_BASE, STEPS_MASK, 0);
		  stepper->step_count++;

	}else if(stepper->total_steps == stepper->step_count){
		GPIOPinWrite(GPIO_PORTF_BASE, STEPS_MASK, 0);
		system_set(IDLE, true);
		stepper = NULL;
	}
}

void stepper_wakeup(void){
	// Enable stepper drivers.
	GPIOPinWrite(GPIO_PORTB_BASE, RESET, RESET);
	GPIOPinWrite(GPIO_PORTB_BASE, SLEEP, SLEEP);

	//set load to interrupt at 1 clock cycle
	TimerLoadSet(TIMER0_BASE, TIMER_A, 1);

    // Enable Stepper Driver Interrupt
	TimerEnable(TIMER0_BASE, TIMER_A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}


