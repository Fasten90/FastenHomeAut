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



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE

extern EventHandler_t EventList[];
extern const EventId_t EventListNum;

#endif	// #ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/



#endif /* EVENTLIST_H_ */
