/*
 *		TaskHandler.h
 *		Created on:		2017-02-02
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Task handler
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-02
 */

#ifndef COMMON_TASKHANDLER_H_
#define COMMON_TASKHANDLER_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef uint8_t TaskID_t;
typedef uint32_t TaskTick_t;


///< TaskResult for Task finish result
typedef enum
{
	TaskResult_Fail,							///< Failed run (task)
	TaskResult_Ok								///< Successful run
} TaskResult_t;


///< Schedule source (Why executed the task by TaskHandler)
typedef enum
{
	ScheduleSource_Unknown,						///< Unknown schedule reason
	ScheduleSource_EventTriggered,				///< Triggered (event)
	ScheduleSource_PeriodicSchedule,			///< Periodical scheduling
	ScheduleSource_RunOnce,						///< Run once
	ScheduleSource_TimeOut						///< TimeOut
} ScheduleSource_t;


///< Task Function pointer
typedef TaskResult_t (*TaskFunctionPointer)(ScheduleSource_t source);


///< Task structure
typedef struct
{
	const char *taskName;						///< Task Name - Init
	const TaskFunctionPointer taskFunction;		///< Task function - Init
	TaskTick_t tick;							///< Task tick [ms] - RunTime
	TaskTick_t taskScheduleRate;				///< Task scheduling rate [ms] - Init/Runtime

	bool isRequestScheduling;					///< Task scheduling request (true, if request) - Runtime
	bool isPeriodicScheduleDisabled;			///< Task schedule (periodic) disabled - Init/Runtime
	bool isRunOnce;								///< Task scheduling once - Init/Runtime
	bool isTimeOutTask;							///< Task is work in TimeOut mode - Init/Runtime
	bool isDisabled;							///< Task is disabled/enabled - Init/Runtime

#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
	uint32_t taskRunCount;						///< How many times ran [count] - RunTime
	bool isDisableLogToStatistics;				///< Disable log to statistics - Init
#endif
} Task_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void TaskHandler_Init(void);
void TaskHandler_Scheduler(void);

void TaskHandler_SetTaskPeriodicTime(TaskID_t taskID, TaskTick_t taskTick);
void TaskHandler_SetTaskOnceRun(TaskID_t taskID, TaskTick_t taskTick);
void TaskHandler_DisableTask(TaskID_t taskID);
void TaskHandler_RequestTaskScheduling(TaskID_t taskID);
void TaskHandler_ClearTimeoutTask(TaskID_t taskID);

void TaskHandler_PrintStatistics(void);
void TaskHandler_PrintTaskRunCounts(void);

#ifdef CONFIG_TASKHANDLER_DEBUG_RUN_ENABLE
const char * TaskHandler_GetActualRunningTaskName(void);
TaskID_t TaskHandler_GetActualRunningTaskID(void);
#else
#define TaskHandler_GetActualRunningTaskID()		(0)
#endif

uint32_t TaskHandler_UnitTest(void);



#endif /* COMMON_TASKHANDLER_H_ */
