/*
 *		compiler.h
 *  	Created on: 	2013-11-19
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		compiler defines/includes file for project
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
 */

#ifndef COMPILER_H_
#define COMPILER_H_


/*------------------------------------------------------------------------------
 *									Includes
 *----------------------------------------------------------------------------*/

///< Options
#include "options.h"

///< Include standard types
#include "GenericTypeDefs.h"

///< For board defines (pins, ports)
#include "board.h"


///< Includes Platform / Microcontroller family libraries
#if defined(CONFIG_PLATFORM_MCU_STM32F4xx)
	// STM32F4 (E.g. F4Discovery)
	#include "stm32f4xx_hal.h"
	#include "stm32_hal_legacy.h"
#elif defined(CONFIG_PLATFORM_MCU_STM32F0xx)
	// STM32F0
	#include "stm32f0xx_hal.h"
	#include "stm32_hal_legacy.h"
#elif defined(CONFIG_PLATFORM_PC_WINDOWS)
	// PC
	#include "windows_hal.h"
#else
#warning "Missed platform / microcontroller family define / include"
#endif


///< Check the compiler define
/*
 * __CC_ARM__
 * __ICARM__
 * __GNUC__
 * __STDC__
 * __clang__ (Added by later - for test)
 */
#if !defined(__CC_ARM__) && !defined(__ICARM__) && !defined(__GNUC__) && !defined(__STDC__) && !defined(__clang__)
#error "Unknown compiler!"
#endif


///< FreeRTOS includes
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


///< Macro for list's element size/num
#define NUM_OF(x) (sizeof(x)/sizeof(x[0]))


///< Stringification
#define STR(str)						#str
#define STR2(str)						STR(str)

///< Concatenation
#define CONCATENATE(con1, con2)			con1##con2
#define CONCATENATE2(con1, con2)		CONCATENATE(con1, con2)


///< Unused argument
#define UNUSED_ARGUMENT(param)			((void)param)


///< Runtime Assert function
// \note	If true, it is OK
//			If false, "error"
#ifdef ASSERT_MODE_HARD
	#define ASSERT(__e)				((__e) ? (void)0 : Assert_Function(__FILE__, __LINE__, #__e))
#else
	#ifdef CONFIG_DEBUG_MODE
		// Assert --> Breakpoint
		#define ASSERT(__e)			do { 						\
										if (!(__e))				\
											DEBUG_BREAKPOINT();	\
									} while(0)
	#else
		// Debug mode turned off (ASSERT() do nothing)
		#define ASSERT(__e)
	#endif
#endif


// #define assert_param(expr) ((void)0)	// incompatible redefinition ...


/**
 * Macro for checking - compile time checking
 *
 * OK		if condition is TRUE
 * Error	if condition is FALSE
 *
 * \note	Only can use on a function
 * \note	Be careful, it only works if sizes/datas available in compile-time.
 */
#define BUILD_ASSERT(condition) ((void)sizeof(char[1 - 2*!(condition)]))
/*
 * Original version:
 * #define BUILD_ASSERT(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
 */


///< Breakpoint
#ifdef CONFIG_DEBUG_MODE
    #ifndef CONFIG_PLATFORM_PC_WINDOWS
	// ASM: Breakpoint
	#define DEBUG_BREAKPOINT()		__asm("BKPT #0\n")
    #else
	#include "Windows.h"
	#define DEBUG_BREAKPOINT()		DebugBreak()
    #endif
#else
#define DEBUG_BREAKPOINT()
#endif


///< Compiler message
// E.g.: #pragma message "Compiling " __FILE__ "..."
// TODO: Not works on MSVC?
#ifndef __TINYC__
	#define DO_PRAGMA(_msg) _Pragma (#_msg)
	#define COMPILER_MESSAGE(_msg) DO_PRAGMA(message ("Compiler message: - " #_msg))
#else
	// TinyCC (TCC) doesn't support _Pragma
	#define COMPILER_MESSAGE(_msg)
#endif


/*------------------------------------------------------------------------------
 *									Global functions
 *----------------------------------------------------------------------------*/

extern void Error_Handler(void);
extern void Assert_Function(char *file, uint32_t line, char *exp);



#endif /* COMPILER_H_ */
