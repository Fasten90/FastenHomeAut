/*
 *		command.c
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		General commands's list
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "FormattedMessage.h"
#include "CommandList.h"
#include "include.h"

#include "CommandHandler.h"
#include "GlobalVarHandler.h"
#include "Globals.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wpedantic"

/// Commands list
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
	{
		.name = "test",
		.commandFunctionPointer = CommandFunction_test,
		.commandArgNum = CommandArgument_0,
		.description = NULL,
		.syntax = NULL,
	},
	{
		.name = "unittest",
		.commandFunctionPointer = CommandFunction_unittest,
		.commandArgNum = CommandArgument_0 | CommandArgument_1,
		.description = "Run unit tests",
		.syntax = "(modul)",
	},
	{
		.name = "moduletest",
		.commandFunctionPointer = CommandFunction_moduletest,
		.commandArgNum = CommandArgument_0,
		.description = "Run module test",
	},
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
		.syntax = "<num> <voltage>"
		.example = "1 3.0",
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
		.commandArgNum = CommandArgument_2,
		.example = "send ThisMessage",
	},
#endif
#ifdef CONFIG_MODULE_RTC_ENABLE
	{
		.name = "rtc",
		.commandFunctionPointer = CommandFunction_RTC,
		.description = "Use RTC (Real Time Clock)",
		.syntax = "<setdate/settime> <date: 20yy-mm-dd / time: hh:mm:ss>",
		.commandArgNum = CommandArgument_1 | CommandArgument_2,
		.example = "setdate 2017-02-08",
	},
#endif
#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
	{
		.name = "#raspi",
		.description = "Raspberry Pi HomeAutMessage sending",
		.commandFunctionPointer = CommandFunction_raspberrypi,
		.commandArgNum = CommandArgument_2,
		.syntax = NULL,
	},
#endif
#ifdef CONFIG_MODULE_ADC_ENABLE
	{
		.name = "temperature",
		.commandFunctionPointer = CommandFunction_temp,
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


//#pragma GCC diagnostic pop
// End of warning suppress


const CommandID_t CommandHandler_CommandNum = COMMANDHANDLER_MAX_COMMAND_NUM;


/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *								Command functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	CLS: Clear screen
 */
CommandResult_t CommandFunction_cls(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

	Terminal_SendCls();

	return CommandResult_Ok;
}



/**
 * \brief	Get version
 */
CommandResult_t CommandFunction_version(uint32_t argc, char** argv)
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
CommandResult_t CommandFunction_welcome(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

	Terminal_SendWelcome();

	return CommandResult_Ok;
}



/**
 * \brief	Help command
 * 			Use: 'help', or 'help <CommandName>'
 * 			List commands or write the command's description
 */
CommandResult_t CommandFunction_help(uint32_t argc, char** argv)
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
CommandResult_t CommandFunction_reset(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

	uint16_t i;

	CommandHandler_SendLine("Reset...");
	for (i=0; i<1000; i++);

	NVIC_SystemReset();

	for (i=0; i<1000; i++);

	return CommandResult_Ok;
}



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
CommandResult_t CommandFunction_led(uint32_t argc, char** argv)
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
	else if (!StrCmp(argv[1], "status"))
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
			LED_SetType setType = LED_SET_DONTCARE;
			bool status = false;
			setType = LED_GetTypeFromString(argv[2]);

			if (setType == LED_SET_DONTCARE)
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



/**
 * \brief	Test function
 */
CommandResult_t CommandFunction_test(uint32_t argc, char** argv)
{
	
	// Suppress warning
	(void)argc;
	(void)argv;
	
	CommandHandler_SendLine("Test start");

	/*
	 * 		Test codes
	 */


	// Test SWO
	COMMUNICATION_SendMessage(CommProt_SWO, "Test message on SWO\n");


	// Test Watchdog clear with infinite loop
	while (1);


	/**
	 * 		End of Test codes
	 */


	CommandHandler_SendLine("Test end");

	return CommandResult_Ok;

}



#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
/**
 * \brief	Get globalvar value
 * 			Use: 'get <globalvarname>'
 */
CommandResult_t CommandFunction_get(uint32_t argc, char** argv)
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
CommandResult_t CommandFunction_set(uint32_t argc, char** argv)
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
CommandResult_t CommandFunction_GlobalVariableHelp(uint32_t argc, char** argv)
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
CommandResult_t CommandFunction_GlobalVariableList(uint32_t argc, char** argv)
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
CommandResult_t CommandFunction_GlobalVariableValueList(uint32_t argc,
		char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

	GlobalVarHandler_PrintAllVariableValues();

	return CommandResult_Ok;

}
#endif	// #ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE



#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE
/**
 * \brief	DAC function
 * 			Use: 'dac <channel> <voltage>'
 */
CommandResult_t CommandFunction_dac(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;

	uint32_t Arg2Num;
	float voltage = 0.0f;

	// Check 1. argument (num)
	if (!StringToUnsignedDecimalNum(argv[1],&Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}

	// Check 2. argument (voltage)
	if (!StringToFloat(argv[2],&voltage))
	{
		return CommandResult_Error_WrongArgument2;
	}

	// Set DAC value
	if (DAC_SetValue(Arg2Num,voltage))
	{
		return CommandResult_Ok_SendSuccessful;
	}
	else
	{
		return CommandResult_Error_WrongArgumentNum;
	}

}
#endif



/**
 * \brief	Run module's unit tests
 */
CommandResult_t CommandFunction_unittest(uint32_t argc, char** argv)
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

	return CommandResult_Ok_SendSuccessful;
}



/**
 * \brief	Run module test
 */
CommandResult_t CommandFunction_moduletest(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

	uint8_t i;


#ifdef CONFIG_MODULE_LED_ENABLE
	// LED test
	CommandHandler_SendLine("LED test");

	// LEDs on
	for (i=LED_NUM_MIN; i<=LED_NUM_MAX; i++)
	{
		LED_SetLed(i, LED_SET_ON);
		DelayMs(500);
	}

	Watchdog_Clear();

	// LEDs off
	for (i=LED_NUM_MIN; i<=LED_NUM_MAX; i++)
	{
		LED_SetLed(i, LED_SET_OFF);
		DelayMs(500);
	}
#endif	// #ifdef CONFIG_MODULE_LED_ENABLE


#ifdef CONFIG_MODULE_BUTTON_ENABLE
	// Clear flag
	BUTTON_Clicked = 0;

	CommandHandler_SendLine("\r\nButton test: Please press button!");

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
	CommandHandler_SendLine("Formatted message test");
	FormattedMessage_Test();


	return CommandResult_Ok;
}



#ifdef CONFIG_MODULE_COMMON_IO_ENABLE
/**
 * \brief	Common IO function
 * 			Initialize and set/get io pin status
 * 			"ioinit <port><pin> <input/output>"
 * 			"ioout <port><pin> <set/reset>"
 * 			"ioin <port><pin>
 */
CommandResult_t CommandFunction_io(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;

	if (!StrCmp(argv[0],"ioinit"))
	{
		// Init
		char port = argv[1][0];
		uint32_t pin;
		if (StringToUnsignedDecimalNum(&argv[1][1],&pin))
		{
			// TODO: enum stringesre
			IO_Type io = IO_UNKNOWN;
			if (!StrCmp(argv[2],"input"))
			{
				io = IO_INPUT;
			}
			else if (!StrCmp(argv[2],"output"))
			{
				io = IO_OUTPUT;
			}
			else
			{
				return CommandResult_Error_WrongArgument2;
			}

			if (IO_Init(port,(uint8_t)pin,io))
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
	else if (!StrCmp(argv[0],"ioout"))
	{
		// Output
		char port = argv[1][0];
		uint32_t pin;
		if (StringToUnsignedDecimalNum(&argv[1][1],&pin))
		{
			Output_Type output = OUTPUT_DONTCARE;
			if (!StrCmp(argv[2],"set"))
			{
				output = OUTPUT_HIGH;
			}
			else if (!StrCmp(argv[2],"reset"))
			{
				output = OUTPUT_LOW;
			}
			else if (!StrCmp(argv[2],"toggle"))
			{
				output = OUTPUT_TOGGLE;
			}
			else if (!StrCmp(argv[2],"status"))
			{
				output = OUTPUT_STATUS;
			}
			// Set output
			bool status = IO_SetOutput(port,pin,output);
			CommandHandler_Printf("Output status: %d\r\n", status);
			return CommandResult_Ok;
		}
		else
		{
			return CommandResult_Error_Unknown;
		}
	}
	else if (!StrCmp(argv[0],"ioin"))
	{
		// Input
		char port = argv[1][0];
		uint32_t pin;
		if (StringToUnsignedDecimalNum(&argv[1][1],&pin))
		{
			bool status = IO_ReadPin(port,pin);
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
CommandResult_t CommandFunction_adc(uint32_t argc, char** argv)
{
	// Suppress warning
	(void)argc;
	(void)argv;

	uint8_t i;

	ADC_ConvertAllMeasuredValues();

	for (i=0; i<ADC_BUFFER_SIZE; i++)
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
CommandResult_t CommandFunction_adcread(uint32_t argc, char** argv)
{

	uint8_t i;
	uint32_t convertValue;
	uint16_t milliSec = 0;
	uint8_t startNum = 0;
	uint8_t endNum = ADC_BUFFER_SIZE-1;
	// TODO: Make a more beautiful solve (for example: EventCheck
	uint8_t uartWriteCntOld = USART_RxBufferWriteCounter;

	if (StringToUnsignedDecimalNum(argv[1], &convertValue))
	{
		milliSec = (uint16_t)convertValue;
	}
	else
	{
		// First argument is wrong
		return CommandResult_Error_WrongArgument1;
	}

	// Check 2. argument
	if (argc == 3)
	{
		if (StringToUnsignedDecimalNum(argv[2], &convertValue))
		{
			startNum = (uint8_t)convertValue;
			endNum = startNum;
		}
		else
		{
			// Second argument is wrong
			return CommandResult_Error_WrongArgument2;
		}
	}

	// Loop for print
	while (1)
	{
		for (i=startNum; i<=endNum; i++)
		{
			// Convert & Print
			ADC_ConvertAllMeasuredValues();
			CommandHandler_Printf("ADC: %d. value: %2.2f\r\n", i, ADC_ConvertedValues[i]);
		}

		// Delay
		DelayMs(milliSec);

		if (uartWriteCntOld != USART_RxBufferWriteCounter)
		{
			// Received a new char
			CommandHandler_SendLine("ADC read is interrupted");
			break;
		}
	}

	return CommandResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_MOTOR_ENABLE
CommandResult_t CommandFunction_Motor(uint32_t argc, char** argv)
{
	(void)argc;

	if (!StrCmp("dc", argv[1]))
	{
		int32_t convertValue;
		if (StringToSignedDecimalNum(argv[2], &convertValue))
		{
			if (convertValue <= 100 && convertValue > 0)
			{
				uint8_t percent = (uint8_t)convertValue;
				Motor_DcMotorChangePercent(percent);
				Motor_DcMotorSeDirection(MotorDir_Forward);
				return CommandResult_Ok_SendSuccessful;
			}
			else if (convertValue == 0)
			{
				Motor_DcMotorChangePercent(0);
				Motor_DcMotorSeDirection(MotorDir_Stop);
				return CommandResult_Ok_SendSuccessful;
			}
			else if (convertValue < 0 && convertValue > -100)
			{
				uint8_t percent = (uint8_t)(convertValue * (-1));
				Motor_DcMotorChangePercent(percent);
				Motor_DcMotorSeDirection(MotorDir_Backward);
				return CommandResult_Ok_SendSuccessful;
			}
			else
			{
				return CommandResult_Error_WrongArgument2;
			}
		}
		else
		{
			return CommandResult_Error_WrongArgument2;
		}
	}
	else if (!StrCmp("servo", argv[1]))
	{
		int32_t convertValue;
		if (StringToSignedDecimalNum(argv[2], &convertValue))
		{
			if (convertValue <= 90 && convertValue >= -90)
			{
				int8_t angle = (int8_t)convertValue;
				Motor_ServoChangeAngle(angle);
				return CommandResult_Ok_SendSuccessful;
			}
			else
			{
				return CommandResult_Error_WrongArgument2;
			}
		}
		else
		{
			return CommandResult_Error_WrongArgument2;
		}
	}
	else
	{
		return CommandResult_Error_WrongArgument1;
	}

	return CommandResult_Error_Unknown;
}
#endif



#ifdef CONFIG_MODULE_ESP8266_ENABLE
CommandResult_t CommandFunction_ESP8266(uint32_t argc, char** argv)
{
	(void)argc;

	if (!StrCmp(argv[1], "sendonwifi"))
	{
		// Send message to ESP8266 sending queue, which will send on ESP8266 TCP connection
		ESP8266_SendMessageOnWifi(argv[2]);
		return CommandResult_Ok_SendSuccessful;
	}
	else if (!StrCmp(argv[1], "sendtomodule"))
	{
		// Send forward to ESP8266 module last parameter
		ESP8266_SendString(argv[2]);
		return CommandResult_Ok_SendSuccessful;
	}
	else if (!StrCmp(argv[1], "debug"))
	{
		// Check debug on/off
		if (!StrCmp(argv[2], "on"))
		{
			// On
			ESP8266_DebugEnableFlag = true;
			return CommandResult_Ok_SendSuccessful;
		}
		else if (!StrCmp(argv[2], "off"))
		{
			// Off
			ESP8266_DebugEnableFlag = false;
			return CommandResult_Ok_SendSuccessful;
		}
		else
		{
			// Wrong command
			return CommandResult_Error_WrongArgument2;
		}
	}
	else
	{
		// Wrong 1. parameter
		return CommandResult_Error_WrongArgument1;
	}
}
#endif



#ifdef CONFIG_MODULE_RTC_ENABLE
CommandResult_t CommandFunction_RTC(uint32_t argc, char** argv)
{
	CommandResult_t result = CommandResult_Unknown;

	if (!StrCmp(argv[1], "setdate") && argc == 3)
	{
		// Set date
		Date_t date;
		if (DateTime_ConvertDateStringToDate(argv[2], &date))
		{
			// Successful
			RTC_SetDate(&date);
			result = CommandResult_Ok_SendSuccessful;
		}
		else
		{
			result = CommandResult_Error_WrongArgument2;
		}
	}
	else if (!StrCmp(argv[1], "settime") && argc == 3)
	{
		// Set time
		Time_t time;
		if (DateTime_ConvertTimeStringToTime(argv[2], &time))
		{
			// Successful
			RTC_SetTime(&time);
			result = CommandResult_Ok_SendSuccessful;
		}
		else
		{
			result = CommandResult_Error_WrongArgument2;
		}
	}
	else if (!StrCmp(argv[1], "status"))
	{
		char datetimestring[DATETIME_STRING_MAX_LENGTH];
		DateTime_t dateTime;
		RTC_GetDateTime(&dateTime);
		DateTime_PrintDateTimeToString(datetimestring, &dateTime);
		CommandHandler_Printf("RTC Date and Time: %s\r\n", datetimestring);

		result = CommandResult_Ok;
	}
	else
	{
		result = CommandResult_Error_WrongArgument1;
	}

	return result;
}
#endif



#ifdef CONFIG_MODULE_ADC_ENABLE
/**
 * \brief	Temperature
 * 			Read temperature and Vref values
 */
CommandResult_t CommandFunction_temp(uint32_t argc, char** argv)
{

	//uprintf("Temperature: %d [C]\r\n",ADC_GetTemp());
	//uprintf("Vref: %d [mV]\r\n",ADC_GetVref());


	ADC_ConvertAllMeasuredValues();

	uprintf("Temperature: ");
	USART_SendFloat(ADC_ConvertedValue_InternalTemperature);
	uprintf(" [C]\r\n");


	return CommandResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_FLASH_ENABLE
/**
 * \brief	Flash erase
 * 			Use: 'flashdel <address> <block/sector>'
 */
CommandResult_t CommandFunction_flashdel(uint32_t argc, char** argv)
{
	uint32_t Arg2Num;

	// Convert arg2 hex
	if ( !StringHexToNum(argv[1],&Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}


	if (!StrCmp(argv[2],"block"))
	{
		FLASH_BlockErase(Arg2Num,5000);
	}
	else if (!StrCmp(argv[2],"sector"))
	{
		FLASH_SectorErase(Arg2Num,5000);
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
CommandResult_t CommandFunction_flashread(uint32_t argc, char** argv)
{

	uint32_t Arg2Num;

	// Convert arg2 hex
	if ( !StringHexToNum(argv[1],&Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}


	uint8_t Buffer[1];
	FLASH_Read(Arg2Num,Buffer,1,5000);


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
CommandResult_t CommandFunction_flashwrite(uint32_t argc, char** argv)
{
	uint32_t Arg2Num;

	// Convert arg2 hex
	if (!StringHexToNum(argv[1],&Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}

	// Convert arg3, decimal
	if (!StringToUnsignedDecimalNum(argv[2],&Arg3Num))
	{
		return CommandResult_Error_WrongArgument2;
	}


	uint8_t Buffer[1];

	Buffer[0] = (uint8_t)Arg3Num;

	FLASH_Write(Arg2Num,Buffer,1,5000);


	CommandHandler_Printf("address: 0x%w\r\n"
			"data:    0x%b\r\n",
			Arg2Num,
			Arg3Num);


	return CommandResult_Ok;
}
#endif	// #ifdef CONFIG_MODULE_FLASH_ENABLE



#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
/**
 * \brief	Raspberry Pi command
 */
CommandResult_t CommandFunction_raspberrypi(uint32_t argc, char** argv)
{
	(void)argc;

	// Check arg 2
	if (!StrCmp(argv[1],"setout"))
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
		if (!StringToUnsignedDecimalNum(argv[2],&value))
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
CommandResult_t CommandFunction_mr(uint32_t argc, char** argv)
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
CommandResult_t CommandFunction_mw(uint32_t argc, char** argv)
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

	if (!StrCmp(argv[0], "mwb"))
	{
		destination1 = (uint8_t *)Arg2Num;
		*destination1 = (uint8_t)Arg3Num;
		CommandHandler_Printf("Write: %02X to: %X", Arg3Num, Arg2Num);
	}
	else if (!StrCmp(argv[0], "mwh"))
	{
		destination2 = (uint16_t *)Arg2Num;
		*destination2 = (uint16_t)Arg3Num;
		CommandHandler_Printf("Write: %04X to: %X", Arg3Num, Arg2Num);
	}
	else if (!StrCmp(argv[0], "mww"))
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
CommandResult_t CommandFunction_go(uint32_t argc, char** argv)
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



/* END OF COMMAND FUNCTIONS */
