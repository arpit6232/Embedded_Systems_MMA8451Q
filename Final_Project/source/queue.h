/*
 * queue.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Header file for instantiation and functionalities for Circular Buffer
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 */


#ifndef QUEUE_H_
#define QUEUE_H_

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define MAX_SIZE 1024

// critical section macro functions
#define START_CRITICAL()	__disable_irq()
#define END_CRITICAL(x)	__set_PRIMASK(x)
#define min(x,y) ((x)<(y)?(x):(y))

typedef struct {
	int write;
	int read;
	size_t size;
	bool Full_Status;
	uint8_t data[MAX_SIZE];
} Q_T;


/*
 * Initializing the FIFO
 *
 * Parameters:
 *   Q_T : Queue Object
 *
 * Returns:
 *   void
 */
extern void Q_Init(Q_T * q);


/*
 * Enqueues data onto the FIFO, up to the limit of the available FIFO
 * capacity.
 *
 * Parameters:
 *   buf      Pointer to the data
 *   nbyte    Max number of bytes to enqueue
 *   Q_T : Queue Object
 *
 * Returns:
 *   The number of bytes actually enqueued, which could be 0. In case
 * of an error, returns -1.
 */
extern size_t Q_Enqueue(Q_T * q, const void *buf , size_t nbyte);


/*
 * Attempts to remove ("dequeue") up to nbyte bytes of data from the
 * FIFO. Removed data will be copied into the buffer pointed to by buf.
 *
 * Parameters:
 *   buf      Destination for the dequeued data
 *   nbyte    Bytes of data requested
 *   Q_T : Queue Object
 *
 * Returns:
 *   The number of bytes actually copied, which will be between 0 and
 *  nbyte. In case of an error, returns -1.
 */
extern size_t Q_Dequeue(Q_T * q, void *buf , size_t nbyte);


/*
 * Returns the number of bytes currently on the FIFO.
 *
 * Parameters:
 *   Q_T : Queue Object
 *
 * Returns:
 *   Number of bytes currently available to be dequeued from the FIFO
 */
extern size_t Q_Length(Q_T * q);


/*
 * Returns the FIFO's capacity
 *
 * Parameters:
 *   Q_T : Queue Object
 *
 * Returns:
 *   The capacity, in bytes, for the FIFO
 */
extern size_t Q_Capacity(Q_T * q);


/*
 * Helper function to check if the cB is empty
 *
 * Parameters:
 *   Q_T : Queue Object
 *
 * Returns:
 *   bool: True/False
 */
extern bool Q_Empty(Q_T * q);


/*
 * Helper function to check if the cB is at complete capacity
 *
 * Parameters:
 *   Q_T : Queue Object
 *
 * Returns:
 *   bool: True/False
 */
extern bool Q_Full(Q_T * q);


/*
 * Helper function to sanity check the current size of the Buffer
 *
 * Parameters:
 *   Q_T : Queue Object
 *
 * Returns:
 *   bool: True/False
 */
extern int Q_Size(Q_T * q);

#endif /* QUEUE_H_ */
