/*
 *		EventHandler.h
 *
 *		Created on:		2017. febr. 5.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 5.
 */

#ifndef COMMON_EVENTHANDLER_H_
#define COMMON_EVENTHANDLER_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "EventList.h"
#include "DateTime.h"



#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


typedef uint32_t	EventData_t;

typedef uint8_t		EventId_t;


///< Event - dynamic struct
typedef struct
{
	uint32_t tick;
	EventLogSubscription_t eventRaised;
} EventRun_t;


typedef enum
{
	EventType_Unknown,
	EventType_Raised,
	EventType_Get,
	EventType_Cleared,

	// Do not use this!
	EventType_Count
} EventType_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void EventHandler_Init(void);
void EventHandler_GenerateEvent(EventName_t eventName, EventData_t eventData, TaskID_t taskSource);
bool EventHandler_CheckEventState(EventName_t eventName, TaskID_t taskSource);
void EventHandler_ClearEvent(EventName_t eventName, TaskID_t taskSource);
const char * EventHandler_GetEventTypeName(EventType_t eventType);

void EventHandler_UnitTest(void);

#else

#define EventHandler_GenerateEvent(...)
bool EventHandler_CheckEventState(...)

#endif // #ifdef CONFIG_MODULE_EVENTLOG_ENABLE


#endif /* COMMON_EVENTHANDLER_H_ */
