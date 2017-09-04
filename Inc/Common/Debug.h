/*
 *		Debug.h
 *
 *		Created on:		2017. aug. 17.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. aug. 17.
 */

#ifndef COMMON_DEBUG_H_
#define COMMON_DEBUG_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "FormattedMessage.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	Debug_ESP8266,
#endif
#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE
	Debug_EventHandler,
#endif

	/*
	 * XXX: Add here new Debug task enums
	 * \note	Do not forget syncronize with DebugTasks
	 */

	// Do not use, it used for enum count
	Debug_Count
} Debug_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/





/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_DEBUG_ENABLE
void Debug_Print(Debug_t debugTask, const char *format, ...);
bool Debug_EnableDisable(Debug_t task, bool enable);
bool Debug_SetDebugTaskWithName(char *name, bool enable);
#else
// Empty macro for suppress warnings
#define Debug_Print(...)
#endif


#endif /* COMMON_DEBUG_H_ */
