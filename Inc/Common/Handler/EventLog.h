/*
 *		EventLog.h
 *		Created on:		2017-02-05
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Event logger
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-05
 */

#ifndef COMMON_EVENTLOG_H_
#define COMMON_EVENTLOG_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "EventList.h"
#include "EventHandler.h"
#include "DateTime.h"


#ifdef CONFIG_MODULE_EVENTLOG_ENABLE


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifndef CONFIG_EVENTLOG_LOG_SIZE
	#define CONFIG_EVENTLOG_LOG_SIZE		(100)
#endif

///< Save log circular mode (1), or once full mode
#ifndef EVENTLOG_BUFFER_USE_CIRCULAR
	#define EVENTLOG_BUFFER_USE_CIRCULAR	(1)
#endif

///< Save DateTime (1), or not (0) (If turned off, it use less RAM)
#ifndef EVENTLOG_SAVE_DATETIME
	#define EVENTLOG_SAVE_DATETIME			(0)
#endif

///< Print event immediately
// #define CONFIG_EVETNLOG_PRINT_IMMEDIATELY



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


typedef uint8_t		EventLogCnt_t;

#if CONFIG_EVENTLOG_LOG_SIZE > 255
	#error "LogCnt_t type is wrong!"
#endif


///< EventLog Record structure for log
typedef struct
{
	EventName_t eventName;					///< Event name (enum)
	EventData_t eventData;					///< Event data / informations
	EventType_t eventType;					///< Event type
	TaskID_t taskSource;					///< Event source (Task)
	uint32_t tick;							///< Event tick
#if (EVENTLOG_SAVE_DATETIME == 1) && (defined(CONFIG_MODULE_RTC_ENABLE) || defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE))
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
void EventLog_LogEvent(EventName_t eventName, EventData_t eventData, TaskID_t taskSource, EventType_t eventType);
void EventLog_PrintLogTable(void);

void EventLog_UnitTest(void);



#endif // #ifdef CONFIG_MODULE_EVENTLOG_ENABLE


#endif /* COMMON_EVENTLOG_H_ */
