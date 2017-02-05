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
#include "EventHandler.h"
#include "EventList.h"

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static EventFunctionResult EventHandler_RunEvent(EventId_t eventId);


/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Check there are running need event?
 */
void EventHandler_CheckEvents(void)
{
	EventId_t i;

	for (i =0; i < EventListNum; i++)
	{
		if (EventList[i].flag == EVENT_RUN && EventList[i].isDisabled == false)
		{
			// Has run flag & enabled event
			EventHandler_RunEvent(i);
		}
	}
}


/**
 * \brief	Run event function
 */
static EventFunctionResult EventHandler_RunEvent(EventId_t eventId)
{
	// Run
	EventFunctionResult result;
	result = EventList[eventId].eventFunction(
			 EventList[eventId].eventType,
			 EventList[eventId].eventStatus);

	return result;
}



/**
 * \brief	Set event flag
 */
void EventHandler_SetEventFlag(EventName_t eventName, EventFlag_t flag)
{
	EventId_t i;

	// Find event
	for (i = 0; i < EventListNum; i++)
	{
		if (eventName == EventList[i].eventName)
		{
			// Set event flag
			EventList[i].flag = flag;
			break;
		}
	}
}

