/*
 * led.c
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Instantiates the LED to interact with the PWM/TPM and adjust brightness
 *      		in accordance to MMA8451Q Tilt angles (Roll, Pitch).
 *      		Green : Indicates Roll
 *      		Blue  : Indicates Pitch
 *      		Increasing Brightness indicates higher angles
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 *
 */

#include "led.h"
#include "MKL25Z4.h"
#include "delay.h"
#include "clock.h"
#include "global_defs.h"


void LED_Init() {
		// Enable clock to ports B and D
		SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;;

		// Make 3 pins read for TPM/PWM Pins
		PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;
		PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(3);
		PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;
		PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(3);
		PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;
		PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(4);

		// Set ports to outputs
		PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
		PTD->PDDR |= MASK(BLUE_LED_POS);

		// Initialize to 0
		RED_PWM = 0;
		GREEN_PWM = 0;
		BLUE_PWM = 0;
}

void LED_Red()
{
	/* Turn on RED LEd,
	 * Turn off Green LED
	 * Turn off Blue LEd*/
	LED_RedOn();
	LED_GreenOff();
	LED_BlueOff();
}

void LED_Green()
{
	/* Turn off Red LEd,
	 * Turn on Green LED
	 * Turn off Blue LEd*/
	LED_RedOff();
	LED_GreenOn();
	LED_BlueOff();
}

void LED_Blue()
{
	/* Turn off RED LEd,
	 * Turn off Green LED
	 * Turn on Blue LEd*/
	LED_RedOff();
	LED_GreenOn();
	LED_BlueOff();
}


void LED_White()
{
	/* Turn on All LEd */
	LED_RedOn();
	LED_GreenOn();
	LED_BlueOn();
}

void LED_Off()
{
	/* Turn off LEd */
	LED_RedOff();
	LED_GreenOff();
	LED_BlueOff();
}

void Led_Down()
{
	/* Turn off all Led's to counter noisy
	 * readings */
	LED_Off();
	LED_Off();
	LED_Off();
	LED_Off();
}

void DoubleFlash()
{
	/* Double Flash all Leds to begin*/
	LED_White();
	delay_ms(50);
	Led_Down();
	delay_ms(50);
	LED_White();
	delay_ms(50);
	Led_Down();
}

void Control_RGB_LEDs(mma8451q_acc_t *acc) {

	// Initialize few variable
	float roll = 0.0,  pitch = 0.0;
	int PWM_Green=0, PWM_Blue = 0;

	// Read Accletation Data from MMA8451Q
	read_full_xyz(acc);

	// Convert acc to Roll and Pitch
	convert_xyz_to_roll_pitch(acc, &roll, &pitch);

	/* Convert 0-90 degree (Pitch and Roll) to PWM
	 * Range of (0-48000) */
	PWM_Green = (((int)roll * NEWRANGE ) / OLDRANGE);
	PWM_Blue = (((int)pitch * NEWRANGE ) / OLDRANGE);

	/* Orientation can be negative
	 * That should not concern LED lighting*/
	if(PWM_Blue < 533) {
		PWM_Blue = PWM_Blue *-1;
	}

	// Necessary to prevent loopback in atan2
	if((int) pitch > 80) {
		PWM_Green = 0;
	}

	// Set the PWM to appropriate brightness
	GREEN_PWM = PWM_Green;
	BLUE_PWM = PWM_Blue;

	// Debug Prints of Roll and Pitch
	MSG_DEBUG("\r\n roll: %d, pitch: %d", (int)roll, (int)pitch);
}

