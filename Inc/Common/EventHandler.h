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
#include "include.h"
#include "EventNames.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define EVENT_RUN			(true)
#define	EVENT_NORUN			(false)


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


typedef enum
{
	EventType_Unknown,				///> Unknown type, do not use!
	EventType_Event,				///> Event
	EventType_Warning,				///> Warning
	Eventtype_Error					///> Error
} EventType_t;


typedef enum
{
	EventFunctionResult_Unknown,
	EventFunctionResult_Ok,
	EventFunctionResult_Failed
} EventFunctionResult;


typedef uint32_t	EventStatus_t;


typedef bool		EventFlag_t;


typedef uint8_t		EventId_t;


/// Command Function pointer
typedef EventFunctionResult ( *EventFunctionPointer )(EventType_t eventType, EventStatus_t eventStatus);


typedef struct
{
	EventName_t eventName;					///> Event ID (enum)
	const char *eventStringName;			///> Event name
	EventType_t eventType;					///> Event type: event, warning, error
	EventFunctionPointer eventFunction;		///> Function which called
	EventFlag_t flag;						///> Flag for run
	EventStatus_t eventStatus;				///> Event status
	bool isDisabled;						///> Event disabling
} EventHandler_t;


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void EventHandler_CheckEvents(void);
void EventHandler_SetEventFlag(EventName_t eventName, EventFlag_t flag);



#endif /* COMMON_EVENTHANDLER_H_ */
