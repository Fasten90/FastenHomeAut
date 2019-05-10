/*
 *    compiler.h
 *    Created on:   2013-11-19
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     compiler defines
 *    Target:       STM32Fx
 */

#ifndef COMPILER_H_
#define COMPILER_H_


/*------------------------------------------------------------------------------
 *                                    Includes
 *----------------------------------------------------------------------------*/

///< Options
#include "options.h"

///< Include standard types
#include "GenericTypeDefs.h"

///< For board defines (pins, ports)
#include "board.h"


///< Includes Platform / Microcontroller family libraries
#if defined(CONFIG_PLATFORM_MCU_STM32F4xx)
    /* STM32F4 (E.g. F4Discovery) */
    #include "stm32f4xx_hal.h"
    #include "stm32_hal_legacy.h"
#elif defined(CONFIG_PLATFORM_MCU_STM32F0xx)
    /* STM32F0 */
    #include "stm32f0xx_hal.h"
    #include "stm32_hal_legacy.h"
#elif defined(CONFIG_PLATFORM_X86)
    /* x86 */
    #include "x86_hal.h"
    #ifdef _WIN32
        #define CONFIG_PLATFORM_X86_WIN
    #elif defined(__linux__)
        #define CONFIG_PLATFORM_X86_LINUX
    #else
        #error "Unsupported x86 OS!"
    #endif
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
 * _MSC_VER  --> MSVC
 */
#if !defined(__CC_ARM__) && !defined(__ICARM__) && !defined(__GNUC__) && !defined(__STDC__) && !defined(__clang__) && !defined(_MSC_VER)
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

#endif    /* #ifdef CONFIG_USE_FREERTOS */


///< Macro for list's element size/num
#define NUM_OF(x) (sizeof(x)/sizeof(x[0]))


///< Stringification
#define STR(str)                        #str
#define STR2(str)                       STR(str)


///< Concatenation
#define CONCATENATE(con1, con2)         con1##con2
#define CONCATENATE2(con1, con2)        CONCATENATE(con1, con2)


///< Unused argument
#define UNUSED_ARGUMENT(param)          ((void)param)


///< Unused variable
#define UNUSED_VARIABLE(param)          ((void)param)


///< Runtime Assert function
/* @note    If true, it is OK */
/*            If false, "error" */
#ifdef ASSERT_MODE_HARD
    #define ASSERT(__e)                ((__e) ? (void)0 : Assert_Function(__FILE__, __LINE__, #__e))
#else
    #ifdef CONFIG_DEBUG_MODE
        /* Assert --> Breakpoint */
        #define ASSERT(__e)            do {                           \
                                           if (!(__e))                \
                                               DEBUG_BREAKPOINT();    \
                                       } while(0)
    #else
        /* Debug mode turned off (ASSERT() do nothing) */
        #define ASSERT(__e)
    #endif
#endif


/* #define assert_param(expr) ((void)0)    // incompatible redefinition ... */


/**
 * Macro for checking - compile time checking
 *
 * OK       if condition is TRUE
 * Error    if condition is FALSE
 *
 * @note    Only can use on a function
 * @note    Be careful, it only works if sizes/datas available in compile-time.
 */
#define BUILD_ASSERT(condition) ((void)sizeof(char[1 - 2*!(condition)]))
/*
 * Original version:
 * #define BUILD_ASSERT(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
 */


///< Breakpoint
#ifdef CONFIG_DEBUG_MODE
    #if !defined(CONFIG_PLATFORM_X86)
        /* ARM - ASM: Breakpoint */
        #define DEBUG_BREAKPOINT()        __asm("BKPT #0\n")
    #elif defined(_MSC_VER)
        /* MSVC Does not support #warning, so we shall be careful */
        #pragma message ("MSVC - Debug breakpoint not supported yet")
    #elif defined (__WIN32__)
        #include "Windows.h"
        #define DEBUG_BREAKPOINT()        DebugBreak()
    #elif defined (__linux__)
        #warning "Linux Debug breakpoint does not supported yet"
        #define DEBUG_BREAKPOINT()
    #else
        #warning "Unknown architect - breakpoint does not used"
        #define DEBUG_BREAKPOINT()
    #endif
#else
#define DEBUG_BREAKPOINT()
#endif


///< Compiler message
/* E.g.: #pragma message "Compiling " __FILE__ "..." */
/* TODO: Not works on MSVC? */
#ifndef __TINYC__
    #define DO_PRAGMA(_msg) _Pragma (#_msg)
    #define COMPILER_MESSAGE(_msg) DO_PRAGMA(message ("Compiler message: - " #_msg))
#else
    /* TinyCC (TCC) doesn't support _Pragma */
    #define COMPILER_MESSAGE(_msg)
#endif


#if defined(_MSC_VER )
    /* MSVC could not support __attribute__ because it is gcc extension */
    #define __attribute__(x)
#endif


/*------------------------------------------------------------------------------
 *                                    Global functions
 *----------------------------------------------------------------------------*/

extern void Error_Handler(void);
extern void Assert_Function(char *file, uint32_t line, char *exp);



#endif /* COMPILER_H_ */
