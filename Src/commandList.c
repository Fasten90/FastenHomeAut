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

#include "commandList.h"
#include "include.h"

#include "CommandHandler.h"
#include "GlobalVarHandler.h"
#include "formattedMessage.h"
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


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

/// Commands list
const CommandStruct CommandList[] =
{

	{
		.name = "help",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_help,
		.commandArgNum = CommandArgument_0 | CommandArgument_1,
		.description = "Commands's list, or write command's descriptions",
		.syntax = "(command)",
	},
	{
		.name = "version",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_version,
		.commandArgNum = CommandArgument_0,
		.description = "Get version",
		.syntax = NULL,
	},
	{
		.name = "welcome",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_welcome,
		.commandArgNum = CommandArgument_0,
		.description = "Send welcome message",
		.syntax = NULL,
	},
	{
		.name = "reset",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_reset,
		.commandArgNum = CommandArgument_0,
		.description = "Software reset",
		.syntax = NULL,
	},
	{
		.name = "cls",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_cls,
		.commandArgNum = CommandArgument_0,
		.description = "Clear Screen",
		.syntax = NULL,
	},
	{
		.name = "test",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_test,
		.commandArgNum = CommandArgument_0,
		.description = NULL,
		.syntax = NULL,
	},
	{
		.name = "set",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_set,
		.commandArgNum = CommandArgument_2,
		.description = "set global variable value",
		.syntax = "<varName> <value>",
	},
	{
		.name = "get",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_get,
		.description = "get global variable value",
		.commandArgNum = CommandArgument_1,
		.syntax = "<varName>",
	},
	{
		.name = "?",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_GlobalVariableHelp,
		.description = "help global variable",
		.commandArgNum = CommandArgument_1,
		.syntax = "<varName>",
	},
	{
		.name = "list",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_GlobalVariableList,
		.commandArgNum = CommandArgument_0,
		.description = "List global variable",
	},
	{
		.name = "listvalues",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_GlobalVariableValueList,
		.commandArgNum = CommandArgument_0,
		.description = "List global variable's values",
	},
	{
		.name = "led",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_led,
		.commandArgNum = CommandArgument_1 | CommandArgument_2,
		.description = "Control LED",
		.syntax = "<num> <on/off/toggle/status>",
	},
#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
	{
		.name = "#raspi",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_raspberrypi,
		.commandArgNum = CommandArgument_2
		.description = "Raspberry Pi HomeAutMessage sending",
		.syntax = NULL,
	},
#endif
	{
		.name = "unittest",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_unittest,
		.commandArgNum = CommandArgument_0 | CommandArgument_1,
		.description = "Run unit tests",
		.syntax = "(modul)",
	},
	{
		.name = "moduletest",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_moduletest,
		.commandArgNum = CommandArgument_0,
		.description = "Run module test",
	},
#ifdef CONFIG_MODULE_COMMON_IO_ENABLE
	{
		.name = "ioinit",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_io,
		.commandArgNum = CommandArgument_2,
		.description = "Init IO pins",
		.syntax = "<port><pin> <input/output>",
	},
	{
		.name = "ioout",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_io,
		.commandArgNum = CommandArgument_2,
		.description = "Set output",
		.syntax = "<port><pin> <set/reset>",
	},
	{
		.name = "ioin",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_io,
		.commandArgNum = CommandArgument_1,
		.description = "Get input",
		.syntax = "<port><pin>",
	},
#endif
#ifdef CONFIG_MODULE_DAC_ENABLE
	{
		.name = "dac",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_dac,
		.commandArgNum = CommandArgument_2,
		.description = "Digital -> Analog",
		.syntax = "<num> <voltage>"
	},
#endif
#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
	{
		.name = "adc",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_adc,
		.description = "Analog -> Digital",
		.syntax = NULL,
		.commandArgNum = CommandArgument_0,
	},
	{
		.name = "adcread",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_adcread,
		.description = "ADC read continuous",
		.syntax = "<milliSec> <num>",
		.commandArgNum = CommandArgument_1 | CommandArgument_2,
	},
#endif
#ifdef CONFIG_MODULE_ADC_ENABLE
	{
		.name = "temperature",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_temp,
		.description = "...",
		.syntax = NULL,
		.commandArgNum = CommandArgument_0,
	},
#endif
#ifdef CONFIG_MODULE_FLASH_ENABLE
	{
		.name = "flashdel",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_flashdel,
		.description = "Delete flash",
		.syntax = "<address> <block/sector>",
		.commandArgNum = CommandArgument_2,
	},

	{
		.name = "flashread",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_flashread,
		.description = "Read from flash",
		.syntax = "<address>",
		.commandArgNum = CommandArgument_1,
	},

	{
		.name = "flashwrite",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_flashwrite,
		.description = "Write flash",
		.syntax = "<address> <byte>",
		.commandArgNum = CommandArgument_2,
	},
#endif

	/*
	{
		.name = "stop",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_stop,
		.description = "stop\r\n"
	},
	*/

	/*
	{
		.name = "start",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_start,
		.description = "start\r\n"
	},
	*/

	/*
	{
		.name = "buzzer",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_buzzer,
		.description = "buzzer\r\n"
		"Syntax: buzzer on/off\r\n"
		"Function: trivial\r\n"
	},
	*/

	/*
	{
		.name = "dl",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_dl,
		.description = "dl\r\n"
					"Syntax: dl <destination> <size>\r\n"
					"  <destination>: destination address (hexadecimal)\r\n"
					"  <size>: byte's num (decimal)\r\n"
					"Function: download from USART to destination address\r\n"
	},
	*/

	/*
	{
		.name = "esp8266",
		.commandFunctionPointer = ( FunctionPointer *)CommandFunction_ESP8266,
		.description = "ESP8266 wifi modul bridge\r\n"
	},
	*/

};


#pragma GCC diagnostic pop
// End of warning suppress


const CommandID_t MONITOR_CommandNum = MONITOR_MAX_COMMAND_NUM;


/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/




/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local functions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/


//////////////////////////////////////////////////////////////////////////////
/*							COMMAND FUNCTIONS 								*/
//////////////////////////////////////////////////////////////////////////////


/**
 * \brief	CLS: Clear screen
 */
CommandResult_t CommandFunction_cls ( uint32_t argc, char** argv )
{

	MONITOR_SendCls();

	return CommandResult_Ok;
}



/**
 * \brief	Get version
 */
CommandResult_t CommandFunction_version ( uint32_t argc, char** argv )
{
	MONITOR_SendLine(Global_Version);

	return CommandResult_Ok;
}



/**
 * \brief	Send Welcome message
 */
CommandResult_t CommandFunction_welcome ( uint32_t argc, char** argv )
{
	MONITOR_SendWelcome();

	return CommandResult_Ok;
}



/**
 * \brief	Help command
 * 			Use: 'help', or 'help <commandname>'
 * 			List commands or write the command's decription
 */
CommandResult_t CommandFunction_help ( uint32_t argc, char** argv )
{

	uint8_t i;

	if (argc == 1)
	{
		// if Arg2 is empty, listing all commands
		MONITOR_SendLine("Using help:\r\n"
				"help <command>\r\n\r\n"
				"Commands list:");
		for (i=0; i < MONITOR_MAX_COMMAND_NUM; i++)
		{
			// Write a command
			MONITOR_SendLine(CommandList[i].name);
		}
		return CommandResult_Ok;
	}
	else if (argc == 2)
	{
		// Arg2 not empty, find the command
		for (i=0; i < MONITOR_MAX_COMMAND_NUM; i++)
		{
			// Find the command
			if (!StrCmp(CommandList[i].name,argv[1]))
			{
				// Command's describe
				MONITOR_WriteCommandHelp(i);
				return CommandResult_Ok;
			}
		}

		duprintf(MONITOR_CommandSource, "Not find command: %s\r\n", argv[1]);
		return CommandResult_Ok;
	}

	return CommandResult_Error_Unknown;
}



/**
 * \brief	Reset command
 */
CommandResult_t CommandFunction_reset ( uint32_t argc, char** argv )
{

	//(void)argc;
	//(void)argv;
	uint16_t i;

	MONITOR_SendLine("Reset...");
	for(i=0; i<1000; i++);

	NVIC_SystemReset();

	for(i=0; i<1000; i++);

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
CommandResult_t CommandFunction_led ( uint32_t argc, char** argv )
{
	// TODO: Szépíteni

	// Convert arg2, decimal
	bool isFirstParamNum = false;
	uint32_t Arg2Num;

	if (StringToUnsignedDecimalNum(argv[1],&Arg2Num))
	{
		isFirstParamNum = true;
	}

	// Check parameters
	if (isFirstParamNum == true && (Arg2Num > LED_NUM_MAX || Arg2Num < LED_NUM_MIN))
	{
		return CommandResult_Error_WrongArgument1;
	}
	else if (isFirstParamNum == false)
	{
		// Not number, check it is color?
		uint8_t ledNum = LED_GetNumFromName(argv[1]);
		if (ledNum != 0)
		{
			Arg2Num = ledNum;
			isFirstParamNum = true;
		}
	}

	// Check 2. arg
	if (isFirstParamNum == false && !StrCmp(argv[1], "status"))
	{
		// "status"
		duprintf(MONITOR_CommandSource, "Led status: %d %d %d\r\n",
				LED_RED_STATUS(),
				LED_BLUE_STATUS(),
				LED_GREEN_STATUS());
	}
	else if (isFirstParamNum == true)
	{
		// 1. param = num (LED num)

		// Get type
		LED_SetType setType = LED_SET_DONTCARE;
		bool status = false;
		setType = LED_GetTypeFromString(argv[2]);

		if (setType == LED_SET_DONTCARE)
		{
			// Error, do nothing
			return CommandResult_Error_WrongArgument2;
		}

		// Set LED
		status = LED_SetLed(Arg2Num,setType);
		duprintf(MONITOR_CommandSource, "LED %d. status: %d\r\n",Arg2Num, status);

	}
	else
	{
		// First param is not status and not LED num
		return CommandResult_Error_WrongArgument1;
	}

	
	return CommandResult_Ok;

}
#endif



#ifdef CONFIG_MODULE_ADC_ENABLE
/**
 * \brief	Temperature
 * 			Read temperature and Vref values
 */
CommandResult_t CommandFunction_temp	( uint32_t argc, char** argv ) {

	//uprintf("Temperature: %d [C]\r\n",ADC_GetTemp());
	//uprintf("Vref: %d [mV]\r\n",ADC_GetVref());
	

	ADC_ConvertAllMeasuredValues();
	
	uprintf("Temperature: ");
	USART_SendFloat(ADC_ConvertedValue_InternalTemperature);
	uprintf(" [C]\r\n");
	

	return CommandResult_Ok;
}
#endif



/**
 * \brief	Test function
 */
CommandResult_t CommandFunction_test	( uint32_t argc, char** argv ) {
	
	//(void)argc;
	//(void)argv;
	//uint8_t i = 0;
	//uint8_t buf[2];
	
	MONITOR_SendLine("Test start");



	// GlobalVar

	//GlobalVarHandler_ListAllVariableParameters();

	/*
	char buffer[30];
	GlobalVarHandler_ProcessCommand("TestVar",NULL,SetGet_Get,Source_DebugSerial,
			buffer,20);

	uprintf("%s\r\n",buffer);

	GlobalVarHandler_ProcessCommand("TestString",NULL,SetGet_Get,Source_DebugSerial,
			buffer,30);

	uprintf("%s\r\n",buffer);
	*/

	// Temperature test
	
	
	//TEMPERATURE_I2C_Init();
	
	
	
	
	
	// ADC
	//ADC_Test();
	
	
	
	// LED TEST
	//LED_Init();

	//LED_On (DESIGNLED_ALL);
	//LED_Off(DESIGNLED_INDEX_RIGHT);

	
	
	

	// Proximity test
	/*
	while (i++ < 100)
	{
		//uprintf("Front proximity value: %d [cm]\r\n",PROX_MeaseuredValueToDistanceConverter( ProxAdcValues[PROX_SENSOR_FRONT] ));
		if ( HAL_UART_Receive(&Debug_UartHandle,buf,1,50) == HAL_OK ) return 1;
		//vTaskDelay(200);
	}
	*/



	
	
	
	
	// Position teszt
	//LOG_QueueSendMessage(LOG_MESSAGE_TYPE_POSITION,0,0);




	// Teszt a jumper lerantashoz

	//
	/*
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable clock
	CONTROL_START_GPIO_CLK_ENABLE();

	// Set settings
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	//GPIO_InitStruct.Alternate =
	GPIO_InitStructure.Pin = CONTROL_START_GPIO_PIN;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;


	HAL_GPIO_Init(CONTROL_START_GPIO_PORT, &GPIO_InitStructure);

	while ( HAL_GPIO_ReadPin(CONTROL_START_GPIO_PORT,CONTROL_START_GPIO_PIN) == 1);
	//
	*/


	/*
	// LOG queue teszt
	int16_t testbuffer[12];

	for (int i=0; i<12; i++) testbuffer[i] = i+1;

	if ( LOG_QueueSendMessage(LOG_MESSAGE_TYPE_EXAMPLE,1, testbuffer[1]) != RETURN_SUCCESS )
	{
		uprintf("Sikertelen elkuldes!\r\n");
	}
	if ( LOG_QueueSendMessage(LOG_MESSAGE_TYPE_EXAMPLE,1, testbuffer[1]) != RETURN_SUCCESS )
	{
		uprintf("Sikertelen elkuldes!\r\n");
	}
	if ( LOG_QueueSendMessage(LOG_MESSAGE_TYPE_EXAMPLE,1, testbuffer[1]) != RETURN_SUCCESS )
	{
		uprintf("Sikertelen elkuldes!\r\n");
	}
	if ( LOG_QueueSendMessage(LOG_MESSAGE_TYPE_EXAMPLE,1, testbuffer[1]) != RETURN_SUCCESS )
	{
		uprintf("Sikertelen elkuldes!\r\n");
	}
	*/



	//SystemClock_Config();
	//HAL_NVIC_ClearPendingIRQ(0xFFFFFFFF);

	/*
	// TEST az uprintf() fuggvenyhez
	uprintf("elso sor\r\n"
			"masodik sor, szam: %d\r\n"
			"harmadik, szam: %d\r\n"
			"\r\n",
			230,
			520);
	

	vTaskDelay(10);
	
	uprintf("blabla\r\n");
	uprintf("blabla\r\n");
	uprintf("blabla\r\n");
	uprintf("blabla\r\n");


	uprintf("elso sor\r\n"
			"masodik sor, szam: %d\r\n"
			"harmadik, szam: %d\r\n"
			"\r\n",
			230,
			520);
	*/

	MONITOR_SendLine("Test end");

	return CommandResult_Ok;

}



/**
 * \brief	set global variable
 * 			Use: 'set <globalvariablename> <value>'
 */
CommandResult_t CommandFunction_set ( uint32_t argc, char** argv )
{

	char resultBuffer[30];

	GlobalVarHandler_ProcessCommand(
			argv[1], argv[2],
			SetGet_Set, CommProt_DebugUart,
			resultBuffer, 30);

	MONITOR_SendLine(resultBuffer);


	return CommandResult_Ok;

}



/**
 * \brief	Get globalvar value
 * 			Use: 'get <globalvarname>'
 */
CommandResult_t CommandFunction_get ( uint32_t argc, char** argv )
{

	char resultBuffer[30];

	GlobalVarHandler_ProcessCommand(
			argv[1], argv[2],
			SetGet_Get, CommProt_DebugUart,
			resultBuffer, 30);

	MONITOR_SendMessage(resultBuffer);

	return CommandResult_Ok;

}



/**
 * \brief	Global variable help
 * 			Use: '? <globalvar>'
 */
CommandResult_t CommandFunction_GlobalVariableHelp ( uint32_t argc, char** argv )
{

	char resultBuffer[80];

	GlobalVarHandler_ProcessCommand(
			argv[1], argv[2],
			SetGet_Help, CommProt_DebugUart,
			resultBuffer, 80);

	MONITOR_SendMessage(resultBuffer);


	return CommandResult_Ok;

}



/**
 * \brief	List global variables
 */
CommandResult_t CommandFunction_GlobalVariableList ( uint32_t argc, char** argv )
{

	GlobalVarHandler_ListAllVariableParameters();

	return CommandResult_Ok;

}



/**
 * \brief	Print all global variable values
 */
CommandResult_t CommandFunction_GlobalVariableValueList ( uint32_t argc, char** argv )
{

	GlobalVarHandler_PrintAllVariableValues();

	return CommandResult_Ok;

}



#ifdef CONFIG_MODULE_FLASH_ENABLE
/**
 * \brief	Flash erase
 * 			Use: 'flashdel <address> <block/sector>'
 */
CommandResult_t CommandFunction_flashdel	( uint32_t argc, char** argv )
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
			
	duprintf(MONITOR_CommandSource, "address erased: 0x%h\r\n",
			Arg2Num
			);
	
	return CommandResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_FLASH_ENABLE
/**
 * \brief	Flash read
 * 			Use: 'flashread <address>'
 */
CommandResult_t CommandFunction_flashread	( uint32_t argc, char** argv )
{
	
	uint32_t Arg2Num;

	// Convert arg2 hex
	if ( !StringHexToNum(argv[1],&Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}


	uint8_t Buffer[1];
	FLASH_Read(Arg2Num,Buffer,1,5000);
	
	
	duprintf(MONITOR_CommandSource, "address: 0x%w\r\n"
			"data:    0x%b\r\n",
			Arg2Num,
			Buffer[0]
			);
	
	return CommandResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_FLASH_ENABLE
/**
 * \brief	Flash write
 * 			Use: 'flashwrite <address> <data>'
 */
CommandResult_t CommandFunction_flashwrite	( uint32_t argc, char** argv )
{
	uint32_t Arg2Num;

	// Convert arg2 hex
	if ( !StringHexToNum(argv[1],&Arg2Num))
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
	

	duprintf(MONITOR_CommandSource, "address: 0x%w\r\n"
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
CommandResult_t CommandFunction_raspberrypi (uint32_t argc, char** argv)
{
	
	// Check arg 2
	if (!StrCmp(argv[1],"setout"))
	{
		// setout
		/*
		HOMEAUTMESSAGE_CreateAndSendHomeAutMessage(
			0,255,
			Function_Command, Command_SetOutput,
			Arg3Num, 1);
		*/

		// Convert arg3, decimal
		if (!StringToUnsignedDecimalNum(argv[2],&Arg3Num))
		{
			return CommandResult_Error_WrongArgument2;
		}

		if (Arg3Num > 0)
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



#ifdef CONFIG_MODULE_DAC_ENABLE
/**
 * \brief	DAC function
 * 			Use: 'dac <channel> <voltage>'
 */
CommandResult_t CommandFunction_dac (uint32_t argc, char** argv)
{
	uint32_t Arg2Num;
	float voltage = 0.0f;

	if (!StringToUnsignedDecimalNum(argv[1],&Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}

	if (!StringToFloat(argv[2],&voltage))
	{
		return CommandResult_Error_WrongArgument2;
	}

	if (DAC_SetValue(Arg2Num,voltage))
	{
		MONITOR_SendLine("Set ok!");
		return CommandResult_Ok;
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
CommandResult_t CommandFunction_unittest (uint32_t argc, char** argv)
{

#ifdef MODULE_STRING_UNIT_TEST_ENABLED
	STRING_UnitTest();
#endif

	return CommandResult_Ok;
}



/**
 * \brief	Run module test
 */
CommandResult_t CommandFunction_moduletest (uint32_t argc, char** argv)
{

	uint8_t i;

	// LED test
	MONITOR_SendLine("LED test");

	// LEDs on
	for (i=LED_NUM_MIN; i<=LED_NUM_MAX; i++)
	{
		LED_SetLed(i, LED_SET_ON);
		DelayMs(500);
	}

	// LEDs off
	for (i=LED_NUM_MIN; i<=LED_NUM_MAX; i++)
	{
		LED_SetLed(i, LED_SET_OFF);
		DelayMs(500);
	}


	// Beep in terminal
	MONITOR_SendLine("Beep test");
	MONITOR_SendChar(USART_KEY_BELL);


	// Send formatted messages
	MONITOR_SendLine("Formatted message test");
	FormattedMessage_Test();


#ifdef CONFIG_MODULE_STL_SELFTEST_ENABLE
	// STL selftest
	MONITOR_SendLine("STL selftest");
	while(!STL_FlashResult)
	{
		STL_DoRunTimeChecks();
	}
	if (STL_FlashResult == 1)
	{
		MONITOR_SendLine("STL Flash CRC test result is OK");
	}
	else
	{
		MONITOR_SendLine("STL Flash CRC test result is Failed");
	}

#endif

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
CommandResult_t CommandFunction_io (uint32_t argc, char** argv)
{

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
			duprintf(MONITOR_CommandSource, "Output status: %d\r\n", status);
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
			duprintf(MONITOR_CommandSource, "Input status: %d\r\n", status);
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
CommandResult_t CommandFunction_adc (uint32_t argc, char** argv)
{

	uint8_t i;

	ADC_ConvertAllMeasuredValues();

	for (i=0; i<ADC_BUFFER_SIZE; i++)
	{
		duprintf(MONITOR_CommandSource,"ADC: %d. value: %2.2f\r\n", i, ADC_ConvertedValues[i]);
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
	// TODO: Elegánsabban megcsinálni:
	uint8_t uartWriteCntOld = USART_RxBufferWriteCounter;

	if (StringToUnsignedDecimalNum(argv[1],&convertValue))
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
		if (StringToUnsignedDecimalNum(argv[2],&convertValue))
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
			duprintf(MONITOR_CommandSource,"ADC: %d. value: %2.2f\r\n", i, ADC_ConvertedValues[i]);
		}

		// Delay
		DelayMs(milliSec);

		if (uartWriteCntOld != USART_RxBufferWriteCounter)
		{
			// Received a new char
			MONITOR_SendLine("ADC read is interrupted");
			break;
		}
	}

	return CommandResult_Ok;
}
#endif



#if 0
// Function: dl (download)
// dl <destination> <size>
// Download to <destination> address <size> bytes from USART
CommandResult_t CommandFunction_dl ( uint32_t argc, char** argv )
{

	unsigned char * destination;
	uint32_t size;
	uint32_t Arg2Num;
	uint32_t Arg3Num;

	if ( argc < 3 ) { USART_SendString("Too few arguments!\r\n"); return 0; }
	if ( argc > 3 ) { USART_SendString("Too many arguments!\r\n"); return 0; }

	if ( StringIsHexadecimalString(argv[1]) ) Arg2Num = StringHexToNum(argv[1]);	// <destination> 	Convert hex to num
	else { USART_SendString("Wrong 1. argument!\r\n"); return 0; }
	if ( StringIsUnsignedDecimalString(argv[2]) ) Arg3Num = StringToUnsignedDecimalNum(argv[2]);	// <size>			Convert dec to num
	else { USART_SendString("Wrong 2. argument!\r\n"); return 0; }

	size = Arg3Num;
	destination = (unsigned char *)Arg2Num;


	// Ellenorizni kell, hogy uint16_t -e
	if ( size > UINT16_MAX )
	{
		USART_SendString("The <size> is too large. Only work with maximum UINT16_MAX\r\n");
		return RETURN_FALSE;
	}

	duprintf(MONITOR_CommandSource, "Destination: 0x%w\r\n"
			"Size: %d bytes\r\n",
			destination,
			size);



	USART_SendString("Please send the binary file...\r\n");


	/*
	for ( i = 0; i < size; i++) {			// Loop: receive char and write, while  <size> byte-s not received.
		//byte = USARTReceiveChar();		// TODO: helyette mas kell
		HAL_UART_Receive(&UartHandle,&byte,1,1000);
		*destination = byte;
		destination++;
	}
	*/
	//taskENTER_CRITICAL();				// TODO: FREERTOS

	// TODO:
	//
	/*
	uint8_t flash_status;

	void save_data_to_flash() {

	  flash_status = FLASH_COMPLETE;

	  FLASH_Unlock();
	  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	  flash_status = FLASH_EraseSector(FLASH_Sector_3, VoltageRange_3);

	  if (flash_status != FLASH_COMPLETE) {
	    FLASH_Lock();
	    return;
	  }

	  uint8_t* address = &flash_data[0];

	  //program first run status bit
	  flash_status = FLASH_ProgramByte((uint32_t)address, 0x00);
	  address += 4;
	  if (flash_status != FLASH_COMPLETE) {
	    FLASH_Lock();
	    return;
	  }

	}
	*/
	//*/

	//HAL_UART_Receive(&UartHandle,destination,size,0xFFFFFFFF);

	MONITOR_SendLine("Jelenleg teszteles alatt, a FLASH programozassal gond van.");
	//FLASH_Test();

		//taskEXIT_CRITICAL(); 				// TODO: FREERTOS

	USART_SendString("Arrived the binary file.\r\n");

	return CommandResult_Ok;
}
#endif



#if 0
// Function: go (jump to an address)
// go <destination>
// jump <destination> address
CommandResult_t CommandFunction_go ( uint32_t argc, char** argv ) {

	uint32_t destination;
	int ( *fpntr )( void );
	uint32_t Arg2Num;

	// TODO: USART_SendLine();
	// TODO: MONITOR_SendMessage(); ez irányítódjon át egy USART_SendString()-re

	// Convert hex
	if ( !StringHexToNum(argv[1],&Arg2Num,0))
	{
		return CommandResult_Error_WrongArgument1;
	}

	destination = Arg2Num;
	duprintf(MONITOR_CommandSource, "Go destination: 0x%w\r\n",
			destination);


	fpntr = (int (*) (void))destination;		// casting
	fpntr();

	// Now, for example: "go 20000151"

	// We can use the "jump" ASM instruction, but not a good idea
	// Programming manual page 92, B instruction

	return CommandResult_Ok;
}
#endif



#if 0
// Function: mr (memory read)
// mr <source> <size>
// source: hex
// size: dec
// read <size> byte-s from <source> address
CommandResult_t CommandFunction_mr ( uint32_t argc, char** argv ) {


	uint32_t *source;
	uint32_t *p;
	unsigned short int size;
	uint32_t i;
	uint32_t Arg2Num;
	uint32_t Arg3Num;

	if ( argc < 3 ) {	USART_SendString("Too few arguments!\r\n");	return 0;	}
	if ( argc > 3 ) {	USART_SendString("Too many arguments!\r\n");	return 0;	}

	if ( StringIsHexadecimalString(argv[1]) ) Arg2Num = StringHexToNum(argv[1]);	// Convert arg2, source to hex
	else { USART_SendString("Wrong 1. argument!"); return 2; }
	if ( StringIsUnsignedDecimalString(argv[2]) ) Arg3Num = StringDecToNum(argv[2]);	// Convert arg3, size to dec
	else { USART_SendString("Wrong 2. argument!"); return 2; }

	source = ( uint32_t *) Arg2Num;		// casting for valid numbers
	size = ( unsigned short int ) Arg3Num;	// <size> max 256
	// TODO: checking the correct address

	duprintf(MONITOR_CommandSource, "Source: 0x%w\r\n"
			"Size: %d\r\n",
			source,size);

	i=0;
	for (p = source; p < source+size/4; p++) {
		if ( !(i % 4) ) duprintf(MONITOR_CommandSource, "\r\n 0x%w:",p);
		duprintf(MONITOR_CommandSource, " %w",*p);
		i++;
	}

	return 1;
}
#endif



#if 0
// FunctioN: mw - memory write
// mwb <destination> <data>
// destination: hex
// data: hex
// mwb, mwh, mww commands
CommandResult_t CommandFunction_mw ( uint32_t argc, char** argv ) {

	unsigned char *destination1;
	unsigned short int *destination2;
	uint32_t *destination3;
	uint32_t Arg2Num;
	uint32_t Arg3Num;

	if ( argc < 3 ) { USART_SendString("Too few arguments!\r\n");	return 0; }
	if ( argc > 3 ) { USART_SendString("Too many arguments!\r\n"); return 0; }

	if ( StringIsHexadecimalString(argv[1]) ) Arg2Num = StringHexToNum(argv[1]);	// Convert hex
	else { USART_SendString("Wrong 1. argument!"); return 2; }
	if ( StringIsHexadecimalString(argv[2]) ) Arg3Num = StringHexToNum(argv[2]);	// Convert hex
	else { USART_SendString("Wrong 2. Argument!"); return 2; }


	if (!StrCmp(argv[0],"mwb")) {
		destination1 = ( unsigned char *) Arg2Num;
		*destination1 = ( unsigned char ) Arg3Num;
		duprintf(MONITOR_CommandSource, "Write: %b to: %w",Arg3Num,Arg2Num);
	}
	else if (!StrCmp(argv[0],"mwh")) {
		destination2 = ( unsigned short int *) Arg2Num;
		*destination2 = ( unsigned short int) Arg3Num;
		duprintf(MONITOR_CommandSource, "Write: %h to: %w",Arg3Num,Arg2Num);
	}
	else if (!StrCmp(argv[0],"mww")) {
		destination3 = ( uint32_t *) Arg2Num;
		*destination3 = ( uint32_t ) Arg3Num;
		duprintf(MONITOR_CommandSource, "Write: %w to: %w",Arg3Num,Arg2Num);
	}


	return 1;
}
#endif



#if 0
// Function: Go to STOP mode
CommandResult_t CommandFunction_stop	( uint32_t argc, char** argv ) {

	// Take anything before stop mode
	LED_ALARM_ON();
	LCD_Instr_DisplayClear();
	LCD_SendString_2line("RadioAlarm","in STOP mode",2,2);

	MONITOR_SendLine("Go to STOP mode...\r\n"
					"Wake up with USER button"
					);

	LOWPOWER_GotoStopMode();

	LED_ALARM_OFF();
	LCD_Instr_DisplayClear();
	LCD_SendString_2line("RadioAlarm","",2,2);
	MONITOR_SendLine("End of STOP mode\r\n");

	return 1;
}
#endif



#if 0
// Function: EEPROM read
// Syntax: romr <address>
CommandResult_t CommandFunction_romr	( uint32_t argc, char** argv ) {

	uint32_t Arg2Num;

	Arg2Num = StringDecToNum(argv[1]);

	//duprintf(MONITOR_CommandSource, "address: 0x%h\r\n"
	//				"data:    0x%b\r\n",
	//				Arg2Num,
	//				EEPROM_ReadByte(Arg2Num)
	//				);

	return 1;
}
#endif



#if 0
// Function: EEPROM write
// Syntax: romw <address> <data>
CommandResult_t CommandFunction_romw	( uint32_t argc, char** argv ) {

	uint32_t Arg2Num;
	uint32_t Arg3Num;

	Arg2Num = StringHexToNum(argv[1]);
	Arg3Num = StringHexToNum(argv[2]);

	duprintf(MONITOR_CommandSource, "address: 0x%h\r\n"
					"data:    0x%b\r\n",
					Arg2Num,
					Arg3Num);
	//EEPROM_WriteByte(Arg2Num,Arg3Num);

	return 1;
}
#endif



#if 0
// Function: Read EEPROM's status register
CommandResult_t CommandFunction_romsr	( uint32_t argc, char** argv ) {

	//duprintf(MONITOR_CommandSource, "Status register: 0x%h\r\n",
	//				EEPROM_ReadStatusRegister ()
	//				);

	return 1;
}
#endif



#if 0
// Function: EEPROM, Write Enable
CommandResult_t CommandFunction_romwe	( uint32_t argc, char** argv ) {

	//EEPROM_WriteEnable ();

	MONITOR_SendLine("EEPROM write enable");

	return 1;
}
#endif



#if 0
// Function: initialize EEPROM
CommandResult_t CommandFunction_rominit	( uint32_t argc, char** argv ) {

	//EEPROM_Init ();

	MONITOR_SendLine("EEPROM initialized\r\n");

	return 1;
}
#endif



#if 0
CommandResult_t CommandFunction_standby	( uint32_t argc, char** argv ) {

	LCD_Instr_DisplayClear();
	LCD_SendString_2line("RadioAlarm","in STANDBY mode",2,2);

	MONITOR_SendLine("Go to STANDBY mode. You can wake up(=reset) with RESET and USER button!");

	LOWPOWER_GotoSTANDBYMode ();

	// Exit from STANDBY mode is same with reset, this point is never reachable

	return 1;
}
#endif



#if 0
CommandResult_t CommandFunction_rtc	( uint32_t argc, char** argv ) {

	uint32_t Arg2Num;

	Arg2Num = StringDecToNum(argv[1]);

	duprintf(MONITOR_CommandSource, "RTC wait %d second(s)\r\n",Arg2Num);

	//RTC_WaitSeconds(Arg2Num);

	MONITOR_SendLine("End");

	return 1;
}
#endif



#if 0
CommandResult_t CommandFunction_start	( uint32_t argc, char** argv )
{
	(void)argc;
	(void)argv;


	MONITOR_SendLine("start command...\r\n");


	// Auto elinditasa a GLOBAL_CommandStartXXX() fuggvennyel
	//GLOBAL_CommandStartMonitor();


	return CommandResult_Ok;
}
#endif



#if 0
CommandResult_t CommandFunction_stop	( uint32_t argc, char** argv )
{

	//(void)argc;
	//(void)argv;

	//GLOBAL_CommandEmergencyStop();

	MONITOR_SendLine("$ Stop command! $");

	return CommandResult_Ok;

}
#endif



#if 0
// Function: buzzer switch on/off
CommandResult_t CommandFunction_buzzer	( uint32_t argc, char** argv ) {

	if ( argc < 2 )
	{
		MONITOR_SendLine("Too few arguments!");
		return RETURN_FALSE;
	}
	if ( argc > 2 )
	{
		MONITOR_SendLine("Too many arguments!");
		return RETURN_FALSE;
	}

	if (!StrCmp(argv[1],"on"))
	{
		// TODO: Buzzer on
		// Buzzer bekapcsolasa
		//BUZZER_Test_Task ();	// Buzzer taszk, nem a legjobb megoldas!
		MONITOR_SendLine("Buzzer on (Important! This is an infinite loop!)");
		//BUZZER_On(); // Important!! Before turn on, need BUZZER_Init() function.
	}
	else if (!StrCmp(argv[1],"off"))
	{
		// Buzzer kikapcsolása
		//BUZZER_DeInit();
		MONITOR_SendLine("Buzzer off");
	}

	return CommandResult_Ok;
}
#endif




#if 0
CommandResult_t CommandFunction_accelerometer	( uint32_t argc, char** argv ) {

	(void)argc;
	(void)argv;

	MONITOR_SendLine("Accelerometer has been started...");

	// TODO:
	//ACCELEROMETER_Task();
	//vTaskResume(ACCELEROMETER_TaskHandle);

	MONITOR_SendLine("End");

	return CommandResult_Ok;
}
#endif



#if 0
CommandResult_t CommandFunction_gyroscope	( uint32_t argc, char** argv ) {

	(void)argc;
	(void)argv;

	MONITOR_SendLine("Gyroscope has been started...");

	// TODO:
	//GYROSCOPE_Task ();
	//vTaskResume(GYROSCOPE_TaskHandle);

	MONITOR_SendLine("End");

	return CommandResult_Ok;
}
#endif




#if 0
CommandResult_t CommandFunction_remotecontrol	( uint32_t argc, char** argv ) {


	return CommandResult_Ok;
}
#endif



#if 0
CommandResult_t CommandFunction_proximity	( uint32_t argc, char** argv ) {

	//(void)argc;
	//(void)argv;

	MONITOR_SendLine("Jelenleg nincs funkcioja!");

//	if ( argc < 2 )
//	{
//		MONITOR_SendLine("Too few arguments!\r\n"
//				" Use: \"prox on\" or \"prox off\"");
//
//		return RETURN_FALSE;
//	}
//	if ( argc > 2 )
//	{
//		MONITOR_SendLine("Too many arguments!");
//		return RETURN_FALSE;
//	}
//
//
//	if (!StrCmp(argv[1],"on"))
//	{
//
//		// LOG start
//		MONITOR_SendLine("Proximity log on");
//		LOG_QueueEnable_PROXIMITY = LOG_ENABLE;
//
//	}
//	else if (!StrCmp(argv[1],"off"))
//	{
//
//		// LOG end
//		LOG_QueueEnable_PROXIMITY = LOG_DISABLE;
//		MONITOR_SendLine("Proximity log off");
//
//	}

	//MONITOR_SendLine("End");

	return CommandResult_Ok;
}
#endif



#if 0
CommandResult_t CommandFunction_log ( uint32_t argc, char** argv )
{

	//(void)argc;

	if ( argc < 2 )
	{
		MONITOR_SendLine("Too few arguments!\r\n"
				" Use: \"log on\" or \"log off\" or \"log on <task>\"");

		return RETURN_FALSE;
	}
//	if ( argc > 2 )
//	{
//		MONITOR_SendLine("Too many arguments!");
//		return RETURN_FALSE;
//	}

	if ( argc == 2 )
	{
		if (!StrCmp(argv[1],"off"))
		{
			// LOG end
			//LOG_Off();
			/*
			LOG_QueueEnable_LSA = LOG_DISABLE;
			LOG_QueueEnable_LSA2 = LOG_DISABLE;
			LOG_QueueEnable_LSA3 = LOG_DISABLE;
			LOG_QueueEnable_ACCELEROMETER= LOG_DISABLE;
			LOG_QueueEnable_GYROSCOPE = LOG_DISABLE;
			LOG_QueueEnable_PROXIMITY = LOG_DISABLE;
			LOG_QueueEnable_ROTARY = LOG_DISABLE;
			LOG_QueueEnable_POSITION = LOG_DISABLE;
			LOG_QueueEnable_MOTOR = LOG_DISABLE;
			LOG_QueueEnable_SMstatus = LOG_DISABLE;
			LOG_QueueEnable_SMevent = LOG_DISABLE;
			LOG_QueueEnable_EXAMPLE = LOG_DISABLE;
			LOG_QueueEnable_EXAMPLE2 = LOG_DISABLE;
			LOG_QueueEnable_EXAMPLE3 = LOG_DISABLE;

			xSemaphoreGive(LOG_TaskKill_Semaphore);
			*/
			MONITOR_CommandSendBackCharEnable = 1;	// Enable to send
			MONITOR_SendLine("Log off");
		}
		else
		{
			MONITOR_SendLine("Wrong! Use \"log on <task>\" or \"log off\"");
			return RETURN_FALSE;
		}

	}

	/*
	if ( argc == 2 )		// "log on", "log off"
	{
		if (!StrCmp(argv[1],"on"))
		{

			MONITOR_SendLine("Log on");

			// LOG start
			vTaskResume(LOG_TaskHandle);

		}
		else if (!StrCmp(argv[1],"off"))
		{

			// LOG end
			xSemaphoreGive(LOG_TaskKill_Semaphore);

			//vTaskSuspend(LOG_TaskHandle );

			//if ( xSemaphoreTake(LOG_TaskKill_Semaphore,10) == pdTRUE)
			//{
			//	vTaskDelete(LOG_TaskHandle);
			//}

			MONITOR_SendLine("Log off");
		}
	}
	else */
	if (argc == 3 )	// log on/off <taszk>
	{
		if (!StrCmp(argv[1],"on"))
		{

			MONITOR_CommandSendBackCharEnable = 1; // Enable to send
		}
		else
		{
			MONITOR_SendLine("2. argumentum rossz! \"on\" vagy \"off\" kell legyen!");
			return RETURN_FALSE;
		}
	}
	else
	{
		return RETURN_FALSE;
	}

	return CommandResult_Ok;

}
#endif



#if 0
CommandResult_t CommandFunction_exit ( uint32_t argc, char** argv )
{
	(void)argc;
	(void)argv;


	MONITOR_SendLine("exit\r\n"
			" MONITOR program leallitasa...");
	//vTaskSuspend(DEBUG_TaskHandle);				// TODO: FREERTOS

	return CommandResult_Ok;
}
#endif



#if 0
CommandResult_t CommandFunction_read ( uint32_t argc, char** argv )
{

	//int i;
	// Distance kiiratas
	//float GLOBAL_GetDistance(void)
	/*
	float *pFloat;
	float temp;
	uint8_t String[30];

	temp = GLOBAL_GetDistance();

	pFloat = &temp;
	FloatToString(*pFloat, String);

	duprintf(MONITOR_CommandSource, "Distance: %s\r\n",String);
	*/


	// Rotary ertek kikuldese, utolso 200

	uint32_t localDataRaw[200];
	float localDataAvg[200];
	int i;
	float *pFloat;
	uint8_t String[20];

	(void)argc;
	(void)argv;

	for ( i=0; i<200; i++ )
	{
		//localDataRaw[i] = dataRaw[i];
		//localDataAvg[i] = dataAvg[i];
	}


	MONITOR_SendLine("\r\n"
			"Raw:");
	#ifdef CONFIG_USE_FREERTOS
	vTaskDelay(1);
	#else
	HAL_Delay(1);
	#endif
	for (i=0; i<200; i++ )
	{
		duprintf(MONITOR_CommandSource, "%d\r\n",localDataRaw[i]);
		#ifdef CONFIG_USE_FREERTOS
		vTaskDelay(1);
		#else
		HAL_Delay(1);
		#endif
	}

	MONITOR_SendLine("\r\n"
			"Avg:");
	for (i=0; i<200; i++ )
	{
		//duprintf(MONITOR_CommandSource, "%d\r\n",localDataAvg[i]);	// egyszeru kiiratas, int. De az Avg float!

		pFloat = &localDataAvg[i];
		FloatToString(*pFloat, String);

		MONITOR_SendLine(String);
	}


	return CommandResult_Ok;

}
#endif



#if 0
// Function: ESP8266 bridge
CommandResult_t CommandFunction_ESP8266	( uint32_t argc, char** argv ) {

	if ( argc > 1 )
	{
		MONITOR_SendLine("Too many arguments!");
		return RETURN_FALSE;
	}



	MONITOR_RemoteControl = 1;

	HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)MONITOR_RemoteControlBuffer, 1);	// TODO: hibalehetoseg: egy mar meglevo receive_IT utan hivjuk ezt

	USART_SendString("Type your sendingmessage to ESP8266:\r\n");

	while(1)
	{

		if (xSemaphoreTake(DEBUG_USART_Rx_Semaphore,1000) == pdTRUE)
		{
			// received an char
			uint8_t receivedCharacter = MONITOR_RemoteControlBuffer[0];
			if (receivedCharacter != '\r')
			{
				LED_GREEN_TOGGLE();
				MONITOR_RemoteControlBuffer[1] = '\0';
				ESP8266_SendString(MONITOR_RemoteControlBuffer);
				USART_SendChar(MONITOR_RemoteControlBuffer[0]);
			}
			else if (receivedCharacter == '\r')
			{
				LED_GREEN_TOGGLE();
				MONITOR_RemoteControlBuffer[0] = '\r';
				MONITOR_RemoteControlBuffer[1] = '\n';
				MONITOR_RemoteControlBuffer[2] = '\0';
				USART_SendString("\r\nSended to ESP8266\r\n"
								 "Wait for the response:\r\n");

				// Receive from ESP8266 (and send on debug usart)
				uint8_t isExit = 0;
				HAL_StatusTypeDef status;

				uint8_t exitBuffer[2] = { '\0', '\0' };
				uint8_t buffer[100];
				//exitBuffer[0] == 0;

				// For Exit: wait char from Debug USART
				HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)exitBuffer, 1);

				ESP8266_UartHandle.RxXferCount = 0;
				for (int i=0; i < 256; i++) buffer[i]='\0';

				// Sending command on ESP8266
				ESP8266_SendString(MONITOR_RemoteControlBuffer);	// sending "\r\n"

				while(!isExit)
				{
					//if ( HAL_UART_Receive(&ESP8266_UartHandle, (uint8_t *)MONITOR_RemoteControlBuffer, 1, 100) == HAL_OK)

					status = HAL_UART_Receive(&ESP8266_UartHandle, (uint8_t *)buffer, 256, 10000);
					if (status == HAL_OK || status == HAL_TIMEOUT )
					{
						uint16_t length = ESP8266_UartHandle.RxXferCount;

						if ( length > 0)
						{
							//USART_SendChar(MONITOR_RemoteControlBuffer[0]);	// blokkol és lassú

							//HAL_UART_Transmit(&Debug_UartHandle, (uint8_t *)MONITOR_RemoteControlBuffer, 1,1);	// ez is lassú
							HAL_UART_Transmit(&Debug_UartHandle, (uint8_t *)buffer, length,5000);	// ez is lassú
							// TODO: nagyobb buffer és több karakter várása?

							ESP8266_UartHandle.RxXferCount = 0;
							for (int i=0; i < 256; i++) buffer[i]='\0';
						}

					}
					if (xSemaphoreTake(DEBUG_USART_Rx_Semaphore,1) == pdTRUE)
					{
					//if ( exitBuffer[0] == '\r')
					//{
						isExit = 1;
						USART_SendString("\r\nType your sending message to ESP8266\r\n");
					}
				}
			}

			// Receiving an char from debug uart
			HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)MONITOR_RemoteControlBuffer, 1);
		}
		else
		{
		}

	}

	// Never reached, so it commented
	MONITOR_RemoteControl = 0;

	return CommandResult_Ok;

}
#endif



/* END OF COMMAND FUNCTIONS */
