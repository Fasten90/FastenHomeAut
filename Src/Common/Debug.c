/*
 *		Debug.c
 *		Created on:		2017-08-17
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Debug print
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-08-17
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_MODULE_DEBUG_ENABLE

#include <stdarg.h>// for "va_list"
#include "FormattedMessage.h"
#include "String.h"
#include "DebugUart.h"
#include "DebugList.h"

#include "Debug.h"


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern DebugRecord_t DebugTasks[];



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Debug print
 * 			The parameterized debug task is has color and task name, which printed out
 */
void Debug_Print(Debug_t debugTask, const char *format, ...)
{
	// Check DebugTasks list size
	BUILD_BUG_ON(DebugTaskListNum == (Debug_Count-1));

	if (debugTask >= Debug_Count)
		return;

	if (DebugTasks[debugTask].isEnabled)
	{
		// Taskname:
		SendTextColor(DebugTasks[debugTask].color);
		if (DebugTasks[debugTask].background) SendBackgroundColor(DebugTasks[debugTask].background);

		uprintf("%s: ", DebugTasks[debugTask].name);

		// Send debug message:
		// Working in at:
		char txBuffer[DEBUGUART_TXBUFFERSIZE];

		#ifdef CONFIG_DEBUG_MODE
		txBuffer[DEBUGUART_TXBUFFERSIZE-1] = 0xEF;
		#endif

		va_list ap;									// argument pointer
		va_start(ap, format); 						// ap on arg
		string_printf(txBuffer, format, ap);		// Separate and process
		va_end(ap);						 			// Cleaning after end

		#ifdef CONFIG_DEBUG_MODE
		if (txBuffer[DEBUGUART_TXBUFFERSIZE-1] != 0xEF) DEBUG_BREAKPOINT();
		#endif

		DebugUart_SendLine(txBuffer);				// Send on Usart

		// Set default color
		SendTextColor(Color_Black);
		if (DebugTasks[debugTask].background) SendBackgroundColor(Color_White);
	}
}



/**
 * \brief	Enable-Disable debug print
 */
bool Debug_EnableDisable(Debug_t task, bool enable)
{
	if (task >= Debug_Count)
		return false;

	DebugTasks[task].isEnabled = enable;
	return true;
}



/**
 * \brief	Enable-Disable debug task with name
 */
bool Debug_SetDebugTaskWithName(char *name, bool enable)
{
	uint8_t i;
	bool result = false;

	for (i = 0; i < DebugTaskListNum; i++)
	{
		if (!StrCmp(name, DebugTasks[i].name))
		{
			// Found
			DebugTasks[i].isEnabled = enable;
			result = true;
			break;
		}
	}

	return result;
}



#endif	// #ifdef CONFIG_MODULE_DEBUG_ENABLE
