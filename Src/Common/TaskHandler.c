/*
 *		TaskHandler.c
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
#include "TaskHandler.h"


#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void TaskHandler_IncrementTicks(TaskTick_t elapsedTick);
static void TaskHandler_RunTask(TaskID_t taskID, ScheduleSource_t source);


/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief Initialize TaskHandler / Tasks
 */
void TaskHandler_Init(void)
{
	TaskID_t i;

	for (i = 0; i < TasksNum; i++)
	{
		TaskList[i].tick = 0;
	}
}



/**
 *	\brief	Schedule tasks (and increment elapsed time)
 *	\note	Recommend call from main loop for scheduling
 */
void TaskHandler_Scheduler(TaskTick_t elapsedTick)
{
	TaskID_t i;

	// Time optimizing...
	if (elapsedTick == 0)
	{
		return;
	}

	TaskHandler_IncrementTicks(elapsedTick);

	// Check list
	for (i = 0; i < TasksNum; i++)
	{
		// Need scheduling?
		if (!TaskList[i].isPeriodisScheduleDisabled
			&& TaskList[i].tick >= TaskList[i].taskScheduleRate
			&& !TaskList[i].isDisabled)
		{
			// Schedule - periodical
			TaskHandler_RunTask(i, ScheduleSource_PeriodicSchedule);
		}
		else if (TaskList[i].isRequestScheduling
			&& !TaskList[i].isDisabled)
		{
			// Schedule - event triggered
			TaskHandler_RunTask(i, ScheduleSource_EventTriggered);
		}
	}
}



/**
 *	\brief	Increment ticks
 */
static void TaskHandler_IncrementTicks(TaskTick_t elapsedTick)
{
	TaskID_t i;

	for (i = 0; i < TasksNum; i++)
	{
		TaskList[i].tick += elapsedTick;
	}
}



/**
 *	\brief Run task
 */
static void TaskHandler_RunTask(TaskID_t taskID, ScheduleSource_t source)
{
	bool result = TaskList[taskID].taskFunction(source);

#ifdef CONFIG_TASKHANDLER_DEBUG_ENABLE
	uprintf("- %s task run %s\r\n", TaskList[taskID].taskName, (result) ? ("successful") : ("failed"));
#else
	(void)result;
#endif

	// Clear tick
	TaskList[taskID].isRequestScheduling = false;
	TaskList[taskID].tick = 0;
}



/**
 *	\brief	Initialize task
 */
void TaskHandler_SetTaskTime(TaskID_t taskID, TaskTick_t taskTick)
{
	 TaskList[taskID].isDisabled = false;
	 TaskList[taskID].taskScheduleRate = taskTick;
	 TaskList[taskID].tick = 0;
}



/**
 * \brief	Disable task
 */
void TaskHandler_DisableTask(TaskName_t taskId)
{
	if (taskId < Task_Count)
	{
		TaskList[taskId].isDisabled = true;
	}
}



/**
 * \brief	Request task scheduling
 */
void TaskHandler_RequestTaskScheduling(TaskID_t taskId)
{
	if (taskId < Task_Count)
	{
		TaskList[taskId].isRequestScheduling = true;
	}
}



#endif	// #ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
