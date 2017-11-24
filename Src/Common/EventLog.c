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
#include "CommandHandler.h"
#include "DebugUart.h"

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
#ifdef CONFIG_EVETNLOG_PRINT_IMMEDIATELY
static void EventLog_DebugPrintLog(EventLogRecord_t * eventRecord);
#endif



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

#ifdef CONFIG_EVETNLOG_PRINT_IMMEDIATELY
	if (eventName != Event_LogEventStated)
		EventLog_DebugPrintLog(&EventLogs[LogCounter]);
#endif

	LogCounter++;
}



#ifdef CONFIG_EVETNLOG_PRINT_IMMEDIATELY
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


static const char * const fixheader = "| %3u | %20s | %9u | %20s | %8X | %10s | %20s |";



/**
 * \brief	Print log
 */
static void EventLog_PrintLog(char *str, EventLogCnt_t cnt, EventLogRecord_t *logRecord)
{
	if (logRecord->eventName > Event_Count)
		return;

	char timeStr[DATETIME_STRING_MAX_LENGTH];
	DateTime_PrintDateTimeToString(timeStr, &logRecord->dateTime);

	usprintf(str, fixheader,
			cnt,
			timeStr,
			logRecord->tick,
			EventList[logRecord->eventName].name,
			logRecord->eventData,
			EventHandler_GetEventTypeName(logRecord->eventType),
			TaskList[logRecord->taskSource].taskName
			);
}



// TODO: Str_FormatBorderAndHeader(str, header, fixheader, ...);



void EventLog_PrintBorder(void)
{
	char str[120];
	char header[49];

	Str_PrintHeader(str, header, fixheader, false);
	DebugUart_SendLine(str);

	Str_PrintHeader(str, header, fixheader, true, "Id", "DateTime", "Tick", "EventName", "Data", "EventType", "TaskName");
	DebugUart_SendLine(str);

	Str_PrintHeader(str, header, fixheader, false);
	DebugUart_SendLine(str);
}



/**
 * \brief	Print all logs
 */
void EventLog_PrintAllLogRecords(void)
{
	// TODO: Get communication protocol from parameter
#if 0
	EventLogCnt_t i;
	char str[120];

	// Send header
	const char * const fixheader = "+-----+----------------------+-----------+----------------------+------------+------------+----------------------+\r\n";
	const char * const headertxt = "|  Id |       DateTime       |    Tick   |      EventName       |   Data     | EventType  |       Taskname       |\r\n";

	CmdH_SendMessage("\r\n");
	CmdH_SendMessage(fixheader);
	CmdH_SendMessage(headertxt);
	CmdH_SendMessage(fixheader);


	// Send i. log record
	for (i = 0; i < CONFIG_EVENTLOG_LOG_SIZE; i++)
	{
		if (EventLogs[i].eventName)
		{
			EventLog_PrintLog(str, i, &EventLogs[i]);
			CmdH_SendMessage(str);
		}
		else
		{
			// There is no more "valid" log record
			break;
		}
	}

	// Send end of log
	CmdH_SendMessage(fixheader);
	CmdH_SendMessage(headertxt);
	CmdH_SendMessage(fixheader);
#else

	EventLogCnt_t i;
	char str[120];


	EventLog_PrintBorder();


	// Send i. log record
	for (i = 0; i < CONFIG_EVENTLOG_LOG_SIZE; i++)
	{
		if (EventLogs[i].eventName)
		{
			EventLog_PrintLog(str, i, &EventLogs[i]);
			DebugUart_SendLine(str);
		}
		else
		{
			// There is no more "valid" log record
			break;
		}
	}

	// Send end of log
	EventLog_PrintBorder();

#endif
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
