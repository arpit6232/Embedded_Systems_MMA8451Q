/*
 * i2carbiter.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Header file for i2carbiter.h to settle a dispute or has ultimate
 *      		authority in a matter in case multiple sensor update is required.
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 */

#ifndef I2CARBITER_H_
#define I2CARBITER_H_

#include "MKL25Z4.h"

/**
 * @brief Data structure for the I2C arbiter
 */
typedef struct {
	const uint8_t slaveAddress;		/*< The 7-bit slave address */
	const uint32_t hash;			/*< Hash of the fields to aid in switching decisions */
	PORT_Type * port;			/*< The port for I2C communication */
	const uint32_t sdaPin;			/*< The pin used to drive SDA */
	const uint32_t sclPin;			/*< The pin used to drive SCL */
	const uint8_t sdaMux;			/*< The mux value for the SDA pin */
	const uint8_t sclMux;			/*< The mux value for the SCL pin */
} i2carbiter_entry_t;


/**
 * @brief Configures n I2C arbiter entry
 * @param[inout] entry The entry
 * @param[in] slaveAddress The 7-bit slave address
 * @param[in] port The port to use
 * @param[in] sdaPin The number of the pin used for SDA
 * @param[in] sclPin The number of the pin used for SCL
 */
void I2CArbiter_PrepareEntry(i2carbiter_entry_t *entry, uint8_t slaveAddress, uint32_t sclPin, uint8_t sclMux, uint32_t sdaPin, uint8_t sdaMux);

/**
 * @brief Configures the I2C arbiter
 * @param[inout] config The control structure
 * @param[in] entries The entries
 * @param[in] entryCount the number of entries
 */
void I2CArbiter_Configure(i2carbiter_entry_t *entries, uint8_t entryCount);

/**
 * @brief Selects an I2C slave and prepares the ports.
 * @param[in] slaveAddress The slave address
 * @return Zero if successful, nonzero otherwise
 */
uint8_t I2CArbiter_Select(uint8_t slaveAddress);


#endif /* I2CARBITER_H_ */
