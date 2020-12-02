/*
 * i2c.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Header file for Instantiation and functionalities for communication over I2C
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 * 		Links: Inspired from
 * 				1) https://github.com/sunsided/frdm-kl25z-marg-fusion/blob/master/frdm-kl25z-acc-uart/Project_Headers/i2c/i2c.h
 *				2) https://github.com/alexander-g-dean/ESF/tree/master/NXP/Code/Chapter_8/I2C-Demo
 */
#ifndef I2C_H_
#define I2C_H_

#include "MKL25Z4.h"
#include "delay.h"
#include "stdint.h"
#include "bme.h"

/**
 * Using Bit Manipulation Engine.
 */
#define I2C_USE_BME 1


/**
 *  @brief According to KINETIS_L_2N97F errata (e6070), repeated start condition can not be sent if prescaler is any other than 1 (0x0).
 *  Setting this define to a nonzero value activates the proposed workaround (temporarily disabling the multiplier).
 */
#define I2C_ENABLE_E6070_SPEEDHACK 	(1)
#define I2C_M_START 	I2C0->C1 |= I2C_C1_MST_MASK
#define I2C_M_STOP  	I2C0->C1 &= ~I2C_C1_MST_MASK
#define I2C_M_RSTART 	I2C0->C1 |= I2C_C1_RSTA_MASK

#define I2C_TRAN			I2C0->C1 |= I2C_C1_TX_MASK
#define I2C_REC				I2C0->C1 &= ~I2C_C1_TX_MASK

#define BUSY_ACK 	    while(I2C0->S & 0x01)
#define TRANS_COMP		while(!(I2C0->S & 0x80))
#define I2C_WAIT 			i2c_wait();

#define NACK 	        I2C0->C1 |= I2C_C1_TXAK_MASK
#define ACK           I2C0->C1 &= ~I2C_C1_TXAK_MASK

/**
 * @brief Encodes the read address from the 7-bit slave address
 */
#define I2C_READ_ADDRESS(slaveAddress) 		((uint8_t)((slaveAddress << 1) | 1))

/**
 * @brief Encodes the write address from the 7-bit slave address
 */
#define I2C_WRITE_ADDRESS(slaveAddress) 	((uint8_t)((slaveAddress << 1) | 0))

/**
 * @brief A mask that describes a no-or modify operation
 */
#define I2C_MOD_NO_OR_MASK	(0x0)

/**
 * @brief A mask that describes a no-and modify operation
 */
#define I2C_MOD_NO_AND_MASK	(~0x0)

/**
 * @brief Initializes the I2C interface
 *
 * @param: None
 * @return : None
 */
void I2C_Init();
/**
 * @brief Resets the bus by toggling master mode if the bus is busy. This will interrupt ongoing traffic.
 *
 * @param: None
 * @return: None
 */
void I2C_ResetBus();

/**
 * @brief Reads an 8-bit register from an I2C slave
 * @param[in] slaveId The device's I2C slave id
 * @param[in] registerAddress Address of the device register to read
 *
 * @return The value at the register
 */
uint8_t I2C_ReadRegister(register uint8_t slaveId, register uint8_t registerAddress);


/**
 * @brief Reads multiple 8-bit registers from an I2C slave
 * @param[in] slaveId The slave device ID
 * @param[in] startRegisterAddress The first register address
 * @param[in] registerCount The number of registers to read; Must be larger than zero.
 * @param[out] buffere The buffer to write into
 *
 * @return: None
 */
void I2C_ReadRegisters(register uint8_t slaveId, register uint8_t startRegisterAddress, register uint8_t registerCount, uint8_t *buffer);

/**
 * @brief Writes an 8-bit value to an 8-bit register on an I2C slave
 * @param[in] slaveId The device's I2C slave id
 * @param[in] registerAddress Address of the device register to read
 * @param[in] value The value to write
 *
 * @return: None
 */
void I2C_WriteRegister(register uint8_t slaveId, register uint8_t registerAddress, register uint8_t value);

/**
 * @brief Reads an 8-bit register from an I2C slave, modifies it by FIRST and-ing with {@see andMask} and THEN or-ing with {@see orMask} and writes it back
 * @param[in] slaveId The slave id
 * @param[in] registerAddress The register to modify
 * @param[in] orMask The mask to OR the register with
 * @param[in] andMask The mask to AND the register with
 *
 * @return The register after modification
 */
uint8_t I2C_ModifyRegister(register uint8_t slaveId, uint8_t register registerAddress, register uint8_t andMask, register uint8_t orMask);


/**
 * @brief Waits for an I2C bus operation to complete
 *
 * @param: None
 * @return: None
 */
__STATIC_INLINE void I2C_Wait()
{
	while((I2C0->S & I2C_S_IICIF_MASK)==0) {}	/* loop until interrupt is detected */

#if !I2C_USE_BME
	I2C0->S |= I2C_S_IICIF_MASK; /* clear interrupt flag */
#else
	BME_OR_B(&I2C0->S, ((1 << I2C_S_IICIF_SHIFT) << I2C_S_IICIF_MASK));
#endif
}

/**
 * @brief Waits for an I2C bus operation to complete
 *
 * @param: None
 * @param: None
 */
__STATIC_INLINE void I2C_WaitWhileBusy()
{
	while((I2C0->S & I2C_S_BUSY_MASK)!=0) {}
}


/**
 * @brief Sends a start condition and enters TX mode.
 *
 * @param: None
 * @return: None
 */
__STATIC_INLINE void I2C_SendStart()
{
#if !I2C_USE_BME
	I2C0->C1 |= ((1 << I2C_C1_MST_SHIFT) & I2C_C1_MST_MASK)
				| ((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK);
#else
	BME_OR_B(&I2C0->C1,
			  ((1 << I2C_C1_MST_SHIFT) & I2C_C1_MST_MASK)
			| ((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK)
		);
#endif
}

/**
 * @brief Enters transmit mode.
 *
 * @param: None
 * @return: None
 */
__STATIC_INLINE void I2C_EnterTransmitMode()
{
#if !I2C_USE_BME
	I2C0->C1 |= ((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK);
#else
	BME_OR_B(&I2C0->C1,
			((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK)
		);
#endif
}

/**
 * @brief Enters receive mode.
 *
 * @param: None
 * @return: None
 */
__STATIC_INLINE void I2C_EnterReceiveMode()
{
#if !I2C_USE_BME
	I2C0->C1 &= ~((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK);
#else
	BME_AND_B(&I2C0->C1,
			(uint8_t)
			~((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK)
		);
#endif
}

/**
 * @brief Enters receive mode and enables ACK.
 *
 * Enabling ACK may be required when more than one data byte will be read.
 *
 * @param: None
 * @return: None
 */
__STATIC_INLINE void I2C_EnterReceiveModeWithAck()
{
#if !I2C_USE_BME
	I2C0->C1 &= ~((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK)
			& ~((1 << I2C_C1_TXAK_SHIFT) & I2C_C1_TXAK_MASK);
#else
	BME_AND_B(&I2C0->C1,
			(uint8_t) ~(
				  ((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK)
				| ((1 << I2C_C1_TXAK_SHIFT) & I2C_C1_TXAK_MASK)
			)
		);
#endif
}

/**
 * @brief Enters receive mode and disables ACK.
 *
 * Disabling ACK may be required when only one data byte will be read.
 *
 * @param: None
 * @return: None
 */
__STATIC_INLINE void I2C_EnterReceiveModeWithoutAck()
{
	/* Straightforward method of clearing TX mode and
	 * setting NACK bit sending.
	 */
#if !I2C_USE_BME
	register uint8_t reg = I2C0->C1;
	reg &= ~((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK);
	reg |=  ((1 << I2C_C1_TXAK_SHIFT) & I2C_C1_TXAK_MASK);
	I2C0->C1 = reg;
#else

	/* Alternative using the Bit Manipulation Engine
	 * and decorated Logic AND/OR stores */
#if 0
	BME_AND_B(&I2C0->C1, ~(1 << I2C_C1_TX_SHIFT));
	BME_OR_B(&I2C0->C1,   1 << I2C_C1_TXAK_SHIFT);
#endif

	/* Even better alternative: BME Bit Field Insert
	 * - TX   bit is 0x10 (5th bit, 0b00010000)
	 * - TXAK bit is 0x08 (4th bit, 0b00001000)
	 * Thus the following can be deduced:
	 * - The mask for clearing both bits is 0x18 (0b00011000)
	 *   This corresponds to a 2 bit wide mask, shifted by 3
	 * - The mask for setting  TXAK bit  is 0x08 (0b00001000)
	 */
	BME_BFI_B(&I2C0->C1, 0x08, 3, 2);

#endif /* USE_BME */
}

/**
 * @brief Sends a repeated start condition and enters TX mode.
 *
 * @param: None
 * @return: None
 */
__STATIC_INLINE void I2C_SendRepeatedStart()
{
#if I2C_ENABLE_E6070_SPEEDHACK
	register uint8_t reg = I2C0->F;
	I2C0->F = reg & ~I2C_F_MULT_MASK; /* NOTE: According to KINETIS_L_2N97F errata (e6070), repeated start condition can not be sent if prescaler is any other than 1 (0x0). A solution is to temporarily disable the multiplier. */
#endif

#if !I2C_USE_BME
	I2C0->C1 |= ((1 << I2C_C1_RSTA_SHIFT) & I2C_C1_RSTA_MASK)
			  | ((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK);
#else
	BME_OR_B(&I2C0->C1,
			  ((1 << I2C_C1_RSTA_SHIFT) & I2C_C1_RSTA_MASK)
			| ((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK)
		);
#endif

#if I2C_ENABLE_E6070_SPEEDHACK
	I2C0->F = reg;
#endif
}

/**
 * @brief Sends a stop condition (also leaves TX mode)
 *
 * @param: None
 * @return: None
 */
__STATIC_INLINE void I2C_SendStop()
{
#if !I2C_USE_BME
	I2C0->C1 &= ~((1 << I2C_C1_MST_SHIFT) & I2C_C1_MST_MASK)
			& ~((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK);
#else
	BME_AND_B(&I2C0->C1,
			(uint8_t) ~(
				  ((1 << I2C_C1_MST_SHIFT) & I2C_C1_MST_MASK)
				| ((1 << I2C_C1_TX_SHIFT) & I2C_C1_TX_MASK)
			)
		);
#endif
}

/**
 * @brief Enables sending of ACK
 *
 * Enabling ACK may be required when more than one data byte will be read.
 *
 * @param: None
 * @return: None
 */
__STATIC_INLINE void I2C_EnableAck()
{
#if !I2C_USE_BME
	I2C0->C1 &= ~((1 << I2C_C1_TXAK_SHIFT) & I2C_C1_TXAK_MASK);
#else
	BME_AND_B(&I2C0->C1,
			(uint8_t)
			~((1 << I2C_C1_TXAK_SHIFT) & I2C_C1_TXAK_MASK)
	);
#endif
}

/**
 * @brief Enables sending of NACK (disabling ACK)
 *
 * Enabling NACK may be required when no more data byte will be read.
 *
 * @param: None
 * @return: None
 */
__STATIC_INLINE void I2C_DisableAck()
{
#if !I2C_USE_BME
	I2C0->C1 |= ((1 << I2C_C1_TXAK_SHIFT) & I2C_C1_TXAK_MASK);
#else
	BME_OR_B(&I2C0->C1,
			((1 << I2C_C1_TXAK_SHIFT) & I2C_C1_TXAK_MASK)
		);
#endif
}

/**
 * @brief Sends a byte over the I2C bus and waits for the operation to complete
 *
 * @param[in] value The byte to send
 *
 * @return: None
 */
__STATIC_INLINE void I2C_SendBlocking(const uint8_t value)
{
	I2C0->D = value;
	I2C_Wait();
}

/**
 * @brief Reads a byte over the I2C bus and drives the clock for another byte
 *
 * @return vale: The received byte
 */
__STATIC_INLINE uint8_t I2C_ReceiveDriving()
{
	register uint8_t value = I2C0->D;
	I2C_Wait();
	return value;
}

/**
 * @brief Reads a byte over the I2C bus and drives the clock for another byte, while sending NACK
 * @return The received byte
 */
__STATIC_INLINE uint8_t I2C_ReceiveDrivingWithNack()
{
	I2C_DisableAck();
	return I2C_ReceiveDriving();
}

/**
 * @brief Reads the last byte over the I2C bus and sends a stop condition
 * @return The received byte
 */
__STATIC_INLINE uint8_t I2C_ReceiveAndStop()
{
	I2C_SendStop();
	return I2C0->D;
}

/**
 * @brief Reads a byte over the I2C bus and sends a repeated start condition.
 * @return The received byte
 *
 * The I2C module is in transmit mode afterwards.
 */
__STATIC_INLINE uint8_t I2C_ReceiveAndRestart()
{
	I2C_SendRepeatedStart();
	return I2C0->D;
}

/**
 * @brief Drives the clock in receiver mode in order to receive the first byte.
 *
 * @param: None
 * @return: None
 */
__STATIC_INLINE void I2C_ReceiverModeDriveClock()
{
//	INTENTIONALLY_UNUSED(register uint8_t) = I2C0->D;
	register uint8_t val = I2C0->D;
	if(val) {
		val = 0;
	}
	I2C_Wait();
}

/**
 * @brief Initiates a register read after the module was brought into TX mode.
 * @param[in] slaveId The slave id
 * @param[in] registerAddress the register to read from
 *
 * @return: None
 */
void I2C_InitiateRegisterReadAt(const register uint8_t slaveId, const register uint8_t registerAddress);

//void i2c_init(void);

/**
 * @brief send start sequence over the i2c
 *
 * @param: None
 * @return: None
 */
void i2c_start(void);

/**
 * @brief Read the slave configuration
 *
 * @param: 1) dev: Slave Device Address
 * 		   2) Address: Register Address to Read config from
 * @return: None
 */
void i2c_read_setup(uint8_t dev, uint8_t address);

/**
 * @brief Read a byte and ack/nack as appropriate
 *
 * @param: uint8_t value: Read multiple Data Generated from register source
 * @return: None
 */
uint8_t i2c_repeated_read(uint8_t);

/**
 * @brief For reading and writing a single byte
		  using 7bit addressing reads a byte from dev:address
 *
 * @param: 1) dev: Slave Device Address
 * 		   2) Address: Register Address to Read data from
 * @return: None
 */
uint8_t i2c_read_byte(uint8_t dev, uint8_t address);

/**
 * @brief using 7bit addressing writes a byte data to dev:address
 *
 * @param: 1) dev: Slave Device Address
 * 		   2) Address: Register Address to Read data from
 * @return: None
 */
void i2c_write_byte(uint8_t dev, uint8_t address, uint8_t data);

#endif /* I2C_H_ */
