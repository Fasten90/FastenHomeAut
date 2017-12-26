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
#include "ColoredMessage.h"
#include "StringHelper.h"
#include "DebugUart.h"
#include "DebugList.h"
#include "Debug.h"
#include "Table.h"



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



// TODO: Add Debug_FixString ~ (without format?)



/**
 * \brief	Debug print
 * 			The parameterized debug task is has color and task name, which printed out
 */
void Debug_Print(Debug_t debugTask, const char *format, ...)
{
	// Check DebugTasks list size

	if (debugTask >= Debug_Count || format == NULL)
		return;

	if (DebugTasks[debugTask].isEnabled)
	{
		// Taskname:
		SendTextColor(DebugTasks[debugTask].color);
		if (DebugTasks[debugTask].background)
		{
			SendBackgroundColor(DebugTasks[debugTask].background);
		}
		uprintf("%s: ", DebugTasks[debugTask].name);

		// Send debug message:
		// Working in at:
		char txBuffer[DEBUGUART_TX_BUFFER_SIZE];

#ifdef CONFIG_DEBUG_MODE
		txBuffer[DEBUGUART_TX_BUFFER_SIZE-1] = 0xEF;
#endif

		va_list ap;									// argument pointer
		va_start(ap, format); 						// ap on arg
		string_printf(txBuffer, format, ap);		// Separate and process
		va_end(ap);						 			// Cleaning after end

#ifdef CONFIG_DEBUG_MODE
		if (txBuffer[DEBUGUART_TX_BUFFER_SIZE-1] != 0xEF)
		{
			DEBUG_BREAKPOINT();
		}
#endif

		DebugUart_SendLine(txBuffer);				// Send on Usart

		// Set default color
		SendTextColor(Color_Black);
		if (DebugTasks[debugTask].background)
		{
			SendBackgroundColor(Color_White);
		}
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

	for (i = 0; i < Debug_Count; i++)
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



/**
 * \brief	Print DebugList Table Header
 */
static void Debug_PrintDebugListTableHeader(const char * fixheader, char * str, char * header)
{
	Table_PrintTableWithBorder(fixheader, str, header, "DebugName", "en");
}



/**
 * \brief	Print Debug list
 */
void Debug_PrintDebugList(void)
{
	uint8_t i;
	static const char const fixheader[] = "| %20s | %3s |";
	char str[2 + 20 + 3 + 3 + 2];
	char header[sizeof(fixheader)];

	Debug_PrintDebugListTableHeader(fixheader, str, header);

	for (i = 0; i < Debug_Count; i++)
	{
		usprintf(str, fixheader,
				DebugTasks[i].name,
				((DebugTasks[i].isEnabled) ? ("x") : (" ")));

		Table_SendLine(str);
	}

	Debug_PrintDebugListTableHeader(fixheader, str, header);
}



#else

// Debug module is not used
// Suppressed "ISO C forbids an empty translation unit [-Wpedantic]"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic pop



#endif	// #ifdef CONFIG_MODULE_DEBUG_ENABLE
