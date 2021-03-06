/*
 *    EventList.h
 *    Created on:   2017-08-31
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Event list
 *    Target:       STM32Fx
 */

#ifndef EVENTLIST_H_
#define EVENTLIST_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
#include "TaskHandler.h"
#endif



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< Event name list
typedef enum
{
    Event_Unknown,
    Event_LogEventStarted,
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
    Event_TaskRequired,
    Event_TaskScheduled,
    Event_TaskFinished,
    Event_TaskCleared,
#endif
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
    Event_DebugUartReceive,
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
    Event_ButtonPressed,
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
    Event_Esp8266ReceivedMessage,
    Event_Esp8266UserEvent,
#endif
#ifdef CONFIG_MODULE_COMMANDHANDLER_ENABLE
    Event_CommandHandler_ProcessCommand,
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
    Event_Display_SpiEvent,
#endif

    /* XXX: Add here new event */
    /* @note    Sync with EventList[] */

    Event_Count

} EventName_t;



#endif /* EVENTLIST_H_ */
