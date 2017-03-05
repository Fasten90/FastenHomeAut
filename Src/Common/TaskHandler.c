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

#ifdef MODULE_TASKHANDLER_UNNITEST_ENABLE
#include "UnitTest.h"
#endif



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
#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
	EventLog_LogEvent(Event_TaskScheduled, EventType_SystemEvent, (taskID << 24 | source << 16));
#endif

	// Clear tick
	TaskList[taskID].isRequestScheduling = false;
	TaskList[taskID].tick = 0;

	// Clear once run
	if (TaskList[taskID].isRunOnce)
	{
		TaskList[taskID].isDisabled = true;
	}
}



/**
 *	\brief	Initialize task
 */
void TaskHandler_SetTaskPeriodicTime(TaskID_t taskID, TaskTick_t taskTick)
{
	 TaskList[taskID].isDisabled = false;
	 TaskList[taskID].taskScheduleRate = taskTick;
	 TaskList[taskID].tick = 0;
	 TaskList[taskID].isRunOnce = false;
}



/**
 *	\brief	Set Task scheduling 1x after x time
 */
void TaskHandler_SetTaskOnceRun(TaskID_t taskID, TaskTick_t taskTick)
{
	 TaskList[taskID].isDisabled = false;
	 TaskList[taskID].taskScheduleRate = taskTick;
	 TaskList[taskID].tick = 0;
	 TaskList[taskID].isRunOnce = true;
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
#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
		EventLog_LogEvent(Event_TaskScheduled, EventType_SystemEvent, taskId);
#endif
	}
}



#ifdef MODULE_TASKHANDLER_UNNITEST_ENABLE
void TaskHandler_UnitTest(void)
{

	uint8_t i;
	bool result;


	// Start Unit test
	UnitTest_Start("TaskHandler", __FILE__);



	// Test Initialization

	// Clear requests: Be careful, it make some wrongs
	for (i = 0; i < TasksNum; i++)
	{
		TaskList[i].isRequestScheduling = false;
	}
	TaskHandler_Init();

	// Check, all tick are 0?
	for (i = 0; i < TasksNum; i++)
	{
		result = (TaskList[i].tick == 0);
		UnitTest_CheckResult(result, "TaskHandler_Init error", __LINE__);
	}



	// Test Scheduler time elapsing

	TaskHandler_Scheduler(1);

	// Check all tick is incremented?
	for (i = 0; i < TasksNum; i++)
	{
		result = (TaskList[i].tick == 1);
		UnitTest_CheckResult(result, "TaskHandler_Scheduler error", __LINE__);
	}



	// TODO: Test TaskHandler_RunTask ?



	// Test TaskHandler_SetTaskPeriodicTime

	// Save values
	uint32_t oldTaskScheduleRate;
	bool oldOnceRun = TaskList[0].isRunOnce;
	oldTaskScheduleRate = TaskList[0].taskScheduleRate;

	TaskHandler_SetTaskPeriodicTime(0, 1234);
	result = (TaskList[0].taskScheduleRate == 1234);
	UnitTest_CheckResult(result, "TaskHandler_SetTaskTime Error", __LINE__);
	result = (TaskList[0].isRunOnce == false);
	UnitTest_CheckResult(result, "TaskHandler_SetTaskTime Error", __LINE__);

	TaskHandler_SetTaskPeriodicTime(0, oldTaskScheduleRate);
	result = (TaskList[0].taskScheduleRate == oldTaskScheduleRate);
	UnitTest_CheckResult(result, "TaskHandler_SetTaskTime Error", __LINE__);
	result = (TaskList[0].isRunOnce == false);
	UnitTest_CheckResult(result, "TaskHandler_SetTaskTime Error", __LINE__);

	// Restore
	TaskList[0].isRunOnce = oldOnceRun;



	// Test TaskHandler_SetTaskOnceRun

	// Save values
	oldOnceRun = TaskList[0].isRunOnce;
	oldTaskScheduleRate =TaskList[0].taskScheduleRate;

	TaskHandler_SetTaskOnceRun(0, 2345);
	result = (TaskList[0].taskScheduleRate == 2345);
	UnitTest_CheckResult(result, "TaskHandler_ScheduleTaskOnce Error", __LINE__);
	result = (TaskList[0].isRunOnce == true);
	UnitTest_CheckResult(result, "TaskHandler_ScheduleTaskOnce Error", __LINE__);

	TaskHandler_SetTaskOnceRun(0, oldTaskScheduleRate);
	result = (TaskList[0].taskScheduleRate == oldTaskScheduleRate);
	UnitTest_CheckResult(result, "TaskHandler_ScheduleTaskOnce Error", __LINE__);
	result = (TaskList[0].isRunOnce == true);
	UnitTest_CheckResult(result, "TaskHandler_ScheduleTaskOnce Error", __LINE__);

	// Restore
	TaskList[0].isRunOnce = oldOnceRun;



	// Test task disabling

	bool oldDisableValue = TaskList[0].isDisabled;

	TaskHandler_DisableTask(0);
	result = (TaskList[0].isDisabled == true);
	UnitTest_CheckResult(result, "TaskHandler_DisableTask Error", __LINE__);

	// Restore task disable value
	TaskList[0].isDisabled = oldDisableValue;



	// Test task request
	bool oldRequest = TaskList[0].isRequestScheduling;

	TaskHandler_RequestTaskScheduling(0);
	result = (TaskList[0].isRequestScheduling == true);
	UnitTest_CheckResult(result, "TaskHandler_RequestTaskScheduling Error", __LINE__);

	// Restore task request value
	TaskList[0].isRequestScheduling = oldRequest;



	// Finish
	UnitTest_End();

}
#endif



#endif	// #ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
