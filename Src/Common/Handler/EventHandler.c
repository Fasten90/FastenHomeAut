/*
 *    EventHandler.c
 *    Created on:   2017-02-05
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Event handler
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "EventList.h"
#include "Communication.h"
#include "StringHelper.h"
#include "MemHandler.h"
#include "DateTime.h"
#include "TaskList.h"
#include "EventLog.h"
#include "EventHandler.h"

#ifdef CONFIG_MODULE_EVENTHANDLER_UNITTEST_ENABLE
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
 * @brief    Initialize EventHandler
 */
void EventHandler_Init(void)
{
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
    ASSERT(TasksNum <= 31);
    // "EventHandler will crash if you use more Subscription than Tasks's num (TasksNum)"

    // Check required task ID-s
    uint8_t i;
    for (i = 0; i < TasksNum; i++)
    {
        if (EventList[i].isHasRequiredTask)
        {
            ASSERT(EventList[i].requiredTaskRunId < TasksNum);
        }
    }
#endif

    memset(Events, 0, sizeof(Events));            // Fill with default value

    // Fill with default value
    memset(Events, 0, sizeof(Events));

    // Generate event
    // TODO: Task ID 0 is good idea? Now not need this event generating
    //EventHandler_GenerateEvent(Event_LogEventStarted, 0, 0);
}



/**
 * @brief    Generate event
 */
void EventHandler_GenerateEvent(EventName_t eventName, EventData_t eventData, TaskID_t taskSource)
{
    // Check parameter
    if (eventName >= Event_Count)
        return;

    // Save event
    Events[eventName].tick = HAL_GetTick();                                    // Actual tick
#ifdef CONFIG_EVENTHANDLER_REQUIRED_TASK_MODE
    Events[eventName].eventRaised = EventList[eventName].subscription;        // Fill with subscription (tasks)
#else
    Events[eventName].eventRaised = 0xFFFFFFFF;                                // Fill all bits
#endif

#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
    // Check, need require a task run?
    if (EventList[eventName].isHasRequiredTask)
    {
        TaskHandler_RequestTaskScheduling(EventList[eventName].requiredTaskRunId);
    }
#endif

    // Log event, if need
#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
    if (EventList[eventName].isNeedLog)
    {
        EventLog_LogEvent(eventName, eventData, taskSource, EventType_Raised);
    }
#else
    (void)taskSource;
    (void)eventData;
#endif
}



/**
 * @brief    Print log
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
        Events[eventName].eventRaised &= ~(1 << taskSource);
    }
    else
    {
        isRaised = false;
    }

    // TODO: Check is is subscripted?

    // Log event, if need
#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
    if (EventList[eventName].isNeedLog)
    {
        EventLog_LogEvent(eventName, taskSource, taskSource, EventType_Get);
    }
#endif

    return isRaised;
}



/**
 * @brief    Clear event (This event is not valid now)
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
#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
    if (EventList[eventName].isNeedLog)
    {
        EventLog_LogEvent(eventName, taskSource, taskSource, EventType_Get);
    }
#else
    (void)taskSource;
#endif
}



/**
 * @brief    Get Event type name
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



#ifdef CONFIG_MODULE_EVENTHANDLER_UNITTEST_ENABLE
/**
 * @brief    EventHandler UnitTest
 */
uint32_t EventHandler_UnitTest(void)
{
    // Start Unit test
    UnitTest_Start("EventHandler", __FILE__);


    // Test EventHandler initialize
    EventHandler_Init();

    // Check, EventList is "free"?
    // @note: Do not forget, this work well, if there is no an interrupted event

    uint32_t i;
    for (i = 0; i < NUM_OF(Events); i++)
    {
        if (i != Event_LogEventStarted)
        {
            UNITTEST_ASSERT(Events[i].tick == 0, "EventList is not cleared!");
            UNITTEST_ASSERT(Events[i].eventRaised == 0, "EventList is not cleared!");
        }
        // LogEventStarted --> has value
    }


    // Test EventHandler_GenerateEvent() & EventHandler_CheckEventState() pair

    // Test: 0. event, 0 data, 0. task source
    EventHandler_GenerateEvent(0, 0, 0);

    // Get 0. event, 0. task source
    UNITTEST_ASSERT(EventHandler_CheckEventState(0, 0) == true, "Event - generate and check is wrong");

    // It is cleared?
    UNITTEST_ASSERT(EventHandler_CheckEventState(0, 0) == false, "Event - generate and check is wrong");

    // Get other event - it is not was set
    UNITTEST_ASSERT(EventHandler_CheckEventState(1, 1) == false, "Event - generate and check is wrong");


    // Test with other parameters
    EventHandler_GenerateEvent(Event_LogEventStarted, 0xFFFFFFFF, Event_LogEventStarted);

    // Get 0. event, 0. task source
    UNITTEST_ASSERT(EventHandler_CheckEventState(Event_LogEventStarted, 10) == true, "Event - generate and check is wrong");

    // It is cleared?
    UNITTEST_ASSERT(EventHandler_CheckEventState(Event_LogEventStarted, 10) == false, "Event - generate and check is wrong");

    // Get other event - it is not was set
    UNITTEST_ASSERT(EventHandler_CheckEventState(Event_LogEventStarted, 9) == true, "Event - generate and check is wrong");


    // Test EventHandler_ClearEvent

    // Set the Event
    EventHandler_GenerateEvent(Event_LogEventStarted, 0xFFFFFFFF, 10);

    // Clear the event
    EventHandler_ClearEvent(Event_LogEventStarted, 10);

    // Check, event is "cleared" ?
    UNITTEST_ASSERT(EventHandler_CheckEventState(Event_LogEventStarted, 10) == false, "Event - Clear is wrong");


    // Finish unit test
    return UnitTest_End();
}

#endif



#endif    // #ifdef CONFIG_MODULE_EVENTLOG_ENABLE
