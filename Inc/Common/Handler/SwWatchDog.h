/*
 *    SwWatchDog.h
 *    Created on:   2017-12-02
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef COMMON_HANDLER_SWWATCHDOG_H_
#define COMMON_HANDLER_SWWATCHDOG_H_


#include "options.h"



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_DEBUG_SW_WATCHDOG_ENABLE
    #define SW_WATCHDOG_INC()                SwWatchDog_Inc()
    #define SW_WATCHDOG_CHECK()                SwWatchDog_CheckState()
#else
    #define SW_WATCHDOG_INC()
    #define SW_WATCHDOG_CHECK()
#endif


#define SW_WATCHDOG_PERIOD                    (1050U)                    // TaskHandler Sw Watchdog period time [ms]



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void SwWatchDog_Inc(void);
void SwWatchDog_CheckState(void);



#endif /* COMMON_HANDLER_SWWATCHDOG_H_ */
