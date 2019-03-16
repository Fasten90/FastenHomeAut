/*
 *    CommandList.c
 *    Created on:   2016-01-01
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     General commands
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "compiler.h"

#include "StringHelper.h"
#include "ColoredMessage.h"
#include "CommandHandler.h"
#include "GlobalVarHandler.h"
#include "GlobVarList.h"
#include "ErrorHandler.h"
#include "Terminal.h"
#include "Button.h"
#include "DateTime.h"
#include "Display.h"
#include "DisplayHandler.h"
#include "Display_SSD1306.h"
#include "IO.h"
#include "Watchdog.h"
#include "MathHelper.h"
#include "CircularBuffer.h"
#include "ESP8266.h"
#include "EventLog.h"
#include "EventHandler.h"
#include "HomeAutMessage.h"
#include "TaskHandler.h"
#include "Motor.h"
#include "CommonIO.h"
#include "MemHandler.h"
#include "TaskList.h"
#include "LinkedList.h"
#include "CommonADC.h"
#include "ADC.h"
#include "CommonDAC.h"
#include "Debug.h"
#include "DebugList.h"
#include "Logic.h"
#include "ConvertTable.h"
#include "SecuredDataTypes.h"
#include "SysTime.h"
#include "CommonUART.h"
#include "CommandList.h"
#include "BoardInfos.h"
#include "Queue.h"
#include "UnitTestList.h"
#include "SelfTest_Errors.h"
#include "ButtonSimulator.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *        Global function declarations - Commands
 *----------------------------------------------------------------------------*/

static CmdH_Result_t CommandFunction_cls(uint32_t argc, char** argv);
static CmdH_Result_t CommandFunction_help(uint32_t argc, char** argv);
static CmdH_Result_t CommandFunction_version(uint32_t argc, char** argv);
static CmdH_Result_t CommandFunction_welcome(uint32_t argc, char** argv);
static CmdH_Result_t CommandFunction_reset(uint32_t argc, char** argv);
#ifdef CONFIG_MODULE_UNITTEST_ENABLE
static CmdH_Result_t CommandFunction_unittest(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_USE_PANEL_PC
static CmdH_Result_t CommandFunction_exit(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_TEST_ENABLE
static CmdH_Result_t CommandFunction_moduletest(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_TEST_MODE
static CmdH_Result_t CommandFunction_test(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
static CmdH_Result_t CommandFunction_set(uint32_t argc, char** argv);
static CmdH_Result_t CommandFunction_get(uint32_t argc, char** argv);
static CmdH_Result_t CommandFunction_GlobalVariableHelp(uint32_t argc, char** argv);
static CmdH_Result_t CommandFunction_GlobalVariableList(uint32_t argc, char** argv);
static CmdH_Result_t CommandFunction_GlobalVariableValueList(uint32_t argc, char** argv);
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
static CmdH_Result_t CommandFunction_GlobalVariableTrace(uint32_t argc, char** argv);
#endif
#endif
#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE
static CmdH_Result_t CommandFunction_dac(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_COMMON_IO_ENABLE
static CmdH_Result_t CommandFunction_io(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
static CmdH_Result_t CommandFunction_adc(uint32_t argc, char** argv);
static CmdH_Result_t CommandFunction_adcread(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE
static CmdH_Result_t CommandFunction_PWM(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_MOTOR_ENABLE
static CmdH_Result_t CommandFunction_Motor(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
static CmdH_Result_t CommandFunction_ESP8266(uint32_t argc, char** argv);
#endif
#if defined(CONFIG_MODULE_RTC_ENABLE) || defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE)
static CmdH_Result_t CommandFunction_Time(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
static CmdH_Result_t CommandFunction_EventLog(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE
static CmdH_Result_t CommandFunction_raspberrypi(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_FLASH_ENABLE
static CmdH_Result_t CommandFunction_flashdel(uint32_t argc, char** argv);
static CmdH_Result_t CommandFunction_flashread(uint32_t argc, char** argv);
static CmdH_Result_t CommandFunction_flashwrite(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_ADC_ENABLE
static CmdH_Result_t CommandFunction_adc(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_COMMANDLIST_RAMREAD_WRITE_COMMAND_ENABLE
static CmdH_Result_t CommandFunction_mr(uint32_t argc, char** argv);
static CmdH_Result_t CommandFunction_mw(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_COMMANDLIST_GO_COMMAND_ENABLE
static CmdH_Result_t CommandFunction_go(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
static CmdH_Result_t CommandFunction_Display(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_IO_ENABLE
static CmdH_Result_t CommandFunction_IO(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_SIMULATION_ENABLE
static CmdH_Result_t CommandFunction_Simulation(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
static CmdH_Result_t CommandFunction_TaskHandler(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_DEBUG_ENABLE
static CmdH_Result_t CommandFunction_Debug(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
static CmdH_Result_t CommandFunction_Button(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_FUNCTION_PERIODICAL_SENDING
static CmdH_Result_t CommandFunction_PeriodicalSending(uint32_t argc, char** argv);
#endif
#if (CIRCULARBUFFER_STATISTICS_ENABLE == 1)
static CmdH_Result_t CommandFunction_CircBuffStat(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_COMMON_UART_ENABLE
static CmdH_Result_t CommandFunction_CommonUART(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_BUTTONSIMULATOR_ENABLE
static CmdH_Result_t CommandFunction_ButtonSimulator(uint32_t argc, char** argv);
#endif


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



///< Commands list
const CmdH_Command_t CmdH_CommandList[] =
{
    /*
     * Help:
     *     .name = "StringOfCommand",
     *     .commandFunctionPointer = function name,
     *     .commandArgNum = CmdH_CommandArgNum_0 | CmdH_CommandArgNum_1 | CmdH_CommandArgNum_2,
     *     .description = "Command description",
     *     .syntax = "<required parameter> (optional parameter)"
     */
    {
        .name = "help",
        .commandFunctionPointer = CommandFunction_help,
        .commandArgNum = CmdH_CommandArgNum_0 | CmdH_CommandArgNum_1,
        .description = "Commands's list, or print a command's description",
        .syntax = "(command)",
        .example = "version"
    },
    {
        .name = "version",
        .commandFunctionPointer = CommandFunction_version,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "Get version",
    },
    {
        .name = "welcome",
        .commandFunctionPointer = CommandFunction_welcome,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "Send welcome message",
    },
    {
        .name = "reset",
        .commandFunctionPointer = CommandFunction_reset,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "Software reset",
    },
    {
        .name = "cls",
        .commandFunctionPointer = CommandFunction_cls,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "Clear Screen",
    },
#ifdef CONFIG_TEST_MODE
    {
        .name = "test",
        .commandFunctionPointer = CommandFunction_test,
        .commandArgNum = CmdH_CommandArgNum_0,
    },
#endif
#ifdef CONFIG_MODULE_TEST_ENABLE
    {
        .name = "moduletest",
        .commandFunctionPointer = CommandFunction_moduletest,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "Run module test",
    },
#endif
#ifdef CONFIG_MODULE_UNITTEST_ENABLE
    {
        .name = "unittest",
        .commandFunctionPointer = CommandFunction_unittest,
        .commandArgNum = CmdH_CommandArgNum_0 | CmdH_CommandArgNum_1,
        .description = "Run unit tests",
        .syntax = "(modul)",
    },
#endif
#ifdef CONFIG_USE_PANEL_PC
    {
        .name = "exit",
        .commandFunctionPointer = CommandFunction_exit,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "exit",
    },
#endif
#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
    {
        .name = "get",
        .commandFunctionPointer = CommandFunction_get,
        .commandArgNum = CmdH_CommandArgNum_1,
        .description = "get global variable value",
        .syntax = "<varName>",
        .example = "devicename",
    },
    {
        .name = "set",
        .commandFunctionPointer = CommandFunction_set,
        .commandArgNum = CmdH_CommandArgNum_2,
        .description = "set global variable value",
        .syntax = "<varName> <value>",
        .example = "devicename MyDeviceName",
    },
    {
        .name = "?",
        .commandFunctionPointer = CommandFunction_GlobalVariableHelp,
        .commandArgNum = CmdH_CommandArgNum_1,
        .description = "help global variable",
        .syntax = "<varName>",
        .example = "devicename",
    },
    {
        .name = "list",
        .commandFunctionPointer = CommandFunction_GlobalVariableList,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "List global variable",
    },
    {
        .name = "listvalues",
        .commandFunctionPointer = CommandFunction_GlobalVariableValueList,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "List global variable's values",
    },
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
    {
        .name = "trace",
        .commandFunctionPointer = CommandFunction_GlobalVariableTrace,
        .commandArgNum = CmdH_CommandArgNum_2,
        .description = "Trace global variable's values",
        .syntax = "<varname> <enable/disable>; period <period:ms>",
        .example = "tick enable",
    },
#endif
#endif    /* #ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE */
#ifdef CONFIG_MODULE_COMMON_IO_ENABLE
    {
        .name = "ioinit",
        .commandFunctionPointer = CommandFunction_io,
        .commandArgNum = CmdH_CommandArgNum_2,
        .description = "Init IO pins",
        .syntax = "<port><pin> <input/output>",
        .example = "a1 input",
    },
    {
        .name = "ioout",
        .commandFunctionPointer = CommandFunction_io,
        .commandArgNum = CmdH_CommandArgNum_2,
        .description = "Set output",
        .syntax = "<port><pin> <set/reset>",
        .example = "a2 set",
    },
    {
        .name = "ioin",
        .commandFunctionPointer = CommandFunction_io,
        .commandArgNum = CmdH_CommandArgNum_1,
        .description = "Get input",
        .syntax = "<port><pin>",
        .example = "a1",
    },
#endif
#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE
    {
        .name = "dac",
        .commandFunctionPointer = CommandFunction_dac,
        .commandArgNum = CmdH_CommandArgNum_2,
        .description = "Digital -> Analog",
        .syntax = "<num> <voltage>",
        .example = "1 3.0",
    },
#endif
#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
    {
        .name = "adc",
        .commandFunctionPointer = CommandFunction_adc,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "Analog -> Digital",
    },
    {
        .name = "adcread",
        .commandFunctionPointer = CommandFunction_adcread,
        .description = "ADC read continuous",
        .syntax = "<milliSec> (num)",
        .commandArgNum = CmdH_CommandArgNum_1 | CmdH_CommandArgNum_2,
        .example = "1000 1",
    },
#endif
#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE
    {
        .name = "pwm",
        .commandFunctionPointer = CommandFunction_PWM,
        .commandArgNum = CmdH_CommandArgNum_1,
        .description = "PWM",
        .syntax = "<PWM percent>",
        .example = "20",
    },
#endif
#ifdef CONFIG_MODULE_MOTOR_ENABLE
    {
        .name = "motor",
        .commandFunctionPointer = CommandFunction_Motor,
        .description = "Motor handling",
        .syntax = "...",
        .commandArgNum = CmdH_CommandArgNum_1 | CmdH_CommandArgNum_2,
    },
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
    {
        .name = "esp8266",
        .commandFunctionPointer = CommandFunction_ESP8266,
        .description = "Use ESP8266 module",
        .syntax = "<send> <message>",
        .commandArgNum = CmdH_CommandArgNum_1 | CmdH_CommandArgNum_2,
        .example = "send ThisMessage",
    },
#endif
#if defined(CONFIG_MODULE_RTC_ENABLE) || defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE)
    {
    #if defined(CONFIG_MODULE_RTC_ENABLE)
        .name = "rtc",
        .description = "Use RTC (Real Time Clock)",
    #elif defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE)
        .name = "system",
        .description = "SystemTime",
    #endif
        .commandFunctionPointer = CommandFunction_Time,
        .commandArgNum = CmdH_CommandArgNum_1 | CmdH_CommandArgNum_2,
        .syntax = "<setdate/settime/status> <date: 20yy-mm-dd / time: hh:mm:ss>",
        .example = "setdate 2017-02-08",
    },
#endif
#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
    {
        .name = "eventlog",
        .commandFunctionPointer = CommandFunction_EventLog,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "List EventLog",
    },
#endif
#ifdef CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE
    {
        .name = "#homeaut",
        .description = "HomeAutMessage sending",
        .commandArgNum = CmdH_CommandArgNum_2,
        .commandFunctionPointer = CommandFunction_raspberrypi,
    },
#endif
#ifdef CONFIG_MODULE_ADC_ENABLE
    {
        .name = "adc",
        .commandFunctionPointer = CommandFunction_adc,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "...",
        .syntax = NULL,
    },
#endif
#ifdef CONFIG_MODULE_FLASH_ENABLE
    {
        .name = "flashdel",
        .commandFunctionPointer = CommandFunction_flashdel,
        .commandArgNum = CmdH_CommandArgNum_2,
        .description = "Delete flash",
        .syntax = "<address> <block/sector>",
    },
    {
        .name = "flashread",
        .commandFunctionPointer = CommandFunction_flashread,
        .commandArgNum = CmdH_CommandArgNum_1,
        .description = "Read from flash",
        .syntax = "<address>",
    },
    {
        .name = "flashwrite",
        .commandFunctionPointer = CommandFunction_flashwrite,
        .commandArgNum = CmdH_CommandArgNum_2,
        .description = "Write flash",
        .syntax = "<address> <byte>",
    },
#endif
#ifdef CONFIG_COMMANDLIST_RAMREAD_WRITE_COMMAND_ENABLE
    {
        .name = "mr",
        .commandFunctionPointer = CommandFunction_mr,
        .commandArgNum = CmdH_CommandArgNum_2,
        .description = "Memory read",
        .syntax = "<address> <size>",
    },
    {
        .name = "mwb",
        .commandFunctionPointer = CommandFunction_mw,
        .commandArgNum = CmdH_CommandArgNum_2,
        .description = "Memory write",
        .syntax = "<address> <byte>",
    },
    {
        .name = "mwh",
        .commandFunctionPointer = CommandFunction_mw,
        .commandArgNum = CmdH_CommandArgNum_2,
        .description = "Memory write",
        .syntax = "<address> <byte>",
    },
    {
        .name = "mww",
        .commandFunctionPointer = CommandFunction_mw,
        .commandArgNum = CmdH_CommandArgNum_2,
        .description = "Memory write",
        .syntax = "<address> <byte>",
    },
#endif
#ifdef CONFIG_COMMANDLIST_GO_COMMAND_ENABLE
    {
        .name = "go",
        .commandFunctionPointer = CommandFunction_go,
        .commandArgNum = CmdH_CommandArgNum_1,
        .description = "Jump to address",
        .syntax = "<address>",
    },
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
    {
        .name = "display",
        .commandFunctionPointer = CommandFunction_Display,
        .commandArgNum = CmdH_CommandArgNum_1 | CmdH_CommandArgNum_2,
        .description = "Use display",
        .syntax = "<write/clear> (line,msg)",
        .example = "write 1,message",
    },
#endif
#ifdef CONFIG_MODULE_IO_ENABLE
    {
        .name = "io",
        .commandFunctionPointer = CommandFunction_IO,
        .commandArgNum = CmdH_CommandArgNum_0 | CmdH_CommandArgNum_1 | CmdH_CommandArgNum_2,
        .description = "Get input-output states",
        .syntax = "<num> <on/off/toggle/status>",
        .example = "1 on",
    },
#endif
#ifdef CONFIG_MODULE_SIMULATION_ENABLE
    {
        .name = "simulate",
        .commandFunctionPointer = CommandFunction_Simulation,
        .commandArgNum = CmdH_CommandArgNum_1 | CmdH_CommandArgNum_2,
        .description = "simulate event, errors, etc.",
    },
#endif
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
    {
        .name = "taskhandler",
        .commandFunctionPointer = CommandFunction_TaskHandler,
        .commandArgNum = CmdH_CommandArgNum_1,
        .description = "taskhandler functions",
        .syntax = "<statistics/runcounts>",
    },
#endif
#ifdef CONFIG_MODULE_DEBUG_ENABLE
    {
        .name = "debug",
        .commandFunctionPointer = CommandFunction_Debug,
        .commandArgNum = CmdH_CommandArgNum_0 | CmdH_CommandArgNum_2,
        .description = "Debug enable/disable",
        .syntax = "<taskname/taskid> <enable/disable>",
    },
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
    {
        .name = "button",
        .commandFunctionPointer = CommandFunction_Button,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "Button state",
    },
#endif
#ifdef CONFIG_FUNCTION_PERIODICAL_SENDING
    {
        .name = "periodicalsending",
        .commandFunctionPointer = CommandFunction_PeriodicalSending,
        .commandArgNum = CmdH_CommandArgNum_1 | CmdH_CommandArgNum_2,
        .description = "Periodical sending message",
        .syntax = "<msg> (periodtime [ms])",
        .example = "sendmessage 500",
    },
#endif
#if (CIRCULARBUFFER_STATISTICS_ENABLE == 1)
    {
        .name = "circbuffstat",
        .commandFunctionPointer = CommandFunction_CircBuffStat,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "Circular Buffer statistics",
    },
#endif
#ifdef CONFIG_MODULE_COMMON_UART_ENABLE
    {
        .name = "commonuart",
        .commandFunctionPointer = CommandFunction_CommonUART,
        .commandArgNum = CmdH_CommandArgNum_1,
        .description = "Common UART",
    },
#endif
#ifdef CONFIG_MODULE_BUTTONSIMULATOR_ENABLE
    {
        .name = "buttonsimulator",
        .commandFunctionPointer = CommandFunction_ButtonSimulator,
        .commandArgNum = CmdH_CommandArgNum_0,
        .description = "Button Simulator",
    },
#endif


    /*
     * XXX: Add new commands here
     *
     * Help:
     *     .name = "StringOfCommand",
     *     .commandFunctionPointer = function name,
     *     .commandArgNum = CmdH_CommandArgNum_0 | CmdH_CommandArgNum_1 | CmdH_CommandArgNum_2,
     *     .description = "Command description",
     *     .syntax = "<required parameter> (optional parameter)"
     */

};


const CmdH_CommandID_t CmdH_CommandNum = NUM_OF(CmdH_CommandList);



/*------------------------------------------------------------------------------
 *                                Command functions
 *----------------------------------------------------------------------------*/


/**
 * @brief    CLS: Clear screen
 */
static CmdH_Result_t CommandFunction_cls(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);

#ifdef CONFIG_MODULE_TERMINAL_ENABLE
    Terminal_SendCls();
#endif

    return CmdH_Result_Ok;
}



/**
 * @brief    Get version
 */
static CmdH_Result_t CommandFunction_version(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);

    CmdH_SendLine(Global_Version);

    return CmdH_Result_Ok;
}



/**
 * @brief    Send Welcome message
 */
static CmdH_Result_t CommandFunction_welcome(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);

#ifdef CONFIG_MODULE_TERMINAL_ENABLE
    Terminal_SendWelcome();
#endif

    return CmdH_Result_Ok;
}



/**
 * @brief    Help command
 *             Use: 'help', or 'help <CommandName>'
 *             List commands or write the command's description
 */
static CmdH_Result_t CommandFunction_help(uint32_t argc, char** argv)
{
    if (argc == 1)
    {
        /* If Arg2 is empty, listing all commands */
        CmdH_PrintAllCommands();
    }
    else if (argc == 2)
    {
        /* Has Arg2, find the command & print help */
        CmdH_SearchCommandAndPrintHelp(argv[1]);
    }

    return CmdH_Result_Ok;
}



/**
 * @brief    Reset command
 */
static CmdH_Result_t CommandFunction_reset(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);

    uint16_t i;

    CmdH_SendLine("Reset...");

    /* Wait */
    for (i = 0; i < 1000; i++);

    /* Reset */
    NVIC_SystemReset();

    /* Wait */
    for (i = 0; i < 1000; i++);

    return CmdH_Result_Ok;
}



#ifdef CONFIG_MODULE_UNITTEST_ENABLE
/**
 * @brief    Run module's unit tests
 */
static CmdH_Result_t CommandFunction_unittest(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);

    CmdH_SendLine("Start unittest");

    UnitTestList_Run();

    CmdH_SendLine("Unittest finished");

    return CmdH_Result_Ok;
}
#endif    /* #ifdef CONFIG_MODULE_UNITTEST_ENABLE */



#ifdef CONFIG_USE_PANEL_PC
/**
 * @brief    Exit (to windows)
 */
static CmdH_Result_t CommandFunction_exit(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);

    CmdH_SendLine("Exit...");

    exit(0);

    return CmdH_Result_Ok;
}
#endif    /* #ifdef CONFIG_USE_PANEL_PC */



#ifdef CONFIG_MODULE_TEST_ENABLE
/**
 * @brief    Run module test
 */
static CmdH_Result_t CommandFunction_moduletest(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);


#ifdef CONFIG_MODULE_IO_ENABLE
    uint8_t i;

    /* LED test */
    CmdH_SendLine("LED test");

    /* LEDs on */
    for (i = LED_NUM_MIN; i <= LED_NUM_MAX; i++)
    {
        IO_Output_SetStatus(i, LED_Cmd_SetOn);
        DelayMs(500);
    }

    Watchdog_Clear();

    /* LEDs off */
    for (i = LED_NUM_MIN; i <= LED_NUM_MAX; i++)
    {
        IO_Output_SetStatus(i, LED_Cmd_SetOff);
        DelayMs(500);
    }
#endif    /* #ifdef CONFIG_MODULE_IO_ENABLE */


#ifdef CONFIG_MODULE_BUTTON_ENABLE
    /* Clear flag */
    BUTTON_Clicked = 0;

    CmdH_SendLine("\r\nButton test: Please press button!");

    /* TODO: Implement with Event... */
    while (!BUTTON_Clicked)
    {
        CmdH_SendChar('.');
        DelayMs(500);
        Watchdog_Clear();
    }

    CmdH_SendLine("Button pressed");
#endif


    /* Beep in terminal */
    CmdH_SendLine("Beep test");
    CmdH_SendChar(TERMINAL_KEY_BELL);


    /* Send formatted messages */
#ifdef CONFIG_MODULE_COLOREDMESSAGE_ENABLE
    CmdH_SendLine("Formatted message test");
    FormattedMessage_Test();
#endif

    return CmdH_Result_Ok;
}
#endif    /* #ifdef CONFIG_MODULE_TEST_ENABLE */



#ifdef CONFIG_TEST_MODE

/* olatile uint32_t importantValue = 0; */

/*
void SetImportantValue(uint32_t newValue)
{
    importantValue = newValue;
}*/

/**
 * @brief    Test function
 */
static CmdH_Result_t CommandFunction_test(uint32_t argc, char** argv)
{
    COMPILER_MESSAGE("Test function turned on! Disable in release!");
    /* pragma message "Compiling " __FILE__ "..." */

    /* Suppress warning */
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);

    CmdH_SendLine("Test start");

    /*
     *         Test codes
     */


    /* Test variadic macros */
    /* Warning: "ISO C does not permit named variadic macros [-Wvariadic-macros] */
    /* in Atollic TrueSTUDIO, 7.1.1 */

    /*
     * Example:
     * #define eprintf(...) fprintf (stderr, __VA_ARGS__)
     * #define eprintf(args...) fprintf (stderr, args)
     */

    /* define eprintf(args...) uprintf(args) */
    /*
    #define eprintf(...)             uprintf(__VA_ARGS__)

        eprintf("Test variadic %d %s\r\n", 20, "testtext");

    */


    /* Test code size with init value */

    /*
    uint32_t testValue = 0;

    if (HAL_GetTick())
    {
        testValue = HAL_GetTick();
        uprintf("GetTick value test: %u", testValue);
    }

    */



#if 0
    /* RTC - Print DateTime in one line with refreshing */

    DateTime_t actualDateTime = { { 17, 5, 7 }, { 19, 45, 0 } };

    /* Clear button state */
    BUTTON_Clicked = 0;
    /* Until pressed button... */
    while (!BUTTON_Clicked)
    {
        /* Wait 1 second */
        DelayMs(1000);

        /* Step 1 second */
        DateTime_StepOneSecond(&actualDateTime);

        /* Send actual DateTime */
        char message[80];
        DateTime_PrintDateTimeToString(message, &actualDateTime);
        CmdH_SendMessage(ESCAPE_DELETELINE);
        /* Cursor to line start */
        CmdH_SendMessage(ESCAPE_CURSOR_TO_LINESTART);
        /* mdH_SendMessage(ESCAPE_CURSORLEFTLOTOF); */

        CmdH_SendMessage(message);
    }
#endif



#if 0
    /* Print DateTime test */

    DateTime_t actualDateTime = { { 17, 5, 7 }, { 21, 38, 0 } };

    /* Clear button state */
    BUTTON_Clicked = 0;
    while (!BUTTON_Clicked)
    {
        /* Step 1 second */
        DelayMs(1000);
        DateTime_StepOneSecond(&actualDateTime);

        /* Send actual DateTime */
        char message[80];
        DateTime_PrintDateTimeToString(message, &actualDateTime);

        Display_PrintString(message, 1, Font_8x5);

        Display_SetClock(&actualDateTime.time);

        /* Should display, because this test is blocked mode */
        Display_Activate();
    }
#endif



    /* Stack test */
    /*
    static bool stackIsFilled = false;
    if (!stackIsFilled)
    {
        mem_StackFillWithGuardValues();
        stackIsFilled = true;
    }
    else
    {
        mem_CheckStackGuardValues();
    }
     */


    /* print() test */
    /* rintf("Example"); */


    /* Debug test */
    /*
    #include "Debug.h"
    Debug_Printf(Debug_ESP8266, "Formatted %s", "text");

    Debug_Printf(Debug_New, "Number: %d,%d,%d", 0, 1, 2);

    Debug_SetEnable(Debug_ESP8266, false);

    Debug_SetEnable(Debug_New, true);

    Debug_Printf(Debug_ESP8266, "Formatted %s", "text");

    Debug_Printf(Debug_New, "Number: %d,%d,%d", 0, 1, 2);
    */


    /*
    uprintf("DigitNum test: %d\r\n", DigitNum(2000, 10));
    uprintf("DigitNum test: %d\r\n", DigitNum(2000, 0));
    */

    /*
    #include "Calc.h"
    uint32_t a = 0;
    Increment(&a);
    uprintf("Incremented value: %d", a);
     */


#if 0
    /* Snake */
    #include "Snake.h"
    Snake_Init();
#endif


    /* The 2 solution are equal in code size */
    /*
    uint32_t test = 0;
    test |= 1 << 1;
    test |= 1 << 2;
    */

    /*
    typedef struct
    {
        uint32_t bit1 : 1;
        uint32_t bit2 : 1;
        uint32_t bit3 : 1;
    } BitTest_t;

    BitTest_t test = { 0 };
    test.bit2 = 1;
    test.bit3 = 1;

    uprintf("Data: %x", test);
    */


    /* erminal_TestLoading(); */


    /* -\_("))_/- */
    /* printf("-\\_(\"))_/-"); */



    /* Which use more space? */
    //
#if 0
    /* 1, */
    DebugUart_SendMessage("message: ");
    DebugUart_SendMessage("1");
    /*    text       data        bss        dec        hex    filename */
    /*    48116        232       4784      53132 */
#endif


    /* 2. */
    /* ebugUart_Printf("message: %s", "1"); */
    /* rint size information */
    /*    text       data        bss        dec        hex    filename */
    /*    48112        232       4784      53128       cf88    C:\Engineer\Projects\AtollicWorkspace\FastenNodeF0\Debug\FastenNodeF0.elf */



    /* Test: 2. */
    /* 0. */
    /* 48076 */

    /* 1. */
    /* ebugUart_SendMessage("message"); */
    /* 48096 text */

    /* 2. format string (it will be slower) */
    /* ebugUart_Printf("message"); */
    /* 48096 */

    /* 1. and 2. solution are equal in code size */



    /* Test dim */
    /*
    static bool dimTest = false;
    if (dimTest)
        dimTest = false;
    else
        dimTest = true;

    SSD1306_dim(dimTest);
    */


    /* Test Get function */
    /* mportantValue = 5; */
    /*
     *    text       data        bss        dec        hex    filename
     *    48892        252       4592      53736       d1e8    C:\Engineer\Projects\AtollicWorkspace\FastenNodeF0\Debug\FastenNodeF0.elf
     */

    /* etImportantValue(5); */
    /*
     * text       data        bss        dec        hex    filename
     * 48916        252       4592      53760       d200    C:\Engineer\Projects\AtollicWorkspace\FastenNodeF0\Debug\FastenNodeF0.elf
     */


    /*
    volatile uint8_t myTestArray[100];
    printf("Array start address: %u, end address: %u\r\n", &myTestArray[0], &myTestArray[99]);

    volatile uint8_t myTestArray2[100];
    printf("Array start address: %u, end address: %u\r\n", &myTestArray2[0], &myTestArray2[99]);
    */
    /*
     * Array start address: 536878260, end address: 536878359
     * Array start address: 536878160, end address: 536878259
     */


    /* oloredMessage_Test(); */


    /* PlantUML test */
    /*
     * @startuml
     * start
     * :1. point;
     * :2. point;
     * stop
     * @enduml
     */

    /* Test EscapeBrowser */
    /*
    const char * const strList[] = { "1", "2", "3", NULL };
    char strBuffer[300] = { 0 };
    EscapeBrowser_PrintNewLineList(strBuffer, strList);
    DebugUart_SendLineBlocked(strBuffer);
    */

    /**
     *         End of Test codes
     */


    CmdH_SendLine("\r\n"
                  "Test end");

    return CmdH_Result_Ok;
}
#endif    /* #ifdef CONFIG_TEST_MODE */



#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
/**
 * @brief    Get globalvar value
 *             Use: 'get <globalvarname>'
 */
static CmdH_Result_t CommandFunction_get(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);

    /* Process */
    GlobVarH_ProcessResult_t result = GlobVarH_ProcessVariableCommand(
            &GlobVarH_Variables, argv[1], argv[2],
            GlobVarH_SetGet_Get, CommProt_DebugUart);

    GlobVarH_WriteResults(result);

    return CmdH_Result_Ok;
}



/**
 * @brief    set global variable
 *             Use: 'set <globalvariablename> <value>'
 */
static CmdH_Result_t CommandFunction_set(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);

    /* Process */
    GlobVarH_ProcessResult_t result = GlobVarH_ProcessVariableCommand(
            &GlobVarH_Variables, argv[1], argv[2],
            GlobVarH_SetGet_Set, CommProt_DebugUart);

    GlobVarH_WriteResults(result);

    return CmdH_Result_Ok;
}



/**
 * @brief    Global variable help
 *             Use: '? <globalvar>'
 */
static CmdH_Result_t CommandFunction_GlobalVariableHelp(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);

    /* Process */
    GlobVarH_ProcessResult_t result = GlobVarH_ProcessVariableCommand(
            &GlobVarH_Variables, argv[1], argv[2],
            GlobVarH_SetGet_Help, CommProt_DebugUart);

    GlobVarH_WriteResults(result);

    return CmdH_Result_Ok;
}



/**
 * @brief    List global variables
 */
static CmdH_Result_t CommandFunction_GlobalVariableList(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);

    GlobVarH_ListAllVariableParameters();

    return CmdH_Result_Ok;
}



/**
 * @brief    Print all global variable values
 */
static CmdH_Result_t CommandFunction_GlobalVariableValueList(uint32_t argc,
        char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);

    GlobVarH_PrintAllVariableValues();

    return CmdH_Result_Ok;
}



#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
/**
 * @brief    Enable/Disable trace
 */
static CmdH_Result_t CommandFunction_GlobalVariableTrace(uint32_t argc,
        char** argv)
{
    CmdH_Result_t result = CmdH_Result_Unknown;

    /* Suppress warning */
    UNUSED_ARGUMENT(argc);

    uint32_t id;
    if (StringToUnsignedDecimalNum(argv[1], &id))
    {
        /* "trace <id> enable/disable" */
        /* Good ID */
        if (!StrCmp("enable", argv[2]))
        {
            GlobVarH_EnableTrace(id, true);
            CmdH_Printf("Enabled trace: %d\r\n", id);
            result = CmdH_Result_Ok;
        }
        else
        {
            GlobVarH_EnableTrace(id, false);
            CmdH_Printf("Disabled trace: %d\r\n", id);
            result = CmdH_Result_Ok;
        }
    }
    else if(!StrCmp("period", argv[1]))
    {
        /* Trace period time setting */
        uint32_t time;
        if (StringToUnsignedDecimalNum(argv[2], &time))
        {
            /* Parameter is good, set Trace task periodic time */
            TaskHandler_SetTaskPeriodicTime(Task_Trace, time);
            result = CmdH_Result_Ok_SendSuccessful;
        }
        else
        {
            result = CmdH_Result_Error_WrongArgument2;
        }
    }
    else
    {
        /* trace <varname> enable/disable */
        /* First parameter is not "id", maybe "name", try process */
        GlobVarH_ProcessResult_t varResult = GlobVarH_ProcessVariableCommand(
                &GlobVarH_Variables, argv[1], argv[2], GlobVarH_SetGet_Trace, CommProt_DebugUart);

        GlobVarH_WriteResults(varResult);

        result = CmdH_Result_Ok_SendSuccessful;
    }

    return result;
}
#endif

#endif    /* #ifdef CONFIG_MODULE_GlobVarHENABLE */



#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE
/**
 * @brief    DAC function
 *            Use: 'dac <channel> <voltage>'
 */
static CmdH_Result_t CommandFunction_dac(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);

    uint32_t channelNum;
    float voltage = 0.0f;

    /* Check 1. argument (channel num) */
    if (!StringToUnsignedDecimalNum(argv[1], &channelNum))
    {
        return CmdH_Result_Error_WrongArgument1;
    }

    /* Check 2. argument (voltage) */
    if (!StringToFloat(argv[2], &voltage))
    {
        return CmdH_Result_Error_WrongArgument2;
    }

    /* Set DAC value */
    if (CommonDAC_SetValue(channelNum, voltage))
    {
        return CmdH_Result_Ok_SendSuccessful;
    }
    else
    {
        return CmdH_Result_Error_WrongArgumentNum;
    }
}
#endif



#ifdef CONFIG_MODULE_COMMON_IO_ENABLE
/**
 * @brief    Common IO function
 *             Initialize and set/get io pin status
 *             "ioinit <port><pin> <input/output>"
 *             "ioout <port><pin> <set/reset>"
 *             "ioin <port><pin>
 */
static CmdH_Result_t CommandFunction_io(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);

    if (!StrCmp("ioinit", argv[0]))
    {
        /* Init */
        char port = argv[1][0];
        uint32_t pin;
        if (StringToUnsignedDecimalNum(&argv[1][1], &pin))
        {
            /* TODO: with enum string? */
            IO_Type io = IO_UNKNOWN;
            if (!StrCmp("input", argv[2]))
            {
                io = IO_INPUT;
            }
            else if (!StrCmp("output", argv[2]))
            {
                io = IO_OUTPUT;
            }
            else
            {
                return CmdH_Result_Error_WrongArgument2;
            }

            if (CommonIO_Init(port, (uint8_t)pin, io))
            {
                return CmdH_Result_Ok;
            }
            else
            {
                return CmdH_Result_Error_Unknown;
            }
        }
        else
        {
            return CmdH_Result_Error_WrongArgument1;
        }
    }
    else if (!StrCmp("ioout", argv[0]))
    {
        /* Output */
        char port = argv[1][0];
        uint32_t pin;
        if (StringToUnsignedDecimalNum(&argv[1][1], &pin))
        {
            Output_Type output = OUTPUT_DONTCARE;
            if (!StrCmp("set", argv[2]))
            {
                output = OUTPUT_HIGH;
            }
            else if (!StrCmp("reset", argv[2]))
            {
                output = OUTPUT_LOW;
            }
            else if (!StrCmp("toggle", argv[2]))
            {
                output = OUTPUT_TOGGLE;
            }
            else if (!StrCmp("status", argv[2]))
            {
                output = OUTPUT_STATUS;
            }
            /* Set output */
            bool status = CommonIO_SetOutput(port,pin,output);
            CmdH_Printf("Output status: %d\r\n", status);
            return CmdH_Result_Ok;
        }
        else
        {
            return CmdH_Result_Error_Unknown;
        }
    }
    else if (!StrCmp("ioin", argv[0]))
    {
        /* Input */
        char port = argv[1][0];
        uint32_t pin;
        if (StringToUnsignedDecimalNum(&argv[1][1], &pin))
        {
            bool status = CommonIO_ReadPin(port, pin);
            CmdH_Printf("Input status: %d\r\n", status);
            return CmdH_Result_Ok;
        }
        else
        {
            return CmdH_Result_Error_WrongArgument1;
        }
    }

    return CmdH_Result_Ok;
}
#endif



#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
/**
 * @brief    Common ADC function
 *             Read ADC values
 */
static CmdH_Result_t CommandFunction_adc(uint32_t argc, char** argv)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);

    uint8_t i;

    CommonADC_ConvertAllMeasuredValues();

    for (i = 0; i < ADC_CHANNEL_NUM; i++)
    {
        CmdH_Printf("ADC: %d. value: %2.2f\r\n", i, ADC_ConvertedValues[i]);
    }

    return CmdH_Result_Ok;
}
#endif



#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
/**
 * @brief    Common ADC function
 *             Read ADC values
 */
static CmdH_Result_t CommandFunction_adcread(uint32_t argc, char** argv)
{

    uint32_t convertValue;
    uint16_t milliSec = 0;
    CmdH_Result_t result;

    /* Check 1. argument */
    if (StringToUnsignedDecimalNum(argv[1], &convertValue))
    {
        /* Received good period value */
        milliSec = (uint16_t)convertValue;
    }
    else if (!StrCmp("stop", argv[1]))
    {
        /* Stop */
        TaskHandler_DisableTask(Task_CommonAdc);
        result = CmdH_Result_Ok_SendSuccessful;
    }
    else
    {
        /* First argument is wrong */
        result = CmdH_Result_Error_WrongArgument1;
    }

    /* Check 2. argument */
    if (argc == 3)
    {
        if (StringToUnsignedDecimalNum(argv[2], &convertValue))
        {
            if (convertValue < ADC_CHANNEL_NUM)
            {
                /* Is Ok */
                TaskHandler_RequestTaskScheduling(Task_CommonAdc);
                TaskHandler_SetTaskPeriodicTime(Task_CommonAdc, milliSec);
                CommonADC_SetPrintNum(convertValue);
                result = CmdH_Result_Ok_SendSuccessful;
            }
            else
            {
                /* Wrong ADC num */
                result = CmdH_Result_Error_WrongArgument2;
            }
        }
        else
        {
            /* Second argument is wrong */
            result = CmdH_Result_Error_WrongArgument2;
        }
    }

    return result;
}
#endif



#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE
static CmdH_Result_t CommandFunction_PWM(uint32_t argc, char** argv)
{
    /* Suppress unused args */
    UNUSED_ARGUMENT(argc);

    uint8_t percent;
    uint32_t convertValue;
    CmdH_Result_t result;

    if (StringToUnsignedDecimalNum(argv[1], &convertValue))
    {
        if (convertValue <= 100)
        {
            percent = (uint8_t)convertValue;
            CommonPWM_ChangePercent(percent);
            result = CmdH_Result_Ok;
        }
        else
        {
            result = CmdH_Result_Error_WrongArgument1;
        }
    }
    else
    {
        result = CmdH_Result_Error_WrongArgument1;
    }

    return result;
}
#endif



#ifdef CONFIG_MODULE_MOTOR_ENABLE
static CmdH_Result_t CommandFunction_Motor(uint32_t argc, char** argv)
{
    UNUSED_ARGUMENT(argc);
    CmdH_Result_t result = CmdH_Result_Unknown;
    int32_t convertValue;


    if (!StrCmp("dc", argv[1]))
    {
        /* Set DC motor */
        if (StringToSignedDecimalNum(argv[2], &convertValue))
        {
            if ((convertValue <= 100) && (convertValue > -100))
            {
                #ifdef CONFIG_FUNCTION_REMOTECONTROLLER_CAR
                IO_Output_SetStatus(IO_LED_Green, IO_Output_Cmd_SetOn);
                #endif
                int8_t percent = (uint8_t)convertValue;
                Motor_StateMachine_SetDc(percent);
                TaskHandler_ClearTimeoutTask(Task_MotorTimeout);
                result = CmdH_Result_Ok_SendSuccessful;
            }
            else
            {
                result = CmdH_Result_Error_WrongArgument2;
            }
        }
        else
        {
            result = CmdH_Result_Error_WrongArgument2;
        }
    }
    else if (!StrCmp("servo", argv[1]))
    {
        /* Set servo motor */
        if (StringToSignedDecimalNum(argv[2], &convertValue))
        {
            if (convertValue <= 90 && convertValue >= -90)
            {
                #ifdef CONFIG_FUNCTION_REMOTECONTROLLER_CAR
                IO_Output_SetStatus(IO_LED_Green, IO_Output_Cmd_SetOn);
                #endif
                int8_t angle = (int8_t)convertValue;
                /*
                Motor_ServoChangeAngle(angle);
                */
                Motor_StateMachine_SetAngle(angle);
                TaskHandler_ClearTimeoutTask(Task_MotorTimeout);

                result = CmdH_Result_Ok_SendSuccessful;
            }
            else
            {
                result = CmdH_Result_Error_WrongArgument2;
            }
        }
        else
        {
            result = CmdH_Result_Error_WrongArgument2;
        }
    }
    else if (!StrCmp("slide", argv[1]))
    {
        /* Motor Slide (DC and Servo too) */
        if (!StrCmp("on", argv[2]))
        {
            MotorTestSlide_Enabled = true;
            result = CmdH_Result_Ok_SendSuccessful;
        }
        else if (!StrCmp("off", argv[2]))
        {
            MotorTestSlide_Enabled = false;
            Motor_ControlStop();
            result = CmdH_Result_Ok_SendSuccessful;
        }
        else
        {
            MotorTestSlide_Enabled = false;
            Motor_ControlStop();
            result = CmdH_Result_Error_WrongArgument2;
        }
    }
    else
    {
        /* "motor <dc value> <turning value>" */
        /* TODO: Merge with commands at above */
        bool isOk = false;

        /* 1. argument: DC */
        if (StringToSignedDecimalNum(argv[1], &convertValue))
        {
            if (convertValue <= 100 && convertValue > -100)
            {
                #ifdef CONFIG_FUNCTION_REMOTECONTROLLER_CAR
                IO_Output_SetStatus(IO_LED_Green, IO_Output_Cmd_SetOn);
                #endif
                int8_t percent = (uint8_t)convertValue;
                Motor_StateMachine_SetDc(percent);
                TaskHandler_ClearTimeoutTask(Task_MotorTimeout);
                result = CmdH_Result_Ok;

                isOk = true;
            }
            else
            {
                result = CmdH_Result_Error_WrongArgument2;
            }
        }
        else
        {
            result = CmdH_Result_Error_WrongArgument2;
        }

        /* 2. argument: servo */
        if (isOk)
        {
            if (StringToSignedDecimalNum(argv[2], &convertValue))
            {
                if (convertValue <= 90 && convertValue >= -90)
                {
                    #ifdef CONFIG_FUNCTION_REMOTECONTROLLER_CAR
                    IO_Output_SetStatus(IO_LED_Green, IO_Output_Cmd_SetOn);
                    #endif
                    int8_t angle = (int8_t)convertValue;
                    Motor_StateMachine_SetAngle(angle);
                    TaskHandler_ClearTimeoutTask(Task_MotorTimeout);

                    result = CmdH_Result_Ok;
                }
                else
                {
                    result = CmdH_Result_Error_WrongArgument2;
                }
            }
            else
            {
                result = CmdH_Result_Error_WrongArgument2;
            }
        }
        /* else: isOk = false --> not need handle */

    }

    return result;
}
#endif



#ifdef CONFIG_MODULE_ESP8266_ENABLE
static CmdH_Result_t CommandFunction_ESP8266(uint32_t argc, char** argv)
{
    UNUSED_ARGUMENT(argc);
    bool result = CmdH_Result_Unknown;

    if (!StrCmp("sendonwifi", argv[1]))
    {
        /* Send message to ESP8266 sending queue, which will send on ESP8266 TCP connection */
        size_t msgLength = StringLength(argv[2]);
        ESP8266_RequestSendTcpMessage(argv[2], msgLength);
        result = CmdH_Result_Ok_SendSuccessful;
    }
    else if (!StrCmp("sendtomodule", argv[1]))
    {
        /* Send forward to ESP8266 module last parameter */
        /* Send: received string + \r\n */
        char sendString[50];
        usnprintf(sendString, 50, "%s\r\n", argv[2]);
        ESP8266_SendString(sendString);
        result = CmdH_Result_Ok_SendSuccessful;
    }
    else if (!StrCmp("reconnect", argv[1]))
    {
        if (!StrCmp("wifi", argv[2]))
        {
            /* WiFi Reconnect */
            ESP8266_RequiredNewState(ESP8266_AdjustableState_ReconnectWifi);
            result = CmdH_Result_Ok_SendSuccessful;
        }
        else if (!StrCmp("tcp", argv[2]))
        {
            /* TCP Reconnect */
            ESP8266_RequiredNewState(ESP8266_AdjustableState_ReconnectTCP);
            result = CmdH_Result_Ok_SendSuccessful;
        }
        else
        {
            result = CmdH_Result_Error_WrongArgument2;
        }
    }
    else if (!StrCmp("ip", argv[1]))
    {
        /* Print ESP8266 IP addresses */
        char ipBuffer[80];
        ESP8266_PrintIpAddress(ipBuffer);
        CmdH_SendLine(ipBuffer);
        result = CmdH_Result_Ok;
    }
    else
    {
        /* Wrong 1. parameter */
        result = CmdH_Result_Error_WrongArgument1;
    }

    return result;
}
#endif



#if defined(CONFIG_MODULE_RTC_ENABLE) || defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE)
static CmdH_Result_t CommandFunction_Time(uint32_t argc, char** argv)
{
    CmdH_Result_t result = CmdH_Result_Unknown;

    if (!StrCmp("setdate", argv[1]) && argc == 3)
    {
        /* Set date */
        Date_t date;
        if (DateTime_ConvertDateStringToDate(argv[2], &date))
        {
            /* Successful convert, set */
            SysTime_SetDate(&date);
            result = CmdH_Result_Ok_SendSuccessful;
        }
        else
        {
            result = CmdH_Result_Error_WrongArgument2;
        }
    }
    else if (!StrCmp("settime", argv[1]) && argc == 3)
    {
        /* Set time */
        Time_t time;
        if (DateTime_ConvertTimeStringToTime(argv[2], &time))
        {
            /* Successful convert, set */
            SysTime_SetTime(&time);
            result = CmdH_Result_Ok_SendSuccessful;
        }
        else
        {
            result = CmdH_Result_Error_WrongArgument2;
        }
    }
    else if (!StrCmp("status", argv[1]))
    {
        char datetimestring[DATETIME_STRING_MAX_LENGTH];
        DateTime_t dateTime;
        SysTime_GetDateTime(&dateTime);
        DateTime_PrintDateTimeToString(datetimestring, &dateTime);
        CmdH_Printf("Date and Time: %s\r\n", datetimestring);

        result = CmdH_Result_Ok;
    }
    else
    {
        result = CmdH_Result_Error_WrongArgument1;
    }

    return result;
}
#endif



#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
static CmdH_Result_t CommandFunction_EventLog(uint32_t argc, char** argv)
{
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);

    /* List all event logs */
    EventLog_PrintLogTable();

    return CmdH_Result_Ok;
}
#endif



#ifdef CONFIG_MODULE_ADC_ENABLE
/**
 * @brief    Read ADC values
 */
static CmdH_Result_t CommandFunction_adc(uint32_t argc, char** argv)
{
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);

    /* Print ADC values */
    ADC_PrintAllValues();

    return CmdH_Result_Ok;
}
#endif



#ifdef CONFIG_MODULE_FLASH_ENABLE
/**
 * @brief    Flash erase
 *             Use: 'flashdel <address> <block/sector>'
 */
static CmdH_Result_t CommandFunction_flashdel(uint32_t argc, char** argv)
{
    uint32_t Arg2Num;

    /* Convert arg2 hex */
    if (!StringHexToNum(argv[1], &Arg2Num))
    {
        return CmdH_Result_Error_WrongArgument1;
    }


    if (!StrCmp("block", argv[2]))
    {
        FLASH_BlockErase(Arg2Num, 5000);
    }
    else if (!StrCmp("sector", argv[2]))
    {
        FLASH_SectorErase(Arg2Num, 5000);
    }
    else
    {
        return RETURN_FALSE;
    }

    CmdH_Printf("address erased: 0x%h\r\n",
            Arg2Num
            );

    return CmdH_Result_Ok;
}



/**
 * @brief    Flash read
 *             Use: 'flashread <address>'
 */
static CmdH_Result_t CommandFunction_flashread(uint32_t argc, char** argv)
{

    uint32_t Arg2Num;

    /* Convert arg2 hex */
    if (!StringHexToNum(argv[1], &Arg2Num))
    {
        return CmdH_Result_Error_WrongArgument1;
    }


    uint8_t Buffer[1];
    FLASH_Read(Arg2Num,Buffer, 1, 5000);


    CmdH_Printf("address: 0x%w\r\n"
            "data:    0x%b\r\n",
            Arg2Num,
            Buffer[0]
            );

    return CmdH_Result_Ok;
}



/**
 * @brief    Flash write
 *             Use: 'flashwrite <address> <data>'
 */
static CmdH_Result_t CommandFunction_flashwrite(uint32_t argc, char** argv)
{
    uint32_t Arg2Num;

    /* Convert arg2 hex */
    if (!StringHexToNum(argv[1], &Arg2Num))
    {
        return CmdH_Result_Error_WrongArgument1;
    }

    /* Convert arg3, decimal */
    if (!StringToUnsignedDecimalNum(argv[2], &Arg3Num))
    {
        return CmdH_Result_Error_WrongArgument2;
    }


    uint8_t Buffer[1];

    Buffer[0] = (uint8_t)Arg3Num;

    FLASH_Write(Arg2Num, Buffer, 1, 5000);


    CmdH_Printf("address: 0x%w\r\n"
            "data:    0x%b\r\n",
            Arg2Num,
            Arg3Num);

    return CmdH_Result_Ok;
}
#endif    /* #ifdef CONFIG_MODULE_FLASH_ENABLE */



#ifdef CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE
/**
 * @brief    HomeAutMessage command
 */
static CmdH_Result_t CommandFunction_raspberrypi(uint32_t argc, char** argv)
{
    UNUSED_ARGUMENT(argc);

    /* Check arg 2 */
    if (!StrCmp("setout", argv[1]))
    {
        /* setout */
        /*
        HomeAutMessage_CreateAndSendHomeAutMessage(
            0,255,
            Function_Command, Command_SetOutput,
            Arg3Num, 1);
        */

        /* Convert arg3, decimal */
        uint32_t value;
        if (!StringToUnsignedDecimalNum(argv[2], &value))
        {
            return CmdH_Result_Error_WrongArgument2;
        }

        if (value > 0)
        {
            LED_BLUE_ON();
            LED_GREEN_ON();
        }
        else
        {
            LED_BLUE_OFF();
            LED_GREEN_OFF();
        }

        return CmdH_Result_Ok;
    }
    else
    {
        /* Wrong command - not "setout" */
        return CmdH_Result_Error_WrongArgument1;
    }
}
#endif



#ifdef CONFIG_COMMANDLIST_RAMREAD_WRITE_COMMAND_ENABLE
/*
 * @brief    Function: mr (memory read)
 * mr <source> <size>
 * source: hex
 * size: dec
 * read <size> byte-s from <source> address
 */
static CmdH_Result_t CommandFunction_mr(uint32_t argc, char** argv)
{
    UNUSED_ARGUMENT(argc);

    uint32_t *source;
    int16_t size;
    uint16_t i;
    uint32_t Arg2Num;
    uint32_t Arg3Num;

    /* Convert arg2, source to hex */
    if (!StringHexToNum(argv[1], &Arg2Num))
    {
        return CmdH_Result_Error_WrongArgument1;
    }
    /* Convert arg3, size to dec */
    if (!StringToUnsignedDecimalNum(argv[2], &Arg3Num))
    {
        return CmdH_Result_Error_WrongArgument2;
    }

    /* casting for valid numbers */
    source = (uint32_t *)Arg2Num;
    size = (int16_t)Arg3Num;
    /* <size> max 256 */

    /* \Note:    Address not checked */

    CmdH_Printf("Source: 0x%X\r\n"
                            "Size: %d\r\n",
                            source, size);

    for (i = 0; i < size; i++)
    {
        if (!(i % 4))
        {
            /* 0., 4., ... */
            CmdH_Printf("\r\n 0x%X:", (uint32_t)&source[i]);
        }
        CmdH_Printf(" %02X", source[i]);
    }

    return CmdH_Result_Ok;
}



/*
 * @brief    FunctioN: mw - memory write
 * mwb <destination> <data>
 * destination: hex
 * data: hex
 * mwb, mwh, mww commands
 */
static CmdH_Result_t CommandFunction_mw(uint32_t argc, char** argv)
{
    UNUSED_ARGUMENT(argc);

    uint8_t *destination1;
    uint16_t *destination2;
    uint32_t *destination3;
    uint32_t Arg2Num;
    uint32_t Arg3Num;

    /* Convert hex */
    if (!StringHexToNum(argv[1], &Arg2Num))
    {
        return CmdH_Result_Error_WrongArgument1;
    }
    /* Convert hex */
    if (!StringHexToNum(argv[2], &Arg3Num))
    {
        return CmdH_Result_Error_WrongArgument2;
    }

    if (!StrCmp("mwb", argv[0]))
    {
        destination1 = (uint8_t *)Arg2Num;
        *destination1 = (uint8_t)Arg3Num;
        CmdH_Printf("Write: %02X to: %X", Arg3Num, Arg2Num);
    }
    else if (!StrCmp("mwh", argv[0]))
    {
        destination2 = (uint16_t *)Arg2Num;
        *destination2 = (uint16_t)Arg3Num;
        CmdH_Printf("Write: %04X to: %X", Arg3Num, Arg2Num);
    }
    else if (!StrCmp("mww", argv[0]))
    {
        destination3 = (uint32_t *)Arg2Num;
        *destination3 = (uint32_t)Arg3Num;
        CmdH_Printf("Write: %08X to: %X", Arg3Num, Arg2Num);
    }
    else
    {
        return CmdH_Result_Error_Unknown;
    }

    return CmdH_Result_Ok;
}
#endif



#ifdef CONFIG_COMMANDLIST_GO_COMMAND_ENABLE
/**
 * @brief    Function: go (jump to an address)
 * go <destination>
 * jump <destination> address
 */
static CmdH_Result_t CommandFunction_go(uint32_t argc, char** argv)
{
    UNUSED_ARGUMENT(argc);

    uint32_t destination;
    int (*fpntr)(void);
    uint32_t Arg2Num;


    /* Convert hex */
    if (!StringHexToNum(argv[1], &Arg2Num))
    {
        return CmdH_Result_Error_WrongArgument1;
    }

    destination = Arg2Num;
    CmdH_Printf("Go destination: 0x%X\r\n", destination);

    fpntr = (int (*)(void))destination;        /* casting */
    fpntr();

    /* Now, for example: "go 20000151" */

    /* We can use the "jump" ASM instruction, but not a good idea */
    /* Programming manual page 92, B instruction */

    return CmdH_Result_Ok;
}
#endif



#ifdef CONFIG_MODULE_DISPLAY_ENABLE
/**
 * @brief    Display module
 */
static CmdH_Result_t CommandFunction_Display(uint32_t argc, char** argv)
{
    UNUSED_ARGUMENT(argc);

    CmdH_Result_t result = CmdH_Result_Unknown;

    if (!StrCmp("write", argv[1]))
    {
        char *separated[2];
        if (STRING_Splitter(argv[2], ",", separated, 2))
        {
            /* Check line parameter */
            uint32_t line;
            if (StringToUnsignedDecimalNum(separated[0], &line))
            {
                /* Print line */
                Display_PrintString(separated[1], line, Font_8x5, NO_FORMAT);
                Display_Activate();
                result = CmdH_Result_Ok_SendSuccessful;
            }
            else
            {
                result = CmdH_Result_Error_WrongArgument2;
            }
        }
        else
        {
            result = CmdH_Result_Error_WrongArgument2;
        }
    }
    else if (!StrCmp("clear", argv[1]))
    {
        /* Clear */
        Display_Clear();
        Display_Activate();
        result = CmdH_Result_Ok_SendSuccessful;
    }
#ifdef CONFIG_MODULE_DISPLAY_TEST
    #ifdef CONFIG_DISPLAY_FONT8X5_ENABLE
    else if (!StrCmp("test1", argv[1]))
    {
        /* Test font - 8x5 */
        Display_Test8x5Font();
        result = CmdH_Result_Ok_SendSuccessful;
    }
    #endif
    #ifdef CONFIG_DISPLAY_FONT12X8_ENABLE
    else if (!StrCmp("test2", argv[1]))
    {
        /* Test font - 12x8 */
        Display_Test12x8Font();
        result = CmdH_Result_Ok_SendSuccessful;
    }
    #endif
    #ifdef CONFIG_DISPLAY_FONT32X20_ENABLE
    else if  (!StrCmp("test3", argv[1]))
    {
        /* Test font - 32x20 */
        Display_Test32x20Font();
        result = CmdH_Result_Ok_SendSuccessful;
    }
    #endif
#endif    /* #ifdef CONFIG_MODULE_DISPLAY_TEST */
#ifdef CONFIG_MODULE_DISPLAY_TEST_WITH_TERMINAL
    else if (!StrCmp("debugprint", argv[1]))
    {
        /* Debugprint Display */
        DisplayHandler_SendOnTerminal();
        result = CmdH_Result_Ok_SendSuccessful;
    }
#endif
    else
    {
        result = CmdH_Result_Error_WrongArgument1;
    }

    return result;
}
#endif



#ifdef CONFIG_MODULE_IO_ENABLE
/**
 * @brief    Get IO states
 */
static CmdH_Result_t CommandFunction_IO(uint32_t argc, char** argv)
{
    CmdH_Result_t result;

    if (argc == 1)
    {
#if IO_INPUTS_NUM > 0
        uint8_t i;

        CmdH_SendLine("Input states:");
        for (i = 0; i < IO_Input_Count; i++)
        {
            CmdH_Printf(" %20s %s\r\n",
                    IO_GetInputName(i),
                    IO_GetInputStateName(IO_GetInputState(i)));
        }
#endif

        /* TODO: Delete, if not need */
        /*
        CmdH_SendLine("Output states:");
        for (i = 0; i < IO_Output_Count; i++)
        {
            CmdH_Printf(" %20s %s\r\n",
                    IO_GetOutputName(i),
                    IO_GetOutputStateName(IO_Output_GetStatus(i)));
        }*/
        char outStateString[IO_OUPUT_STATES_STRING_MAX_LENGTH];
        IO_Output_PrintStates(outStateString);
        CmdH_SendLine(outStateString);

        result = CmdH_Result_Ok;
    }
    else if (argc > 1)
    {
        /**
         * @brief    Set LED (turn on, turn off, toggle, status)
         *             Use:
         *                led <num> on
         *                led <num> off
         *                led <num> toggle
         *                led <num> status
         *                led status
         */

        /* Convert arg2, decimal */
        bool isFirstParamNum = false;
        uint32_t Arg2Num;
        bool isGetStatus = false;

        /* Check 1. argument */
        if (StringToUnsignedDecimalNum(argv[1], &Arg2Num))
        {
            isFirstParamNum = true;
        }
        else if (!StrCmp("status", argv[1]))
        {
            isGetStatus = true;
        }
        else
        {
            /* Not number, check it is color? */
            uint8_t ledNum = IO_Output_GetOutputNumFromName(argv[1]);
            if (ledNum != 0)
            {
                Arg2Num = ledNum;
                isFirstParamNum = true;
            }
        }

        /* Continue */
        if (isGetStatus)
        {
            /* "status" */
            /* Print IOs statuses */
            char outStateString[IO_OUPUT_STATES_STRING_MAX_LENGTH];
            IO_Output_PrintStates(outStateString);
            CmdH_SendLine(outStateString);

            /* TODO: Print input statuses */
            result = CmdH_Result_Ok;
        }
        else if (isFirstParamNum == true)
        {
            /* 1. param = num (IO num) */

            /* Check parameters */
            if (Arg2Num >= IO_Output_Count)
            {
                /* First argument is wrong number */
                result = CmdH_Result_Error_WrongArgument1;
            }
            else
            {
                /* Good count */
                /* Get type "set type" */
                IO_Output_Cmd_t setType = IO_Output_GetTypeFromString(argv[2]);
                IO_Output_Name_t ioNum = Arg2Num;
                IO_Status_t status = false;

                if (setType == IO_Output_Cmd_DontCare)
                {
                    /* Error, do nothing */
                    result = CmdH_Result_Error_WrongArgument2;
                }
                else
                {
                    /* Set IO */
                    status = IO_Output_SetStatus(ioNum, setType);
                    CmdH_Printf("IO %d. (name: %s) status: %s\r\n", ioNum, IO_Output_GetName(ioNum), IO_GetStatusName(status));
                    result = CmdH_Result_Ok;
                }
            }
        }
        else
        {
            /* First param is not status and not LED num */
            result = CmdH_Result_Error_WrongArgument1;
        }
    }
    else
    {
        result = CmdH_Result_Error_Unknown;
    }

    return result;
}
#endif



#ifdef CONFIG_MODULE_SIMULATION_ENABLE
/**
 * @brief    Simulation
 */
static CmdH_Result_t CommandFunction_Simulation(uint32_t argc, char** argv)
{
    UNUSED_ARGUMENT(argc);
    CmdH_Result_t result = CmdH_Result_Unknown;
    static bool Simulation_IsEnabled = false;

    if (!Simulation_IsEnabled)
    {
        if (!StrCmp("enable", argv[1]) && argc == 2)
        {
            Simulation_IsEnabled = true;
            result = CmdH_Result_Ok_SendSuccessful;
        }
        else
        {
            /* Error */
            CmdH_SendLine("Simulation is not enabled! Type \"simulation enable\" command first!");
            result = CmdH_Result_Ok;    /* Not ok, but do not send other error message */
        }
    }
    else
    {
#ifdef CONFIG_MODULE_SELFTEST_ERRORS_ENABLE
        /* Enabled simulation */
        if (!StrCmp("infloop", argv[1]))
        {
            /* @note    Be careful!!!! */
            /* Infinite loop test for WatchDog test */
            while(1);
            result = CmdH_Result_Ok_SendSuccessful;
        }
        else if (!StrCmp("genfault", argv[1]))
        {
            if (!StrCmp("constwrite", argv[2]))
            {
                SelfTest_Errors_Constwrite();

                result = CmdH_Result_Ok_SendSuccessful;
            }
            else if (!StrCmp("zerodivide", argv[2]))
            {
                SelfTest_Errors_ZeroDivide();

                result = CmdH_Result_Ok_SendSuccessful;
            }
            else if (!StrCmp("failpointer", argv[2]))
            {
                SelfTest_Errors_MemFault();

                result = CmdH_Result_Ok_SendSuccessful;
            }
            else if (!StrCmp("stackoverflow", argv[2]))
            {
                SelfTest_Erros_StackOverFlow();

                result = CmdH_Result_Ok_SendSuccessful;
            }
            else
            {
                result = CmdH_Result_Error_WrongArgument2;
            }
        }
#endif /* CONFIG_MODULE_SELFTEST_ERRORS_ENABLE */
        /* TODO: Not too beautiful solution these defines */
#ifdef CONFIG_SWO_ENABLE
        if (!StrCmp("SWO", argv[1]))
        {
            /* Test SWO */
            /* OMMUNICATION_SendMessage(CommProt_SWO, "Test message on SWO\n"); */
            SWO_SendMessage("Test message on SWO\n");
        }
#endif
        /* TODO: Not too beautiful solution these defines */
#ifdef CONFIG_MODULE_IO_ENABLE
        if (!StrCmp("input", argv[1]))
        {
            uint32_t pin;
            if (StringToUnsignedDecimalNum(argv[2], &pin))
            {
                IO_SetInputState((Input_t)pin, InputState_Active);
                result = CmdH_Result_Ok_SendSuccessful;
            }
            else
            {
                result = CmdH_Result_Error_WrongArgument2;
            }
        }
        else if (!StrCmp("output", argv[1]))
        {
            uint32_t pin;
            if (StringToUnsignedDecimalNum(argv[2], &pin))
            {
                IO_SetOutputState((Output_t)pin, OutputState_Active);
                result = CmdH_Result_Ok_SendSuccessful;
            }
            else
            {
                result = CmdH_Result_Error_WrongArgument2;
            }
        }
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
        else if (!StrCmp("buttonpress", argv[1]))
        {
            /* Button press simulation */
            if (argc == 3)
            {
                bool isOk = true;
                if (!StrCmp("up", argv[2]))
                    Logic_ButtonEventHandler(PressedButton_Up, ButtonPress_Short);
                else if (!StrCmp("down", argv[2]))
                    Logic_ButtonEventHandler(PressedButton_Down, ButtonPress_Short);
                else if (!StrCmp("right", argv[2]))
                    Logic_ButtonEventHandler(PressedButton_Right, ButtonPress_Short);
                else if (!StrCmp("left", argv[2]))
                    Logic_ButtonEventHandler(PressedButton_Left, ButtonPress_Short);
                else
                    isOk = false;

                if (isOk)
                    result = CmdH_Result_Ok_SendSuccessful;
                else
                    result = CmdH_Result_Error_WrongArgument2;
            }
            else
            {
                result = CmdH_Result_Error_TooFewArgument;
            }
        }
#endif
#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE
        else if (!StrCmp("event", argv[1]))
        {
            /* Generate an event */
            if (StringIsUnsignedDecimalString(argv[2]))
            {
                /* Event "number" */
                uint32_t value;
                if (StringToUnsignedDecimalNum(argv[2], &value))
                {
                    EventHandler_GenerateEvent(value, 0, 0);
                    result = CmdH_Result_Ok_SendSuccessful;
                }
                else
                {
                    result = CmdH_Result_Error_WrongArgument2;
                }
            }
            else
            {
                /* Not number, search string in event list */
                EventId_t i;
                bool isOk = false;

                for (i = 0; i < EventsNum; i++)
                {
                    if (!StrCmp(EventList[i].name, argv[2]))
                    {
                        /* Equal */
                        EventHandler_GenerateEvent(i, 0, 0);
                        isOk = true;
                        break;
                    }
                }

                if (isOk)
                    result = CmdH_Result_Ok_SendSuccessful;
                else
                    result = CmdH_Result_Error_WrongArgument2;
            }
        }
#endif
        else
        {
            result = CmdH_Result_Error_WrongArgument1;
        }
    }

    return result;
}
#endif



#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
/**
 * @brief    TaksHandler functions
 */
static CmdH_Result_t CommandFunction_TaskHandler(uint32_t argc, char** argv)
{
    UNUSED_ARGUMENT(argc);
    CmdH_Result_t result = CmdH_Result_Unknown;

    #ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
    if (!StrCmp("statistics", argv[1]))
    {
        /* TaskHandler statistics */
        TaskHandler_PrintStatistics();

        result = CmdH_Result_Ok;
    }
    else if (!StrCmp("runcounts", argv[1]))
    {
        /* TaskHandler - Run counts */
        TaskHandler_PrintTaskRunCounts();

        result = CmdH_Result_Ok;
    }
    else
    {
        result = CmdH_Result_Error_WrongArgument1;
    }
    #else
    UNUSED_ARGUMENT(argv);
    #endif

    return result;
}
#endif



#ifdef CONFIG_MODULE_DEBUG_ENABLE
static CmdH_Result_t CommandFunction_Debug(uint32_t argc, char** argv)
{
    CmdH_Result_t result = CmdH_Result_Error_Unknown;

    if (argc == 1)
    {
        /* Debug list */
        Debug_PrintDebugList();
        result = CmdH_Result_Ok;
    }
    else if (argc == 3)
    {
        uint32_t value;
        bool enable = false;
        bool isOk = false;

        if (!StrCmp("enable", argv[2]))
        {
            /* Enable */
            enable = true;
            isOk = true;
        }
        else if (!StrCmp("disable", argv[2]))
        {
            /* Disable */
            enable = false;
            isOk = true;
        }
        else
        {
            /* Wrong 2. parameter */
            isOk = false;
            result = CmdH_Result_Error_WrongArgument2;
        }

        if (isOk)
        {
            /* Check 1. parameter */
            if (StringToUnsignedDecimalNum(argv[1], &value))
            {
                /* 1. parameter is number */
                if (Debug_SetEnable((Debug_t)value, enable))
                {
                    /* Successful */
                    result = CmdH_Result_Ok_SendSuccessful;
                }
                else
                {
                    result = CmdH_Result_Error_WrongArgument1;
                }
            }
            else
            {
                /* Find in "list" */
                if (Debug_SetDebugTaskWithName(argv[1], enable))
                {
                    result = CmdH_Result_Ok_SendSuccessful;
                }
                else
                {
                    result = CmdH_Result_Error_WrongArgument1;
                }
            }
        }
        /* lse: Handled in "enable" / "disable" case */
    }

    return result;
}
#endif



#ifdef CONFIG_MODULE_BUTTON_ENABLE
static CmdH_Result_t CommandFunction_Button(uint32_t argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);

#if BUTTON_NUM > 1
    uint8_t i;

    /* Print all button state */
    for (i = 0; i < BUTTON_NUM; i++)
    {
        bool buttonState = BUTTON_GetButtonState(i);
        const char * buttonName = BUTTON_GetButtonName(i);

        CmdH_Printf("Button: %s is %s\r\n", buttonName, buttonState ? "pressed" : "released");
    }
#else
    /* Print one button state */
    bool buttonState = BUTTON_GetButtonState(0);
    const char * buttonName = BUTTON_GetButtonName(0);
    CmdH_Printf("Button: %s is %s\r\n", buttonName, buttonState ? "pressed" : "released");
#endif

    return CmdH_Result_Ok;
}
#endif



#ifdef CONFIG_FUNCTION_PERIODICAL_SENDING
static CmdH_Result_t CommandFunction_PeriodicalSending(uint32_t argc, char** argv)
{
    CmdH_Result_t result = CmdH_Result_Unknown;

    /* TODO: Or GlobalVarHandler? enable flag, period, message */
    Logic_SetPeriodicalMessageSendg(argv[1]);

    if (argc == 3)
    {
        /* Has period parameter */
        uint32_t value;
        if (StringToUnsignedDecimalNum(argv[2], &value))
        {
            TaskHandler_SetTaskPeriodicTime(Task_PerodicalSending, value);
            result = CmdH_Result_Ok_SendSuccessful;
        }
        else
        {
            result = CmdH_Result_Error_WrongArgument2;
        }
    }
    else
    {
        TaskHandler_RequestTaskScheduling(Task_PerodicalSending);
        result = CmdH_Result_Ok_SendSuccessful;
    }

    return result;
}
#endif



#if (CIRCULARBUFFER_STATISTICS_ENABLE == 1)
static CmdH_Result_t CommandFunction_CircBuffStat(uint32_t argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);

    char resp[200];

    CircularBuffer_PrintStatistics(resp, 200, DebugUart.tx);
    DebugUart_SendMessage(resp);

    DelayMs(10);

    CircularBuffer_PrintStatistics(resp, 200, DebugUart.rx);
    DebugUart_SendMessage(resp);

    return CmdH_Result_Ok;
}
#endif



#ifdef CONFIG_MODULE_COMMON_UART_ENABLE
static CmdH_Result_t CommandFunction_CommonUART(uint32_t argc, char** argv)
{
    UNUSED(argc);

    CommonUART_SendMessage(argv[1]);

    return CmdH_Result_Ok_SendSuccessful;
}
#endif



#ifdef CONFIG_MODULE_BUTTONSIMULATOR_ENABLE
static CmdH_Result_t CommandFunction_ButtonSimulator(uint32_t argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);

    CmdH_SendLine("Started Button simulating. Press the cursors!");

    ButtonSimulator_Set(true);

    return CmdH_Result_Ok_SendSuccessful;
}
#endif



/* END OF COMMAND FUNCTIONS */
