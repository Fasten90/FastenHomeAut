/*
 *    TaskHandler.c
 *    Created on:   2017-02-02
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Task handler
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"
#include "TaskList.h"
#include "TaskHandler.h"
#include "CommandHandler.h"
#include "DebugUart.h"
#include "StringHelper.h"
#include "EventHandler.h"
#include "SwWatchDog.h"

#ifdef CONFIG_MODULE_TASKHANDLER_UNNITEST_ENABLE
#include "UnitTest.h"
#endif

#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
#include "Table.h"
#endif



#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS

///< TaskHandler statistics infos
typedef struct
{
    uint32_t startTick;             ///< Start time of Task running
    uint32_t runTime;               ///< Run time of task
    TaskID_t taskId;                ///< Task ID (which task)
} TaskHandlerStat_t;

static TaskHandlerStat_t TaskHandler_StatisticsRanTaskTicks[TASKHANDLER_STATISTICS_LIMIT] = { 0 };
static uint8_t TaskHandler_StatisticsIndex = 0;
#endif

#ifdef CONFIG_TASKHANDLER_DEBUG_RUN_ENABLE
static const char * TaskHandler_ActualRunningTaskName = NULL;
static TaskID_t TaskHandler_ActualRunningTaskID = 0;
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
 * @brief       Initialize TaskHandler / Tasks
 */
void TaskHandler_Init(void)
{
    TaskID_t i;

    /* Check TaskList contain */
    for (i = 0; i < TasksNum; i++)
    {
        /* Need task function */
        ASSERT(TaskList[i].taskFunction != NULL);

        /* Need task name */
        ASSERT(TaskList[i].taskName != NULL);

        /* Need period */
        ASSERT(TaskList[i].taskScheduleRate != 0);

        /*if (isWrong)
            uprintf("TaskHandler list error in %d. element (name: %s)", i, TaskList[i].taskName);*/
    }


    /* TODO: Optimize with static and dynamic list */
    /* Init task ticks */
    for (i = 0; i < TasksNum; i++)
    {
        TaskList[i].tick = 0;
#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
        TaskList[i].taskRunCount = 0;
#endif
    }
}



/**
 * @brief       Schedule tasks (and increment elapsed time)
 * @note        Recommend call last from main
 * @note        Never return, Do not call anything after this function
 */
void TaskHandler_Scheduler(void)
{
    uint32_t actualTick = 0;
    uint32_t oldTick = HAL_GetTick();
    uint32_t elapsedTick = 0;


    /* Run TaskHandler - Infinite loop */
    /* @note    Be careful, after this while loop is not reached */
    while (1)
    {
        /* Calculate elapsed tick */
        actualTick = HAL_GetTick();
        elapsedTick = actualTick - oldTick;
        oldTick = actualTick;

        /* Time optimizing... */
        /* TODO: Check "required" tasks! */
        if (elapsedTick != 0)
        {
            TaskID_t i;

            TaskHandler_IncrementTicks(elapsedTick);

            /* Check schedule list */
            for (i = 0; i < TasksNum; i++)
            {
                /* Need scheduling? */
                if (!TaskList[i].isDisabled)
                {
                    /* TODO: only parameter give in if(), and after that, call TaskHandler_RunTask with parameter */
                    if (!TaskList[i].isPeriodicScheduleDisabled
                        && TaskList[i].tick >= TaskList[i].taskScheduleRate)
                    {
                        /* Schedule - periodical */
                        TaskHandler_RunTask(i, ScheduleSource_PeriodicSchedule);
                    }
                    else if (TaskList[i].isRunOnce
                        && TaskList[i].tick >= TaskList[i].taskScheduleRate)
                    {
                        /* Schedule - once */
                        TaskHandler_RunTask(i, ScheduleSource_RunOnce);
                    }
                    else if (TaskList[i].isTimeOutTask
                        && TaskList[i].tick >= TaskList[i].taskScheduleRate)
                    {
                        /* TimeOut task */
                        TaskHandler_RunTask(i, ScheduleSource_TimeOut);
                    }
                    else if (TaskList[i].isRequestScheduling)
                    {
                        /* Schedule - event triggered */
                        TaskHandler_RunTask(i, ScheduleSource_EventTriggered);
                    }
                    /* else: Enabled but not scheduled */
                }
            }
        }
    }    /* End of while(1) */

    /* Never reach this / Never return */
}



/**
 * @brief       Increment ticks
 */
static void TaskHandler_IncrementTicks(TaskTick_t elapsedTick)
{
    TaskID_t i;

    if (elapsedTick == 0)
        return;

    for (i = 0; i < TasksNum; i++)
    {
        TaskList[i].tick += elapsedTick;
    }
}



/**
 * @brief       Run task
 */
static void TaskHandler_RunTask(TaskID_t taskID, ScheduleSource_t source)
{
    TaskResult_t result = TaskResult_Fail;

    /* Clear once run */
    if (TaskList[taskID].isRunOnce)
    {
        TaskList[taskID].isDisabled = true;
    }

    /* Clear request */
    TaskList[taskID].isRequestScheduling = false;

#ifdef CONFIG_TASKHANDLER_DEBUG_RUN_ENABLE
    TaskHandler_ActualRunningTaskName = (const char *)TaskList[taskID].taskName;
    TaskHandler_ActualRunningTaskID = taskID;
#endif

    /* Software WatchDog: We are "alive" */
    SW_WATCHDOG_INC();

#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
    /* Measure run time: */
    uint32_t startTime = HAL_GetTick();
#endif

    /* Execute task */
    result = TaskList[taskID].taskFunction(source);

#ifdef CONFIG_TASKHANDLER_DEBUG_ENABLE
    uprintf("- %s task run %s\r\n", TaskList[taskID].taskName, (result == TaskResult_Ok) ? ("successful") : ("failed"));
#else
    UNUSED_ARGUMENT(result);
#endif
#ifdef CONFIG_EVENTLOG_TASKHANDLER_LOG_ENABLE
    EventHandler_GenerateEvent(Event_TaskScheduled, taskID, 0);
#endif

    /* Clear tick */
    /* TODO: Tick clearing or subtraction? */
    TaskList[taskID].tick = 0;

#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
    TaskList[taskID].taskRunCount++;

    uint32_t runTime = HAL_GetTick() - startTime;

    if (!TaskList[taskID].isDisableLogToStatistics)
    {
        /* Need log to statistics */
        /* TODO: Put to statistics function */
        TaskHandler_StatisticsRanTaskTicks[TaskHandler_StatisticsIndex].startTick = startTime;
        TaskHandler_StatisticsRanTaskTicks[TaskHandler_StatisticsIndex].runTime = runTime;
        TaskHandler_StatisticsRanTaskTicks[TaskHandler_StatisticsIndex].taskId = taskID;

        TaskHandler_StatisticsIndex++;

        if (TaskHandler_StatisticsIndex >= TASKHANDLER_STATISTICS_LIMIT)
        {
            TaskHandler_StatisticsIndex = 0;
        }
    }
#endif

#ifdef CONFIG_TASKHANDLER_DEBUG_RUN_ENABLE
    TaskHandler_ActualRunningTaskName = NULL;
    TaskHandler_ActualRunningTaskID = 0;
#endif
}



/**
 * @brief       Initialize task
 */
void TaskHandler_SetTaskPeriodicTime(TaskID_t taskID, TaskTick_t taskTick)
{
    if (taskID < Task_Count)
    {
        TaskList[taskID].isRunOnce = false;
        TaskList[taskID].isDisabled = false;
        TaskList[taskID].taskScheduleRate = taskTick;
        TaskList[taskID].tick = 0;
    }
    /* TODO: Error report */
}



/**
 * @brief       Set Task scheduling 1x after x time
 */
void TaskHandler_SetTaskOnceRun(TaskID_t taskID, TaskTick_t taskTick)
{
    if (taskID < Task_Count)
    {
        TaskList[taskID].isRunOnce = true;
        TaskList[taskID].isDisabled = false;
        TaskList[taskID].taskScheduleRate = taskTick;
        TaskList[taskID].tick = 0;
    }
    /* TODO: Error report */
}



/**
 * @brief       Disable task
 */
void TaskHandler_DisableTask(TaskID_t taskID)
{
    if (taskID < Task_Count)
    {
        TaskList[taskID].isDisabled = true;
    }
    /* TODO: Error report */
}



/**
 * @brief       Request task scheduling
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
 * @brief       Clear TimeOut task tick
 */
void TaskHandler_ClearTimeoutTask(TaskID_t taskID)
{
    if (taskID < Task_Count && TaskList[taskID].isTimeOutTask == true)
    {
        /* Clear */
        TaskList[taskID].tick = 0;
        TaskList[taskID].isDisabled = false;
#ifdef CONFIG_EVENTLOG_TASKHANDLER_LOG_ENABLE
        EventHandler_GenerateEvent(Event_TaskCleared, taskID, 0);
#endif
    }
    /* TODO: Error report */
}



#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
/**
 * @brief       Print TaskHandler statistics border
 */
static void TaskHandler_PrintStatisticsTableHeader(const char * fixheader, char * str, uint8_t strMaxLen, char * header)
{
    Table_PrintTableWithBorder(fixheader, str, strMaxLen, header, "StartTick", "Run", "TaskName");
}



/**
 * @brief       Calculate and print TaskHandler statistics
 */
void TaskHandler_PrintStatistics(void)
{
    uint32_t oldestTick = HAL_GetTick();
    uint8_t runTimes = 0;
    uint8_t i;
    uint32_t lastTick = 0;
    /* int32_t lastRunTime = 0; */

    /* Calculate Statistics values */
    for (i = 0; i < TASKHANDLER_STATISTICS_LIMIT; i++)
    {
        if (TaskHandler_StatisticsRanTaskTicks[i].startTick > 0)
        {
            /* This tick is "ran task tick" */
            if (TaskHandler_StatisticsRanTaskTicks[i].startTick < oldestTick)
            {
                /* This tick is the oldest tick */
                oldestTick = TaskHandler_StatisticsRanTaskTicks[i].startTick;
            }

            /* Time ++ */
            runTimes += TaskHandler_StatisticsRanTaskTicks[i].runTime;
            if (!(lastTick == TaskHandler_StatisticsRanTaskTicks[i].startTick
                && TaskHandler_StatisticsRanTaskTicks[i].runTime == 0))
                {
                /* Upper time */
                runTimes += 1;
                }

            /* Save last values */
            lastTick = TaskHandler_StatisticsRanTaskTicks[i].startTick;
            /* astRunTime = TaskHandler_StatisticsRanTaskTicks[i].runTime; */
        }
    }

    /* If reached here, we have the oldest tick + ran num */
    /* Calculate CPU usage */
    uint32_t actualTick = HAL_GetTick();
    uint32_t allTime = actualTick - oldestTick;
    uint8_t cpuPercent = 0;

    if (allTime != 0)
        cpuPercent = runTimes / allTime;

    DebugUart_Printf("TaskHandler usage:\r\n"
            " Logged time: %d [ms]\r\n"
            " All tasks runtimes: %d [ms]\r\n"
            " CPU Usage: %d%%\r\n",
            actualTick - oldestTick,
            runTimes,
            cpuPercent
            );


    /* Print Statistics table */

    const char const fixheader[] = "| %9d | %3d | %20s |";
    const uint8_t strLength = 2 + 9 + 3 + 3 + 3 + 20 + 2 + 1;
    char str[strLength];
    char header[sizeof(fixheader)];


    TaskHandler_PrintStatisticsTableHeader(fixheader, str, strMaxLen, header);

    /* Print all table */
    for (i = 0; i < TASKHANDLER_STATISTICS_LIMIT; i++)
    {
        if (TaskHandler_StatisticsRanTaskTicks[i].startTick > 0)
        {
            /* Print: "<StartTick> - <RunTime: ms> - <TaskName>" */
            usnprintf(str, strLength, fixheader,
                    TaskHandler_StatisticsRanTaskTicks[i].startTick,
                    TaskHandler_StatisticsRanTaskTicks[i].runTime,
                    TaskList[TaskHandler_StatisticsRanTaskTicks[i].taskId].taskName);
            Table_SendLine(str);
        }
    }

    TaskHandler_PrintStatisticsTableHeader(fixheader, str, strMaxLen, header);
}



/**
 * @brief       Print TaskHandler RunTimes statistics border
 */
static void TaskHandler_PrintTaskRunCountsTableHeader(const char * fixheader, char * str, uint8_t strMaxLen, char * header)
{
    Table_PrintTableWithBorder(fixheader, str, strMaxLen, header, "TaskName", "RunCnt");
}



/**
 * @brief       Print TaskHandler runtimes statistics (Run counts)
 */
void TaskHandler_PrintTaskRunCounts(void)
{
    TaskID_t i;
    static const char const fixheader[] = "| %20s | %9u |";
    const uint8_t strLen = 2 + 20 + 3 + 9 + 2 + 1;
    char str[strLen];
    char header[sizeof(fixheader)];

    TaskHandler_PrintTaskRunCountsTableHeader(fixheader, str, strLen, header);

    for (i = 0; i < Task_Count; i++)
    {
        usnprintf(str, strLen, fixheader,
                TaskList[i].taskName, TaskList[i].taskRunCount);
        Table_SendLine(str);
    }

    TaskHandler_PrintTaskRunCountsTableHeader(fixheader, str, header);
}
#endif    /* #ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS */



#ifdef CONFIG_TASKHANDLER_DEBUG_RUN_ENABLE
/**
 * @brief       Get actual running task name
 */
const char * TaskHandler_GetActualRunningTaskName(void)
{
    return TaskHandler_ActualRunningTaskName;
}



/**
 * @brief       Get actual running task ID
 */
TaskID_t TaskHandler_GetActualRunningTaskID(void)
{
    return TaskHandler_ActualRunningTaskID;
}
#endif



#ifdef CONFIG_MODULE_TASKHANDLER_UNNITEST_ENABLE
uint32_t TaskHandler_UnitTest(void)
{
    uint8_t i;
    bool result;

    /* Start Unit test */
    UnitTest_Start("TaskHandler", __FILE__);


    /* Test Initialization */

    /* Clear requests: Be careful, it make some wrongs */
    for (i = 0; i < TasksNum; i++)
    {
        TaskList[i].isRequestScheduling = false;
    }
    TaskHandler_Init();

    /* Check, all tick are 0? */
    for (i = 0; i < TasksNum; i++)
    {
        result = (TaskList[i].tick == 0);
        UNITTEST_ASSERT(result, "TaskHandler_Init error");
    }


    /* Test Scheduler time elapsing */
    TaskHandler_IncrementTicks(1);

    /* Check all tick is incremented? */
    for (i = 0; i < TasksNum; i++)
    {
        result = (TaskList[i].tick == 1);
        UNITTEST_ASSERT(result, "TaskHandler_Scheduler error");
    }



    /* TODO: Test TaskHandler_RunTask ? */



    /* Test TaskHandler_SetTaskPeriodicTime */

    /* Save values */
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

    /* Restore */
    TaskList[0].isRunOnce = oldOnceRun;



    /* Test TaskHandler_SetTaskOnceRun */

    /* Save values */
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

    /* Restore */
    TaskList[0].isRunOnce = oldOnceRun;



    /* Test task disabling */

    bool oldDisableValue = TaskList[0].isDisabled;

    TaskHandler_DisableTask(0);
    result = (TaskList[0].isDisabled == true);
    UNITTEST_ASSERT(result, "TaskHandler_DisableTask Error");

    /* Restore task disable value */
    TaskList[0].isDisabled = oldDisableValue;



    /* Test task request */
    bool oldRequest = TaskList[0].isRequestScheduling;

    TaskHandler_RequestTaskScheduling(0);
    result = (TaskList[0].isRequestScheduling == true);
    UNITTEST_ASSERT(result, "TaskHandler_RequestTaskScheduling Error");

    /* Restore task request value */
    TaskList[0].isRequestScheduling = oldRequest;


    /* TODO: Test TimeOut task! */


    /* Finish */
    return UnitTest_End();
}
#endif



#endif    /* #ifdef CONFIG_MODULE_TASKHANDLER_ENABLE */
