/*
 *		TaskHandler.c
 *		Created on:		2017-02-02
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Task handler
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-02
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "include.h"
#include "TaskList.h"
#include "TaskHandler.h"
#include "CommandHandler.h"
#include "DebugUart.h"
#include "String.h"
#include "EventHandler.h"

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

#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
#define TASKHANDLER_STATISTICS_LIMIT		(100)

typedef struct
{
	uint32_t startTick;
	uint32_t runTime;
	TaskID_t taskId;
} TaskHandlerStat_t;

static TaskHandlerStat_t TaskHandler_StatisticsRanTaskTicks[TASKHANDLER_STATISTICS_LIMIT] = { 0 };
static uint8_t TaskHandler_StatisticsIndex = 0;
#endif

#ifdef CONFIG_DEBUG_SW_WATCHDOG_ENABLE
static uint32_t TaskHandler_RunCnt = 0;
static const char * TaskHandler_ActualTask = NULL;
#endif



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

	// Check TaskList size and enums
	BUILD_BUG_ON(TasksNum != Task_Count);

	// Check TaskList contain
	for (i = 0; i < TasksNum; i++)
	{
		// Need task function
		ASSERT(TaskList[i].taskFunction != NULL);

		// Need task name
		ASSERT(TaskList[i].taskName != NULL);

		// Need period
		ASSERT(TaskList[i].taskScheduleRate != 0);

		/*if (isWrong)
			uprintf("TaskHandler list error in %d. element (name: %s)", i, TaskList[i].taskName);*/
	}


	// TODO: Optimize with static and dynamic list
	// Init task ticks
	for (i = 0; i < TasksNum; i++)
	{
		TaskList[i].tick = 0;
		TaskList[i].taskRunCount = 0;
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

#ifdef CONFIG_DEBUG_SW_WATCHDOG_ENABLE
	TaskHandler_RunCnt++;
#endif

	// Check list
	for (i = 0; i < TasksNum; i++)
	{
		// Need scheduling?
		if (!TaskList[i].isDisabled)
		{
			if (!TaskList[i].isPeriodisScheduleDisabled
				&& TaskList[i].tick >= TaskList[i].taskScheduleRate)
			{
				// Schedule - periodical
				TaskHandler_RunTask(i, ScheduleSource_PeriodicSchedule);
			}
			else if (TaskList[i].isRunOnce
				&& TaskList[i].tick >= TaskList[i].taskScheduleRate)
			{
				// Schedule - once
				TaskHandler_RunTask(i, ScheduleSource_RunOnce);
			}
			else if (TaskList[i].isTimeOutTask
				&& TaskList[i].tick >= TaskList[i].taskScheduleRate)
			{
				// TimeOut task
				TaskHandler_RunTask(i, ScheduleSource_TimeOut);
			}
			else if (TaskList[i].isRequestScheduling)
			{
				// Schedule - event triggered
				TaskHandler_RunTask(i, ScheduleSource_EventTriggered);
			}
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
	bool result = TASK_RESULT_FAILED;

	// Clear once run
	if (TaskList[taskID].isRunOnce)
	{
		TaskList[taskID].isDisabled = true;
	}

#ifdef CONFIG_DEBUG_SW_WATCHDOG_ENABLE
	TaskHandler_ActualTask = (const char *)TaskList[taskID].taskName;
#endif

#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
	// Measure run time:
	uint32_t startTime = HAL_GetTick();
#endif

	// Execute task
	TaskList[taskID].taskFunction(source);

#ifdef CONFIG_TASKHANDLER_DEBUG_ENABLE
	uprintf("- %s task run %s\r\n", TaskList[taskID].taskName, (result) ? ("successful") : ("failed"));
#else
	(void)result;
#endif
#ifdef CONFIG_EVENTLOG_TASKHANDLER_LOG_ENABLE
	EventHandler_GenerateEvent(Event_TaskScheduled, taskID, 0);
#endif

	// Clear tick
	TaskList[taskID].isRequestScheduling = false;
	// TODO: Tick clearing or subtraction?
	TaskList[taskID].tick = 0;

#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
	TaskList[taskID].taskRunCount++;

	uint32_t runTime = HAL_GetTick() - startTime;
	TaskHandler_StatisticsRanTaskTicks[TaskHandler_StatisticsIndex].startTick = startTime;
	TaskHandler_StatisticsRanTaskTicks[TaskHandler_StatisticsIndex].runTime = runTime;
	TaskHandler_StatisticsRanTaskTicks[TaskHandler_StatisticsIndex].taskId = taskID;

	TaskHandler_StatisticsIndex++;

	if (TaskHandler_StatisticsIndex >= TASKHANDLER_STATISTICS_LIMIT)
	{
		TaskHandler_StatisticsIndex = 0;
	}
#endif

#ifdef CONFIG_DEBUG_SW_WATCHDOG_ENABLE
	TaskHandler_ActualTask = NULL;
#endif
}



/**
 *	\brief	Initialize task
 */
void TaskHandler_SetTaskPeriodicTime(TaskID_t taskID, TaskTick_t taskTick)
{
	 TaskList[taskID].isRunOnce = false;
	 TaskList[taskID].isDisabled = false;
	 TaskList[taskID].taskScheduleRate = taskTick;
	 TaskList[taskID].tick = 0;
}



/**
 *	\brief	Set Task scheduling 1x after x time
 */
void TaskHandler_SetTaskOnceRun(TaskID_t taskID, TaskTick_t taskTick)
{
	 TaskList[taskID].isRunOnce = true;
	 TaskList[taskID].isDisabled = false;
	 TaskList[taskID].taskScheduleRate = taskTick;
	 TaskList[taskID].tick = 0;
}



/**
 * \brief	Disable task
 */
void TaskHandler_DisableTask(TaskName_t taskID)
{
	if (taskID < Task_Count)
	{
		TaskList[taskID].isDisabled = true;
	}
}



/**
 * \brief	Request task scheduling
 */
void TaskHandler_RequestTaskScheduling(TaskID_t taskID)
{
	if (taskID < Task_Count)
	{
		TaskList[taskID].isRequestScheduling = true;
		TaskList[taskID].isDisabled = false;
#ifdef CONFIG_EVENTLOG_TASKHANDLER_LOG_ENABLE
		EventHandler_GenerateEvent(Event_TaskRequired, taskID, 0);
#endif
	}
}



/**
 * \brief	Clear TimeOut task tick
 */
void TaskHandler_ClearTimeoutTask(TaskID_t taskID)
{
	if (taskID < Task_Count && TaskList[taskID].isTimeOutTask == true)
	{
		// Clear
		TaskList[taskID].tick = 0;
		TaskList[taskID].isDisabled = false;
#ifdef CONFIG_EVENTLOG_TASKHANDLER_LOG_ENABLE
		EventHandler_GenerateEvent(Event_TaskCleared, taskID, 0);
#endif
	}
}



#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
/**
 * \brief	Calculate and print TaskHandler statistics
 */
void TaskHandler_PrintStatistics(void)
{
	uint32_t oldestTick = HAL_GetTick();
	uint8_t runTimes = 0;
	uint8_t i;
	uint32_t lastTick = 0;
	//uint32_t lastRunTime = 0;

	// Calculate Statistics values
	for (i = 0; i < TASKHANDLER_STATISTICS_LIMIT; i++)
	{
		if (TaskHandler_StatisticsRanTaskTicks[i].startTick > 0)
		{
			// This tick is "ran task tick"
			if (TaskHandler_StatisticsRanTaskTicks[i].startTick < oldestTick)
			{
				// This tick is the oldest tick
				oldestTick = TaskHandler_StatisticsRanTaskTicks[i].startTick;
			}

			// Time ++
			runTimes += TaskHandler_StatisticsRanTaskTicks[i].runTime;
			if (!(lastTick == TaskHandler_StatisticsRanTaskTicks[i].startTick
				&& TaskHandler_StatisticsRanTaskTicks[i].runTime == 0) )
				{
				// Upper time
				runTimes += 1;
				}

			// Save last values
			lastTick = TaskHandler_StatisticsRanTaskTicks[i].startTick;
			//lastRunTime = TaskHandler_StatisticsRanTaskTicks[i].runTime;
		}
	}

	// If reached here, we have the oldest tick + ran num
	// Calculate CPU usage
	uint32_t actualTick = HAL_GetTick();
	uint32_t allTime = actualTick - oldestTick;
	uint8_t cpuPercent = runTimes / allTime;

	CommandHandler_Printf("TaskHandler usage:\r\n"
			" Logged time: %d\r\n"
			" All tasks runtimes: %d [ms]\r\n"
			" CPU Usage: %d%%\r\n",
			actualTick - oldestTick,
			runTimes,
			cpuPercent
			);

	CommandHandler_Printf("+%11c+%5c+-%20c-+\r\n", '-', '-', '-');
	CommandHandler_Printf("|%11s|%5s| %20s |\r\n", "<StartTick>", "<Run>", "<TaskName>");
	CommandHandler_Printf("+%11c+%5c+-%20c-+\r\n", '-', '-', '-');

	// Print all table
	for (i = 0; i < TASKHANDLER_STATISTICS_LIMIT; i++)
	{
		if (TaskHandler_StatisticsRanTaskTicks[i].startTick > 0)
		{
			// Print: "<StartTick> - <RunTime: ms> - <TaskName>"
			CommandHandler_Printf("| %9d | %3d | %20s |\r\n",
					TaskHandler_StatisticsRanTaskTicks[i].startTick,
					TaskHandler_StatisticsRanTaskTicks[i].runTime,
					TaskList[TaskHandler_StatisticsRanTaskTicks[i].taskId].taskName);
		}
	}

	CommandHandler_Printf("+%11c+%5c+-%20c-+\r\n", '-', '-', '-');
}



/**
 * \brief	Print TaskHandler runtimes statistics (Run counts)
 */
void TaskHandler_PrintTaskRunCounts(void)
{
	TaskID_t i;

	CommandHandler_Printf("+-%20c-+-%9c-+\r\n", '-', '-');
	CommandHandler_Printf("| %20s | %9s |\r\n", "<TaskName>", "<RunCnt>");
	CommandHandler_Printf("+-%20c-+-%9c-+\r\n", '-', '-');

	for (i = 0; i < Task_Count; i++)
	{
		CommandHandler_Printf("| %20s | %9u |\r\n", TaskList[i].taskName, TaskList[i].taskRunCount);
	}

	CommandHandler_Printf("+-%20c-+-%9c-+\r\n", '-', '-');
}
#endif



#ifdef CONFIG_DEBUG_SW_WATCHDOG_ENABLE
/**
 *	\brief	TaskHandler - Software WatchDog
 *	\note	Call this function from timer / SysTickHandler
 *			If main TaskHandler cnt is not increment, we know, the TaskHandler frozened
 */
void TaskHandler_SwWatchdog(void)
{
	static uint32_t ms = 0;
	static uint32_t TaskHandler_LastRunCnt = 0;

	ms++;
	if (ms >= TASKHANDLER_SW_WATCHDOG_PERIOD)
	{
		ms = 0;
		if (TaskHandler_RunCnt == TaskHandler_LastRunCnt)
		{
			// TaskHandler cn is not changed... :(
			char msg[60];

			usprintf(msg, "TaskHandler frozened: %s\r\n",TaskHandler_ActualTask);
			HAL_UART_Transmit(&Debug_UartHandle, (uint8_t *)msg, StringLength(msg), 10);

			// Be careful: Error_Handler is use the SysTick handler...
			Error_Handler();
		}

		TaskHandler_LastRunCnt = TaskHandler_RunCnt;
	}
}
#endif



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
		UNITTEST_ASSERT(result, "TaskHandler_Init error");
	}


	// Test Scheduler time elapsing

	TaskHandler_Scheduler(1);

	// Check all tick is incremented?
	for (i = 0; i < TasksNum; i++)
	{
		result = (TaskList[i].tick == 1);
		UNITTEST_ASSERT(result, "TaskHandler_Scheduler error");
	}



	// TODO: Test TaskHandler_RunTask ?



	// Test TaskHandler_SetTaskPeriodicTime

	// Save values
	uint32_t oldTaskScheduleRate;
	bool oldOnceRun = TaskList[0].isRunOnce;
	oldTaskScheduleRate = TaskList[0].taskScheduleRate;

	TaskHandler_SetTaskPeriodicTime(0, 1234);
	result = (TaskList[0].taskScheduleRate == 1234);
	UNITTEST_ASSERT(result, "TaskHandler_SetTaskTime Error");
	result = (TaskList[0].isRunOnce == false);
	UNITTEST_ASSERT(result, "TaskHandler_SetTaskTime Error");

	TaskHandler_SetTaskPeriodicTime(0, oldTaskScheduleRate);
	result = (TaskList[0].taskScheduleRate == oldTaskScheduleRate);
	UNITTEST_ASSERT(result, "TaskHandler_SetTaskTime Error");
	result = (TaskList[0].isRunOnce == false);
	UNITTEST_ASSERT(result, "TaskHandler_SetTaskTime Error");

	// Restore
	TaskList[0].isRunOnce = oldOnceRun;



	// Test TaskHandler_SetTaskOnceRun

	// Save values
	oldOnceRun = TaskList[0].isRunOnce;
	oldTaskScheduleRate =TaskList[0].taskScheduleRate;

	TaskHandler_SetTaskOnceRun(0, 2345);
	result = (TaskList[0].taskScheduleRate == 2345);
	UNITTEST_ASSERT(result, "TaskHandler_ScheduleTaskOnce Error");
	result = (TaskList[0].isRunOnce == true);
	UNITTEST_ASSERT(result, "TaskHandler_ScheduleTaskOnce Error");

	TaskHandler_SetTaskOnceRun(0, oldTaskScheduleRate);
	result = (TaskList[0].taskScheduleRate == oldTaskScheduleRate);
	UNITTEST_ASSERT(result, "TaskHandler_ScheduleTaskOnce Error");
	result = (TaskList[0].isRunOnce == true);
	UNITTEST_ASSERT(result, "TaskHandler_ScheduleTaskOnce Error");

	// Restore
	TaskList[0].isRunOnce = oldOnceRun;



	// Test task disabling

	bool oldDisableValue = TaskList[0].isDisabled;

	TaskHandler_DisableTask(0);
	result = (TaskList[0].isDisabled == true);
	UNITTEST_ASSERT(result, "TaskHandler_DisableTask Error");

	// Restore task disable value
	TaskList[0].isDisabled = oldDisableValue;



	// Test task request
	bool oldRequest = TaskList[0].isRequestScheduling;

	TaskHandler_RequestTaskScheduling(0);
	result = (TaskList[0].isRequestScheduling == true);
	UNITTEST_ASSERT(result, "TaskHandler_RequestTaskScheduling Error");

	// Restore task request value
	TaskList[0].isRequestScheduling = oldRequest;


	// TODO: Test TimeOut task!


	// Finish
	UnitTest_End();

}
#endif



#endif	// #ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
