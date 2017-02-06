/*
 *		Watchdog.h
 *
 *		Created on:		2017. febr. 6.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 6.
 */

#ifndef MODULES_WATCHDOG_H_
#define MODULES_WATCHDOG_H_




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

void Watchdog_Init(void);
void Watchdog_Clear(void);

#else

// If Watchdog not enabled
#define Watchdog_Clear()

#endif	// #ifdef CONFIG_MODULE_WATCHDOG_ENABLE


#endif /* MODULES_WATCHDOG_H_ */
