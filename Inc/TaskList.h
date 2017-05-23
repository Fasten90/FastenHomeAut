/*
 *		TaskList.h
 *
 *		Created on:		2017. febr. 2.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 2.
 */

#ifndef TASKLIST_H_
#define TASKLIST_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "TaskHandler.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
	Task_Led200ms,
	Task_Led1sec,
	Task_Led5sec,
#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
	Task_WdtClr,
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	Task_Esp8266,
#endif
#ifdef CONFIG_MODULE_MOTOR_ENABLE
	Task_MotorControl,
#endif
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	Task_ProcessDebugUartReceivedCommand,
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
	Task_ButtonPressed,
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
	Task_Display,
#endif
#ifdef CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE
	Task_SystemTime,
#endif

	// XXX:

	Task_Count
} TaskName_t;


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
extern Task_t TaskList[];
extern const TaskID_t TasksNum;
#endif

/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/



#endif /* TASKLIST_H_ */
