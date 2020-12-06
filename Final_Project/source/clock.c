/*
 * clock.c
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Instantiation and functionalities for Processor Clock and TPM
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 *		Links:  https://github.com/alexander-g-dean/ESF/tree/master/NXP/Code/Chapter_7/PWM_LED
 */

#include "clock.h"
#include "sysclock.h"
#include "MKL25Z4.h"
#include "stdint.h"
#include "global_defs.h"


void InitClock() {

	// Instantiate Clock to 48 Mhz
	sysclock_init(); // 48Mhz
}

void InitTPM() {

	// CLock Gating TPM
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK | SIM_SCGC6_TPM2_MASK;

	//set clock source for tpm: 48 MHz
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

	//load the counter and mod
	TPM0->MOD = PWM_PERIOD-1;
	TPM2->MOD = PWM_PERIOD-1;

	// Prescalar set to 1, no division
	TPM0->SC |= (TPM_SC_CPWMS(0)| TPM_SC_CMOD(1));
	TPM2->SC |= (TPM_SC_CPWMS(0)| TPM_SC_CMOD(1));

	// Set channel 1 to edge-aligned low-true PWM
	TPM0->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;
	TPM2->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;
	TPM2->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;

	// Clock prescaler is 7 (PWM clock devided by 128)
	// This makes PWM clock as 48000000/128 = 375000 Hz (375Khz)
	TPM0->SC |= TPM_SC_PS(7);
	TPM2->SC |= TPM_SC_PS(7);

	// Setting Initial Duty cycle to 0
	TPM2->CONTROLS[0].CnV = 0;
	TPM2->CONTROLS[1].CnV = 0;
	TPM0->CONTROLS[1].CnV = 0;

	LOG("\n\r Clock Gating and Instantiation for PWM Complete");

}

