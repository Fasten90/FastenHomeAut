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
#include "include.h"
#include "EventList.h"
#include "Communication.h"
#include "EventLog.h"



#ifdef CONFIG_MODULE_EVENTLOG_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

static EventLogRecord_t EventLogs[CONFIG_EVENTLOG_LOG_SIZE] = { 0 };
static LogCnt_t LogCounter = 0;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void EventLog_PrintLog(char *str, EventLogRecord_t *logRecord);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief	Initialize EventHandler
 */
void EventLog_Init(void)
{
	memset(EventLogs, 0, sizeof(EventLogs));
	EventLog_LogEvent(Event_LogEventStated, EventType_SystemEvent, 0);
}



/**
 * \brief	Log event (to history)
 */
void EventLog_LogEvent(EventName_t eventName, EventType_t eventType, EventStatus_t eventStatus)
{
	if (LogCounter >= CONFIG_EVENTLOG_LOG_SIZE-1)
	{
#if EVENTLOG_BUFFER_USE_CIRCULAR == 1
		LogCounter = 0;
#else
		// In not circular buffer mode, do not save record, exit
		return;
#endif
	}

	// Save event
	EventLogs[LogCounter].eventName = eventName;
	EventLogs[LogCounter].eventType = eventType;
	EventLogs[LogCounter].eventStatus = eventStatus;
	EventLogs[LogCounter].tick = HAL_GetTick();
#ifdef CONFIG_MODULE_RTC_ENABLE
	RTC_GetDateTime(&EventLogs[LogCounter].dateTime);
#endif

	LogCounter++;

}



/**
 * \brief	Print log
 */
static void EventLog_PrintLog(char *str, EventLogRecord_t *logRecord)
{
	usprintf(str, "| %3u | %2u | 0x%X | %9u |",
			logRecord->eventName,
			logRecord->eventType,
			logRecord->eventStatus,
			logRecord->tick);
}



/**
 * \brief	Print all logs
 */
void EventLog_PrintAllLogRecords(void)
{
	// TODO: Get communication protocol from parameter
	CommProtocol_t comm = CommProt_DebugUart;

	LogCnt_t i;
	char str[40];

	// Send header
	const char * const fixheader = "+-----+-----+----+------------+-----------+\r\n";
	const char * const headertxt = "| ID  | Name|Type|   Status   |    Tick   |\r\n";

	COMMUNICATION_SendMessage(comm, fixheader);
	COMMUNICATION_SendMessage(comm, headertxt);
	COMMUNICATION_SendMessage(comm, fixheader);


	// Send i. log record
	for (i = 0; i < CONFIG_EVENTLOG_LOG_SIZE; i++)
	{
		if (EventLogs[i].eventName)
		{
			EventLog_PrintLog(str, &EventLogs[i]);
			COMMUNICATION_Printf(comm, "| %3u ", i);
			COMMUNICATION_SendMessage(comm, str);
			COMMUNICATION_SendMessage(comm, "\r\n");
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



#endif	// #ifdef CONFIG_MODULE_EVENTLOG_ENABLE
