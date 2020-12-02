/*
 * clock.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Header file for Instantiation and functionalities for clock and TPM
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 *		Links:  https://github.com/alexander-g-dean/ESF/tree/master/NXP/Code/Chapter_7/PWM_LED
 */

#ifndef CLOCK_H_
#define CLOCK_H_

// Shamelessly from https://www.youtube.com/watch?v=uiSTB4jkxhw

#define XTAL_FREQ			(8000000u) 	/* Hz, FRDM-KL25Z has an on-board 8 MHz xtal */
#define XTAL_PEE_DIVIDE		(4u)		/* divide by 4 (8 MHz --> 2 MHz) */
#define XTAL_PEE_UPSCALE	(24u)		/* scale up by 24 (2 MHz --> 48 MHz) */

#define CORE_CLOCK			(XTAL_FREQ/XTAL_PEE_DIVIDE*XTAL_PEE_UPSCALE) /* Hz */

#define PWM_PERIOD (48000)	/* PWM Brighness Period */
#define FULL_ON (PWM_PERIOD-1)
#define FULL_OFF (0)

/**
 * @brief Enables the Main Proessor clock to 48Mhz
 *
 * ​ @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
void InitClock();

/**
 * @brief Instantiaes and Sets up TPM0 and TPM2
 *
 * ​ @param​ ​ none
​ *
​ * ​ ​@return​ ​ none
 */
void InitTPM();

#endif /* CLOCK_H_ */
