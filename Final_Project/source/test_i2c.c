/*
 * test_i2c.c
 *
 *  Created on: Dec 3, 2020
 *      Author: Arpit Savarkar
 *
 *   @brief test cases to test I2C,
 *   Inspired from : https://www.totalphase.com/blog/2019/01/i2c-protocol-testing-validation-checklist-embedded-systems/
 */


#include "test_i2c.h"
#include "mma8451q.h"
#include "assert.h"

static int g_tests_passed = 0;
static int g_tests_total = 0;
static int g_skip_tests = 0;

void i2c_test_setup() {

	mma8451q_acc_t val;
	MMA8451Q_InitializeData(&val);
	read_full_xyz(&val);

	// Validation Read of a 7-bit known address
	uint8_t id = I2C_ReadRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_WHOAMI);
	test_equal(id, 0x1A);
	assert(id == 0x1A);

	// Validation of Invalid Slave Address
	id = I2C_ReadRegister(0x00, 0x00);
	test_equal(id, 255);
	assert(id == 255);

	// Static Board Acceleration Limits on Ground
	id = val.x < 0;
	assert(id);
	test_equal(1, val.x < 0);
	id = val.y > 0;
	assert(id);
	test_equal(1, val.y > 0);
	id = val.z > 3000 ;
	assert(id);
	test_equal(1, val.z > 3000);

}

void test_i2c_mma() {

//	g_tests_passed = 0;
//	g_tests_total = 0;
//	g_skip_tests = 0;

	i2c_test_setup();

	PRINTF("\r\n %s: passed %d/%d test cases", __FUNCTION__, g_tests_passed, g_tests_total);

	PRINTF("\r\n");

}
