/*
 *		include.h
 *
 *  	Created on: 	2013.11.19.
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		include file for project
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef INCLUDE_H_
#define INCLUDE_H_


#include "options.h"



// Include standard types
#include "GenericTypeDefs.h"


/// For board defines (pins, ports)
#include "board.h"



/*------------------------------------------------------------------------------
 *									Includes
 *----------------------------------------------------------------------------*/

/// Includes microcontroller family libraries
#ifdef CONFIG_MICROCONTROLLER_STM32F4xx
	// STM32F4 Discovery
	#include "stm32f4xx_hal.h"
	#include "stm32_hal_legacy.h"	// for defines
#endif


#ifdef CONFIG_MICROCONTROLLER_STM32F0xx
	// STM32F4 Discovery
	#include "stm32f0xx_hal.h"
	#include "stm32_hal_legacy.h"	// for defines
#endif


/// FreeRTOS includes
#ifdef CONFIG_USE_FREERTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "list.h"

/*
FreeRTOS/Source/tasks.c
FreeRTOS/Source/queue.c
FreeRTOS/Source/list.c
FreeRTOS/Source/portable/[compiler]/[architecture]/port.c.
FreeRTOS/Source/portable/MemMang/heap_x.c where 'x' is 1, 2, 3, 4 or 5.
*/

#endif	// #ifdef CONFIG_USE_FREERTOS



// Macro for list's element size/num
#define NUM_OF(x) (sizeof(x)/sizeof(x[0]))


// Macro for checking
// Error, if condition is true
// Ok, if condition is false
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))


// #define assert_param(expr) ((void)0)	// incompatible redefinition ...


#ifdef CONFIG_DEBUG_MODE
#define DEBUG_BREAKPOINT()		__asm("BKPT #0\n")		// ASM: Break debugger
#else
#define DEBUG_BREAKPOINT()
#endif


#define STR(a)					#a


#define UNUSED_ARGUMENT(param)	((void)param)


// Assert
// if condition is true, it is ok
// if condition is false, it will generate error
#define ASSERT(__e)				((__e) ? (void)0 : Assert_Function(__FILE__, __LINE__, #__e))


extern void Error_Handler(void);
extern void Assert_Function(char *file, uint32_t line, char *exp);



#endif /* INCLUDE_H_ */
