/*
 * buffer.c
 *
 *  Created on: Nov 23, 2020
 *      Author: root
 */


#include "MKL25Z4.h"
#include "buffer.h"

/**
 * @brief Resets the ring buffer
 * @param[in] buffer The ring buffer instance
 * @param[in] data The data buffer to be used; Length must be at least a size of two.
 * @param[in] size The data buffer size; Must be a size of two
 */
uint8_t RingBuffer_Init(buffer_t *const buffer, uint8_t (*const data)[], const uint32_t size)
{
	// only allow buffers sizes that are a power of two
	if ((size & (size-1)) != 0)
	{
		return 1;
	}

    // cast to remove const qualifier -- ESPECIALLY not enabled on GCC
	*(uint8_t**)(&buffer->data) = (uint8_t*)*data;
	*(uint32_t*)(&buffer->size) = size;
	*(uint32_t*)(&buffer->mask) = size-1;
	buffer->writeIndex = buffer->readIndex = 0;

	return 0;
}
