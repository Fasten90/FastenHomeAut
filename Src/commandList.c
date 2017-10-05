/*
 *		CommandList.c
 *		Created on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		General commands's list
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "include.h"

#include "String.h"
#include "FormattedMessage.h"
#include "CommandHandler.h"
#include "GlobalVarHandler.h"
#include "Globals.h"
#include "Terminal.h"
#include "Button.h"
#include "DateTime.h"
#include "Display.h"
#include "Display_SSD1306.h"
#include "LED.h"
#include "Watchdog.h"
#include "Calc.h"
#include "CircularBuffer.h"
#include "ESP8266.h"
#include "EventLog.h"
#include "EventHandler.h"
#include "HomeAutMessage.h"
#include "TaskHandler.h"
#include "Motor.h"
#include "CommonIO.h"
#include "MEM.h"
#include "TaskList.h"
#include "IO.h"
#include "LinkedList.h"
#include "CommonAdc.h"
#include "ADC.h"
#include "CommonDac.h"
#include "Debug.h"
#include "DebugList.h"
#include "Logic.h"
#include "ConvertTable.h"

#include "CommandList.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *		Global function declarations - Commands
 *----------------------------------------------------------------------------*/

	static CommandResult_t CommandFunction_cls(uint32_t argc, char** argv);
	static CommandResult_t CommandFunction_help(uint32_t argc, char** argv);
	static CommandResult_t CommandFunction_version(uint32_t argc, char** argv);
	static CommandResult_t CommandFunction_welcome(uint32_t argc, char** argv);
	static CommandResult_t CommandFunction_reset(uint32_t argc, char** argv);
#ifdef CONFIG_MODULE_UNITTEST_ENABLE
	static CommandResult_t CommandFunction_unittest(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_TEST_ENABLE
	static CommandResult_t CommandFunction_moduletest(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_TEST_MODE
	static CommandResult_t CommandFunction_test(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_LED_ENABLE
	static CommandResult_t CommandFunction_led(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
	static CommandResult_t CommandFunction_set(uint32_t argc, char** argv);
	static CommandResult_t CommandFunction_get(uint32_t argc, char** argv);
	static CommandResult_t CommandFunction_GlobalVariableHelp(uint32_t argc, char** argv);
	static CommandResult_t CommandFunction_GlobalVariableList(uint32_t argc, char** argv);
	static CommandResult_t CommandFunction_GlobalVariableValueList(uint32_t argc, char** argv);
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
	static CommandResult_t CommandFunction_GlobalVariableTrace(uint32_t argc, char** argv);
#endif
#endif
#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE
	static CommandResult_t CommandFunction_dac(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_COMMON_IO_ENABLE
	static CommandResult_t CommandFunction_io(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
	static CommandResult_t CommandFunction_adc(uint32_t argc, char** argv);
	static CommandResult_t CommandFunction_adcread(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE
	static CommandResult_t CommandFunction_PWM(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_MOTOR_ENABLE
	static CommandResult_t CommandFunction_Motor(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	static CommandResult_t CommandFunction_ESP8266(uint32_t argc, char** argv);
#endif
#if defined(CONFIG_MODULE_RTC_ENABLE) || defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE)
	static CommandResult_t CommandFunction_Time(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
	static CommandResult_t CommandFunction_EventLog(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE
	static CommandResult_t CommandFunction_raspberrypi(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_FLASH_ENABLE
	static CommandResult_t CommandFunction_flashdel(uint32_t argc, char** argv);
	static CommandResult_t CommandFunction_flashread(uint32_t argc, char** argv);
	static CommandResult_t CommandFunction_flashwrite(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_ADC_ENABLE
	static CommandResult_t CommandFunction_adc(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_DEBUG_RAMREAD_WRITE_COMMAND_ENABLE
	static CommandResult_t CommandFunction_mr(uint32_t argc, char** argv);
	static CommandResult_t CommandFunction_mw(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_DEBUG_GO_COMMAND_ENABLE
	static CommandResult_t CommandFunction_go(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
	static CommandResult_t CommandFunction_Display(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_IO_ENABLE
	static CommandResult_t CommandFunction_IoStates(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_SIMULATION_ENABLE
	static CommandResult_t CommandFunction_Simulation(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
	static CommandResult_t CommandFunction_TaskHandler(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_DEBUG_ENABLE
	static CommandResult_t CommandFunction_Debug(uint32_t argc, char** argv);
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
	static CommandResult_t CommandFunction_Button(uint32_t argc, char** argv);
#endif



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



///< Commands list
const CommandStruct CommandList[] =
{
	/*
	 * Help:
	 * 	.name = "StringOfCommand",
	 * 	.commandFunctionPointer = function name,
	 * 	.commandArgNum = CommandArgument_0 | CommandArgument_1 | CommandArgument_2,
	 * 	.description = "Command description",
	 * 	.syntax = "<required parameter> (optional parameter)"
	 */
	{
		.name = "help",
		.commandFunctionPointer = CommandFunction_help,
		.commandArgNum = CommandArgument_0 | CommandArgument_1,
		.description = "Commands's list, or print a command's description",
		.syntax = "(command)",
		.example = "version"
	},
	{
		.name = "version",
		.commandFunctionPointer = CommandFunction_version,
		.commandArgNum = CommandArgument_0,
		.description = "Get version",
		.syntax = NULL,
	},
	{
		.name = "welcome",
		.commandFunctionPointer = CommandFunction_welcome,
		.commandArgNum = CommandArgument_0,
		.description = "Send welcome message",
		.syntax = NULL,
	},
	{
		.name = "reset",
		.commandFunctionPointer = CommandFunction_reset,
		.commandArgNum = CommandArgument_0,
		.description = "Software reset",
		.syntax = NULL,
	},
	{
		.name = "cls",
		.commandFunctionPointer = CommandFunction_cls,
		.commandArgNum = CommandArgument_0,
		.description = "Clear Screen",
		.syntax = NULL,
	},
#ifdef CONFIG_TEST_MODE
	{
		.name = "test",
		.commandFunctionPointer = CommandFunction_test,
		.commandArgNum = CommandArgument_0,
		.description = NULL,
		.syntax = NULL,
	},
#endif
#ifdef CONFIG_MODULE_TEST_ENABLE
	{
		.name = "moduletest",
		.commandFunctionPointer = CommandFunction_moduletest,
		.commandArgNum = CommandArgument_0,
		.description = "Run module test",
	},
#endif
#ifdef CONFIG_MODULE_UNITTEST_ENABLE
	{
		.name = "unittest",
		.commandFunctionPointer = CommandFunction_unittest,
		.commandArgNum = CommandArgument_0 | CommandArgument_1,
		.description = "Run unit tests",
		.syntax = "(modul)",
	},
#endif
#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
	{
		.name = "get",
		.commandFunctionPointer = CommandFunction_get,
		.description = "get global variable value",
		.commandArgNum = CommandArgument_1,
		.syntax = "<varName>",
		.example = "devicename",
	},
	{
		.name = "set",
		.commandFunctionPointer = CommandFunction_set,
		.commandArgNum = CommandArgument_2,
		.description = "set global variable value",
		.syntax = "<varName> <value>",
		.example = "devicename MyDeviceName",
	},
	{
		.name = "?",
		.commandFunctionPointer = CommandFunction_GlobalVariableHelp,
		.description = "help global variable",
		.commandArgNum = CommandArgument_1,
		.syntax = "<varName>",
		.example = "devicename",
	},
	{
		.name = "list",
		.commandFunctionPointer = CommandFunction_GlobalVariableList,
		.commandArgNum = CommandArgument_0,
		.description = "List global variable",
	},
	{
		.name = "listvalues",
		.commandFunctionPointer = CommandFunction_GlobalVariableValueList,
		.commandArgNum = CommandArgument_0,
		.description = "List global variable's values",
	},
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
	{
		.name = "trace",
		.commandFunctionPointer = CommandFunction_GlobalVariableTrace,
		.commandArgNum = CommandArgument_2,
		.description = "Trace global variable's values",
		.syntax = "<varname> <enable/disable>; period <period:ms>",
		.example = "tick enable"
	},
#endif
#endif	// #ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
#ifdef CONFIG_MODULE_LED_ENABLE
	{
		.name = "led",
		.commandFunctionPointer = CommandFunction_led,
		.commandArgNum = CommandArgument_1 | CommandArgument_2,
		.description = "Control LED",
		.syntax = "<num> <on/off/toggle/status>",
		.example = "1 on",
	},
#endif
#ifdef CONFIG_MODULE_COMMON_IO_ENABLE
	{
		.name = "ioinit",
		.commandFunctionPointer = CommandFunction_io,
		.commandArgNum = CommandArgument_2,
		.description = "Init IO pins",
		.syntax = "<port><pin> <input/output>",
		.example = "a1 input",
	},
	{
		.name = "ioout",
		.commandFunctionPointer = CommandFunction_io,
		.commandArgNum = CommandArgument_2,
		.description = "Set output",
		.syntax = "<port><pin> <set/reset>",
		.example = "a2 set",
	},
	{
		.name = "ioin",
		.commandFunctionPointer = CommandFunction_io,
		.commandArgNum = CommandArgument_1,
		.description = "Get input",
		.syntax = "<port><pin>",
		.example = "a1",
	},
#endif
#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE
	{
		.name = "dac",
		.commandFunctionPointer = CommandFunction_dac,
		.commandArgNum = CommandArgument_2,
		.description = "Digital -> Analog",
		.syntax = "<num> <voltage>",
		.example = "1 3.0"
	},
#endif
#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
	{
		.name = "adc",
		.commandFunctionPointer = CommandFunction_adc,
		.description = "Analog -> Digital",
		.commandArgNum = CommandArgument_0,
	},
	{
		.name = "adcread",
		.commandFunctionPointer = CommandFunction_adcread,
		.description = "ADC read continuous",
		.syntax = "<milliSec> (num)",
		.commandArgNum = CommandArgument_1 | CommandArgument_2,
		.example = "1000 1",
	},
#endif
#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE
	{
		.name = "pwm",
		.commandFunctionPointer = CommandFunction_PWM,
		.commandArgNum = CommandArgument_1,
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
		.commandArgNum = CommandArgument_1 | CommandArgument_2,
	},
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	{
		.name = "esp8266",
		.commandFunctionPointer = CommandFunction_ESP8266,
		.description = "Use ESP8266 module",
		.syntax = "<send> <message>",
		.commandArgNum = CommandArgument_1 | CommandArgument_2,
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
		.syntax = "<setdate/settime> <date: 20yy-mm-dd / time: hh:mm:ss>",
		.commandArgNum = CommandArgument_1 | CommandArgument_2,
		.example = "setdate 2017-02-08",
	},
#endif
#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
	{
		.name = "eventlog",
		.commandFunctionPointer = CommandFunction_EventLog,
		.description = "List EventLog",
		.syntax = NULL,
		.commandArgNum = CommandArgument_0,
		.example = NULL,
	},
#endif
#ifdef CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE
	{
		.name = "#homeaut",
		.description = "HomeAutMessage sending",
		.commandFunctionPointer = CommandFunction_raspberrypi,
		.commandArgNum = CommandArgument_2,
		.syntax = NULL,
	},
#endif
#ifdef CONFIG_MODULE_ADC_ENABLE
	{
		.name = "adc",
		.commandFunctionPointer = CommandFunction_adc,
		.description = "...",
		.syntax = NULL,
		.commandArgNum = CommandArgument_0,
	},
#endif
#ifdef CONFIG_MODULE_FLASH_ENABLE
	{
		.name = "flashdel",
		.commandFunctionPointer = CommandFunction_flashdel,
		.description = "Delete flash",
		.syntax = "<address> <block/sector>",
		.commandArgNum = CommandArgument_2,
	},
	{
		.name = "flashread",
		.commandFunctionPointer = CommandFunction_flashread,
		.description = "Read from flash",
		.syntax = "<address>",
		.commandArgNum = CommandArgument_1,
	},
	{
		.name = "flashwrite",
		.commandFunctionPointer = CommandFunction_flashwrite,
		.description = "Write flash",
		.syntax = "<address> <byte>",
		.commandArgNum = CommandArgument_2,
	},
#endif
#ifdef CONFIG_DEBUG_RAMREAD_WRITE_COMMAND_ENABLE
	{
		.name = "mr",
		.commandFunctionPointer = CommandFunction_mr,
		.description = "Memory read",
		.syntax = "<address> <size>",
		.commandArgNum = CommandArgument_2,
	},
	{
		.name = "mwb",
		.commandFunctionPointer = CommandFunction_mw,
		.description = "Memory write",
		.syntax = "<address> <byte>",
		.commandArgNum = CommandArgument_2,
	},
	{
		.name = "mwh",
		.commandFunctionPointer = CommandFunction_mw,
		.description = "Memory write",
		.syntax = "<address> <byte>",
		.commandArgNum = CommandArgument_2,
	},
	{
		.name = "mww",
		.commandFunctionPointer = CommandFunction_mw,
		.description = "Memory write",
		.syntax = "<address> <byte>",
		.commandArgNum = CommandArgument_2,
	},
#endif
#ifdef CONFIG_DEBUG_GO_COMMAND_ENABLE
	{
		.name = "go",
		.commandFunctionPointer = CommandFunction_go,
		.description = "Jump to address",
		.syntax = "<address>",
		.commandArgNum = CommandArgument_1,
	},
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
	{
		.name = "display",
		.commandFunctionPointer = CommandFunction_Display,
		.description = "Use display",
		.syntax = "<write/clear> (line,msg)",
		.example = "write 1,message",
		.commandArgNum = CommandArgument_1 | CommandArgument_2,
	},
#endif
#ifdef CONFIG_MODULE_IO_ENABLE
	{
		.name = "iostates",
		.commandFunctionPointer = CommandFunction_IoStates,
		.description = "Get input-output states",
		.syntax = "",
		.example = "",
		.commandArgNum = CommandArgument_0,
	},
#endif
#ifdef CONFIG_MODULE_SIMULATION_ENABLE
	{
		.name = "simulate",
		.commandFunctionPointer = CommandFunction_Simulation,
		.description = "simulate event, errors, etc.",
		.syntax = "",
		.example = "",
		.commandArgNum = CommandArgument_1 | CommandArgument_2,
	},
#endif
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
	{
		.name = "taskhandler",
		.commandFunctionPointer = CommandFunction_TaskHandler,
		.description = "taskhandler functions",
		.syntax = "<statistics/runcounts>",
		.example = "",
		.commandArgNum = CommandArgument_1,
	},
#endif
#ifdef CONFIG_MODULE_DEBUG_ENABLE
	{
		.name = "debug",
		.commandFunctionPointer = CommandFunction_Debug,
		.description = "Debug enable/disable",
		.syntax = "<taskname/taskid> <enable/disable>",
		.example = "",
		.commandArgNum = CommandArgument_2,
	},
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
	{
		.name = "button",
		.commandFunctionPointer = CommandFunction_Button,
		.description = "Button state",
		.syntax = "",
		.example = "",
		.commandArgNum = CommandArgument_0,
	},
#endif

	/*
	 * XXX: Add new commands here
	 *
	 * Help:
	 * 	.name = "StringOfCommand",
	 * 	.commandFunctionPointer = function name,
	 * 	.commandArgNum = CommandArgument_0 | CommandArgument_1 | CommandArgument_2,
	 * 	.description = "Command description",
	 * 	.syntax = "<required parameter> (optional parameter)"
	 */

};


const CommandID_t CommandHandler_CommandNum = NUM_OF(CommandList);



/*------------------------------------------------------------------------------
 *								Command functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	CLS: Clear screen
 */
static CommandResult_t CommandFunction_cls(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

#ifdef CONFIG_MODULE_TERMINAL_ENABLE
	Terminal_SendCls();
#endif

	return CommandResult_Ok;
}



/**
 * \brief	Get version
 */
static CommandResult_t CommandFunction_version(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

	CommandHandler_SendLine(Global_Version);

	return CommandResult_Ok;
}



/**
 * \brief	Send Welcome message
 */
static CommandResult_t CommandFunction_welcome(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

#ifdef CONFIG_MODULE_TERMINAL_ENABLE
	Terminal_SendWelcome();
#endif

	return CommandResult_Ok;
}



/**
 * \brief	Help command
 * 			Use: 'help', or 'help <CommandName>'
 * 			List commands or write the command's description
 */
static CommandResult_t CommandFunction_help(uint32_t argc, char** argv)
{

	if (argc == 1)
	{
		// if Arg2 is empty, listing all commands
		CommandHandler_PrintAllCommands();
	}
	else if (argc == 2)
	{
		// Has Arg2, find the command & print help
		CommandHandler_SearchCommandAndPrintHelp(argv[1]);
	}

	return CommandResult_Ok;
}



/**
 * \brief	Reset command
 */
static CommandResult_t CommandFunction_reset(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

	uint16_t i;

	CommandHandler_SendLine("Reset...");

	// Wait
	for (i = 0; i < 1000; i++);

	// Reset
	NVIC_SystemReset();

	// Wait
	for (i = 0; i < 1000; i++);

	return CommandResult_Ok;
}



#ifdef CONFIG_MODULE_UNITTEST_ENABLE
/**
 * \brief	Run module's unit tests
 */
static CommandResult_t CommandFunction_unittest(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

	CommandHandler_SendLine("Start unittest");

#ifdef MODULE_STRING_UNITTEST_ENABLE
	STRING_UnitTest();
#endif

#ifdef MODULE_DATETIME_UNITTEST_ENABLE
	DateTime_UnitTest();
#endif

#ifdef MODULE_HOMEAUTMESSAGE_UNITTEST_ENABLE
	HomeAutMessage_UnitTest();
#endif

#ifdef MODULE_COMMANDHANDLER_UNITTEST_ENABLE
	CommandHandler_UnitTest();
#endif

#ifdef MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE
	GlobalVarHandler_UnitTest();
#endif

#ifdef MODULE_CIRCULARBUFFER_UNITTEST_ENABLE
	CircularBuffer_UnitTest();
#endif

#ifdef MODULE_TASKHANDLER_UNNITEST_ENABLE
	TaskHandler_UnitTest();
#endif

#ifdef MODULE_EVENTLOG_UNITTEST_ENABLE
	EventLog_UnitTest();
#endif

#ifdef MODULE_CALC_UNITTEST_ENABLE
	Calc_UnitTest();
#endif

#ifdef MODULE_MEM_UNITTEST_ENABLE
	MEM_UnitTest();
#endif

#ifdef MODULE_LINKEDLIST_UNITTEST_ENABLE
	LinkedList_UnitTest();
#endif

#ifdef MODULE_EVENTHANDLER_UNITTEST_ENABLE
	EventHandler_UnitTest();
#endif

#ifdef MODULE_CONVERTTABLE_UNITTEST_ENABLE
	ConvertTable_UnitTest();
#endif

#ifdef MODULE_SECUREDDATATYPES_UNITTEST_ENABLE
	SecuredDataTypes_UnitTest();
#endif

	return CommandResult_Ok;
}
#endif	// #ifdef CONFIG_MODULE_UNITTEST_ENABLE



#ifdef CONFIG_MODULE_TEST_ENABLE
/**
 * \brief	Run module test
 */
static CommandResult_t CommandFunction_moduletest(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;


#ifdef CONFIG_MODULE_LED_ENABLE
	uint8_t i;

	// LED test
	CommandHandler_SendLine("LED test");

	// LEDs on
	for (i = LED_NUM_MIN; i <= LED_NUM_MAX; i++)
	{
		LED_SetLed(i, LED_Set_On);
		DelayMs(500);
	}

	Watchdog_Clear();

	// LEDs off
	for (i = LED_NUM_MIN; i <= LED_NUM_MAX; i++)
	{
		LED_SetLed(i, LED_Set_Off);
		DelayMs(500);
	}
#endif	// #ifdef CONFIG_MODULE_LED_ENABLE


#ifdef CONFIG_MODULE_BUTTON_ENABLE
	// Clear flag
	BUTTON_Clicked = 0;

	CommandHandler_SendLine("\r\nButton test: Please press button!");

	// TODO: Implement with Event...
	while (!BUTTON_Clicked)
	{
		CommandHandler_SendChar('.');
		DelayMs(500);
		Watchdog_Clear();
	}

	CommandHandler_SendLine("Button pressed");
#endif


	// Beep in terminal
	CommandHandler_SendLine("Beep test");
	CommandHandler_SendChar(TERMINAL_KEY_BELL);


	// Send formatted messages
#ifdef CONFIG_MODULE_FORMATTEDMESSAGE_ENABLE
	CommandHandler_SendLine("Formatted message test");
	FormattedMessage_Test();
#endif

	return CommandResult_Ok;
}
#endif	// #ifdef CONFIG_MODULE_TEST_ENABLE



#ifdef CONFIG_TEST_MODE

/**
 * \brief	Test function
 */
static CommandResult_t CommandFunction_test(uint32_t argc, char** argv)
{

	// Suppress warning
	(void)argc;
	(void)argv;

	CommandHandler_SendLine("Test start");

	/*
	 * 		Test codes
	 */


	// Test variadic macros
	// Warning: "ISO C does not permit named variadic macros [-Wvariadic-macros]
	// in Atollic TrueSTUDIO, 7.1.1
/*
 * Example:
 * #define eprintf(...) fprintf (stderr, __VA_ARGS__)
 * #define eprintf(args...) fprintf (stderr, args)
 */

	//#define eprintf(args...) uprintf(args)
	/*
	#define eprintf(...) 			uprintf(__VA_ARGS__)

		eprintf("Test variadic %d %s\r\n", 20, "testtext");

	*/


	// Test code size with init value

	/*
	uint32_t testValue = 0;

	if (HAL_GetTick())
	{
		testValue = HAL_GetTick();
		uprintf("GetTick value test: %u", testValue);
	}

	*/



	/*
	// RTC - Print DateTime in one line with refreshing

	DateTime_t actualDateTime = { { 17, 5, 7 }, { 19, 45, 0 } };

	// Clear button state
	BUTTON_Clicked = 0;
	// Until pressed button...
	while (!BUTTON_Clicked)
	{
		// Wait 1 second
		DelayMs(1000);

		// Step 1 second
		DateTime_Step(&actualDateTime);

		// Send actual DateTime
		char message[80];
		DateTime_PrintDateTimeToString(message, &actualDateTime);
		CommandHandler_SendMessage(ESCAPE_DELETELINE);
		// Cursor to line start
		CommandHandler_SendMessage(ESCAPE_CURSOR_TO_LINESTART);
		//CommandHandler_SendMessage(ESCAPE_CURSORLEFTLOTOF);

		CommandHandler_SendMessage(message);
	}
	*/



	/*
	uint8_t i;
	char c;

	for (i = 0; i <= 100; i++)
	{
		DelayMs(500);

		// Clear line
		CommandHandler_SendMessage(ESCAPE_DELETELINE);
		CommandHandler_SendMessage(ESCAPE_CURSOR_TO_LINESTART);

		// Send "/ 0-100%" - loading line
		switch(i%4)
		{
			case 0:		c = '|'; 	break;
			case 1:		c = '/';	break;
			case 2: 	c = '-';	break;
			case 3: 	c = '\\';	break;
			default: 	c = '?';	break;
		}

		CommandHandler_Printf("%c %3d%%", c, i);
	}
	*/



	/*
	// Print DateTime test

	DateTime_t actualDateTime = { { 17, 5, 7 }, { 21, 38, 0 } };

	// Clear button state
	BUTTON_Clicked = 0;
	while (!BUTTON_Clicked)
	{
		// Step 1 second
		DelayMs(1000);
		DateTime_Step(&actualDateTime);

		// Send actual DateTime
		char message[80];
		DateTime_PrintDateTimeToString(message, &actualDateTime);

		Display_PrintString(message, 1, Font_8x5);

		Display_SetClock(&actualDateTime.time);

		// Should display, because this test is blocked mode
		Display_Activate();
	}
	*/



	// Stack test
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


	// print() test
	//printf("Example");


	// Debug test
	/*
	#include "Debug.h"
	Debug_Print(Debug_ESP8266, "Formatted %s", "text");

	Debug_Print(Debug_New, "Number: %d,%d,%d", 0, 1, 2);

	Debug_EnableDisable(Debug_ESP8266, false);

	Debug_EnableDisable(Debug_New, true);

	Debug_Print(Debug_ESP8266, "Formatted %s", "text");

	Debug_Print(Debug_New, "Number: %d,%d,%d", 0, 1, 2);
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


	/*
	// Snake
#include "Snake.h"
	Snake_Init();
	 */


	// The 2 solution are equal in code size
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



	/**
	 * 		End of Test codes
	 */


	CommandHandler_SendLine("\r\n"
							"Test end");

	return CommandResult_Ok;
}
#endif	// #ifdef CONFIG_TEST_MODE



#ifdef CONFIG_MODULE_LED_ENABLE
/**
 * \brief	Set LED (turn on, turn off, toggle, status)
 * 			Use:
 *				led <num> on
 *				led <num> off
 *				led <num> toggle
 *				led <num> status
 *				led status
 */
static CommandResult_t CommandFunction_led(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;

	CommandResult_t result;

	// Convert arg2, decimal
	bool isFirstParamNum = false;
	uint32_t Arg2Num;
	bool isGetStatus = false;

	// Check 1. argument
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
		// Not number, check it is color?
		uint8_t ledNum = LED_GetNumFromName(argv[1]);
		if (ledNum != 0)
		{
			Arg2Num = ledNum;
			isFirstParamNum = true;
		}
	}

	// Continue
	if (isGetStatus)
	{
		// "status"
		// Print LEDs statuses
		char str[25];
		LED_GetLedStates(str);
		CommandHandler_SendLine(str);
		result = CommandResult_Ok;
	}
	else if (isFirstParamNum == true)
	{
		// 1. param = num (LED num)

		// Check parameters
		if (Arg2Num > LED_NUM_MAX || Arg2Num < LED_NUM_MIN)
		{
			// First argument is wrong number
			result = CommandResult_Error_WrongArgument1;
		}
		else
		{
			// Good count
			// Get type "set type"
			LED_SetType_t setType = LED_GetTypeFromString(argv[2]);
			bool status = false;

			if (setType == LED_Set_DontCare)
			{
				// Error, do nothing
				result = CommandResult_Error_WrongArgument2;
			}
			else
			{
				// Set LED
				status = LED_SetLed(Arg2Num,setType);
				CommandHandler_Printf("LED %d. status: %d\r\n", Arg2Num, status);
				result = CommandResult_Ok;
			}
		}
	}
	else
	{
		// First param is not status and not LED num
		result = CommandResult_Error_WrongArgument1;
	}

	return result;
}
#endif



#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
/**
 * \brief	Get globalvar value
 * 			Use: 'get <globalvarname>'
 */
static CommandResult_t CommandFunction_get(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;

	// Process
	GlobalVarHandler_ProcessCommand(
			argv[1], argv[2],
			SetGet_Get, CommandHandler_CommandSource);


	return CommandResult_Ok;
}



/**
 * \brief	set global variable
 * 			Use: 'set <globalvariablename> <value>'
 */
static CommandResult_t CommandFunction_set(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;

	// Process
	GlobalVarHandler_ProcessCommand(
			argv[1], argv[2],
			SetGet_Set, CommandHandler_CommandSource);


	return CommandResult_Ok;
}



/**
 * \brief	Global variable help
 * 			Use: '? <globalvar>'
 */
static CommandResult_t CommandFunction_GlobalVariableHelp(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;

	// Process
	GlobalVarHandler_ProcessCommand(
			argv[1], argv[2],
			SetGet_Help, CommandHandler_CommandSource);


	return CommandResult_Ok;
}



/**
 * \brief	List global variables
 */
static CommandResult_t CommandFunction_GlobalVariableList(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

	GlobalVarHandler_ListAllVariableParameters();

	return CommandResult_Ok;
}



/**
 * \brief	Print all global variable values
 */
static CommandResult_t CommandFunction_GlobalVariableValueList(uint32_t argc,
		char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

	GlobalVarHandler_PrintAllVariableValues();

	return CommandResult_Ok;
}



#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
/**
 * \brief	Enable/Disable trace
 */
static CommandResult_t CommandFunction_GlobalVariableTrace(uint32_t argc,
		char** argv)
{
	CommandResult_t result = CommandResult_Unknown;

	// Suppress warning
	(void)argc;

	uint32_t id;
	if (StringToUnsignedDecimalNum(argv[1], &id))
	{
		// "trace <id> enable/disable"
		// Good ID
		if (!StrCmp("enable", argv[2]))
		{
			GlobalVarHandler_EnableTrace(id, true);
			CommandHandler_Printf("Enabled trace: %d\r\n", id);
			result = CommandResult_Ok;
		}
		else
		{
			GlobalVarHandler_EnableTrace(id, false);
			CommandHandler_Printf("Disabled trace: %d\r\n", id);
			result = CommandResult_Ok;
		}
	}
	else if(!StrCmp("period", argv[1]))
	{
		// Trace period time setting
		uint32_t time;
		if (StringToUnsignedDecimalNum(argv[2], &time))
		{
			// Parameter is good, set Trace task periodic time
			TaskHandler_SetTaskPeriodicTime(Task_Trace, time);
			result = CommandResult_Ok_SendSuccessful;
		}
		else
		{
			result = CommandResult_Error_WrongArgument2;
		}
	}
	else
	{
		// trace <varname> enable/disable
		// First parameter is not "id", maybe "name", try process
		GlobalVarHandler_ProcessCommand(argv[1], argv[2], SetGet_Trace, CommandHandler_CommandSource);
		result = CommandResult_Ok_SendSuccessful;
	}

	return result;
}
#endif

#endif	// #ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE



#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE
/**
 * \brief	DAC function
 *			Use: 'dac <channel> <voltage>'
 */
static CommandResult_t CommandFunction_dac(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;

	uint32_t channelNum;
	float voltage = 0.0f;

	// Check 1. argument (channel num)
	if (!StringToUnsignedDecimalNum(argv[1], &channelNum))
	{
		return CommandResult_Error_WrongArgument1;
	}

	// Check 2. argument (voltage)
	if (!StringToFloat(argv[2], &voltage))
	{
		return CommandResult_Error_WrongArgument2;
	}

	// Set DAC value
	if (CommonDAC_SetValue(channelNum, voltage))
	{
		return CommandResult_Ok_SendSuccessful;
	}
	else
	{
		return CommandResult_Error_WrongArgumentNum;
	}
}
#endif



#ifdef CONFIG_MODULE_COMMON_IO_ENABLE
/**
 * \brief	Common IO function
 * 			Initialize and set/get io pin status
 * 			"ioinit <port><pin> <input/output>"
 * 			"ioout <port><pin> <set/reset>"
 * 			"ioin <port><pin>
 */
static CommandResult_t CommandFunction_io(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;

	if (!StrCmp("ioinit", argv[0]))
	{
		// Init
		char port = argv[1][0];
		uint32_t pin;
		if (StringToUnsignedDecimalNum(&argv[1][1], &pin))
		{
			// TODO: with enum string?
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
				return CommandResult_Error_WrongArgument2;
			}

			if (CommonIO_Init(port, (uint8_t)pin, io))
			{
				return CommandResult_Ok;
			}
			else
			{
				return CommandResult_Error_Unknown;
			}
		}
		else
		{
			return CommandResult_Error_WrongArgument1;
		}
	}
	else if (!StrCmp("ioout", argv[0]))
	{
		// Output
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
			// Set output
			bool status = CommonIO_SetOutput(port,pin,output);
			CommandHandler_Printf("Output status: %d\r\n", status);
			return CommandResult_Ok;
		}
		else
		{
			return CommandResult_Error_Unknown;
		}
	}
	else if (!StrCmp("ioin", argv[0]))
	{
		// Input
		char port = argv[1][0];
		uint32_t pin;
		if (StringToUnsignedDecimalNum(&argv[1][1], &pin))
		{
			bool status = CommonIO_ReadPin(port, pin);
			CommandHandler_Printf("Input status: %d\r\n", status);
			return CommandResult_Ok;
		}
		else
		{
			return CommandResult_Error_WrongArgument1;
		}
	}

	return CommandResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
/**
 * \brief	Common ADC function
 * 			Read ADC values
 */
static CommandResult_t CommandFunction_adc(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

	uint8_t i;

	CommonADC_ConvertAllMeasuredValues();

	for (i = 0; i < ADC_CHANNEL_NUM; i++)
	{
		CommandHandler_Printf("ADC: %d. value: %2.2f\r\n", i, ADC_ConvertedValues[i]);
	}

	return CommandResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
/**
 * \brief	Common ADC function
 * 			Read ADC values
 */
static CommandResult_t CommandFunction_adcread(uint32_t argc, char** argv)
{

	uint32_t convertValue;
	uint16_t milliSec = 0;
	CommandResult_t result;

	// Check 1. argument
	if (StringToUnsignedDecimalNum(argv[1], &convertValue))
	{
		// Received good period value
		milliSec = (uint16_t)convertValue;
	}
	else if (!StrCmp("stop", argv[1]))
	{
		// Stop
		TaskHandler_DisableTask(Task_CommonAdc);
		result = CommandResult_Ok_SendSuccessful;
	}
	else
	{
		// First argument is wrong
		result = CommandResult_Error_WrongArgument1;
	}

	// Check 2. argument
	if (argc == 3)
	{
		if (StringToUnsignedDecimalNum(argv[2], &convertValue))
		{
			if (convertValue < ADC_CHANNEL_NUM)
			{
				// Is Ok
				TaskHandler_RequestTaskScheduling(Task_CommonAdc);
				TaskHandler_SetTaskPeriodicTime(Task_CommonAdc, milliSec);
				CommonADC_SetPrintNum(convertValue);
				result = CommandResult_Ok_SendSuccessful;
			}
			else
			{
				// Wrong ADC num
				result = CommandResult_Error_WrongArgument2;
			}
		}
		else
		{
			// Second argument is wrong
			result = CommandResult_Error_WrongArgument2;
		}
	}

	return result;
}
#endif



#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE
static CommandResult_t CommandFunction_PWM(uint32_t argc, char** argv)
{
	// Suppress unused args
	(void)argc;

	uint8_t percent;
	uint32_t convertValue;
	CommandResult_t result;

	if (StringToUnsignedDecimalNum(argv[1], &convertValue))
	{
		if (convertValue <= 100)
		{
			percent = (uint8_t)convertValue;
			CommonPWM_ChangePercent(percent);
			result = CommandResult_Ok;
		}
		else
		{
			result = CommandResult_Error_WrongArgument1;
		}
	}
	else
	{
		result = CommandResult_Error_WrongArgument1;
	}

	return result;
}
#endif



#ifdef CONFIG_MODULE_MOTOR_ENABLE
static CommandResult_t CommandFunction_Motor(uint32_t argc, char** argv)
{
	(void)argc;
	CommandResult_t result = CommandResult_Unknown;


	if (!StrCmp("dc", argv[1]))
	{
		// Set DC motor
		int32_t convertValue;
		if (StringToSignedDecimalNum(argv[2], &convertValue))
		{
			/*
			if (convertValue <= 100 && convertValue > 0)
			{
				uint8_t percent = (uint8_t)convertValue;
				Motor_DcMotorChangePercent(percent);
				Motor_DcMotorSeDirection(MotorDir_Forward);
				result = CommandResult_Ok_SendSuccessful;
			}
			else if (convertValue == 0)
			{
				Motor_DcMotorChangePercent(0);
				Motor_DcMotorSeDirection(MotorDir_Stop);
				result = CommandResult_Ok_SendSuccessful;
			}
			else if (convertValue < 0 && convertValue > -100)
			{
				uint8_t percent = (uint8_t)(convertValue * (-1));
				Motor_DcMotorChangePercent(percent);
				Motor_DcMotorSeDirection(MotorDir_Backward);
				result = CommandResult_Ok_SendSuccessful;
			}
			*/
			if (convertValue <= 100 && convertValue > -100)
			{
				int8_t percent = (uint8_t)convertValue;
				Motor_StateMachine_SetDc(percent);
				TaskHandler_ClearTimeoutTask(Task_MotorTimeout);
				result = CommandResult_Ok_SendSuccessful;
			}
			else
			{
				result = CommandResult_Error_WrongArgument2;
			}
		}
		else
		{
			result = CommandResult_Error_WrongArgument2;
		}
	}
	else if (!StrCmp("servo", argv[1]))
	{
		// Set servo motor
		int32_t convertValue;
		if (StringToSignedDecimalNum(argv[2], &convertValue))
		{
			if (convertValue <= 90 && convertValue >= -90)
			{

				int8_t angle = (int8_t)convertValue;
				/*
				Motor_ServoChangeAngle(angle);
				*/
				Motor_StateMachine_SetAngle(angle);
				TaskHandler_ClearTimeoutTask(Task_MotorTimeout);

				result = CommandResult_Ok_SendSuccessful;
			}
			else
			{
				result = CommandResult_Error_WrongArgument2;
			}
		}
		else
		{
			result = CommandResult_Error_WrongArgument2;
		}
	}
	else if (!StrCmp("slide", argv[1]))
	{
		// Motor Slide (DC and Servo too)
		if (!StrCmp("on", argv[2]))
		{
			MotorTestSlide_Enabled = true;
			result = CommandResult_Ok_SendSuccessful;
		}
		else if (!StrCmp("off", argv[2]))
		{
			MotorTestSlide_Enabled = false;
			Motor_ControlStop();
			result = CommandResult_Ok_SendSuccessful;
		}
		else
		{
			MotorTestSlide_Enabled = false;
			Motor_ControlStop();
			result = CommandResult_Error_WrongArgument2;
		}
	}
	else
	{
		result = CommandResult_Error_WrongArgument1;
	}

	return result;
}
#endif



#ifdef CONFIG_MODULE_ESP8266_ENABLE
static CommandResult_t CommandFunction_ESP8266(uint32_t argc, char** argv)
{
	(void)argc;
	bool result = CommandResult_Unknown;

	if (!StrCmp("sendonwifi", argv[1]))
	{
		// Send message to ESP8266 sending queue, which will send on ESP8266 TCP connection
		ESP8266_RequestSendTcpMessage(argv[2]);
		result = CommandResult_Ok_SendSuccessful;
	}
	else if (!StrCmp("sendtomodule", argv[1]))
	{
		// Send forward to ESP8266 module last parameter
		ESP8266_SendString(argv[2]);
		result = CommandResult_Ok_SendSuccessful;
	}
	else if (!StrCmp("debug", argv[1]))
	{
		// Check debug on/off
		if (!StrCmp("on", argv[2]))
		{
			// On
			ESP8266_DebugEnableFlag = true;
			result = CommandResult_Ok_SendSuccessful;
		}
		else if (!StrCmp("off", argv[2]))
		{
			// Off
			ESP8266_DebugEnableFlag = false;
			result = CommandResult_Ok_SendSuccessful;
		}
		else
		{
			// Wrong command
			result = CommandResult_Error_WrongArgument2;
		}
	}
	else if (!StrCmp("ip", argv[1]))
	{
		// Print ESP8266 IP addresses
		char ipBuffer[80];
		ESP8266_PrintIpAddress(ipBuffer);
		CommandHandler_SendLine(ipBuffer);
		result = CommandResult_Ok;
	}
	else
	{
		// Wrong 1. parameter
		result = CommandResult_Error_WrongArgument1;
	}

	return result;
}
#endif



#if defined(CONFIG_MODULE_RTC_ENABLE) || defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE)
static CommandResult_t CommandFunction_Time(uint32_t argc, char** argv)
{
	CommandResult_t result = CommandResult_Unknown;

	if (!StrCmp("setdate", argv[1]) && argc == 3)
	{
		// Set date
		Date_t date;
		if (DateTime_ConvertDateStringToDate(argv[2], &date))
		{
			// Successful convert, set
			DateTime_SetDate(&date);
			result = CommandResult_Ok_SendSuccessful;
		}
		else
		{
			result = CommandResult_Error_WrongArgument2;
		}
	}
	else if (!StrCmp("settime", argv[1]) && argc == 3)
	{
		// Set time
		Time_t time;
		if (DateTime_ConvertTimeStringToTime(argv[2], &time))
		{
			// Successful convert, set
			DateTime_SetTime(&time);
			result = CommandResult_Ok_SendSuccessful;
		}
		else
		{
			result = CommandResult_Error_WrongArgument2;
		}
	}
	else if (!StrCmp("status", argv[1]))
	{
		char datetimestring[DATETIME_STRING_MAX_LENGTH];
		DateTime_t dateTime;
		DateTime_GetDateTime(&dateTime);
		DateTime_PrintDateTimeToString(datetimestring, &dateTime);
		CommandHandler_Printf("Date and Time: %s\r\n", datetimestring);

		result = CommandResult_Ok;
	}
	else
	{
		result = CommandResult_Error_WrongArgument1;
	}

	return result;
}
#endif



#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
static CommandResult_t CommandFunction_EventLog(uint32_t argc, char** argv)
{
	(void)argc;
	(void)argv;

	// List all event logs
	EventLog_PrintAllLogRecords();

	return CommandResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_ADC_ENABLE
/**
 * \brief	Read ADC values
 */
static CommandResult_t CommandFunction_adc(uint32_t argc, char** argv)
{
	(void)argc;
	(void)argv;

	// Print ADC values
	ADC_PrintAllValues();

	return CommandResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_FLASH_ENABLE
/**
 * \brief	Flash erase
 * 			Use: 'flashdel <address> <block/sector>'
 */
static CommandResult_t CommandFunction_flashdel(uint32_t argc, char** argv)
{
	uint32_t Arg2Num;

	// Convert arg2 hex
	if (!StringHexToNum(argv[1], &Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
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

	CommandHandler_Printf("address erased: 0x%h\r\n",
			Arg2Num
			);

	return CommandResult_Ok;
}



/**
 * \brief	Flash read
 * 			Use: 'flashread <address>'
 */
static CommandResult_t CommandFunction_flashread(uint32_t argc, char** argv)
{

	uint32_t Arg2Num;

	// Convert arg2 hex
	if (!StringHexToNum(argv[1], &Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}


	uint8_t Buffer[1];
	FLASH_Read(Arg2Num,Buffer, 1, 5000);


	CommandHandler_Printf("address: 0x%w\r\n"
			"data:    0x%b\r\n",
			Arg2Num,
			Buffer[0]
			);

	return CommandResult_Ok;
}



/**
 * \brief	Flash write
 * 			Use: 'flashwrite <address> <data>'
 */
static CommandResult_t CommandFunction_flashwrite(uint32_t argc, char** argv)
{
	uint32_t Arg2Num;

	// Convert arg2 hex
	if (!StringHexToNum(argv[1], &Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}

	// Convert arg3, decimal
	if (!StringToUnsignedDecimalNum(argv[2], &Arg3Num))
	{
		return CommandResult_Error_WrongArgument2;
	}


	uint8_t Buffer[1];

	Buffer[0] = (uint8_t)Arg3Num;

	FLASH_Write(Arg2Num, Buffer, 1, 5000);


	CommandHandler_Printf("address: 0x%w\r\n"
			"data:    0x%b\r\n",
			Arg2Num,
			Arg3Num);

	return CommandResult_Ok;
}
#endif	// #ifdef CONFIG_MODULE_FLASH_ENABLE



#ifdef CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE
/**
 * \brief	HomeAutMessage command
 */
static CommandResult_t CommandFunction_raspberrypi(uint32_t argc, char** argv)
{
	(void)argc;

	// Check arg 2
	if (!StrCmp("setout", argv[1]))
	{
		// setout
		/*
		HomeAutMessage_CreateAndSendHomeAutMessage(
			0,255,
			Function_Command, Command_SetOutput,
			Arg3Num, 1);
		*/

		// Convert arg3, decimal
		uint32_t value;
		if (!StringToUnsignedDecimalNum(argv[2], &value))
		{
			return CommandResult_Error_WrongArgument2;
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

		return CommandResult_Ok;
	}
	else
	{
		// Wrong command - not "setout"
		return CommandResult_Error_WrongArgument1;
	}
}
#endif



#ifdef CONFIG_DEBUG_RAMREAD_WRITE_COMMAND_ENABLE
/*
 * \brief	Function: mr (memory read)
 * mr <source> <size>
 * source: hex
 * size: dec
 * read <size> byte-s from <source> address
 */
static CommandResult_t CommandFunction_mr(uint32_t argc, char** argv)
{
	(void)argc;

	uint32_t *source;
	int16_t size;
	uint16_t i;
	uint32_t Arg2Num;
	uint32_t Arg3Num;

	// Convert arg2, source to hex
	if (!StringHexToNum(argv[1], &Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}
	// Convert arg3, size to dec
	if (!StringToUnsignedDecimalNum(argv[2], &Arg3Num))
	{
		return CommandResult_Error_WrongArgument2;
	}

	// casting for valid numbers
	source = (uint32_t *)Arg2Num;
	size = (int16_t)Arg3Num;
	// <size> max 256

	// TODO: Checking the correct address

	CommandHandler_Printf("Source: 0x%X\r\n"
							"Size: %d\r\n",
							source, size);

	for (i = 0; i < size; i++)
	{
		if (!(i % 4))
		{
			// 0., 4., ...
			CommandHandler_Printf("\r\n 0x%X:", (uint32_t)&source[i]);
		}
		CommandHandler_Printf(" %02X", source[i]);
	}

	return CommandResult_Ok;
}



/*
 * \brief	FunctioN: mw - memory write
 * mwb <destination> <data>
 * destination: hex
 * data: hex
 * mwb, mwh, mww commands
 */
static CommandResult_t CommandFunction_mw(uint32_t argc, char** argv)
{
	(void)argc;

	uint8_t *destination1;
	uint16_t *destination2;
	uint32_t *destination3;
	uint32_t Arg2Num;
	uint32_t Arg3Num;

	// Convert hex
	if (!StringHexToNum(argv[1], &Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}
	// Convert hex
	if (!StringHexToNum(argv[2], &Arg3Num))
	{
		return CommandResult_Error_WrongArgument2;
	}

	if (!StrCmp("mwb", argv[0]))
	{
		destination1 = (uint8_t *)Arg2Num;
		*destination1 = (uint8_t)Arg3Num;
		CommandHandler_Printf("Write: %02X to: %X", Arg3Num, Arg2Num);
	}
	else if (!StrCmp("mwh", argv[0]))
	{
		destination2 = (uint16_t *)Arg2Num;
		*destination2 = (uint16_t)Arg3Num;
		CommandHandler_Printf("Write: %04X to: %X", Arg3Num, Arg2Num);
	}
	else if (!StrCmp("mww", argv[0]))
	{
		destination3 = (uint32_t *)Arg2Num;
		*destination3 = (uint32_t)Arg3Num;
		CommandHandler_Printf("Write: %08X to: %X", Arg3Num, Arg2Num);
	}
	else
	{
		return CommandResult_Error_Unknown;
	}

	return CommandResult_Ok;
}
#endif



#ifdef CONFIG_DEBUG_GO_COMMAND_ENABLE
/**
 * \brief	Function: go (jump to an address)
 * go <destination>
 * jump <destination> address
 */
static CommandResult_t CommandFunction_go(uint32_t argc, char** argv)
{
	(void)argc;

	uint32_t destination;
	int (*fpntr)(void);
	uint32_t Arg2Num;


	// Convert hex
	if (!StringHexToNum(argv[1], &Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}

	destination = Arg2Num;
	CommandHandler_Printf("Go destination: 0x%X\r\n", destination);

	fpntr = (int (*)(void))destination;		// casting
	fpntr();

	// Now, for example: "go 20000151"

	// We can use the "jump" ASM instruction, but not a good idea
	// Programming manual page 92, B instruction

	return CommandResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_DISPLAY_ENABLE
/**
 * \brief	Display module
 */
static CommandResult_t CommandFunction_Display(uint32_t argc, char** argv)
{
	(void)argc;

	CommandResult_t result = CommandResult_Unknown;

	if (!StrCmp("write", argv[1]))
	{
		char *separated[2];
		if (STRING_Splitter(argv[2], ',', separated, 2))
		{
			// Check line parameter
			uint32_t line;
			if (StringToUnsignedDecimalNum(separated[0], &line))
			{
				// Print line
				Display_PrintString(separated[1], line, Font_8x5, NO_FORMAT);
				Display_Activate();
				result = CommandResult_Ok_SendSuccessful;
			}
			else
			{
				result = CommandResult_Error_WrongArgument2;
			}
		}
		else
		{
			result = CommandResult_Error_WrongArgument2;
		}
	}
	else if (!StrCmp("clear", argv[1]))
	{
		// Clear
		Display_Clear();
		Display_Activate();
		result = CommandResult_Ok_SendSuccessful;
	}
#ifdef CONFIG_MODULE_DISPLAY_TEST
	#ifdef CONFIG_DISPLAY_FONT8X5_ENABLE
	else if (!StrCmp("test1", argv[1]))
	{
		// Test code
		Display_CarAnimationDisable_flag = true;
		Display_Test8x5Font();
		result = CommandResult_Ok_SendSuccessful;
	}
	#endif
	#ifdef CONFIG_DISPLAY_FONT12X8_ENABLE
	else if (!StrCmp("test2", argv[1]))
	{
		// Test code
		Display_CarAnimationDisable_flag = true;
		Display_Test12x8Font();
		result = CommandResult_Ok_SendSuccessful;
	}
	#endif
	#ifdef CONFIG_DISPLAY_FONT32X20_ENABLE
	else if  (!StrCmp("test3", argv[1]))
	{
		// Test code
		Display_CarAnimationDisable_flag = true;

		// Test font - 32x20
		Display_Test32x20Font();

		result = CommandResult_Ok_SendSuccessful;
	}
	#endif
#endif	// #ifdef CONFIG_MODULE_DISPLAY_TEST
#ifdef CONFIG_MODULE_DISPLAY_TEST_WITH_TERMINAL
	else if (!StrCmp("debugprint", argv[1]))
	{
		// Debugprint Display
		Display_SendOnTerminal();
		result = CommandResult_Ok_SendSuccessful;
	}
#endif
	else
	{
		result = CommandResult_Error_WrongArgument1;
	}

	return result;
}
#endif



#ifdef CONFIG_MODULE_IO_ENABLE
/**
 * \brief	Get IO states
 */
static CommandResult_t CommandFunction_IoStates(uint32_t argc, char** argv)
{
	(void)argc;
	(void)argv;

#if IO_INPUTS_NUM > 0 || IO_OUTPUTS_NUM > 0
	uint8_t i;
#endif

#if IO_INPUTS_NUM > 0
	CommandHandler_SendLine("Input states:");
	for (i = 0; i < Input_Count; i++)
	{
		CommandHandler_Printf(" %20s %s\r\n",
				IO_GetInputName(i),
				IO_GetInputStateName(IO_GetInputState(i)));
	}
#endif

#if IO_OUTPUTS_NUM > 0
	CommandHandler_SendLine("Output states:");
	for (i = 0; i < Output_Count; i++)
	{
		CommandHandler_Printf(" %20s %s\r\n",
				IO_GetOutputName(i),
				IO_GetOutputStateName(IO_GetOutputState(i)));
	}
#endif

	return CommandResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_SIMULATION_ENABLE
/**
 * \brief	Simulation
 */
static CommandResult_t CommandFunction_Simulation(uint32_t argc, char** argv)
{
	(void)argc;
	CommandResult_t result = CommandResult_Unknown;
	static bool Simulation_IsEnabled = false;

	if (!Simulation_IsEnabled)
	{
		if (!StrCmp("enable", argv[1]) && argc == 2)
		{
			Simulation_IsEnabled = true;
			result = CommandResult_Ok_SendSuccessful;
		}
		else
		{
			// Error
			CommandHandler_SendLine("Simulation is not enabled!");
			result = CommandResult_Ok;	// Not ok, but do not send other error message
		}
	}
	else
	{
		// Enabled simulation
		if (!StrCmp("infloop", argv[1]))
		{
			// @note	Be careful!!!!
			// Infinite loop test for WatchDog test
			while(1);
			result = CommandResult_Ok_SendSuccessful;
		}
		else if (!StrCmp("genfault", argv[1]))
		{
			if (!StrCmp("constwrite", argv[2]))
			{
				/*
				 * Generate Fault to test FaultHandler()
				 * \note	!! Be careful !! It is an error, the sw will crash (go to FaultHandler) !!
				 */

				// Const write
				// cppcheck-suppress stringLiteralWrite
				static const char const buffer[] = "const";
				char * pnt = (char *)buffer;

				uprintf("Buffer: %s\r\n", buffer);

				pnt[2] = 'e';

				uprintf("Buffer: %s\r\n", buffer);

				result = CommandResult_Ok_SendSuccessful;
			}
			else if (!StrCmp("zerodivide", argv[2]))
			{
				/* Test zero dividing
				 * \note !! Be careful !! It is an error, the sw will crash (go to FaultHandler) !!
				 */
				uint32_t a = 5;
				uint32_t b = 0;
				uint32_t c;

				// Zero division
				// cppcheck-suppress zerodiv
				c = a/b;

				uprintf("ZeroDivide result: %d\r\n", c);

				result = CommandResult_Ok_SendSuccessful;
			}
			else if (!StrCmp("failpointer", argv[2]))
			{
				// Test invalid pointer
				const uint32_t constValue = 0x12345678;
				uint32_t * wrongPointer = (uint32_t *)constValue;
				*wrongPointer = 0;

				uprintf("WrongPointer value: %d\r\n", *wrongPointer);

				result = CommandResult_Ok_SendSuccessful;
			}
			else
			{
				result = CommandResult_Error_WrongArgument2;
			}
		}
#ifdef CONFIG_SWO_ENABLE
		else if (!StrCmp("SWO", argv[1]))
		{
			// Test SWO
			COMMUNICATION_SendMessage(CommProt_SWO, "Test message on SWO\n");
		}
#endif
#ifdef CONFIG_MODULE_IO_ENABLE
		else if (!StrCmp("input", argv[1]))
		{
			uint32_t pin;
			if (StringToUnsignedDecimalNum(argv[2], &pin))
			{
				IO_SetInputState((Input_t)pin, InputState_Active);
				result = CommandResult_Ok_SendSuccessful;
			}
			else
			{
				result = CommandResult_Error_WrongArgument2;
			}
		}
	#if IO_OUTPUTS_NUM > 0
		else if (!StrCmp("output", argv[1]))
		{
			uint32_t pin;
			if (StringToUnsignedDecimalNum(argv[2], &pin))
			{
				IO_SetOutputState((Output_t)pin, OutputState_Active);
				result = CommandResult_Ok_SendSuccessful;
			}
			else
			{
				result = CommandResult_Error_WrongArgument2;
			}
		}
	#endif
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
		else if (!StrCmp("buttonpress", argv[1]))
		{
			// Button press simulation
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
					result = CommandResult_Ok_SendSuccessful;
				else
					result = CommandResult_Error_WrongArgument2;
			}
			else
			{
				result = CommandResult_Error_TooFewArgument;
			}
		}
#endif
#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE
		else if (!StrCmp("event", argv[1]))
		{
			// Generate an event
			if (StringIsUnsignedDecimalString(argv[2]))
			{
				// Event "number"
				uint32_t value;
				if (StringToUnsignedDecimalNum(argv[2], &value))
				{
					EventHandler_GenerateEvent(value, 0, Task_ProcessDebugUartReceivedCommand);
					result = CommandResult_Ok_SendSuccessful;
				}
				else
				{
					result = CommandResult_Error_WrongArgument2;
				}
			}
			else
			{
				// Not number, search string in event list
				EventId_t i;
				bool isOk = false;

				for (i = 0; i < EventsNum; i++)
				{
					if (!StrCmp(EventList[i].name, argv[2]))
					{
						// Equal
						EventHandler_GenerateEvent(i, 0, Task_ProcessDebugUartReceivedCommand);
						isOk = true;
						break;
					}
				}

				if (isOk)
					result = CommandResult_Ok_SendSuccessful;
				else
					result = CommandResult_Error_WrongArgument2;
			}
		}
#endif
		else
		{
			result = CommandResult_Error_WrongArgument1;
		}
	}

	return result;
}
#endif



#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
/**
 * \brief	TaksHandler functions
 */
static CommandResult_t CommandFunction_TaskHandler(uint32_t argc, char** argv)
{
	(void)argc;
	CommandResult_t result = CommandResult_Unknown;

	#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
	if (!StrCmp("statistics", argv[1]))
	{
		// TaskHandler statistics
		TaskHandler_PrintStatistics();

		result = CommandResult_Ok;
	}
	else if (!StrCmp("runcounts", argv[1]))
	{
		// TaskHandler - Run counts
		TaskHandler_PrintTaskRunCounts();

		result = CommandResult_Ok;
	}
	#endif
	else
	{
		result = CommandResult_Error_WrongArgument1;
	}

	return result;
}
#endif



#ifdef CONFIG_MODULE_DEBUG_ENABLE
static CommandResult_t CommandFunction_Debug(uint32_t argc, char** argv)
{
	(void)argc;

	uint32_t value;
	bool enable = false;
	bool isOk = false;
	CommandResult_t result;

	if (!StrCmp("enable", argv[2]))
	{
		// Enable
		enable = true;
		isOk = true;
	}
	else if (!StrCmp("disable", argv[2]))
	{
		// Disable
		enable = false;
		isOk = true;
	}
	else
	{
		// Wrong 2. parameter
		isOk = false;
		result = CommandResult_Error_WrongArgument2;
	}

	if (isOk)
	{
		// Check 1. parameter
		if (StringToUnsignedDecimalNum(argv[1], &value))
		{
			// 1. parameter is number
			if (Debug_EnableDisable((Debug_t)value, enable))
			{
				// Successful
				result = CommandResult_Ok_SendSuccessful;
			}
			else
			{
				result = CommandResult_Error_WrongArgument1;
			}
		}
		else
		{
			// Find in "list"
			if (Debug_SetDebugTaskWithName(argv[1], enable))
			{
				result = CommandResult_Ok_SendSuccessful;
			}
			else
			{
				result = CommandResult_Error_WrongArgument1;
			}
		}
	}

	return result;
}
#endif



#ifdef CONFIG_MODULE_BUTTON_ENABLE
static CommandResult_t CommandFunction_Button(uint32_t argc, char** argv)
{
	UNUSED(argc);
	UNUSED(argv);

#if BUTTON_NUM > 1
	uint8_t i;

	// Print all button state
	for (i = 0; i < BUTTON_NUM; i++)
	{
		bool buttonState = BUTTON_GetButtonState(i);
		const char * buttonName = BUTTON_GetButtonName(i);

		CommandHandler_Printf("Button: %s is %s\r\n", buttonName, buttonState ? "pressed" : "released");
	}
#else
	// Print one button state
	bool buttonState = BUTTON_GetButtonState(0);
	const char * buttonName = BUTTON_GetButtonName(0);
	CommandHandler_Printf("Button: %s is %s\r\n", buttonName, buttonState ? "pressed" : "released");
#endif

	return CommandResult_Ok;
}
#endif


/* END OF COMMAND FUNCTIONS */
