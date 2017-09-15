/*
 *		DebugList.c
 *		Created on:		2017-09-08
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Debug "task" list
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-09-08
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "include.h"
#include "FormattedMessage.h"
#include "Debug.h"
#include "DebugList.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

///< Debug(task) list
DebugRecord_t DebugTasks[] =
{
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	{
		.name = "ESP8266",
		.isEnabled = true,
		.color = Color_Blue,
	},
#endif
#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE
	{
		.name = "Event",
		.isEnabled = true,
		.color = Color_Blue,
	},
#endif
#ifdef CONFIG_FUNCTION_GAME_SNAKE
	{
		.name = "Snake",
		.isEnabled = true,
		.color = Color_Green
	},
#endif
	{
		.name = "New",
		.isEnabled = true,
		.color = Color_Red
	}

	/*
	 * XXX: Add here new Debug task struct
	 * \note	Do not forget syncronize with Debug_t enum
	 */
};


uint8_t DebugTaskListNum = NUM_OF(DebugTasks);



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
 * \brief
 */
void DebugList_Init(void)
{

}


