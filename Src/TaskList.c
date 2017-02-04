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


static TaskResult_t Task1Function(TaskID_t id);
static TaskResult_t Task2Function(TaskID_t id);
static TaskResult_t Task3Function(TaskID_t id);



/// Tasks list
Task_t TaskList[] =
{
	{
		.taskName = "Task1",
		.taskFunction = Task1Function,
		.taskScheduleRate = 200
	},
	{
		.taskName = "Task2",
		.taskFunction = Task2Function,
		.taskScheduleRate = 1000
	},
	{
		.taskName = "Task3",
		.taskFunction = Task3Function,
		.taskScheduleRate = 5000
	}

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
static TaskResult_t Task1Function(TaskID_t id)
{
	uprintf("Run %s %d\r\n", TaskList[id].taskName, id);
	LED_SetLed(1, LED_SET_TOGGLE);

	return TASK_RESULT_OK;
}



/**
 * \brief
 */
static TaskResult_t Task2Function(TaskID_t id)
{
	uprintf("Run %s %d\r\n", TaskList[id].taskName, id);
	LED_SetLed(2, LED_SET_TOGGLE);

	return TASK_RESULT_OK;
}



/**
 * \brief
 */
static TaskResult_t Task3Function(TaskID_t id)
{
	uprintf("Run %s %d\r\n", TaskList[id].taskName, id);
	LED_SetLed(3, LED_SET_TOGGLE);

	return TASK_RESULT_OK;
}


#endif //#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
