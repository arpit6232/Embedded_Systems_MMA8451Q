/*
 * statemachine.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Header file for Instantiation and functionalities for Maaging State Machine
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "stdint.h"
#include "stdbool.h"

/* Flag for breaking between transition */
extern volatile uint8_t flag;

/* Data Structure to hold possible states */
typedef enum {

	s_ROUTINE,
	s_ACCEL
} state_t;

/* Timeout Period for flashing in case of jerk detection */
#define ACCEL_TIMEOUT 10000

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
void state_machine(void);




#endif /* STATEMACHINE_H_ */
