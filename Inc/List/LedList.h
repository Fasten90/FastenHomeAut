/*
 *		LedList.h
 *		Created on:		2018-02-03
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2018-02-03
 */

#ifndef LEDLIST_H_
#define LEDLIST_H_

#include "options.h"
#include "board.h"
#include "LED.h"



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< Available LEDs (on board)
typedef enum
{
	LED_Unknown,
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)
	LED_Green,
#endif
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS)
	LED_Blue,
	LED_Red,
#endif

	// Do not use:
	LED_Count
} LED_Name_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void LedList_Init(void);



#endif /* LEDLIST_H_ */
