/*
 *    EventLog.c
 *    Created on:   2017-02-05
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Event logger
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
#include "EventLog.h"
#include "TaskList.h"
#include "Debug.h"
#include "CommandHandler.h"
#include "DebugUart.h"
#include "Table.h"
#if (EVENTLOG_SAVE_DATETIME == 1)
#include "SysTime.h"
#endif

#ifdef CONFIG_MODULE_EVENTLOG_UNITTEST_ENABLE
    #include "UnitTest.h"
    #include "ErrorHandler.h"
    #include "Timing.h"
#endif



#ifdef CONFIG_MODULE_EVENTLOG_ENABLE


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

static EventLogRecord_t EventLogs[CONFIG_EVENTLOG_LOG_SIZE] = { 0 };
static EventLogCnt_t LogCounter = 0;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/


static void EventLog_PrintLogTableHeader(const char * fixheader, char * str, uint8_t strMaxLen, char * header);
#ifdef CONFIG_EVETNLOG_PRINT_IMMEDIATELY
static void EventLog_DebugPrintLog(EventLogRecord_t * eventRecord);
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

/**
 * @brief       Initialize EventLog
 */
void EventLog_Init(void)
{
    memset(EventLogs, 0, sizeof(EventLogs));

    LogCounter = 0;

#if ( EVENTLOG_SAVE_EVENT_AT_INIT == 1 )
    /* ventHandler_GenerateEvent(Event_LogEventStarted, 0, 0); */
#endif
}



/**
 * @brief       Log event (to history)
 */
void EventLog_LogEvent(EventName_t eventName, EventData_t eventData, TaskID_t taskSource, EventType_t eventType)
{
    if (LogCounter > CONFIG_EVENTLOG_LOG_SIZE-1)
    {
#if EVENTLOG_BUFFER_USE_CIRCULAR == 1
        LogCounter = 0;            /* In circular buffer mode, continue at begin of buffer */
#else
        return;                    /* In not circular buffer mode, do not save record, exit */
#endif
    }

    /* Save event */
    EventLogs[LogCounter].eventName = eventName;
    EventLogs[LogCounter].eventData = eventData;
    EventLogs[LogCounter].eventType = eventType;
    EventLogs[LogCounter].taskSource = taskSource;
    EventLogs[LogCounter].tick = HAL_GetTick();
#if (EVENTLOG_SAVE_DATETIME == 1)
    #if defined(CONFIG_MODULE_RTC_ENABLE)
    RTC_GetDateTime(&EventLogs[LogCounter].dateTime);
    #elif defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE)
    DateTime_t dateTime;
    SysTime_GetDateTime(&dateTime);
    memcpy(&EventLogs[LogCounter].dateTime, &dateTime, sizeof(dateTime));
    #endif
#endif

#ifdef CONFIG_EVETNLOG_PRINT_IMMEDIATELY
    if (eventName != Event_LogEventStated)
        EventLog_DebugPrintLog(&EventLogs[LogCounter]);
#endif

    LogCounter++;
}



#ifdef CONFIG_EVETNLOG_PRINT_IMMEDIATELY
/**
 * @brief       Debug print
 */
static void EventLog_DebugPrintLog(EventLogRecord_t * eventRecord)
{
    Debug_Printf(Debug_EventHandler,
            "%s - Data: 0x%X, type: %s, task: %s, tick: %d",
            EventList[eventRecord->eventName].name,
            eventRecord->eventData,
            EventHandler_GetEventTypeName(eventRecord->eventType),
            TaskList[eventRecord->taskSource].taskName,
            eventRecord->tick);
}
#endif



static void EventLog_PrintLogTableHeader(const char * fixheader, char * str, uint8_t strMaxLen, char * header)
{
#if (EVENTLOG_SAVE_DATETIME == 1)
    Table_PrintTableWithBorder(fixheader, str, strMaxLen, header, "Id", "DateTime", "Tick", "EventName", "Data", "EventType", "TaskName");
#else
    Table_PrintTableWithBorder(fixheader, str, strMaxLen, header, "Id", "Tick", "EventName", "Data", "EventType", "TaskName");
#endif
}



/**
 * @brief       Print all logs
 */
void EventLog_PrintLogTable(void)
{
    EventLogCnt_t i;

#if (EVENTLOG_SAVE_DATETIME == 1)
    const char const fixheader[] = "| %3u | %20s | %9u | %20s | %8X | %10s | %20s |";
    const uint8_t strLength = 2 + 3 + 3 + 20 + 3 + 9 + 3 + 20 + 3 + 10 + 3 + 8 + 3 + 20 + 2 + 1;
    char str[strLength];
#else
    const char const fixheader[] = "| %3u | %9u | %20s | %8X | %10s | %20s |";
    const uint8_t strLength = 2 + 3 + 3 + 9 + 3 + 20 + 3 + 10 + 3 + 8 + 3 + 20 + 2 + 1;
    char str[strLength];
#endif

    char header[sizeof(fixheader)];

    EventLog_PrintLogTableHeader(fixheader, str, strLength, header);


    /* Send i. log record */
    for (i = 0; i < CONFIG_EVENTLOG_LOG_SIZE; i++)
    {
        if (EventLogs[i].eventName)
        {

#if (EVENTLOG_SAVE_DATETIME == 1)
    char timeStr[DATETIME_STRING_MAX_LENGTH];
    DateTime_PrintDateTimeToString(timeStr, &logRecord->dateTime);
#endif

    usnprintf(str, strLength, fixheader,
            i,
#if (EVENTLOG_SAVE_DATETIME == 1)
            timeStr,
#endif
            EventLogs[i].tick,
            EventList[EventLogs[i].eventName].name,
            EventLogs[i].eventData,
            EventHandler_GetEventTypeName(EventLogs[i].eventType),
            TaskList[EventLogs[i].taskSource].taskName
            );

            Table_SendLine(str);
        }
        else
        {
            /* There is no more "valid" log record */
            break;
        }
    }

    /* Send end of log */
    EventLog_PrintLogTableHeader(fixheader, str, strLength, header);
}



#ifdef CONFIG_MODULE_EVENTLOG_UNITTEST_ENABLE
uint32_t EventLog_UnitTest(void)
{
    /* Start Unit test */
    UnitTest_Start("EventLog", __FILE__);


    /* Need wait 1 ms, for first eventlog create tick result (!=0) */
    DelayMs(1);

    /* Test EventLog initialize */

    EventLog_Init();

#if ( EVENTLOG_SAVE_EVENT_AT_INIT == 1 )
    /* Check, first record is "LogEventStarted" ? */
    UNITTEST_ASSERT(EventLogs[0].eventName == Event_LogEventStarted, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[0].eventType == EventType_Raised, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[0].eventData == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[0].tick != 0, "EventLog_Init error");

    /* Check, second record is empty ? */
    UNITTEST_ASSERT(EventLogs[1].eventName == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[1].eventType == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[1].eventData == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[1].tick == 0, "EventLog_Init error");


    /* Test log event (record) */

    EventLog_LogEvent(Event_LogEventStarted, 0x12345678, 0, EventType_Raised);

    /* Check, second record is "LogEventStarted" ? */
    UNITTEST_ASSERT(EventLogs[1].eventName == Event_LogEventStarted, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[1].eventType == EventType_Raised, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[1].eventData == 0x12345678, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[1].tick != 0, "EventLog_Init error");

#else

    /* Check, first record is empty ? */
    UNITTEST_ASSERT(EventLogs[0].eventName == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[0].eventType == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[0].eventData == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[0].tick == 0, "EventLog_Init error");

    /* Check, second record is empty ? */
    UNITTEST_ASSERT(EventLogs[1].eventName == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[1].eventType == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[1].eventData == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[1].tick == 0, "EventLog_Init error");


    /* Test log event (record) */

    EventLog_LogEvent(Event_LogEventStarted, 0x12345678, 0, EventType_Raised);

    /* Check, second record is "LogEventStarted" ? */
    UNITTEST_ASSERT(EventLogs[0].eventName == Event_LogEventStarted, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[0].eventType == EventType_Raised, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[0].eventData == 0x12345678, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[0].tick != 0, "EventLog_Init error");

    /* Check, second record is empty ? */
    UNITTEST_ASSERT(EventLogs[1].eventName == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[1].eventType == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[1].eventData == 0, "EventLog_Init error");
    UNITTEST_ASSERT(EventLogs[1].tick == 0, "EventLog_Init error");

#endif

    /* Finish unit test */
    return UnitTest_End();
}
#endif



#endif    /* #ifdef CONFIG_MODULE_EVENTLOG_ENABLE */
