/*
 *		Watchdog.h
 *		Created on:		2017-02-06
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Watchdog
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-06
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_WATCHDOG_ENABLE


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

// Watchdog enabled: function implement in .c file
void Watchdog_Init(void);
void Watchdog_Clear(void);

#else
// If Watchdog not enabled
#define Watchdog_Clear()

#endif	// #ifdef CONFIG_MODULE_WATCHDOG_ENABLE


#endif /* WATCHDOG_H_ */
