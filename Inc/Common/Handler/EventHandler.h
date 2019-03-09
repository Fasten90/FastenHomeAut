/*
 *    EventHandler.h
 *    Created on:   2017-02-05
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Event handler
 *    Target:       STM32Fx
 */

#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_



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

typedef uint32_t    EventData_t;
typedef uint8_t        EventId_t;

/* EventLog - subscription bits - for TaskHandler */
typedef uint32_t    EventLogSubscription_t;


///< Event - dynamic struct
typedef struct
{
    uint32_t tick;                                ///< Tick (When occurred the event)
    EventLogSubscription_t eventRaised;            ///< Event flags
} EventRun_t;


typedef enum
{
    EventType_Unknown,
    EventType_Raised,
    EventType_Get,
    EventType_Cleared,

    /* Do not use this! */
    EventType_Count
} EventType_t;


#ifndef CONFIG_MODULE_TASKHANDLER_ENABLE
typedef uint8_t TaskID_t;
#endif


///< Event - static struct
typedef struct
{
    const char * name;                        ///< Event name (string)
    bool isNeedLog;                            ///< Need log?
#ifdef CONFIG_EVENTHANDLER_REQUIRED_TASK_MODE
    EventLogSubscription_t subscription;    ///< Subscriptions (bits)
#endif
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
    bool isHasRequiredTask;                    ///< Need require task scheduling? (Valid with below variable)
    TaskID_t requiredTaskRunId;                ///< Task, which need execute, when event raised
#endif

    /* @note: Please Do not change this structure! */
} Event_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

///< EventList - in EventList.c !!
extern const Event_t EventList[];

///< EventList length - in EventList.c !!
extern const EventId_t EventsNum;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void EventHandler_Init(void);
void EventHandler_GenerateEvent(EventName_t eventName, EventData_t eventData, TaskID_t taskSource);
bool EventHandler_CheckEventState(EventName_t eventName, TaskID_t taskSource);
void EventHandler_ClearEvent(EventName_t eventName, TaskID_t taskSource);
const char * EventHandler_GetEventTypeName(EventType_t eventType);

uint32_t EventHandler_UnitTest(void);

#else

#define EventHandler_GenerateEvent(...)
bool EventHandler_CheckEventState(...)

#endif /* #ifdef CONFIG_MODULE_EVENTLOG_ENABLE */



#endif /* EVENTHANDLER_H_ */
