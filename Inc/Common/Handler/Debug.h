/*
 *		Debug.h
 *		Created on:		2017-08-17
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Debug prints (with colorized messages and enabling)
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-08-17
 */

#ifndef COMMON_DEBUG_H_
#define COMMON_DEBUG_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "ColoredMessage.h"
#include "DebugList.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

///< Enable background
//#define CONFIG_DEBUG_BACKGROUND_ENABLE



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef struct
{
	const char * name;					///< Name (string)
	bool isEnabled;						///< Need to debugprint?
	MsgColors_t color;					///< Text color
#ifdef CONFIG_DEBUG_BACKGROUND_ENABLE
	MsgColors_t background;				///< Background color
#endif
} DebugRecord_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_DEBUG_ENABLE
void Debug_Print(Debug_t debugTask, const char *format, ...);
bool Debug_SetEnable(Debug_t task, bool enable);
bool Debug_SetDebugTaskWithName(char *name, bool enable);
void Debug_PrintDebugList(void);
#else
// Empty macro for suppress warnings
#define Debug_Print(...)
#endif


#endif /* COMMON_DEBUG_H_ */
