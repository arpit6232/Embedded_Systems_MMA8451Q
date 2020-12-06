/*
 * test_queue.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Header file for Testing Queue functions
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 */

#ifndef TEST_QUEUE_H_
#define TEST_QUEUE_H_

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "MKL25Z4.h"
#include "queue.h"

#define max(x,y) ((x) > (y) ? (x) : (y))



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
​ * ​ ​ @brief​ ​ Sets up the testing harness for Circular Buffer
 *
​ * ​ ​ @param​ ​ void
 *
 *   @return​ ​ void
 */
void queue_test_setup();

/*
​ * ​ ​ @brief​ ​ Helper Function to track testing
 *
​ * ​ ​ @param​ ​ void
 *
 *   @return​ ​ void
 */
void test_queue();

#endif /* TEST_QUEUE_H_ */
