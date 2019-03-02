/*
 *    Watchdog.h
 *    Created on:   2017-02-06
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Watchdog
 *    Target:       STM32Fx
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"



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

#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
// Watchdog enabled: function implement in .c file
void Watchdog_Init(void);
void Watchdog_Clear(void);

#else
// If Watchdog not enabled
#define Watchdog_Clear()

#endif    // #ifdef CONFIG_MODULE_WATCHDOG_ENABLE


#endif /* WATCHDOG_H_ */
