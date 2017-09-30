/*
 *		EventLog.c
 *		Created on:		2017-02-05
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Event logger
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-05
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
#include "EventLog.h"
#include "TaskList.h"
#include "Debug.h"

#ifdef MODULE_EVENTLOG_UNITTEST_ENABLE
	#include "UnitTest.h"
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

static void EventLog_PrintLog(char *str, EventLogCnt_t cnt, EventLogRecord_t *logRecord);
static void EventLog_DebugPrintLog(EventLogRecord_t * eventRecord);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief	Initialize EventLog
 */
void EventLog_Init(void)
{
	memset(EventLogs, 0, sizeof(EventLogs));

	LogCounter = 0;

	EventHandler_GenerateEvent(Event_LogEventStated, 0, 0);
}



/**
 * \brief	Log event (to history)
 */
void EventLog_LogEvent(EventName_t eventName, EventData_t eventData, TaskID_t taskSource, EventType_t eventType)
{
	if (LogCounter > CONFIG_EVENTLOG_LOG_SIZE-1)
	{
#if EVENTLOG_BUFFER_USE_CIRCULAR == 1
		LogCounter = 0;			// In circular buffer mode, continue at begin of buffer
#else
		return;					// In not circular buffer mode, do not save record, exit
#endif
	}

	// Save event
	EventLogs[LogCounter].eventName = eventName;
	EventLogs[LogCounter].eventData = eventData;
	EventLogs[LogCounter].eventType = eventType;
	EventLogs[LogCounter].taskSource = taskSource;
	EventLogs[LogCounter].tick = HAL_GetTick();
#if defined(CONFIG_MODULE_RTC_ENABLE)
	RTC_GetDateTime(&EventLogs[LogCounter].dateTime);
#elif defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE)
	EventLogs[LogCounter].dateTime = DateTime_SystemTime;
#endif

#ifdef CONFIG_MODULE_DEBUG_ENABLE
	if (eventName != Event_LogEventStated)
		EventLog_DebugPrintLog(&EventLogs[LogCounter]);
#endif

	LogCounter++;
}



#ifdef CONFIG_MODULE_DEBUG_ENABLE
/**
 * \brief	Debug print
 */
static void EventLog_DebugPrintLog(EventLogRecord_t * eventRecord)
{
	Debug_Print(Debug_EventHandler,
			"%s - Data: 0x%X, type: %s, task: %s, tick: %d",
			EventList[eventRecord->eventName].name,
			eventRecord->eventData,
			EventHandler_GetEventTypeName(eventRecord->eventType),
			TaskList[eventRecord->taskSource].taskName,
			eventRecord->tick);
}
#endif



/**
 * \brief	Print log
 */
static void EventLog_PrintLog(char *str, EventLogCnt_t cnt, EventLogRecord_t *logRecord)
{
	if (logRecord->eventName > Event_Count)
		return;

	char timeStr[DATETIME_STRING_MAX_LENGTH];
	DateTime_PrintDateTimeToString(timeStr, &logRecord->dateTime);

	usprintf(str, "| %3u | %20s | %9u | %20s | 0x%X | %10s | %20s |\r\n",
			cnt,
			timeStr,
			logRecord->tick,
			EventList[logRecord->eventName].name,
			logRecord->eventData,
			EventHandler_GetEventTypeName(logRecord->eventType),
			TaskList[logRecord->taskSource].taskName
			);
}



/**
 * \brief	Print all logs
 */
void EventLog_PrintAllLogRecords(void)
{
	// TODO: Get communication protocol from parameter
	CommProtocol_t comm = CommProt_DebugUart;

	EventLogCnt_t i;
	char str[120];

	// Send header
	const char * const fixheader = "+-----+----------------------+-----------+----------------------+------------+------------+----------------------+\r\n";
	const char * const headertxt = "|  Id |       DateTime       |    Tick   |      EventName       |   Data     | EventType  |       Taskname       |\r\n";

	COMMUNICATION_SendMessage(comm, "\r\n");
	COMMUNICATION_SendMessage(comm, fixheader);
	COMMUNICATION_SendMessage(comm, headertxt);
	COMMUNICATION_SendMessage(comm, fixheader);


	// Send i. log record
	for (i = 0; i < CONFIG_EVENTLOG_LOG_SIZE; i++)
	{
		if (EventLogs[i].eventName)
		{
			EventLog_PrintLog(str, i, &EventLogs[i]);
			COMMUNICATION_SendMessage(comm, str);
		}
		else
		{
			// There is no more "valid" log record
			break;
		}
	}

	// Send end of log
	COMMUNICATION_SendMessage(comm, fixheader);
	COMMUNICATION_SendMessage(comm, headertxt);
	COMMUNICATION_SendMessage(comm, fixheader);
}



#ifdef MODULE_EVENTLOG_UNITTEST_ENABLE
void EventLog_UnitTest(void)
{
	bool result;

	// Start Unit test
	UnitTest_Start("EventLog", __FILE__);


	// Test EventLog initialize

	EventLog_Init();

	// Check, first record is "LogEventStarted" ?
	result = true;
	result &= (EventLogs[0].eventName == Event_LogEventStated);
	result &= (EventLogs[0].eventType == EventType_Raised);
	result &= (EventLogs[0].eventData == 0);
	result &= (EventLogs[0].tick != 0);
	UNITTEST_ASSERT(result, "EventLog_Init error");

	// Check, second record is empty?
	result = true;
	result &= (EventLogs[1].eventName == 0);
	result &= (EventLogs[1].eventType == 0);
	result &= (EventLogs[1].eventData == 0);
	result &= (EventLogs[1].tick == 0);
	UNITTEST_ASSERT(result, "EventLog_Init error");


	// Test log event (record)

	EventLog_LogEvent(Event_LogEventStated, 0x12345678, 0, EventType_Raised);

	// Check, second record is "LogEventStarted" ?
	result = true;
	result &= (EventLogs[1].eventName == Event_LogEventStated);
	result &= (EventLogs[1].eventType == EventType_Raised);
	result &= (EventLogs[1].eventData == 0x12345678);
	result &= (EventLogs[1].tick != 0);
	UNITTEST_ASSERT(result, "EventLog_LogEvent error");


	// Finish unit test
	UnitTest_End();

}
#endif



#endif	// #ifdef CONFIG_MODULE_EVENTLOG_ENABLE
