/*
 * systick.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Header File for Mangement of Sytick Timer and Interrupt
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 * 		Links: Inspired by https://learningmicro.wordpress.com/configuring-device-clock-and-using-systick-system-tick-timer-module-to-generate-software-timings/
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

/**
* @brief Defines for the system tick behaviour
*/
typedef uint32_t ticktime_t;

#define SYSTICK_FREQUENCY		(4000u) /* Hz */

#define SYSTEM_CLOCK_FREQ      48000000UL  // 48 Mhz
#define SYTICK_TIME_FREQ       (4000U) // 1000 Khz
#define SYSTICK_TMR_RELOAD_VAL ((SYSTEM_CLOCK_FREQ / SYTICK_TIME_FREQ) - 1UL) // 48000 - 1

/**
* @brief Function to initialize the SysTick interrupt
*/
void InitSysTick();

/**
​ * ​ ​ @brief​ ​  This functions returns the time in ms since the power on.
                Max time=0xffffffff ms after that it rolls back to 0.
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ none
​ */
ticktime_t now(); // returns time since startup


/**
​ * ​ ​ @brief​ ​ Resets the Flags and Trans_tick to DEFAULT(0)
 *           Doesn't affect now() values
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ none
​ */
void reset_timer();


/**
​ * ​ ​ @brief​ ​ Returns the number of ticks from reset
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ Integer - Number of Ticks
​ */
ticktime_t get_timer();


#endif /* SYSTICK_H_ */
