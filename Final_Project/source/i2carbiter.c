/*
 * i2carbiter.c
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Instantiation and functionalities for Preparing Ports and i2c to read from sensor
 *      		The reason for this file is to provide the functionality to update KL25Z Ports to
 *      		subscribe from appropriate ports, as multiple sensors can be connected to other
 *      		Ports which can require a different setup
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 */

#include "MKL25Z4.h"
#include "bme.h"
#include "i2carbiter.h"


/**
 * @brief Control structure for the I2C arbiter
 */
typedef struct {
	uint32_t lastSelectedHash;		/*< The last selected hash */
	uint8_t lastSelectedSlave;		/*< The last selected slave address */
	uint8_t lastSelectedSlaveIndex;	/*< The list index of the last selected slave */
	i2carbiter_entry_t* entries;	/*< The arbiter entries */
	const uint8_t entryCount;		/*< The number of arbiter entries */
} i2carbiter_t;

/**
 * @brief The port configuration
 */
static i2carbiter_t configuration;

/**
 * @brief Configures n I2C arbiter entry
 * @param[inout] entry The entry
 * @param[in] port The port to use
 * @param[in] sdaPin The number of the pin used for SDA
 * @param[in] sclPin The number of the pin used for SCL
 */
void I2CArbiter_PrepareEntry(i2carbiter_entry_t *entry, uint8_t slaveAddress, uint32_t sclPin, uint8_t sclMux, uint32_t sdaPin, uint8_t sdaMux)
{
//	entry->port = port;
	*(uint8_t*)&entry->slaveAddress = slaveAddress;
	*(uint32_t*)&entry->sdaPin = sdaPin;
	*(uint8_t*)&entry->sdaMux = sdaMux;
	*(uint32_t*)&entry->sclPin = sclPin;
	*(uint8_t*)&entry->sclMux = sclMux;

	/* hash the unique configuration
	 * The number 37 and 23 are arbitrary co-primes.
	 */
	register uint32_t hash = 37;
	hash = hash * 23 + (uint32_t)PORTE;
	hash = hash * 23 + sclPin;
	hash = hash * 23 + sdaPin;

	*(uint32_t*)&entry->hash = hash;
}

/**
 * @brief Configures the I2C arbiter
 * @param[inout] config The control structure
 * @param[in] entries The entries
 * @param[in] entryCount the number of entries
 */
void I2CArbiter_Configure(i2carbiter_entry_t *entries, uint8_t entryCount)
{
	configuration.lastSelectedSlave = 0;
	configuration.lastSelectedHash = 0;
	configuration.entries = entries;
	*(uint32_t*)&configuration.entryCount = entryCount;

	/* assume the first slave will be used first */
	I2CArbiter_Select(entries[0].slaveAddress);
}

/**
 * @brief Selects an I2C slave and prepares the ports.
 * @param[in] slaveAddress The slave address
 * @return Zero if successful, nonzero otherwise
 */
uint8_t I2CArbiter_Select(uint8_t slaveAddress)
{
	register uint32_t lastSelectedSlave = configuration.lastSelectedSlave;

	/* early exit */
	if (lastSelectedSlave == slaveAddress)
	{
		return 0;
	}

	/* find the current slave */
	register uint32_t lastSelectedHash = configuration.lastSelectedHash;
	register int count = configuration.entryCount;
	for (int i=0; i<count; ++i)
	{
		i2carbiter_entry_t* token = &configuration.entries[i];
		if (token->slaveAddress == slaveAddress)
		{
			/* try to avoid switching by comparing the port address / pin hashes */
			if (token->hash != lastSelectedHash)
			{
				/* disable last slave */
				if (lastSelectedSlave != 0)
				{
					/* disable last selected slave */
					i2carbiter_entry_t* entry = &configuration.entries[configuration.lastSelectedSlaveIndex];
					PORTE->PCR[entry->sdaPin] &= ~PORT_PCR_MUX_MASK;
					PORTE->PCR[entry->sclPin] &= ~PORT_PCR_MUX_MASK;
				}

				/* TODO: candidate for BME */

				/* enable new slave */
				PORTE->PCR[token->sdaPin] &= ~PORT_PCR_MUX_MASK;
				PORTE->PCR[token->sdaPin] |= PORT_PCR_MUX(token->sdaMux);
				PORTE->PCR[token->sclPin] &= ~PORT_PCR_MUX_MASK;
				PORTE->PCR[token->sclPin] |= PORT_PCR_MUX(token->sclMux);
			}

			/* set up lookup */
			configuration.lastSelectedHash = token->hash;
			configuration.lastSelectedSlave = slaveAddress;
			configuration.lastSelectedSlaveIndex = i;

			return 0;
		}
	}

	return 1;
}
