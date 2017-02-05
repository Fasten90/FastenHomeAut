/*
 *		EventList.c
 *
 *		Created on:		2017. febr. 5.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 5.
 */




/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "EventHandler.h"
#include "EventList.h"


#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE


/*------------------------------------------------------------------------------
 *  Function prototypes
 *----------------------------------------------------------------------------*/

// Event functions like
//typedef EventFunctionResult ( *EventFunctionPointer )(EventType_t eventType, EventStatus_t eventStatus);
EventFunctionResult EventExampleFunction(EventType_t eventType, EventStatus_t eventStatus);
EventFunctionResult EventDebugUartCommandReceived(EventType_t eventType, EventStatus_t eventStatus);


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
EventHandler_t EventList[] =
{
	{
	.eventName = Event_TestExample,
	.eventStringName = "TestEvent",
	.eventType = EventType_Event,
	.eventFunction = EventExampleFunction
	},
	{
	.eventName = Event_DebugUartReceivedCommand,
	.eventStringName = "DebugUartReceivedACommand",
	.eventType = EventType_Event,
	.eventFunction = EventDebugUartCommandReceived
	}

	// XXX: Add new event here

};


const EventId_t EventListNum = sizeof(EventList)/sizeof(EventList[0]);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


EventFunctionResult EventExampleFunction(EventType_t eventType, EventStatus_t eventStatus)
{
	(void)eventType;
	(void)eventStatus;

	LED_SetLed(1, LED_SET_TOGGLE);

	return EventFunctionResult_Ok;
}



EventFunctionResult EventDebugUartCommandReceived(EventType_t eventType, EventStatus_t eventStatus)
{
	(void)eventType;
	(void)eventStatus;

	CommandHandler_CheckCommand();

	return EventFunctionResult_Ok;
}



#endif	// #ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE
