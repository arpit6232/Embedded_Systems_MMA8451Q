/*
 * init_sensors.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Initialize Sensors with appropriate Setup Configurations
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 */

#ifndef INIT_SENSORS_H_
#define INIT_SENSORS_H_

#ifndef INIT_SENSORS_H
#define INIT_SENSORS_H

#define ENABLE_MMA8451Q 1						/*! Used to enable or disable MMA8451Q fetching */

#define MMA8451Q_INT_PORT	PORTA				/*! Port at which the MMA8451Q INT1 and INT2 pins are attached */
#define MMA8451Q_INT_GPIO	GPIOA				/*! Port at which the MMA8451Q INT1 and INT2 pins are attached */
#define MMA8451Q_INT1_PIN	14					/*! Pin at which the MMA8451Q INT1 is attached */
#define MMA8451Q_INT2_PIN	15					/*! Pin at which the MMA8451Q INT2 is attached */

/**
* @brief Sets up the MMA8451Q communication
*
* @param: None
* @return: None
*/
void InitMMA8451Q();


#endif

#endif /* INIT_SENSORS_H_ */
