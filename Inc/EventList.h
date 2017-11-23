/*
 *		EventList.h
 *		Created on:		2017-08-31
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Event list
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-08-31
 */

#ifndef EVENTLIST_H_
#define EVENTLIST_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
//#include "EventHandler.h"

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
	Event_LogEventStated,
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

	// XXX: Add here new event
	// \note	Sync with EventList[]

	Event_Count

} EventName_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



#endif /* EVENTLIST_H_ */
