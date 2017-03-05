/*
 *		TaskHandler.h
 *
 *		Created on:		2017. febr. 2.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 2.
 */

#ifndef COMMON_TASKHANDLER_H_
#define COMMON_TASKHANDLER_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define TASKHANDLER_


// TaskResult defines
#define TASK_RESULT_OK			(true)
#define TASK_RESULT_FAILED		(false)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef bool TaskResult_t;
typedef uint8_t TaskID_t;
typedef uint32_t TaskTick_t;

/// Task Function pointer
typedef TaskResult_t (*TaskFunctionPointer)(TaskID_t id);


typedef struct
{
	const char *taskName;						///> Task Name
	const TaskFunctionPointer taskFunction;		///> Task function
	TaskTick_t tick;							///> Task tick [ms]
	TaskTick_t taskScheduleRate;				///> Task scheduling rate [ms]
	bool isRequestScheduling;					///> Task scheduling request (true, if request)
	bool isPeriodisScheduleDisabled;			///> Task schedule (periodic) disabled
	bool isRunOnce;								///> Task scheduling once
	bool isDisabled;							///> Task is disabled/enabled
} Task_t;


typedef enum
{
	ScheduleSource_Unknown,						///> Unknown schedule reason
	ScheduleSource_EventTriggered,				///> Triggered (event)
	ScheduleSource_PeriodicSchedule				///> Periodical scheduling
} ScheduleSource_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void TaskHandler_Init(void);
void TaskHandler_Scheduler(TaskTick_t elapsedTick);

void TaskHandler_SetTaskPeriodicTime(TaskID_t taskID, TaskTick_t taskTick);
void TaskHandler_SetTaskOnceRun(TaskID_t taskID, TaskTick_t taskTick);
void TaskHandler_DisableTask(TaskID_t taskID);
void TaskHandler_RequestTaskScheduling(TaskID_t taskId);

void TaskHandler_UnitTest(void);



#endif /* COMMON_TASKHANDLER_H_ */
