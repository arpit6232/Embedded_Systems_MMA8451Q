/*
 * statemachine.c
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Header file for  Instantiation and functionalities for Maaging State Machine
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 */

#include "bme.h"
#include "assert.h"
#include "clock.h"
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

#include "global_defs.h"


/* Structure for State Handling */
struct mma_state_t{
	state_t state;
} mma_t  = {
	.state = s_ROUTINE
};



/*!
* \def DATA_FUSE_MODE Set to the <code>1</code> to enable raw sensor data transmission or disable with <code>0</code> to enable data fusion
*/
#define DATA_FETCH_MODE 0

/*!
* \def DATA_FUSE_MODE Set to the opposite of {\ref DATA_FETCH_MODE} and enables sensor fusion
*/
#define DATA_FUSE_MODE (!DATA_FETCH_MODE)
static volatile uint8_t poll_mma8451q = 1;
volatile uint8_t flag;


/**
 * @brief Handler for interrupts on port A
 */
void PORTA_IRQHandler()
{

    register uint32_t isfr_mma = MMA8451Q_INT_PORT->ISFR;

	/* check MMA8451Q */
    register uint32_t fromMMA8451Q 	= (isfr_mma & ((1 << MMA8451Q_INT1_PIN) | (1 << MMA8451Q_INT2_PIN)));
		if (fromMMA8451Q) {
		PORTA->PCR[14] |= PORT_PCR_ISF_MASK;
//		uint8_t Int_SourceTrans = I2C_ReadRegister(MMA8451Q_I2CADDR, 0x1E);
		uint8_t Int_SourceTrans = I2C_ReadRegister(MMA8451Q_I2CADDR, 0x16);

		/* clear interrupts using BME decorated logical OR store */
		PORTA->ISFR |= (1 << MMA8451Q_INT1_PIN) | (1 << MMA8451Q_INT2_PIN);
	}
		flag =1;
}


/**
 * @brief State Machine Function,
 * 				1) Updates the state and events in accordance to the Normal Run Vs Jerk Detection
 * 				2) Initializes the State with the Stop State
 * 				3) Updates Operation based on interrupt handled in PORTA_Irq
 * 				4) Brighness increases with increase in Angle
 * 				5) LED flash in case of Jolt or Jerk. ​
 *
 * ​ @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
void state_machine(void) {

	// Instantiate States
	state_t new_state = mma_t.state;

	// Instantiate Acceleration Object
	mma8451q_acc_t acc;

	// Sets it to default which is Zero
	MMA8451Q_InitializeData(&acc);
	int readMMA;
	MSG_DEBUG("\n\r Initializing Inertial Sensor State Machine");


	while(1) {
		switch(new_state) {
		case s_ROUTINE:

			// Begin Standard Routine PWM Based LED functionaing
			reset_timer();
			flag = 0;
			MSG_DEBUG("\n\r Inertial Sensor Acceleration within Bounds");

			while(flag != 1) { // While Jerk is not detected
				LED_RedOff();
				readMMA = 1;

				// This functionality is provided so as to replace
				// This inertial sensor with any other sensor
				if (readMMA) {
					Control_RGB_LEDs(&acc);
				}
			}
			if(flag == 1) { // If jerk detected update State
				new_state = s_ACCEL;
			}
			break;

		case s_ACCEL: // Jerk Detected State
			reset_timer();
			MSG_DEBUG("\n\r Accelerated too Fast");

			// Flash LED until timeout when Jerk Detected.
			while(get_timer() < ACCEL_TIMEOUT) {
				Control_RGB_LEDs(&acc);
				delay_ms(100);
				GREEN_PWM = 0;
				BLUE_PWM = 0;
				delay_ms(100);
			}
			flag = 0;
			// Update State
			new_state = s_ROUTINE;
		}
	}

}
