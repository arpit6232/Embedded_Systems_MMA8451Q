/*
 * sysclock.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Header file for Instantiation and functionalities for system clock based on MCG
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 * 		Links: Inspired by https://learningmicro.wordpress.com/configuring-device-clock-and-using-systick-system-tick-timer-module-to-generate-software-timings/
 */

#ifndef SYSCLOCK_H_
#define SYSCLOCK_H_


#define SYSCLOCK_FREQUENCY (48000000U)

/*
 * @brief: Initializes the system clock. You should call this first in your
 * 			program.
 *
 * 	@param: None
 * 	@return: None
 */
void sysclock_init();

#endif /* SYSCLOCK_H_ */
