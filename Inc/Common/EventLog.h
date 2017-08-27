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

#ifndef COMMON_EVENTLOG_H_
#define COMMON_EVENTLOG_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "options.h"
#include "GenericTypeDefs.h"
#include "EventList.h"


#ifdef CONFIG_MODULE_EVENTLOG_ENABLE


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifndef CONFIG_EVENTLOG_LOG_SIZE
#define CONFIG_EVENTLOG_LOG_SIZE		(100)
#endif

#define EVENTLOG_BUFFER_USE_CIRCULAR	0


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


typedef enum
{
	EventType_Unknown,				///< Unknown type, do not use!
	EventType_SystemEvent,			///< System event
	EventType_UserEvent,			///< User event
	EventType_Warning,				///< Warning
	EventType_Error,				///< Error
	EventType_Finished,				///< Finished
	EventType_Required				///< Required
} EventType_t;


typedef uint32_t	EventStatus_t;

typedef uint8_t		EventId_t;

typedef uint8_t		LogCnt_t;

#if CONFIG_EVENTLOG_LOG_SIZE > 255
#error "LogCnt_t type is wrong!"
#endif


typedef struct
{
	EventName_t eventName;					///< Event ID (enum)
	const char *eventStringName;			///< Event name
} EventTable_t;


typedef struct
{
	EventName_t eventName;					///< Event name (enum)
	EventType_t eventType;					///< Event type
	EventStatus_t eventStatus;				///< Event status / informations
	uint32_t tick;							///< Event tick
#ifdef CONFIG_MODULE_RTC_ENABLE
	DateTime_t dateTime;					///< Event date time
#endif
} EventLogRecord_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void EventLog_Init(void);
void EventLog_LogEvent(EventName_t eventName, EventType_t eventType, EventStatus_t eventStatus);
void EventLog_PrintAllLogRecords(void);

void EventLog_UnitTest(void);


#endif // #ifdef CONFIG_MODULE_EVENTLOG_ENABLE


#endif /* COMMON_EVENTLOG_H_ */
