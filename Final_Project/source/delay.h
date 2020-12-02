/*
 * delay.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Functionaliites for Busy Waiting Delay.
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 *
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "MKL25Z4.h"

/**
 * @brief The system tick counter
 */
extern const volatile uint32_t SystemMilliseconds;


/**
 * @brief Returns the current system time in milliseconds
 * @return The time.
 */
static inline uint32_t systemTime()
{
	return SystemMilliseconds;
}

/**
 * @brief Delays using the system tick counter
 *
 * @param[in] ms The delay time in milliseconds
 *
 * @return none.
 */
static inline void delay_ms(const uint16_t ms)
{
	const uint32_t start_ticks = SystemMilliseconds;
	do {
		__DSB();
		__WFI();
	} while((SystemMilliseconds - start_ticks) < ms);
}

#endif /* DELAY_H_ */
