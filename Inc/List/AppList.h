/*
 *    AppList.h
 *    Created on:   2019-03-22
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef APPLIST_H_
#define APPLIST_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "AppList.h"
#include "Logic.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
    AppType_MainMenu,

#ifdef CONFIG_FUNCTION_GAME_SNAKE
    AppType_Snake,
#endif
#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
    AppType_Input,
#endif
#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
    AppType_CarAnimation,
#endif
#ifdef CONFIG_DISPLAY_CLOCK_LARGE
    AppType_Clock,
#endif
#ifdef CONFIG_DISPLAY_ESP8266_ENABLE
    AppType_ESP8266,
#endif
#ifdef CONFIG_FUNCTION_TRAFFIC_LIGHT
    AppType_TrafficLight,
#endif
#ifdef CONFIG_FUNCTION_ELEVATOR
    AppType_Elevator,
#endif

    /* Note: Last item, do not put anything after */
    AppType_Count

} App_Type_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const App_List_t AppList[];

extern const uint8_t AppList_Num;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



#endif /* APPLIST_H_ */
