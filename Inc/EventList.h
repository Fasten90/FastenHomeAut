/*
 *		EventList.h
 *
 *		Created on:		2017. febr. 5.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 5.
 */

#ifndef EVENTLIST_H_
#define EVENTLIST_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "EventHandler.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


typedef enum
{
	Event_Unknown,
	Event_TestExample,
	Event_DebugUartReceivedCommand,

#ifdef CONFIG_MODULE_BUTTON_ENABLE
	Event_ButtonPressed,
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	Event_Esp8266ReceivedMessage,
#endif


	// XXX: Add here new event

} EventName_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/



#endif /* EVENTLIST_H_ */
