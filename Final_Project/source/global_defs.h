/*
 * global_defs.h
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Debug Functions Defines
 */

#ifndef GLOBAL_DEFS_H_
#define GLOBAL_DEFS_H_

#include "stdio.h"
#include "fsl_debug_console.h"

#ifdef DEBUG
	#define MSG_DEBUG PRINTF
#else // non-debug mode - get rid of printing message
	#define MSG_DEBUG(...)
#endif

#endif /* GLOBAL_DEFS_H_ */
