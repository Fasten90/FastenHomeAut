/*
 *		TaskList.h
 *		Created on:		2017-02-02
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Task list
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-02
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

///< Task's enums for Task schedule requiring
typedef enum
{
#ifdef CONFIG_MODULE_LED_ENABLE
	Task_Led,
#endif
#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
	Task_WdtClr,
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	Task_Esp8266,
#endif
#ifdef CONFIG_MODULE_MOTOR_ENABLE
	Task_MotorControl,
	Task_MotorTimeout,
#endif
#ifdef CONFIG_FUNCTION_REMOTECONTROLLER
	Task_RemoteController,
#endif
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
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
#ifdef CONFIG_MODULE_TASK_SOFTWARE_WATCHDOG_ENABLE
	Task_SwWDT,
#endif
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
	Task_Trace,
#endif
#ifdef CONFIG_FUNCTION_CHARGER
	Task_IO,
#endif
#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
	Task_CommonAdc,
#endif
#ifdef CONFIG_DEBUG_SELFTEST
	Task_SelfTest,
#endif

	// XXX: Add new Task enum here!
	// \note: !! Be careful !! Sync with TaskList

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
