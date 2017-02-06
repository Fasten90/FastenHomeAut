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
static void TaskHandler_RunTask(TaskID_t taskID);


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
 */
void TaskHandler_CheckSchedules(TaskTick_t elapsedTick)
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
		if (TaskList[i].tick >= TaskList[i].taskScheduleRate
			&& !TaskList[i].isDisabled)
		{
			// Schedule...
			TaskHandler_RunTask(i);
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
static void TaskHandler_RunTask(TaskID_t taskID)
{
	bool result = TaskList[taskID].taskFunction(taskID);

#ifdef CONFIG_TASKHANDLER_DEBUG_ENABLE
	uprintf("- %s task run %s\r\n", TaskList[taskID].taskName, (result) ? ("successful") : ("failed"));
#else
	(void)result;
#endif

	// Clear tick
	TaskList[taskID].tick = 0;
}


#endif	// #ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
