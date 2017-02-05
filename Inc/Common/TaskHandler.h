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
	bool isDisabled;							///> Task is disabled/enabled
} Task_t;


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void TaskHandler_Init(void);
void TaskHandler_CheckSchedules(TaskTick_t elapsedTick);




#endif /* COMMON_TASKHANDLER_H_ */
