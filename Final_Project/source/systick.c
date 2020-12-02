/*
 * systick.c
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Mangement of Sytick Timer and Intrrupt
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 * 		Links: Inspired by https://learningmicro.wordpress.com/configuring-device-clock-and-using-systick-system-tick-timer-module-to-generate-software-timings/
 */



#include "MKL25Z4.h"
#include "sysclock.h"
#include "systick.h"
#include "fsl_debug_console.h"
#include "global_defs.h"

/**
 * @brief The system tick counter
 */
volatile uint32_t SystemMilliseconds = 0;
volatile ticktime_t trans_tick;
volatile ticktime_t Timer_U32;

volatile ticktime_t g_program_start;
volatile ticktime_t g_timer_start;

/**
 * @brief 250µs Counter
 */
static uint32_t freeRunner = 0;


/**
​ * ​ ​ @brief​ ​  Instantiate a Systick Timer
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ none
​ */
void InitSysTick() {

	SysTick->LOAD = (SYSTICK_TMR_RELOAD_VAL);  // 1000 Hz
	NVIC_SetPriority(SysTick_IRQn, 3); // NVIC Interrupt Priority // 3
	NVIC_ClearPendingIRQ(SysTick_IRQn); // Clear Pending IRq's
	NVIC_EnableIRQ(SysTick_IRQn);
	SysTick->VAL = 0; // Clear Timer
	SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk ;  // Mask to Initialize TIcks, Enamble CTRL Mask and use Processer CLock Source of 48 Mhz

	Timer_U32 = 0; // Overall CLock - Initialization Precauton
	g_program_start = g_timer_start = 0;
	MSG_DEBUG("\n\r Clock Gating and Initialization of SysTick Complete ");
}

/**
​ * ​ ​ @brief​ ​  This functions returns the time in ms since the power on.
                Max time=0xffffffff ms after that it rolls back to 0.
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ none
​ */
ticktime_t now() {
	return Timer_U32 - g_program_start;
}


/**
​ * ​ ​ @brief​ ​ Resets the Flags and Trans_tick to DEFAULT(0)
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ none
​ */
void reset_timer() {
	g_timer_start = Timer_U32;
}


/**
​ * ​ ​ @brief​ ​ Returns the number of ticks from reset
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ Integer - Number of Ticks
​ */
ticktime_t get_timer() {

	return (Timer_U32 - g_timer_start);
}


/**
​ * ​ ​ @brief​ ​  TIRQ Handler to Update Time.
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ none
​ */
void SysTick_Handler() {
	SystemMilliseconds += ((++freeRunner) & 0b100) >> 2;
	freeRunner &= 0b11;

	Timer_U32++; // Keep Track of the total timer
}
