/*
 * test_i2c.c
 *
 *  Created on: Dec 3, 2020
 *      Author: Arpit Savarkar
 *
 *   @brief Header file for test cases to test I2C
 */


#ifndef TEST_I2C_H_
#define TEST_I2C_H_

#include "i2c.h"
#include "global_defs.h"
#include "MKL25Z4.h"


#define test_assert(value) {                                            \
  g_tests_total++;                                                      \
  if (!g_skip_tests) {                                                  \
    if (value) {                                                        \
      g_tests_passed++;                                                 \
    } else {                                                            \
      printf("ERROR: test failure at line %d\n", __LINE__);             \
      g_skip_tests = 1;                                                 \
    }                                                                   \
  }                                                                     \
}

#define test_equal(value1, value2) {                                    \
  g_tests_total++;                                                      \
  if (!g_skip_tests) {                                                  \
    long res1 = (long)(value1);                                         \
    long res2 = (long)(value2);                                         \
    if (res1 == res2) {                                                 \
      g_tests_passed++;                                                 \
    } else {                                                            \
      printf("ERROR: test failure at line %d: %ld != %ld\n", __LINE__, res1, res2); \
      g_skip_tests = 1;                                                 \
    }                                                                   \
  }                                                                     \
}


/*
​ * ​ ​ @brief​ ​ Sets up the testing harness for I2C Communication
 *
​ * ​ ​ @param​ ​ void
 *
 *   @return​ ​ void
 */
void i2c_test_setup();

/*
​ * ​ ​ @brief​ ​ Helper Function to track testing
 *
​ * ​ ​ @param​ ​ void
 *
 *   @return​ ​ void
 */
void test_i2c_mma(void);


#endif /* TEST_I2C_H_ */
