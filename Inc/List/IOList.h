/*
 *    IOList.h
 *    Created on:   2018-02-03
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef IOLIST_H_
#define IOLIST_H_

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

#if defined(CONFIG_FUNCTION_TRAFFIC_LIGHT)
    IO_AppTrafficLight_Red,
    IO_AppTrafficLight_Yellow,
    IO_AppTrafficLight_Green,
#endif /* CONFIG_FUNCTION_TRAFFIC_LIGHT */

    /* Do not use: */
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

    /* Do not use: */
    IO_Input_Count
} IO_Input_Name_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void IO_List_Init(void);



#endif /* IOLIST_H_ */
