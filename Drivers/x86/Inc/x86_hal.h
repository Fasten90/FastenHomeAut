/*
 *		windows_hal.h
 *		Created on:		2017-11-05
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017-11-05
 */

#ifndef WINDOWS_HAL_H_
#define WINDOWS_HAL_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

// For suppress error (TXDATA_FLUSH ...)
#define STM32F030x8


// CMSIS files include
#include "core_windows.h"

// TODO: rename this file?
#include "stm32f030xc.h"

// TODO: Put to conf file at Inc dir?
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


// For suppress warnings
#define HAL_SYSTICK_IRQHandler()

//#define SystemClock_Config()



#endif /* WINDOWS_HAL_H_ */
