/*
 * queue.c
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Instantiation and functionalities for Circular Buffer
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 */

#include "MKL25Z4.h"
#include "queue.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>


/*
 * Initializing the FIFO
 *
 * Parameters:
 *   Q_T : Queue Object
 *
 * Returns:
 *   void
 */
void Q_Init(Q_T * q) {
	q->write = 0;
	q->read = 0;
	q->Full_Status = false;
	q->size = 0;
	for (int i=0; i<MAX_SIZE; i++)
	    q->data[i] = '_';  // to simplify our lives when debugging
}


/*
 * Returns the FIFO's capacity
 *
 * Parameters:
 *   Q_T : Queue Object
 *
 * Returns:
 *   The capacity, in bytes, for the FIFO
 */
size_t Q_Capacity(Q_T * q) {
	return MAX_SIZE;
}

/*
 * Helper function to check if the cB is empty
 *
 * Parameters:
 *   Q_T : Queue Object
 *
 * Returns:
 *   bool: True/False
 */
bool Q_Empty(Q_T * q) {
	assert(q);
	return (q->write == q->read);
}

/*
 * Helper function to sanity check the current size of the Buffer
 *
 * Parameters:
 *   Q_T : Queue Object
 *
 * Returns:
 *   bool: True/False
 */
int Q_Size(Q_T * q) {
	assert(q);
	return q->size;
}


/*
 * Helper function to check if the cB is at complete capacity
 *
 * Parameters:
 *   Q_T : Queue Object
 *
 * Returns:
 *   bool: True/False
 */
bool Q_Full(Q_T * q){
	assert(q);
	return (Q_Length(q) == MAX_SIZE);
}


/*
 * Returns the number of bytes currently on the FIFO.
 *
 * Parameters:
 *   Q_T : Queue Object
 *
 * Returns:
 *   Number of bytes currently available to be dequeued from the FIFO
 */
size_t Q_Length(Q_T * q){
	uint32_t masking_state;
	size_t val=0;

	masking_state = __get_PRIMASK();
	START_CRITICAL();

	if(q->Full_Status){
		val = MAX_SIZE;
	}
	else if(q->write >= q->read){
		val = q->write - q->read;
	}
	else {
		val = MAX_SIZE - (q->read - q->write);
	}
	END_CRITICAL(masking_state);
	return val;
}


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
size_t Q_Enqueue(Q_T * q, const void *buf , size_t nbyte) {
	size_t len1 = 0;
	size_t len2=0;
	uint32_t masking_state;

	if(q->Full_Status) {
		return 0;
	}

	masking_state = __get_PRIMASK();
	START_CRITICAL();

	if(Q_Empty(q)) {
		len1 = nbyte;
		q->write = len1;
		if(nbyte == MAX_SIZE) {
			len1 = MAX_SIZE;
			q->Full_Status = true;
			q->write = 0;
		}
		memcpy(q->data, buf, len1);
		q->read = 0;
		q->size += (len1 + len2);
		END_CRITICAL(masking_state);
		return len1 + len2;
	}

	if(q->read < q->write){
		len1 = min(nbyte, MAX_SIZE - q->write);
		memcpy(q->data + q->write, buf, len1);
		q->write += len1;


		if (q->write < MAX_SIZE) {
			END_CRITICAL(masking_state);
					return len1 + len2;
		}

		q->write = 0;
		if(q->read == 0) {
			q->Full_Status = true;
			q->size += (len1 + len2);
			END_CRITICAL(masking_state);
					return len1 + len2;
		}

		nbyte -= len1;
		buf += len1;
	}


	// 2nd stage
	len2 = min(nbyte, q->read - q->write);
	memcpy(q->data + q->write, buf, len2);
	q->write += len2;

	if(q->write == q->read) {
		q->Full_Status = true;
	}
	q->size+= (len1 + len2);

	END_CRITICAL(masking_state);
	return len1 + len2;

}

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
size_t Q_Dequeue(Q_T * q, void *buf , size_t nbyte) {

	size_t len1 = 0, len2=0;
	uint32_t masking_state;

	masking_state = __get_PRIMASK();

	START_CRITICAL();

	if(Q_Empty(q) && !q->Full_Status) {
		q->size-= (len1 + len2);
		END_CRITICAL(masking_state);
		return len1 + len2;
	}

	q->Full_Status = false;

	len1 = min(nbyte, MAX_SIZE - q->read);
	if((q->write > q->read) && (len1 > q->write - q->read)) {
		len1 = q->write - q->read;
	}
	memcpy(buf, q->data + q->read, len1);
	q->read += len1;
	if(q->read < MAX_SIZE) {
		q->size-= (len1 + len2);
		END_CRITICAL(masking_state);
		return len1 + len2;
	}


	len2 = min(nbyte - len1, q->write);
	memcpy(buf+len1, q->data, len2);
	q->read = len2;

	END_CRITICAL(masking_state);
	return len1 + len2;

}
