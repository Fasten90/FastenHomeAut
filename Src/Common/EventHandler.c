/*
 *		EventHandler.c
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

#include "options.h"
#include "EventList.h"
#include "Communication.h"
#include "String.h"
#include "MEM.h"
#include "DateTime.h"
#include "TaskList.h"
#include "EventLog.h"
#include "EventHandler.h"

#ifdef MODULE_EVENTHANDLER_UNITTEST_ENABLE
#include "UnitTest.h"
#endif



#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

static EventRun_t Events[Event_Count] = { 0 };



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * \brief	Initialize EventHandler
 */
void EventHandler_Init(void)
{
	// Check EventList size
	BUILD_BUG_ON(Event_Count != EventsNum);

	// Check required task ID-s
	uint8_t i;
	for (i = 0; i < TasksNum; i++)
	{
		if (EventList[i].isHasRequiredTask)
			ASSERT(EventList[i].requiredTaskRunId < TasksNum);
	}


	// Fill with default value
	memset(Events, 0, sizeof(Events));

	// Generate event
	// TODO: Task ID?
	EventHandler_GenerateEvent(Event_LogEventStated, 0, 0);
}



/**
 * \brief	Generate event
 */
void EventHandler_GenerateEvent(EventName_t eventName, EventData_t eventData, TaskID_t taskSource)
{
	// Check parameter
	if (eventName >= Event_Count)
		return;

	// Save event
	Events[eventName].tick = HAL_GetTick();									// Actual tick
	Events[eventName].eventRaised = EventList[eventName].subscription;		// Fill with subscription (tasks)

	// Check, need require a task run?
	if (EventList[eventName].isHasRequiredTask)
	{
		TaskHandler_RequestTaskScheduling(EventList[eventName].requiredTaskRunId);
	}

	// Log event, if need
	if (EventList[eventName].isNeedLog)
	{
		EventLog_LogEvent(eventName, eventData, taskSource, EventType_Raised);
	}
}



/**
 * \brief	Print log
 */
bool EventHandler_CheckEventState(EventName_t eventName, TaskID_t taskSource)
{
	bool isRaised = false;

	// Check parameter
	if (eventName >= Event_Count)
		return false;

	// Check the taskSource parameter is subscripted and event is raised?
	if (Events[eventName].eventRaised & (1 << taskSource))
	{
		isRaised = true;
	}
	else
	{
		isRaised = false;
	}

	// TODO: Check is is subscripted?

	// Log event, if need
	if (EventList[eventName].isNeedLog)
	{
		EventLog_LogEvent(eventName, taskSource, taskSource, EventType_Get);
	}

	return isRaised;
}



/**
 * \brief	Clear event (This event is not valid now)
 */
void EventHandler_ClearEvent(EventName_t eventName, TaskID_t taskSource)
{
	// Check parameter
	if (eventName >= Event_Count)
		return;

	// TODO: Need check subscription?
	// Clear event
	Events[eventName].eventRaised = 0;

	// Log event, if need
	if (EventList[eventName].isNeedLog)
	{
		EventLog_LogEvent(eventName, taskSource, taskSource, EventType_Get);
	}
}



/**
 * \brief	Get Event type name
 */
const char * EventHandler_GetEventTypeName(EventType_t eventType)
{
	const char * str = NULL;
	switch (eventType)
	{
		case EventType_Raised:
			str = "Raised";
			break;

		case EventType_Get:
			str = "Get";
			break;

		case EventType_Cleared:
			str = "Cleared";
			break;

		case EventType_Unknown:
		case EventType_Count:
		default:
			break;
	}

	return str;
}



#ifdef MODULE_EVENTHANDLER_UNITTEST_ENABLE
void EventHandler_UnitTest(void)
{

	bool result;


	// Start Unit test
	UnitTest_Start("EventHandler", __FILE__);


	// Test EventHandler initialize
	// TODO: Do it...
	result = true;
	UNITTEST_ASSERT(result, "EventHandler... error");



	// Finish unit test
	UnitTest_End();

}
#endif



#endif	// #ifdef CONFIG_MODULE_EVENTLOG_ENABLE
