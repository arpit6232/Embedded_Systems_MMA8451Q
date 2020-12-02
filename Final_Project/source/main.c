/******************************************************************************
*​​Copyright​​ (C) ​​2020 ​​by ​​Arpit Savarkar
*​​Redistribution,​​ modification ​​or ​​use ​​of ​​this ​​software ​​in​​source​ ​or ​​binary
*​​forms​​ is​​ permitted​​ as​​ long​​ as​​ the​​ files​​ maintain​​ this​​ copyright.​​ Users​​ are
*​​permitted​​ to ​​modify ​​this ​​and ​​use ​​it ​​to ​​learn ​​about ​​the ​​field​​ of ​​embedded
*​​software. ​​Arpit Savarkar ​​and​ ​the ​​University ​​of ​​Colorado ​​are ​​not​ ​liable ​​for
*​​any ​​misuse ​​of ​​this ​​material.
*
******************************************************************************/
/**
 * @file Final_Project.c
 * @brief Application Entry point for Lighting LED's through PWM based on roll and pitch of
 * 		  MMA8451Q Inertial Sensor.
 *
 * This file provides functions prototypes and abstractions and instantiation of to
 * tie into the MMA8451Q accelerometer over I2C. Changes the LED colors in
 * response to the device orientation, and also use the LED (blinking pattern)
 * to show sudden acceleration. Configures a GPIO interrupt so
 * the MMA8451Q can trigger the KL25Z when it detects the sudden
 * acceleration.
 *
 * @author Arpit Savarkar
 * @date November 27 2020
 * @version 1.3

  Sources of Reference :
  Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers

  @brief: Application Entry Point for the MMA8451Q accelerometer over I2C.
  Change the LED colors in response to the device orientation, and also use the LED
  (blinking pattern) to show sudden acceleration.
  Configured a GPIO interrupt so the MMA8451Q can trigger the KL25Z when it
  detects the sudden acceleration.

  I would like to thank Howdy Pierece and the SA's Rakesh Kumar and Saket Penurkar
  for his support during the bulding of this project

*/
 
/**
 * @file    Final_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

#include "bme.h"
#include "assert.h"
#include "clock.h"
#include "systick.h"
#include "systick.h"
#include "delay.h"
#include "buffer.h"
#include "math.h"

#include "i2c.h"
#include "i2carbiter.h"
#include "led.h"

#include "init_sensors.h"
#include "mma8451q.h"
#include "statemachine.h"


#define I2CARBITER_COUNT 	(1)					/*< Number of I2C devices we're talking to */
i2carbiter_entry_t i2carbiter_entries[I2CARBITER_COUNT]; /*< Structure for the pin enabling/disabling manager */


/************************************************************************/
/* I2C arbiter configuration                                            */
/************************************************************************/

void InitI2CArbiter()
{
    /* prior to configuring the I2C arbiter, enable the clocks required for
    * the used pins
    */
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK;

    /* configure I2C arbiter
    * The arbiter takes care of pin selection
    */
    I2CArbiter_PrepareEntry(&i2carbiter_entries[0], MMA8451Q_I2CADDR, 24, 5, 25, 5);
    I2CArbiter_Configure(i2carbiter_entries, I2CARBITER_COUNT);
}

/*
 * @brief   Application entry point.
 */
int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    PRINTF("Hello World\r\n");

    /* initialize the core clock and the systick timer */
//	InitClock();
	InitSysTick();
	InitTPM();

	/* initialize the RGB led */
	LED_Init();

	/* Began the Code */
	DoubleFlash();

	/* initialize the I2C bus */
	I2C_Init();
//	i2c_init();

//	/* initialize I2C arbiter */
	InitI2CArbiter();

	/* initialize the Sensor */
	InitMMA8451Q();
//	assert(init_mma());

	/* Began the Code */
	Led_Down();

	state_machine();

    return 0 ;
}
