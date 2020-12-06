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
#include "MKL25Z4.h"

#include "bme.h"
#include "assert.h"
#include "clock.h"
#include "systick.h"
#include "systick.h"
#include "delay.h"
#include "math.h"

#include "i2c.h"
#include "i2carbiter.h"
#include "uart.h"
#include "led.h"

#include "init_sensors.h"
#include "mma8451q.h"
#include "statemachine.h"

#include "test_i2c.h"
#include "global_defs.h"
#include "test_queue.h"


/************************************************************************/
/* UART Configurations                                                  */
/************************************************************************/
#define UART_BAUDRATE	115200


/************************************************************************/
/* I2C arbiter configuration                                            */
/************************************************************************/

#define I2CARBITER_COUNT 	(1)					/*< Number of I2C devices we're talking to */
i2carbiter_entry_t i2carbiter_entries[I2CARBITER_COUNT]; /*< Structure for the pin enabling/disabling manager */

void InitI2CArbiter()
{
    /* prior to configuring the I2C arbiter, enable the clocks required for
    * the used pins
    */
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK;

    /* configure I2C arbiter
    * The arbiter takes care of pin selection
    */
    I2CArbiter_PrepareEntry(&i2carbiter_entries[0], MMA8451Q_I2CADDR, MMA8451_SCL, MMA8451Q_I2C_MUX, MMA8451Q_SDA, MMA8451Q_I2C_MUX);
    I2CArbiter_Configure(i2carbiter_entries, I2CARBITER_COUNT);

    LOG("\r\n Interrupt Enabled for Jolt Detection on PORT A");
}

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Initialize Core Clock*/
    InitClock();

    /* Initialize UART */
    Init_UART0(UART_BAUDRATE);
    test_queue();

    /* initialize systick timer */
   	InitSysTick();
   	delay_ms(500);

   	/* Initialize PWM on LED Ports*/
   	InitTPM();
   	delay_ms(500);

   	/* initialize the RGB led */
   	LED_Init();
   	delay_ms(500);

   	/* Began the Code */
   	DoubleFlash();
   	Led_Down();

   	/* initialize the I2C bus */
   	I2C_Init();
   	delay_ms(500);

   	// Test I2C - MMA Functionality
   	test_i2c_mma();
   	delay_ms(500);

    LOG("\r\n ************************************************************************************");
    LOG("\r\n Welcome to MMA8451Q Sensor Suite for Acceleration Detection");
    LOG("\r\n ->  The Code runs in an state machine, which contains ");
    LOG("\r\n 		two state for PWM Orientation and Jolt detection respectively");
    LOG("\r\n -> For Jerk Detection Kindly Jolt kindly shake the Microcontroller");
    LOG("\r\n -> LED's light up according roll and pitch orientation of the microcontroller");
    LOG("\r\n ************************************************************************************");

    LOG("\r\n");

	LOG("\r\n ************************************************************************************");
	LOG("\r\n Kindly Follow the Manual Test Routine as in Manual_Test_Routine.pdf");
	LOG("\r\n ************************************************************************************");

	LOG("\r\n");

	/* initialize I2C arbiter */
	InitI2CArbiter();
	delay_ms(500);

	/* initialize the Sensor */
	InitMMA8451Q();
	delay_ms(500);

	/* Began the Code */
	Led_Down();

	LOG("\r\n ************************************************************************************");
	LOG("\r\n Orient Device to View Change in Orientation over LED");
	LOG("\r\n ************************************************************************************");

	/* State Machine */
	state_machine();

    return 0 ;
}
