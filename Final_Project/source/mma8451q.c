/*
 * init_sensors.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: DataSheet and DataStructures to handle interaction with MMA8451Q sensor.
 *
 *    Sources of Reference :
 * 		1) https://www.nxp.com/docs/en/data-sheet/MMA8451Q.pdf
 * 		2) https://www.nxp.com/docs/en/application-note/AN4070.pdf
 * 		3) https://www.nxp.com/docs/en/application-note/AN4071.pdf
 * 		4) https://github.com/adafruit/Adafruit_MMA8451_Library
 */

#include "endian.h"
#include "mma8451q.h"
#include "stdint.h"
#include "assert.h"
#include "MKL25Z4.h"
#include "math.h"


#define CTRL_REG1_ACTIVE_SHIFT 	(0x00U)
#define CTRL_REG1_ACTIVE_MASK 	(0x01U)
#define CTRL_REG1_DR_MASK 		(0x38u)
#define CTRL_REG1_DR_SHIFT 		(0x3u)
#define CTRL_REG1_LNOISE_MASK 	(0x4u)
#define CTRL_REG1_LNOISE_SHIFT 	(0x2u)

#define CTRL_REG2_MODS_MASK 	(0x3u)
#define CTRL_REG2_MODS_SHIFT 	(0x0u)
#define CTRL_REG2_SMODS_MASK 	(0x18u)
#define CTRL_REG2_SMODS_SHIFT 	(0x3u)
#define CTRL_REG2_SLPE_MASK 	(0x4u)
#define CTRL_REG2_SLPE_SHIFT 	(0x2u)

#define CTRL_REG3_IPOL_MASK 	(0x2u)
#define CTRL_REG3_IPOL_SHIFT 	(0x1u)
#define CTRL_REG3_PPOD_MASK 	(0x1u)
#define CTRL_REG3_PPOD_SHIFT 	(0x00u)

#define XYZ_DATA_CFG_FS_SHIFT 	(0x0u)
#define XYZ_DATA_CFG_FS_MASK 	(0x03u)
#define XYZ_DATA_CFG_HPF_OUT_SHIFT (0x04u)
#define XYZ_DATA_CFG_HPF_OUT_MASK (0x10u)

#define MMA_ADDR 0x3A
#define MMA8451Q_INT_PORT	PORTA				/*! Port at which the MMA8451Q INT1 and INT2 pins are attached */
#define MMA8451Q_INT_GPIO	GPIOA				/*! Port at which the MMA8451Q INT1 and INT2 pins are attached */
#define MMA8451Q_INT1_PIN	14					/*! Pin at which the MMA8451Q INT1 is attached */
#define MMA8451Q_INT2_PIN	15					/*! Pin at which the MMA8451Q INT2 is attached */


#define MPU6050_INT_PORT	PORTA				/*! Port at which the MPU6050 INT pin is attached */
#define MPU6050_INT_GPIO	GPIOA				/*! Port at which the MPU6050 INT pin is attached */
#define MPU6050_INT_PIN		13					/*! Pin at which the MPU6050 INT is attached */


/**
 * @brief Reads the accelerometer data in 14bit no-fifo mode
 * @param[out] The accelerometer data; Must not be null.
 * @return : None
 */
void MMA8451Q_ReadAcceleration14bitNoFifo(mma8451q_acc_t *const data)
{
	/* in 14bit mode there are 7 registers to be read (1 status + 6 data) */
	static const uint8_t registerCount = 7;

	/* address the buffer by skipping the padding field */
	uint8_t *buffer = &data->status;

	/* read the register data */
	I2C_ReadRegisters(MMA8451Q_I2CADDR, MMA8451Q_REG_STATUS, registerCount, buffer);

	/* apply fix for endianness */
	if (endianCorrectionRequired(FROM_BIG_ENDIAN))
	{
		data->x = ENDIANSWAP_16(data->x);
		data->y = ENDIANSWAP_16(data->y);
		data->z = ENDIANSWAP_16(data->z);
	}

	/* correct the 14bit layout to 16bit layout */
	data->x >>= 2;
	data->y >>= 2;
	data->z >>= 2;
}

/**
 * @brief Sets the data rate and the active mode
 * @param: 1) mma8451q_confreg_t *const configuration: Existing Configuration to be set to MMA8451Q
 * 		   2) datarate: Update Hz for the sensor
 * 		   3) lownoise: Bool - Set if Low noise mode
 */
void MMA8451Q_SetDataRate(mma8451q_confreg_t *const configuration, mma8451q_datarate_t datarate, mma8451q_lownoise_t lownoise)
{
	if (MMA8451Q_CONFIGURE_DIRECT == configuration)
	{
		const register uint8_t value = ((datarate << CTRL_REG1_DR_SHIFT) & CTRL_REG1_DR_MASK) | ((lownoise << CTRL_REG1_LNOISE_SHIFT) & CTRL_REG1_LNOISE_MASK);
		const register uint8_t mask = (uint8_t)~(CTRL_REG1_DR_MASK | CTRL_REG1_LNOISE_MASK);
		I2C_ModifyRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG1, mask, value);
	}
	else
	{
		configuration->CTRL_REG1 &= ~(CTRL_REG1_DR_MASK | CTRL_REG1_LNOISE_MASK);
		configuration->CTRL_REG1 |= ((datarate << CTRL_REG1_DR_SHIFT) & CTRL_REG1_DR_MASK) | ((lownoise << CTRL_REG1_LNOISE_SHIFT) & CTRL_REG1_LNOISE_MASK);
	}
}

/**
 * @brief Reads the SYSMOD register from the MMA8451Q.
 *
 * @return Current system mode.
 */
uint8_t MMA8451Q_SystemMode()
{
	return I2C_ReadRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_SYSMOD);
}

/**
 * @brief Reads the REG_PL_CFG register from the MMA8451Q.
 *
 * @return Current portrait/landscape mode.
 */
uint8_t MMA8451Q_LandscapePortraitConfig()
{
	return I2C_ReadRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_PL_CFG);
}

/**
 * @brief Configures the oversampling modes
 *
 * @param[in] oversampling The oversampling mode
 */
void MMA8451Q_SetOversampling(mma8451q_confreg_t *const configuration, mma8451q_oversampling_t oversampling)
{
	if (MMA8451Q_CONFIGURE_DIRECT == configuration)
	{
		const register uint8_t value = (oversampling << CTRL_REG2_MODS_SHIFT) & CTRL_REG2_MODS_MASK;
		const register uint8_t mask = (uint8_t)~(CTRL_REG2_MODS_MASK);
		I2C_ModifyRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG2, mask, value);
	}
	else
	{
		configuration->CTRL_REG2 &= ~(CTRL_REG2_MODS_MASK);
		configuration->CTRL_REG2 |= (oversampling << CTRL_REG2_MODS_SHIFT) & CTRL_REG2_MODS_MASK;
	}
}

/**
 * @brief Configures the oversampling modes in sleep mode
 *
 * @param[in] oversampling The oversampling mode
 */
void MMA8451Q_SetSleepOversampling(mma8451q_confreg_t *const configuration, mma8451q_oversampling_t oversampling)
{
	if (MMA8451Q_CONFIGURE_DIRECT == configuration)
	{
		const register uint8_t value = (oversampling << CTRL_REG2_SMODS_SHIFT) & CTRL_REG2_SMODS_MASK;
		const register uint8_t mask = (uint8_t)~(CTRL_REG2_SMODS_MASK);
		I2C_ModifyRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG2, mask, value);
	}
	else
	{
		configuration->CTRL_REG2 &= ~(CTRL_REG2_SMODS_MASK);
		configuration->CTRL_REG2 |= (oversampling << CTRL_REG2_SMODS_SHIFT) & CTRL_REG2_SMODS_MASK;
	}
}


/**
 * @brief Configures the transient mode
 * @param: None
 * @return : None
 */
void MMA8451Q_SetTransient()
{
//	I2C_WriteRegister(register uint8_t slaveId, register uint8_t registerAddress, register uint8_t value);

	// Enable X and Y Axes and enable the latch: Register 0x1D Configuration Register
	I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_TRANSIENT_CFG, 0x16);

	// Set the Threshold: Register 0x1F
	/*
	 * Note: Step count is 0.063g per count
	 * 0.5g / 0.063g = 7.93. Therefore set the threshold to 8 counts
	 */
	I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_TRANSIENT_THS, 0x08);

	// Set the Debounce Counter for 50 ms: Register 0x20
	I2C_WriteRegister(MMA8451Q_I2CADDR, 0x20, 0x05);


}



/**
 * @brief Configures the Motion mode
 * @param: None
 * @return: None
 */
void MMA8451Q_SetMotion()
{
//	I2C_WriteRegister(register uint8_t slaveId, register uint8_t registerAddress, register uint8_t value);

	// Enable X and Y Axes and enable the latch: Register 0x15 Configuration Register
	I2C_WriteRegister(MMA8451Q_I2CADDR, 0x15, 0xD8);

	// Set the Debounce Counter for 50 ms: Register 0x20
//	I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_FF_MT_THS, 0x30);
	I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_FF_MT_THS, 0x1E);

	//Set the debounce counter to eliminate false readings for 100 Hz sample rate with a requirement of 100 ms timer.
	I2C_WriteRegister(MMA8451Q_I2CADDR, 0x18, 0x0A);

	// Enable Motion/Freefall Interrupt Function in the System (CTRL_REG4)
	I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG4, 0x04);

	// Route the Motion/Freefall Interrupt Function to INT1 hardware pin (CTRL_REG5)
	I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG5, 0x04);

}




/**
 * @brief Configures the sensitivity and the high pass filter
 * @param[in] sensitivity The sensitivity
 * @param[in] highpassEnabled Set to 1 to enable the high pass filter or to 0 otherwise (default)
 * @return : None
 */
void MMA8451Q_SetSensitivity(mma8451q_confreg_t *const configuration, mma8451q_sensitivity_t sensitivity, mma8451q_hpo_t highpassEnabled)
{
	if (MMA8451Q_CONFIGURE_DIRECT == configuration)
	{
		I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_XYZ_DATA_CFG, (sensitivity & 0x03) | ((highpassEnabled << 4) & 0x10));
	}
	else
	{
		configuration->XYZ_DATA_CFG = ((sensitivity << XYZ_DATA_CFG_FS_SHIFT) & XYZ_DATA_CFG_FS_MASK)
									| ((highpassEnabled << XYZ_DATA_CFG_HPF_OUT_SHIFT) & XYZ_DATA_CFG_HPF_OUT_MASK);
	}
}

/**
 * @brief Enables or disables interrupts
 * @param[in] mode The mode
 * @param[in] polarity The polarity
 *
 * @return: None
 */
void MMA8451Q_SetInterruptMode(mma8451q_confreg_t *const configuration, mma8451q_intmode_t mode, mma8451q_intpol_t polarity)
{
	if (MMA8451Q_CONFIGURE_DIRECT == configuration)
	{
		const uint8_t value = ((mode << CTRL_REG3_PPOD_SHIFT) & CTRL_REG3_PPOD_MASK)
									| ((polarity << CTRL_REG3_IPOL_SHIFT) & CTRL_REG3_IPOL_MASK);
		const uint8_t mask = (uint8_t)~(CTRL_REG3_IPOL_MASK | CTRL_REG3_PPOD_MASK);
		I2C_ModifyRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG3, mask, value);
	}
	else
	{
		configuration->CTRL_REG3 &= ~(CTRL_REG3_IPOL_MASK | CTRL_REG3_PPOD_MASK);;
		configuration->CTRL_REG3 |= ((mode << CTRL_REG3_PPOD_SHIFT) & CTRL_REG3_PPOD_MASK)
								| ((polarity << CTRL_REG3_IPOL_SHIFT) & CTRL_REG3_IPOL_MASK);
	}
}

/**
 * @brief Configures and enables specific interrupts
 * @param[in] irq The interrupt
 * @param[in] pin The pin
 *
 * @return: None
 */
void MMA8451Q_ConfigureInterrupt(mma8451q_confreg_t *const configuration, mma8451q_interrupt_t irq, mma8451q_intpin_t pin)
{
	/* interrupt pin. Assume that the interrupt 1 should be set */
	uint8_t clearMask = I2C_MOD_NO_AND_MASK;
	uint8_t setMask = (1 << irq);

	/* if pin 2 should be used, revert */
	if (MMA8451Q_INTPIN_INT2 == pin)
	{
		clearMask = ~(1 << irq);
		setMask = I2C_MOD_NO_OR_MASK;
	}

	if (MMA8451Q_CONFIGURE_DIRECT == configuration)
	{
		I2C_ModifyRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG5, clearMask, setMask);

		/* interrupt enable */
		I2C_ModifyRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG4, I2C_MOD_NO_AND_MASK, 1 << irq);
	}
	else
	{
		configuration->CTRL_REG4 |= 1 << irq;
		configuration->CTRL_REG5 &= clearMask;
		configuration->CTRL_REG5 |= setMask;
	}
}

/**
 * @brief Clears the interrupt configuration
 * @param: configuration: Current Configuration to Update for the intertial Sensor
 * @return: None
 */
void MMA8451Q_ClearInterruptConfiguration(mma8451q_confreg_t *const configuration)
{
	if (MMA8451Q_CONFIGURE_DIRECT == configuration)
	{
		I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG4, 0);
		I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG5, 0);
	}
	else
	{
		configuration->CTRL_REG4 = 0;
		configuration->CTRL_REG5 = 0;
	}
}

/**
 * @brief Fetches the configuration into a {@see mma8451q_confreg_t} data structure
 * @param[inout] The configuration data data; Must not be null.
 *
 * @return : None
 */
void MMA8451Q_FetchConfiguration(mma8451q_confreg_t *const configuration)
{
	assert(configuration != 0x0);

	/* loop while the bus is still busy */
	I2C_WaitWhileBusy();

	/* start register addressing */
	I2C_SendStart();
	I2C_SendBlocking(I2C_WRITE_ADDRESS(MMA8451Q_I2CADDR));
	I2C_SendBlocking(MMA8451Q_REG_F_SETUP);

	/* start read */
	I2C_SendRepeatedStart();
	I2C_SendBlocking(I2C_READ_ADDRESS(MMA8451Q_I2CADDR));
	I2C_EnterReceiveModeWithAck();
	I2C_ReceiverModeDriveClock();

	/* read the registers */
	configuration->F_SETUP = I2C_ReceiveDriving();
	configuration->TRIG_CFG = I2C_ReceiveDriving();
	*((uint8_t*)&configuration->SYSMOD) = I2C_ReceiveDriving();

	I2C_ReceiverModeDriveClock(); /* skip 1 register */

	*((uint8_t*)&configuration->WHO_AM_I) = I2C_ReceiveDriving();
	configuration->XYZ_DATA_CFG = I2C_ReceiveDriving();
	configuration->HP_FILTER_CUTOFF = I2C_ReceiveDriving();

	I2C_ReceiverModeDriveClock(); /* skip 1 register */

	configuration->PL_CFG = I2C_ReceiveDriving();
	configuration->PL_COUNT = I2C_ReceiveDriving();
	configuration->PL_BF_ZCOMP = I2C_ReceiveDriving();
	configuration->P_L_THS_REG = I2C_ReceiveDriving();
	configuration->FF_MT_CFG = I2C_ReceiveDriving();

	I2C_ReceiverModeDriveClock(); /* skip 1 register */

#if 1
	/* After FF_MT_THS (0x17) and FF_MT_COUNT (0x18)
	 * the next 4 registers are undefined, so bulk-reading
	 * over them may yield in undesired behaviour
	 */
	configuration->FF_MT_THS = I2C_ReceiveDrivingWithNack();
	configuration->FF_MT_COUNT = I2C_ReceiveAndRestart();

	/* restart read at 0x1D */
	I2C_SendBlocking(I2C_WRITE_ADDRESS(MMA8451Q_I2CADDR));
	I2C_SendBlocking(MMA8451Q_TRANSIENT_CFG);

	/* re-enter read mode */
	I2C_SendRepeatedStart();
	I2C_SendBlocking(I2C_READ_ADDRESS(MMA8451Q_I2CADDR));
	I2C_EnterReceiveModeWithAck();
	I2C_ReceiverModeDriveClock();
#else
	/* bulk-read through the next registers */
	configuration->FF_MT_THS = I2C_ReceiveDriving();
	configuration->FF_MT_COUNT = I2C_ReceiveDriving();

	I2C_ReceiverModeDriveClock(); /* skip 4 registers */
	I2C_ReceiverModeDriveClock();
	I2C_ReceiverModeDriveClock();
	I2C_ReceiverModeDriveClock();
#endif

	configuration->TRANSIENT_CFG = I2C_ReceiveDriving();
	configuration->TRANSIENT_SCR = I2C_ReceiveDriving();
	configuration->TRANSIENT_THS = I2C_ReceiveDriving();
	configuration->TRANSIENT_COUNT = I2C_ReceiveDriving();
	configuration->PULSE_CFG = I2C_ReceiveDriving();

	I2C_ReceiverModeDriveClock(); /* skip 1 register */

	configuration->PULSE_THSX = I2C_ReceiveDriving();
	configuration->PULSE_THSY = I2C_ReceiveDriving();
	configuration->PULSE_THSZ = I2C_ReceiveDriving();
	configuration->PULSE_TMLT = I2C_ReceiveDriving();
	configuration->PULSE_LTCY = I2C_ReceiveDriving();
	configuration->PULSE_WIND = I2C_ReceiveDriving();
	configuration->ASLP_COUNT = I2C_ReceiveDriving();
	configuration->CTRL_REG1 = I2C_ReceiveDriving();
	configuration->CTRL_REG2 = I2C_ReceiveDriving();
	configuration->CTRL_REG3 = I2C_ReceiveDriving();
	configuration->CTRL_REG4 = I2C_ReceiveDriving();
	configuration->CTRL_REG5 = I2C_ReceiveDriving();
	configuration->OFF_X = I2C_ReceiveDriving();
	configuration->OFF_Y = I2C_ReceiveDrivingWithNack();
	configuration->OFF_Z = I2C_ReceiveAndStop();
}

/**
 * @brief Stores the configuration from a {@see mma8451q_confreg_t} data structure
 * @param[in] The configuration data data; Must not be null.
 *
 * @return: None
 */
void MMA8451Q_StoreConfiguration(const mma8451q_confreg_t *const configuration)
{
	assert(configuration != 0x0);

	/* loop while the bus is still busy */
	I2C_WaitWhileBusy();

	/* start register addressing at 0x2A - enter passive mode */
	I2C_SendStart();
	I2C_SendBlocking(I2C_WRITE_ADDRESS(MMA8451Q_I2CADDR));
	I2C_SendBlocking(MMA8451Q_REG_CTRL_REG1);
	I2C_SendBlocking(configuration->CTRL_REG1 & ~((1 << CTRL_REG1_ACTIVE_SHIFT) & CTRL_REG1_ACTIVE_MASK)); /* 0x2A, clear active mode */

	/* start register addressing at 0x09 */
	I2C_SendRepeatedStart();
	I2C_SendBlocking(I2C_WRITE_ADDRESS(MMA8451Q_I2CADDR));
	I2C_SendBlocking(MMA8451Q_REG_F_SETUP);
	I2C_SendBlocking(configuration->F_SETUP); /* 0x09 */
	I2C_SendBlocking(configuration->TRIG_CFG); /* 0x0A */

	/* repeat write at 0x0E */
	I2C_SendRepeatedStart();
	I2C_SendBlocking(I2C_WRITE_ADDRESS(MMA8451Q_I2CADDR));
	I2C_SendBlocking(MMA8451Q_REG_XYZ_DATA_CFG);
	I2C_SendBlocking(configuration->XYZ_DATA_CFG); /* 0x0E */
	I2C_SendBlocking(configuration->HP_FILTER_CUTOFF); /* 0x0F */

	/* repeat write at 0x11 */
	I2C_SendRepeatedStart();
	I2C_SendBlocking(I2C_WRITE_ADDRESS(MMA8451Q_I2CADDR));
	I2C_SendBlocking(MMA8451Q_REG_PL_CFG);
	I2C_SendBlocking(configuration->PL_CFG); /* 0x11 */
	I2C_SendBlocking(configuration->PL_COUNT); /* 0x12 */
	I2C_SendBlocking(configuration->PL_BF_ZCOMP); /* 0x13 */
	I2C_SendBlocking(configuration->P_L_THS_REG); /* 0x14 */
	I2C_SendBlocking(configuration->FF_MT_CFG); /* 0x15 */

	/* repeat write at 0x17 */
	I2C_SendRepeatedStart();
	I2C_SendBlocking(I2C_WRITE_ADDRESS(MMA8451Q_I2CADDR));
	I2C_SendBlocking(MMA8451Q_REG_FF_MT_THS);
	I2C_SendBlocking(configuration->FF_MT_THS); /* 0x17 */
	I2C_SendBlocking(configuration->FF_MT_COUNT); /* 0x18 */

	/* repeat write at 0x1D */
	I2C_SendRepeatedStart();
	I2C_SendBlocking(I2C_WRITE_ADDRESS(MMA8451Q_I2CADDR));
	I2C_SendBlocking(MMA8451Q_TRANSIENT_CFG);
	I2C_SendBlocking(configuration->TRANSIENT_CFG); /* 0x1D */

	/* repeat write at 0x1F */
	I2C_SendRepeatedStart();
	I2C_SendBlocking(I2C_WRITE_ADDRESS(MMA8451Q_I2CADDR));
	I2C_SendBlocking(MMA8451Q_TRANSIENT_THS);
	I2C_SendBlocking(configuration->TRANSIENT_THS); /* 0x1F */
	I2C_SendBlocking(configuration->TRANSIENT_COUNT); /* 0x20 */
	I2C_SendBlocking(configuration->PULSE_CFG); /* 0x21 */

	/* repeat write at 0x23 */
	I2C_SendRepeatedStart();
	I2C_SendBlocking(I2C_WRITE_ADDRESS(MMA8451Q_I2CADDR));
	I2C_SendBlocking(MMA8451Q_PULSE_THSX);
	I2C_SendBlocking(configuration->PULSE_THSX); /* 0x23 */
	I2C_SendBlocking(configuration->PULSE_THSY); /* 0x24 */
	I2C_SendBlocking(configuration->PULSE_THSZ); /* 0x25 */
	I2C_SendBlocking(configuration->PULSE_TMLT); /* 0x26 */
	I2C_SendBlocking(configuration->PULSE_LTCY); /* 0x27 */
	I2C_SendBlocking(configuration->PULSE_WIND); /* 0x28 */
	I2C_SendBlocking(configuration->ASLP_COUNT); /* 0x29 */
	I2C_SendBlocking(configuration->CTRL_REG1 & ~((1 << CTRL_REG1_ACTIVE_SHIFT) & CTRL_REG1_ACTIVE_MASK));  /* 0x2A, clear active mode */
	I2C_SendBlocking(configuration->CTRL_REG2);  /* 0x2B */
	I2C_SendBlocking(configuration->CTRL_REG3); /* 0x2C */
	I2C_SendBlocking(configuration->CTRL_REG4); /* 0x2D */
	I2C_SendBlocking(configuration->CTRL_REG5); /* 0x2E */
	I2C_SendBlocking(configuration->OFF_X); /* 0x2F */
	I2C_SendBlocking(configuration->OFF_Y); /* 0x30 */
	I2C_SendBlocking(configuration->OFF_Z); /* 0x31 */

	/* rewind to 0x2A - enter desired mode */
	I2C_SendRepeatedStart();
	I2C_SendBlocking(I2C_WRITE_ADDRESS(MMA8451Q_I2CADDR));
	I2C_SendBlocking(MMA8451Q_REG_CTRL_REG1);
	I2C_SendBlocking(configuration->CTRL_REG1); /* 0x2A, write real value */

	I2C_SendStop();
}


/**
 * @brief Convert the acceleration data read to roll and pitch
 *
 * @param: 1) configuration: Inertial Sensor Configuration to read from
 * 		   2) roll: float angle tilt calculated in degrees
 * 		   3) pitch: float angle tilt calculated in degrees
 */
void convert_xyz_to_roll_pitch(mma8451q_acc_t *acc, float *roll, float *pitch) {
	float ax = acc->xyz[0]/COUNTS_PER_G,
				ay = acc->xyz[1]/COUNTS_PER_G,
				az = acc->xyz[2]/COUNTS_PER_G;

	*roll = atan2(ay, az)*180/M_PI;
	*pitch = atan2(ax, sqrt(ay*ay + az*az))*180/M_PI;

}

/**
 * @brief Read acceleration data from inerital sensor and updat
 *
 * @param: configuration: Inertial Sensor Config storage for acceleration data.
 */
void read_full_xyz(mma8451q_acc_t *acc)
{
	int i;
	uint8_t data[6];
	int16_t temp[3];

	i2c_start();
	i2c_read_setup(MMA_ADDR , REG_XHI);

	// Read five bytes in repeated mode
	for( i=0; i<5; i++)	{
		data[i] = i2c_repeated_read(0);
	}
	// Read last byte ending repeated mode
	data[i] = i2c_repeated_read(1);

	for ( i=0; i<3; i++ ) {
		temp[i] = (int16_t) ((data[2*i]<<8) | data[2*i+1]);
	}

	// Align for 14 bits
	acc->xyz[0] = temp[0]/4;
	acc->xyz[1] = temp[1]/4;
	acc->xyz[2] = temp[2]/4;
}


//int init_mma()
//{
//	//set active mode, 14 bit samples and 800 Hz ODR
//	i2c_write_byte(MMA_ADDR, MMA8451Q_REG_CTRL_REG1, 0x01);
//	MMA8451Q_INT_PORT->PCR[MMA8451Q_INT1_PIN] = PORT_PCR_MUX(0x1) | PORT_PCR_IRQC(0xA) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* interrupt on falling edge, pull-up for open drain/active low line */
////	MMA8451Q_INT_GPIO->PDDR &= ~(GPIO_PDDR_PDD(1 << MMA8451Q_INT1_PIN) | GPIO_PDDR_PDD(1 << MMA8451Q_INT2_PIN));
//	MMA8451Q_INT_GPIO->PDDR &= ~(GPIO_PDDR_PDD(1 << MMA8451Q_INT1_PIN));
//
//
//	//    MMA8451Q_REG_CTRL_REG5
//	I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG1, 0x00);
//	I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG3, 0x00);             // Push-pull, active low interrupt
//	I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG4, 0x01);             // Enable DRDY interrupt
//	I2C_WriteRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_CTRL_REG5, 0x01);             // DRDY interrupt routed to INT1 -
//
////    MMA8451Q_StoreConfiguration(configuration);
////    MMA8451Q_EnterActiveMode();
//
//
//
//	//    /* prepare interrupts for pin change / PORTA */
//	NVIC_SetPriority(PORTA_IRQn, 2); // 0, 1, 2, or 3
//	NVIC_ClearPendingIRQ(PORTA_IRQn);
//	NVIC_EnableIRQ(PORTA_IRQn);
//	return 1;
//}
