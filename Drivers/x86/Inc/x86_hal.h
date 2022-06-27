/*
 *    x86_hal.h
 *    Created on:   2017-11-05
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       x86
 */


#ifndef X86_HAL_H_
#define X86_HAL_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/


/*  CMSIS files include */
#include "core_windows.h"

#include "x86.h"

/*  TODO: Put to conf file at Inc dir? */
#include "x86_hal_dma.h"
#include "x86_hal_spi.h"
#include "x86_hal_uart.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

/* Peripheral Control functions  ************************************************/
void              HAL_Init(void);
void              HAL_IncTick(void);
void              HAL_Delay(__IO uint32_t Delay);
uint32_t          HAL_GetTick(void);
void              HAL_SuspendTick(void);
void              HAL_ResumeTick(void);


/*  For suppress warnings */
#define HAL_SYSTICK_IRQHandler()

/* #define SystemClock_Config() */



#endif /* X86_HAL_H_ */
