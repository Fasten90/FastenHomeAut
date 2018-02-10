/*
 *		EventList.c
 *		Created on:		2017-08-31
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Event list
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-08-31
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"
#include "EventHandler.h"
#include "EventList.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

///< Event list for Event handler
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
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
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
		.name = "Esp8266RecvMsg",
		.isNeedLog = true,
	},
	{
		.name = "Esp8266UserEvent",
		.isNeedLog = true,
	},
#endif
#ifdef CONFIG_MODULE_COMMANDHANDLER_ENABLE
	{
		.name = "CmdProcess",
		.isNeedLog = true,
	},
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
	{
		.name = "DisplaySpiEvent",
		.isNeedLog = true,
	}
#endif

	// XXX: Add here new event
	// \note	Sync with EventName_t
};


///< Events num
const EventId_t EventsNum = NUM_OF(EventList);



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief	Initialize EventList - check
 */
void EventList_init(void)
{
	// Check EventList size
	BUILD_ASSERT(NUM_OF(EventList) == Event_Count);
}


