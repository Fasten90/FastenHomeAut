/*
 *		DebugList.h
 *		Created on:		2017-09-08
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Debug "task" list
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-09-08
 */

#ifndef DEBUGLIST_H_
#define DEBUGLIST_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< Debug
typedef enum
{
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	Debug_ESP8266,
#endif
#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE
	Debug_EventHandler,
#endif
#ifdef CONFIG_FUNCTION_GAME_SNAKE
	Debug_GameSnake,
#endif

	/*
	 * XXX: Add here new Debug task enums
	 * \note	Do not forget synchronize with DebugTasks
	 */

	// Do not use, it used for enum count
	Debug_Count
} Debug_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern uint8_t DebugTaskListNum;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/



#endif /* DEBUGLIST_H_ */
