/*
 *		IOList.h
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
#include "IO.h"



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< Available IO - outputs (on board)
typedef enum
{
	IO_Output_Unknown,
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)
	IO_LED_Green,
#endif
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS)
	IO_LED_Blue,
	IO_LED_Red,
#endif

	// Do not use:
	IO_Output_Count
} IO_Output_Name_t;


///< Available IO - inputs (on board)
typedef enum
{
	IO_Input_Unknown,
#ifdef CONFIG_MODULE_IO_INPUT_MOTION_ENABLE
	IO_Input_MotionMove,
#endif
#ifdef CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE
	IO_Input_SoundImpact,
#endif
#ifdef CONFIG_MODULE_IO_BATTERY_CHARGER_ENABLE
	IO_Input_BatteryCharger,
#endif

	// Do not use:
	IO_Input_Count
} IO_Input_Name_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void IO_List_Init(void);



#endif /* LEDLIST_H_ */
