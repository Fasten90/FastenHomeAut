/*
 *		EventList.c
 *
 *		Created on:		2017-08-31
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. aug. 31.
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "include.h"
#include "EventList.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


const Event_t EventList[] =
{
	{
		// Unknown (0) event
		.name = "-",
		// \note Do not log this!
		.isNeedLog = false,
	},
	{
		.name = "EventHandlerStarted",
		.isNeedLog = true,
	},
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
	{
		.name = "TaskRequired",
		.isNeedLog = true,
	},
	{
		.name = "TaskScheduled",
		.isNeedLog = true,
	},
	{
		.name = "TaskFinished",
		.isNeedLog = true,
	},
	{
		.name = "TaskCleared",
		.isNeedLog = true,
	},
#endif
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	{
		.name = "DebugUartReceive",
		.isNeedLog = true,
	},
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
	{
		.name = "ButtonPressed",
		.isNeedLog = true,
	},
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	{
		.name = "Esp8266ReceivedMessage",
		.isNeedLog = true,
	},
	{
		.name = "Esp8266UserEvent",
		.isNeedLog = true,
	},
#endif
#ifdef CONFIG_MODULE_COMMANDHANDLER_ENABLE
	{
		.name = "CommandProcessCommand",
		.isNeedLog = true,
	},
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
	{
		.name = "DisplaySpiEvent",
		.isNeedLog = false,
	}
#endif

	// XXX: Add here new event
	// \note	Sync with EventName_t
};



///< Events num
const EventId_t EventsNum = sizeof(EventList)/sizeof(EventList[0]);


/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

