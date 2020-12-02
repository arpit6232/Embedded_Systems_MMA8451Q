/*
 * i2c.c
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Instantiation and functionalities for communication over I2C
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 * 		Links: Inspired from
 * 				1) https://github.com/sunsided/frdm-kl25z-marg-fusion/blob/master/frdm-kl25z-acc-uart/Sources/i2c/i2c.c
 *				2) https://github.com/alexander-g-dean/ESF/tree/master/NXP/Code/Chapter_8/I2C-Demo
 */

#include "i2c.h"
#include "delay.h"
#include "assert.h"

int lock_detect=0;
int i2c_lock=0;
/**
 * @brief Initialises the I2C interface
 */
void I2C_Init()
{
	/* enable clock gating to I2C0 */
#if !I2C_USE_BME
	SIM->SCGC4 |= (1 << SIM_SCGC4_I2C0_SHIFT) & SIM_SCGC4_I2C0_MASK;
#else
	BME_OR_W(&SIM->SCGC4, (1 << SIM_SCGC4_I2C0_SHIFT) & SIM_SCGC4_I2C0_MASK);
#endif

#if 1 /* in ancient times this was hardcoded */

	/* enable the clock gate to port E */
#if !I2C_USE_BME
	SIM->SCGC5 |= (1 << SIM_SCGC5_PORTE_SHIFT) & SIM_SCGC5_PORTE_MASK;
#else
	BME_OR_W(&SIM->SCGC5, (1 << SIM_SCGC5_PORTE_SHIFT) & SIM_SCGC5_PORTE_MASK);
#endif

	/* configure port E pins to I2C operation for MMA8451Q */
	PORTE->PCR[24] = PORT_PCR_MUX(5); /* SCL */
	PORTE->PCR[25] = PORT_PCR_MUX(5); /* SDA */

#endif

	/* configure the I2C clock */
	/*
	 * I2C0 is clocked by the bus clock, that is core/2.
	 * For the MMA8451Q inertial sensor on the FRDM-25KLZ board the
	 * maximum SCL frequency is 400 kHz.
	 * Assuming PEE mode with core=48MHz, 400 kHz = 48MHz/2 / 60,
	 * which means a prescaler (SCL divider) of 60.
	 * According to table 38-41, I2C divider and hold values,
	 * the closest SCL diver is 64 (375 kHz SCL), which is ICR value 0x12.
	 * Alternatively, the SCL divider of 30 can be used (ICR=0x05) in combination
	 * with a multiplicator of 2 (MULT=0x01).
	 * A note states that ICR values lower than 0x10 might result in a varying
	 * SCL divider (+/- 4). However the data sheet does not state anything
	 * useful about that.
	 */
	I2C0->F = I2C_F_MULT(0x00) | I2C_F_ICR(0x12); /* divide by 64 instead, so 375 kHz */

	/* enable the I2C module */
	I2C0->C1 = (1 << I2C_C1_IICEN_SHIFT) & I2C_C1_IICEN_MASK;

	// Select high drive mode
	I2C0->C2 |= (I2C_C2_HDRS_MASK);
}


void i2c_init(void)
{
	//clock i2c peripheral and port E
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK);

	//set pins to I2C function
	PORTE->PCR[24] |= PORT_PCR_MUX(5);
	PORTE->PCR[25] |= PORT_PCR_MUX(5);

	//set to 100k baud
	//baud = bus freq/(scl_div+mul)
 	//~400k = 24M/(64); icr=0x12 sets scl_div to 64

 	I2C0->F = (I2C_F_ICR(0x10) | I2C_F_MULT(0));

	//enable i2c and set to master mode
	I2C0->C1 |= (I2C_C1_IICEN_MASK);

	// Select high drive mode
	I2C0->C2 |= (I2C_C2_HDRS_MASK);
}


/**
 * @brief Reads an 8-bit register from an I2C slave
 */
uint8_t I2C_ReadRegister(register uint8_t slaveId, register uint8_t registerAddress)
{
	/* loop while the bus is still busy */
	I2C_WaitWhileBusy();

	/* send I2C start signal and set write direction, also enables ACK */
	I2C_SendStart();

	/* send the slave address and wait for the I2C bus operation to complete */
	I2C_SendBlocking(I2C_WRITE_ADDRESS(slaveId));

	/* send the register address */
	I2C_SendBlocking(registerAddress);

	/* signal a repeated start condition */
	I2C_SendRepeatedStart();

	/* send the read address */
	I2C_SendBlocking(I2C_READ_ADDRESS(slaveId));

	/* switch to receive mode but disable ACK because only one data byte will be read */
	I2C_EnterReceiveModeWithoutAck();

	/* read a dummy byte to drive the clock */
	I2C_ReceiverModeDriveClock();

	/* stop signal */
	I2C_SendStop();

	/* fetch the last received byte */
	register uint8_t result = I2C0->D;
	return result;
}

/**
 * @brief Reads multiple 8-bit registers from an I2C slave
 * @param[in] slaveId The slave device ID
 * @param[in] startRegisterAddress The first register address
 * @param[in] registerCount The number of registers to read; Must be greater than or equal to two.
 * @param[out] buffere The buffer to write into
 */
static void I2C_ReadRegistersInternal(register uint8_t slaveId, register uint8_t startRegisterAddress, register uint8_t registerCount, uint8_t *const buffer)
{
	assert(registerCount >= 2);

	/* loop while the bus is still busy */
	I2C_WaitWhileBusy();

	/* send I2C start signal and set write direction, also enables ACK */
	I2C_SendStart();

	/* send the slave address and wait for the I2C bus operation to complete */
	I2C_SendBlocking(I2C_WRITE_ADDRESS(slaveId));

	/* send the register address */
	I2C_SendBlocking(startRegisterAddress);

	/* signal a repeated start condition */
	I2C_SendRepeatedStart();

	/* send the read address */
	I2C_SendBlocking(I2C_READ_ADDRESS(slaveId));

	/* switch to receive mode and assume more than one register */
	I2C_EnterReceiveModeWithAck();

	/* read a dummy byte to drive the clock */
	I2C_ReceiverModeDriveClock();

	/* for all remaining bytes, read */
	--registerCount;
	uint8_t index = 0;
	while (--registerCount > 0)
	{
		/* fetch and store value */
		register uint8_t value = I2C0->D;
		buffer[index++] = value;

		/* wait for completion */
		I2C_Wait();
	}

	/* disable ACK and read second-to-last byte */
	I2C_DisableAck();

	/* fetch and store value */
	buffer[index++] = I2C0->D;

	/* wait for completion */
	I2C_Wait();

	/* stop signal */
	I2C_SendStop();

	/* fetch the last received byte */
	buffer[index++] = I2C0->D;
}

/**
 * @brief Reads multiple 8-bit registers from an I2C slave
 * @param[in] slaveId The slave device ID
 * @param[in] startRegisterAddress The first register address
 * @param[in] registerCount The number of registers to read; Must be larger than zero.
 * @param[out] buffer The buffer to write into
 */
void I2C_ReadRegisters(register uint8_t slaveId, register uint8_t startRegisterAddress, register uint8_t registerCount, register uint8_t *buffer)
{
	assert(registerCount > 0);

	if (registerCount >= 2)
	{
		I2C_ReadRegistersInternal(slaveId, startRegisterAddress, registerCount, buffer);
	}
	else
	{
		assert(1 == registerCount);
		register uint8_t result = I2C_ReadRegister(slaveId, startRegisterAddress);
		buffer[0] = result;
	}
}

/**
 * @brief Reads an 8-bit register from an I2C slave
 */
void I2C_WriteRegister(register uint8_t slaveId, register uint8_t registerAddress, register uint8_t value)
{
	/* loop while the bus is still busy */
	I2C_WaitWhileBusy();

	/* send I2C start signal and set write direction*/
	I2C_SendStart();

	/* send the slave address and wait for the I2C bus operation to complete */
	I2C_SendBlocking(I2C_WRITE_ADDRESS(slaveId));

	/* send the register address */
	I2C_SendBlocking(registerAddress);

	/* send the register address */
	I2C_SendBlocking(value);

	/* issue stop signal by clearing master mode. */
	I2C_SendStop();
}

/**
 * @brief Reads an 8-bit register from an I2C slave, modifies it by FIRST and-ing with {@see andMask} and THEN or-ing with {@see orMask} and writes it back
 * @param[in] slaveId The slave id
 * @param[in] registerAddress The register to modify
 * @param[in] orMask The mask to OR the register with
 * @param[in] andMask The mask to AND the register with
 * @return The register after modification
 */
uint8_t I2C_ModifyRegister(register uint8_t slaveId, register uint8_t registerAddress, register uint8_t andMask, register uint8_t orMask)
{
	/* loop while the bus is still busy */
	I2C_WaitWhileBusy();

	/* send the slave address and register */
	I2C_SendStart();
	I2C_SendBlocking(I2C_WRITE_ADDRESS(slaveId));
	I2C_SendBlocking(registerAddress);

	/* signal a repeated start condition */
	I2C_SendRepeatedStart();
	I2C_SendBlocking(I2C_READ_ADDRESS(slaveId));

	/* switch to receive mode but disable ACK because only one data byte will be read */
	I2C_EnterReceiveModeWithoutAck();
	I2C_ReceiverModeDriveClock();

	/* instead of a stop signal, send repeated start again */
	I2C_SendRepeatedStart();

	/* fetch the last received byte */
	register uint8_t value = I2C0->D;

	/* modify the register */
	value &= andMask;
	value |= orMask;

	/* send the slave address and wait for the I2C bus operation to complete */
	I2C_SendBlocking(I2C_WRITE_ADDRESS(slaveId));

	/* send the register address */
	I2C_SendBlocking(registerAddress);

	/* send the register address */
	I2C_SendBlocking(value);

	/* issue stop signal by clearing master mode. */
	I2C_SendStop();
	return value;
}

/**
 * @brief Resets the bus by toggling master mode if the bus is busy. This will interrupt ongoing traffic, so use with caution.
 */
void I2C_ResetBus()
{
	BME_OR_B(&I2C0->S, ((1 << I2C_S_IICIF_SHIFT) << I2C_S_IICIF_MASK));
	BME_OR_B(&I2C0->S, ((1 << I2C_S_IICIF_SHIFT) << I2C_S_IICIF_MASK)); /* clear interrupt flag */
}

/**
 * @brief Initiates a register read after the module was brought into TX mode.
 * @param[in] slaveId The slave id
 * @param[in] registerAddress the register to read from
 */
void I2C_InitiateRegisterReadAt(const register uint8_t slaveId, const register uint8_t registerAddress)
{
	/* send register id */
	I2C_SendBlocking(I2C_WRITE_ADDRESS(slaveId));
	I2C_SendBlocking(registerAddress);

	/* enter read mode */
	I2C_SendRepeatedStart();
	I2C_SendBlocking(I2C_READ_ADDRESS(slaveId));
	I2C_EnterReceiveModeWithAck();
	I2C_ReceiverModeDriveClock();
}

/**
 * @brief Processing the I2C bus operation to complete
 *
 * @param: None
 * @param: None
 */
void i2c_busy(void){
	// Start Signal
	lock_detect=0;
	I2C0->C1 &= ~I2C_C1_IICEN_MASK;
	I2C_TRAN;
	I2C_M_START;
	I2C0->C1 |=  I2C_C1_IICEN_MASK;
	// Write to clear line
	I2C0->C1 |= I2C_C1_MST_MASK; /* set MASTER mode */
	I2C0->C1 |= I2C_C1_TX_MASK; /* Set transmit (TX) mode */
	I2C0->D = 0xFF;
	while ((I2C0->S & I2C_S_IICIF_MASK) == 0U) {
	} /* wait interrupt */
	I2C0->S |= I2C_S_IICIF_MASK; /* clear interrupt bit */


							/* Clear arbitration error flag*/
	I2C0->S |= I2C_S_ARBL_MASK;


							/* Send start */
	I2C0->C1 &= ~I2C_C1_IICEN_MASK;
	I2C0->C1 |= I2C_C1_TX_MASK; /* Set transmit (TX) mode */
	I2C0->C1 |= I2C_C1_MST_MASK; /* START signal generated */

	I2C0->C1 |= I2C_C1_IICEN_MASK;
							/*Wait until start is send*/

							/* Send stop */
	I2C0->C1 &= ~I2C_C1_IICEN_MASK;
	I2C0->C1 |= I2C_C1_MST_MASK;
	I2C0->C1 &= ~I2C_C1_MST_MASK; /* set SLAVE mode */
	I2C0->C1 &= ~I2C_C1_TX_MASK; /* Set Rx */
	I2C0->C1 |= I2C_C1_IICEN_MASK;


								/* wait */
							/* Clear arbitration error & interrupt flag*/
	I2C0->S |= I2C_S_IICIF_MASK;
	I2C0->S |= I2C_S_ARBL_MASK;
	lock_detect=0;
	i2c_lock=1;
}

/**
 * @brief Waits for an I2C bus operation to complete
 *
 * @param: None
 * @param: None
 */
void i2c_wait(void) {
	lock_detect = 0;
	while(((I2C0->S & I2C_S_IICIF_MASK)==0) & (lock_detect < 200)) {
		lock_detect++;
	}
	if (lock_detect >= 200)
		i2c_busy();
	I2C0->S |= I2C_S_IICIF_MASK;
}


void i2c_start()
{
	I2C_TRAN;							/*set to transmit mode */
	I2C_M_START;					/*send start	*/
}


void i2c_read_setup(uint8_t dev, uint8_t address)
{
	I2C0->D = dev;			  /*send dev address	*/
	I2C_WAIT							/*wait for completion */

	I2C0->D =  address;		/*send read address	*/
	I2C_WAIT							/*wait for completion */

	I2C_M_RSTART;				   /*repeated start */
	I2C0->D = (dev|0x1);	 /*send dev address (read)	*/
	I2C_WAIT							 /*wait for completion */

	I2C_REC;						   /*set to receive mode */

}


uint8_t i2c_repeated_read(uint8_t isLastRead)
{
	uint8_t data;

	lock_detect = 0;

	if(isLastRead)	{
		NACK;								/*set NACK after read	*/
	} else	{
		ACK;								/*ACK after read	*/
	}

	data = I2C0->D;				/*dummy read	*/
	I2C_WAIT							/*wait for completion */

	if(isLastRead)	{
		I2C_M_STOP;					/*send stop	*/
	}
	data = I2C0->D;				/*read data	*/

	return  data;
}



uint8_t i2c_read_byte(uint8_t dev, uint8_t address)
{
	uint8_t data;

	I2C_TRAN;							/*set to transmit mode */
	I2C_M_START;					/*send start	*/
	I2C0->D = dev;			  /*send dev address	*/
	I2C_WAIT							/*wait for completion */

	I2C0->D =  address;		/*send read address	*/
	I2C_WAIT							/*wait for completion */

	I2C_M_RSTART;				   /*repeated start */
	I2C0->D = (dev|0x1);	 /*send dev address (read)	*/
	I2C_WAIT							 /*wait for completion */

	I2C_REC;						   /*set to recieve mode */
	NACK;									 /*set NACK after read	*/

	data = I2C0->D;					/*dummy read	*/
	I2C_WAIT								/*wait for completion */

	I2C_M_STOP;							/*send stop	*/
	data = I2C0->D;					/*read data	*/

	return data;
}



void i2c_write_byte(uint8_t dev, uint8_t address, uint8_t data)
{

	I2C_TRAN;							/*set to transmit mode */
	I2C_M_START;					/*send start	*/
	I2C0->D = dev;			  /*send dev address	*/
	I2C_WAIT						  /*wait for ack */

	I2C0->D =  address;		/*send write address	*/
	I2C_WAIT

	I2C0->D = data;				/*send data	*/
	I2C_WAIT
	I2C_M_STOP;

}


