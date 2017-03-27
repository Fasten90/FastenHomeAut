/*
 *		TaskList.c
 *
 *		Created on:		2017. febr. 2.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 2.
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "TaskList.h"

#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


#ifdef CONFIG_MODULE_LED_ENABLE
static TaskResult_t TaskLed1Function(ScheduleSource_t source);
static TaskResult_t TaskLed2Function(ScheduleSource_t source);
static TaskResult_t TaskLed3Function(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
static TaskResult_t TaskWatchdogClear(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
static TaskResult_t TaskEsp8266(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_MOTOR_ENABLE
static TaskResult_t TaskMotor(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
static TaskResult_t Task_ProcessDebugUartCommandReceived(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
static TaskResult_t Task_ProcessButtonPressed(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
static TaskResult_t Task_DisplayChangeImage(ScheduleSource_t source);
#endif


/// Tasks list
Task_t TaskList[] =
{
#ifdef CONFIG_MODULE_LED_ENABLE
	{
		.taskName = "Led200ms",
		.taskFunction = TaskLed1Function,
		.taskScheduleRate = 200
	},
	{
		.taskName = "Led1sec",
		.taskFunction = TaskLed2Function,
		.taskScheduleRate = 1000
	},
	{
		.taskName = "Led5sec",
		.taskFunction = TaskLed3Function,
		.taskScheduleRate = 5000
	},
#endif
#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
	{
		.taskName = "WdtClr",
		.taskFunction = TaskWatchdogClear,
		.taskScheduleRate = 1000,
	},
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	{
		.taskName = "Esp8266",
		.taskFunction = TaskEsp8266,
		.taskScheduleRate = 1000,
	},
#endif
#ifdef CONFIG_MODULE_MOTOR_ENABLE
	{
		.taskName = "MotorTask",
		.taskFunction = TaskMotor,
		.taskScheduleRate = 100
	},
#endif
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	{
		.taskName = "DebugUartReceivedACommand",
		.taskFunction = Task_ProcessDebugUartCommandReceived,
		//.isPeriodisScheduleDisabled = true,
		.taskScheduleRate = 50
	},
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
	{
		.taskName = "ButtonPressed",
		.taskFunction = Task_ProcessButtonPressed,
		.isPeriodisScheduleDisabled = true,
	},
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
	{
		.taskName = "DisplayRefresh",
		.taskFunction = Task_DisplayChangeImage,
		.taskScheduleRate = 300
	},
#endif
	// \note Be careful, taskList order need to be equal with TaskName_t

	// XXX: Add here new tasks

};



/// Tasks num
const TaskID_t TasksNum = (sizeof(TaskList)/sizeof(TaskList[0]));



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


// XXX: Function form:
//TaskResult_t (*TaskFunctionPointer)(TaskID_t id);



#ifdef CONFIG_MODULE_LED_ENABLE
/**
 * \brief
 */
static TaskResult_t TaskLed1Function(ScheduleSource_t source)
{
#ifdef CONFIG_TASKHANDLER_DEBUG_ENABLE
	uprintf("Run %s %d\r\n", TaskList[id].taskName, id);
#else
	(void)source;
#endif

	LED_SetLed(1, LED_SET_TOGGLE);

	return TASK_RESULT_OK;
}



/**
 * \brief
 */
static TaskResult_t TaskLed2Function(ScheduleSource_t source)
{
#ifdef CONFIG_TASKHANDLER_DEBUG_ENABLE
	uprintf("Run %s %d\r\n", TaskList[id].taskName, id);
#else
	(void)source;
#endif

	LED_SetLed(2, LED_SET_TOGGLE);

	return TASK_RESULT_OK;
}



/**
 * \brief
 */
static TaskResult_t TaskLed3Function(ScheduleSource_t source)
{
#ifdef CONFIG_TASKHANDLER_DEBUG_ENABLE
	uprintf("Run %s %d\r\n", TaskList[id].taskName, id);
#else
	(void)source;
#endif

	LED_SetLed(3, LED_SET_TOGGLE);

	return TASK_RESULT_OK;
}
#endif	// #ifdef CONFIG_MODULE_LED_ENABLE



#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
/**
 * \brief	Watchdog clear task
 */
static TaskResult_t TaskWatchdogClear(ScheduleSource_t source)
{
	(void)source;

	Watchdog_Clear();

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_ESP8266_ENABLE
static TaskResult_t TaskEsp8266(ScheduleSource_t source)
{
	(void)source;

	ESP8266_StatusMachine();

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_MOTOR_ENABLE
static TaskResult_t TaskMotor(ScheduleSource_t source)
{
	(void)source;

	Motor_StatusMachine();

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
static TaskResult_t Task_ProcessDebugUartCommandReceived(ScheduleSource_t source)
{
	(void)source;

	CommandHandler_CheckCommand();

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_BUTTON_ENABLE
static TaskResult_t Task_ProcessButtonPressed(ScheduleSource_t source)
{
	(void)source;

	// Toggle LED
#ifdef CONFIG_MODULE_LED_ENABLE
	LED_SetLed(LED_GREEN_NUM, LED_SET_TOGGLE);
#endif

	// Clear flag
	BUTTON_Clicked = 0;


	#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
	// Test for Raspberry Pi
	RASPBERRYPI_SendMessage(1, Function_Alarm, Alarm_PressedButton , 17);
	#endif

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_DISPLAY_ENABLE
static TaskResult_t Task_DisplayChangeImage(ScheduleSource_t source)
{
	(void)source;

	if (!CarAnimationDisable_flag)
	{
		Display_ChangeCarImage();
	}

	return TASK_RESULT_OK;

}
#endif



#endif //#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
