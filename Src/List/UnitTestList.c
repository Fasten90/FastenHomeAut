/*
 *        UnitTestList.c
 *        Created on:        2018-03-03
 *        Author:            Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        -
 *        Target:            STM32Fx
 *        Version:        -
 *        Last modified:    2018-03-03
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#include "MathHelper.h"
#include "CircularBuffer.h"
#include "CommandHandler.h"
#include "ConvertTable.h"
#include "DateTime.h"
#include "EventHandler.h"
#include "HomeAutMessage.h"
#include "LinkedList.h"
#include "MemHandler.h"
#include "Queue.h"
#include "SecuredDataTypes.h"
#include "StringHelper.h"
#include "TaskHandler.h"
#include "GlobalVarHandler.h"
#include "EventLog.h"
#include "MeasurementTimer.h"

#include "UnitTestList.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief  UnitTest - Run
 */
uint32_t UnitTestList_Run(void)
{
    uint32_t result = 0;

#ifdef MODULE_STRING_UNITTEST_ENABLE
    result += StringHelper_UnitTest();
#endif

#ifdef MODULE_DATETIME_UNITTEST_ENABLE
    result += DateTime_UnitTest();
#endif

#ifdef MODULE_HOMEAUTMESSAGE_UNITTEST_ENABLE
    result += HomeAutMessage_UnitTest();
#endif

#ifdef MODULE_COMMANDHANDLER_UNITTEST_ENABLE
    result += CmdH_UnitTest();
#endif

#if defined(CONFIG_MODULE_GLOBALVARHANDLER_ENABLE) && defined(MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE)
    result += GlobVarH_UnitTest();
#endif

#ifdef MODULE_CIRCULARBUFFER_UNITTEST_ENABLE
    result += CircularBuffer_UnitTest();
#endif

#ifdef MODULE_TASKHANDLER_UNNITEST_ENABLE
    result += TaskHandler_UnitTest();
#endif

#if defined(CONFIG_MODULE_EVENTLOG_ENABLE) && defined(MODULE_EVENTLOG_UNITTEST_ENABLE)
    result += EventLog_UnitTest();
#endif

#ifdef MODULE_MATHHELPER_UNITTEST_ENABLE
    result += MathHelper_UnitTest();
#endif

#ifdef MODULE_MEM_UNITTEST_ENABLE
    result += MEM_UnitTest();
#endif

#ifdef MODULE_LINKEDLIST_UNITTEST_ENABLE
    result += LinkedList_UnitTest();
#endif

#ifdef MODULE_EVENTHANDLER_UNITTEST_ENABLE
    result += EventHandler_UnitTest();
#endif

#if defined(MODULE_CONVERTTABLE_UNITTEST_ENABLE) && defined(CONFIG_MODULE_CONVERTTABLE_ENABLE)
    result += ConvertTable_UnitTest();
#endif

#ifdef MODULE_SECUREDDATATYPES_UNITTEST_ENABLE
    result += SecuredDataTypes_UnitTest();
#endif

#ifdef MODULE_QUEUE_UNITTEST_ENABLE
    result += Queue_UnitTest();
#endif

#ifdef MODULE_MEASUREMENTTIMER_UNITTEST_ENABLE
    result += MeasurementTimer_UnitTest();
#endif

    return result;
}


