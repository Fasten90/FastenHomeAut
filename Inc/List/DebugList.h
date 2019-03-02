/*
 *    DebugList.h
 *    Created on:   2017-09-08
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Debug 
 *    Target:       STM32Fx
 */

#ifndef DEBUGLIST_H_
#define DEBUGLIST_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< Debug
typedef enum
{
#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE
    Debug_EventHandler,
#endif
#ifdef CONFIG_MODULE_COMMANDHANDLER_ENABLE
    Debug_CommandHandler,
#endif
#ifdef CONFIG_FUNCTION_GAME_SNAKE
    Debug_GameSnake,
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
    Debug_ESP8266,
#endif
#ifdef CONFIG_BUTTON_DEBUG_ENABLE
    Debug_Button,
#endif
#ifdef CONFIG_MODULE_WEBPAGE_ENABLE
    Debug_WebPage,
#endif

    /*
     * XXX: Add here new Debug task enums
     * @note    Do not forget synchronize with DebugTasks
     */

    // Do not use, it used for enum count
    Debug_Count
} Debug_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/



#endif /* DEBUGLIST_H_ */
