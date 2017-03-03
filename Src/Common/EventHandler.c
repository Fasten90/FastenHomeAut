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



#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE

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
static void EventHandler_ClearEventFlag(EventId_t eventId);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



#endif	// #ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE
