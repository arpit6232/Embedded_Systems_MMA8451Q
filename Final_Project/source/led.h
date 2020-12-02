/*
 * led.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Header file for Instantiation and functionalities for LED to interact with the PWM
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 *
 */

#ifndef LED_H_
#define LED_H_

#include "MKL25Z4.h"
#include "clock.h"
#include "mma8451q.h"


// basic light switch
#define LED1_POS (1)	// on port A
#define LED2_POS (2)	// on port A
#define SW1_POS (5)		// on port A
#define FULL_BRIGHTNESS (48000)
#define RED_PWM 	TPM2->CONTROLS[0].CnV
#define GREEN_PWM 	TPM2->CONTROLS[1].CnV
#define BLUE_PWM 	TPM0->CONTROLS[1].CnV

// Helper defines to Convert Inertial Ranges to PWM
#define OLDRANGE (180)
#define NEWRANGE (48000)

#define MASK(x) (1UL << (x))

// Freedom KL25Z LEDs
#define RED_LED_POS (18)		// on port B
#define GREEN_LED_POS (19)	// on port B
#define BLUE_LED_POS (1)		// on port D

/**
 * @brief Disables the red LED
 *
 * ​ @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
#define LED_RedOff()	TPM2->CONTROLS[0].CnV = 0;

/**
 * @brief Enables the red LED
 *
 * ​ @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
#define LED_RedOn()		TPM2->CONTROLS[0].CnV = FULL_BRIGHTNESS;

/**
 * @brief Enables the green LED
 *  @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
#define LED_GreenOn()	TPM0->CONTROLS[1].CnV = 0;

/**
 * @brief Disables the green LED
 *  @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
#define LED_GreenOff()	TPM0->CONTROLS[1].CnV = FULL_BRIGHTNESS;

/**
 * @brief Enables the blue LED
 *  @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
#define LED_BlueOn()	TPM2->CONTROLS[1].CnV = 0;

/**
 * @brief Disables the blue LED
 *  @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
#define LED_BlueOff()	TPM2->CONTROLS[1].CnV = FULL_BRIGHTNESS;

/**
 * @brief Lights up LED based on PWM values raised by roll and pitch
 *  @param​ ​ mma8451q_acc_t *acc: Pointer to MMA8451Q based struct,
 *  		instantiated to default params
​ *
​ * ​ ​@return​ ​ none
 */
void Control_RGB_LEDs(mma8451q_acc_t *acc);

/**
 * @brief Sets up the GPIOs for LED driving
 *
 */
void LED_Init();

/**
 * @brief Lights the red LED
 *  @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
void LED_Red();

/**
 * @brief Lights the Green LED
 *  @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
void LED_Green();

/**
 * @brief Lights the Blue LED
 *  @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
void LED_Blue();

/**
 * @brief Lights all LEDs
 *  @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
void LED_White();

/**
 * @brief Disables all LEDs
 *  @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
void LED_Off();

/**
 * @brief Disables all LEDs multiple times to counter debounce effects
 *  @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
void Led_Down();

/**
 * @brief LED Double Flash!
 *  @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
void DoubleFlash();


#endif /* LED_H_ */
