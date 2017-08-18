/*
 *		Debug.c
 *
 *		Created on:		2017. aug. 17.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. aug. 17.
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_MODULE_DEBUG
#include "FormattedMessage.h"
#include "String.h"
#include "DebugUart.h"
#include <stdarg.h>// for "va_list"

#include "Debug.h"



typedef struct
{
	const char * name;
	bool isEnabled;
	FormattedStringColors_t color;
	// TODO: Idea: add background color
} DebugRecord_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

DebugRecord_t DebugTasks[] =
{
	{
		.name = "ESP8266",
		.isEnabled = true,
		.color = Color_Blue,
	},
	{
		.name = "New",
		.isEnabled = true,
		.color = Color_Red
	}
};



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
	BUILD_BUG_ON((sizeof(DebugTasks)/sizeof(DebugTasks[0])) == (Debug_Count-1));

	if (debugTask >= Debug_Count)
		return;

	if (DebugTasks[debugTask].isEnabled)
	{
		// Taskname:
		SendTextColor(DebugTasks[debugTask].color);
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
	}
}



/**
 * \brief	Enable-Disable debug print
 */
void Debug_EnableDisable(Debug_t task, bool enable)
{
	if (task >= Debug_Count)
		return;

	DebugTasks[task].isEnabled = enable;
}



#endif	// #ifdef CONFIG_MODULE_DEBUG
