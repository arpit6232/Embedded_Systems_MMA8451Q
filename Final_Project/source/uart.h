/*
 * queue.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Header file for instantiation and functionalities for communication over UART
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 * 		Links: https://github.com/alexander-g-dean/ESF/tree/master/NXP/Code
 */

#ifndef UART_H_
#define UART_H_

#include "bme.h"
#include "MKL25Z4.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define USE_UART_INTERRUPTS 	(0) // 0 for polled UART communications, 1 for interrupt-driven
#define UART_OVERSAMPLE_RATE 	(16)
#define BUS_CLOCK 				(24e6)
#define SYS_CLOCK				(48e6)

// critical section macro functions
#define START_CRITICAL()	__disable_irq()
#define END_CRITICAL(x)	__set_PRIMASK(x)


/*
 * Initializing the UART for BAUD_RATE: 38400, Data Size: 8, Parity: None, Stop Bits: 2
 *
 * Parameters:
 *   baud_rate: uint32_t for the requested baud rate
 *
 * Returns:
 *   void
 */
void Init_UART0(uint32_t baud_rate);


/*
 * Transmits String over to UART
 *
 * Parameters:
 *   str: String to Transmit over UART
 *	 count: The Length of the String to transmit
 * Returns:
 *   void
 */
void Send_String(const void* str, size_t count);


/*
 * Receive the Data from UART to Receive Buffer to store
 *
 * Parameters:
 *   str: String to Transmit over UART
 *	 count: The Length of the String to transmit
 * Returns:
 *   void
 */
size_t Receive_String(void* str, size_t count);


#endif /* UART_H_ */
