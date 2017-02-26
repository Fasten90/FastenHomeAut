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


static TaskResult_t TaskLed1Function(TaskID_t id);
static TaskResult_t TaskLed2Function(TaskID_t id);
static TaskResult_t TaskLed3Function(TaskID_t id);
#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
static TaskResult_t TaskWatchdogClear(TaskID_t id);
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
static TaskResult_t TaskEsp8266(TaskID_t id);
#endif
#ifdef CONFIG_MODULE_MOTOR_ENABLE
static TaskResult_t TaskMotor(TaskID_t id);
#endif

/// Tasks list
Task_t TaskList[] =
{
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
	}
#endif
#ifdef CONFIG_MODULE_MOTOR_ENABLE
	{
		.taskName = "MotorTask",
		.taskFunction = TaskMotor,
		.taskScheduleRate = 100
	}
#endif
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


/**
 * \brief
 */
static TaskResult_t TaskLed1Function(TaskID_t id)
{
#ifdef CONFIG_TASKHANDLER_DEBUG_ENABLE
	uprintf("Run %s %d\r\n", TaskList[id].taskName, id);
#else
	(void)id;
#endif

	LED_SetLed(1, LED_SET_TOGGLE);

	return TASK_RESULT_OK;
}



/**
 * \brief
 */
static TaskResult_t TaskLed2Function(TaskID_t id)
{
#ifdef CONFIG_TASKHANDLER_DEBUG_ENABLE
	uprintf("Run %s %d\r\n", TaskList[id].taskName, id);
#else
	(void)id;
#endif

	LED_SetLed(2, LED_SET_TOGGLE);

	return TASK_RESULT_OK;
}



/**
 * \brief
 */
static TaskResult_t TaskLed3Function(TaskID_t id)
{
#ifdef CONFIG_TASKHANDLER_DEBUG_ENABLE
	uprintf("Run %s %d\r\n", TaskList[id].taskName, id);
#else
	(void)id;
#endif

	LED_SetLed(3, LED_SET_TOGGLE);

	return TASK_RESULT_OK;
}



#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
/**
 * \brief	Watchdog clear task
 */
static TaskResult_t TaskWatchdogClear(TaskID_t id)
{
	(void)id;

	Watchdog_Clear();

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_ESP8266_ENABLE
static TaskResult_t TaskEsp8266(TaskID_t id)
{
	(void)id;

	ESP8266_StatusMachine();

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_MOTOR_ENABLE
static TaskResult_t TaskMotor(TaskID_t id)
{
	(void)id;

	Motor_StatusMachine();

	return TASK_RESULT_OK;
}
#endif


#endif //#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
