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

//#include "CommandHandler.h"
//#include "usart.h"
#include "include.h"

#include "command.h"
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
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
extern UART_HandleTypeDef Debug_UartHandle;
#endif


uint32_t Arg2Num;
uint32_t Arg3Num;


const CommandStruct CommandList[] =
{

	{
		.name = "help",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_help,
		.description = "Commands's list, or write command's propertities",
		.syntax = "<command>",
		.CommandArgNum = CommandArgument_0 | CommandArgument_1,
	},
	
	{
		.name = "version",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_version,
		.description = "Get version",
		.syntax = NULL,
		.CommandArgNum = CommandArgument_0,
	},
	
	{
		.name = "welcome",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_welcome,
		.description = "Send welcome message",
		.syntax = NULL,
		.CommandArgNum = CommandArgument_0,
	},

	{
		.name = "reset",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_reset,
		.description = "Software reset",
		.syntax = NULL,
		.CommandArgNum = CommandArgument_0,
	},

	{
		.name = "cls",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_cls,
		.description = "Clear Screen",
		.syntax = NULL,
		.CommandArgNum = CommandArgument_0,
	},

	{
		.name = "test",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_test,
		.description = "...",
		.syntax = NULL,
		.CommandArgNum = CommandArgument_0,
	},

	{
		.name = "set",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_set,
		.description = "set global variable value",
		.syntax = "<varName> <value>",
		.CommandArgNum = CommandArgument_2,
	},

	{
		.name = "get",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_get,
		.description = "get global variable value",
		.syntax = "<varName>",
		.CommandArgNum = CommandArgument_1,
	},

	{
		.name = "?",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_GlobalVariableHelp,
		.description = "help global variable",
		.syntax = "<varName>",
		.CommandArgNum = CommandArgument_1,
	},

	{
		.name = "list",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_GlobalVariableList,
		.description = "List global variable",
		.CommandArgNum = CommandArgument_0,
	},
	
	{
		.name = "led",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_led,
		.description = "Control LED",
		.syntax = "<on/off/toggle/status> <num>",
		.CommandArgNum = CommandArgument_1 | CommandArgument_2,
	},
	{
		.name = "#raspi",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_raspberrypi,
		.description = "Raspberry Pi HomeAutMessage sending",
		.syntax = "-",
		.CommandArgNum = CommandArgument_2
	},
	{
		.name = "dac",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_dac,
		.CommandArgNum = CommandArgument_2
	},
	{
		.name = "unittest",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_unittest,
		.description = "Run unit tests",
		.syntax = "optional: <modul>",
		.CommandArgNum = CommandArgument_0 | CommandArgument_1
	}
#ifdef CONFIG_MODULE_ADC_ENABLE
	{
		.name = "temperature",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_temp,
		.description = "...",
		.syntax = NULL,
		.CommandArgNum = CommandArgument_0,
	},
#endif
#ifdef CONFIG_MODULE_FLASH_ENABLE
	{
		.name = "flashdel",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_flashdel,
		.description = "Delete flash",
		.syntax = "<address> <block/sector>",
		.CommandArgNum = CommandArgument_2,
	},

	{
		.name = "flashread",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_flashread,
		.description = "Read from flash",
		.syntax = "<address>",
		.CommandArgNum = CommandArgument_1,
	},

	{
		.name = "flashwrite",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_flashwrite,
		.description = "Write flash",
		.syntax = "<address> <byte>",
		.CommandArgNum = CommandArgument_2,
	},
#endif

	/*
	{
		.name = "stop",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_stop,
		.description = "stop\r\n"
	},
	*/

	/*
	{
		.name = "start",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_start,
		.description = "start\r\n"
	},
	*/

	/*
	{
		.name = "buzzer",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_buzzer,
		.description = "buzzer\r\n"
		"Syntax: buzzer on/off\r\n"
		"Function: trivial\r\n"
	},
	*/

	/*
	{
		.name = "dl",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_dl,
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
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_ESP8266,
		.description = "ESP8266 wifi modul bridge\r\n"
	},
	*/

};


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

	USART_SEND_CLS();

	return CommandResult_Ok;
}



/**
 * \brief	Get version
 */
CommandResult_t CommandFunction_version ( uint32_t argc, char** argv )
{
	USART_SendLine(Global_Version);

	return CommandResult_Ok;
}



/**
 * \brief	Send Welcome message
 */
CommandResult_t CommandFunction_welcome ( uint32_t argc, char** argv )
{
	MONITOR_SendPrimitiveWelcome();

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
		USART_SendLine("Using help:\r\n"
				"help <command>\r\n\r\n"
				"Commands list:");
		for (i=0; i < MONITOR_MAX_COMMAND_NUM; i++)
		{
			// Write a command
			USART_SendLine(CommandList[i].name);
		}
		return CommandResult_Ok;
	}
	else if (argc == 2)
	{
		// Arg2 not empty, find the command
		for (i=0; i < MONITOR_MAX_COMMAND_NUM; i++)
		{										// Find the command
			if (!StrCmp(CommandList[i].name,argv[1]))
			{
				// Command's describe
				MONITOR_WriteAnCommandHelp(i);
				return CommandResult_Ok;
			}
		}

		USART_SendLine("There is not find command");
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

	USART_SendLine("Reset...");
	for(i=0; i<1000; i++);

	NVIC_SystemReset();

	for(i=0; i<1000; i++);

	return CommandResult_Ok;
}



#ifdef CONFIG_MODULE_LED_ENABLE
/**
 * \brief	Set LED (turn on, turn off, toggle, status)
 * 			Commands:
 *			led on <num>
 *			led off <num>
 *			led toggle <num>
 *			led status <dummy>
 */
CommandResult_t CommandFunction_led ( uint32_t argc, char** argv )
{

	// Convert arg2, decimal
	if (!UnsignedDecimalStringToNum(argv[2],&Arg3Num))
	{
		return CommandResult_Error_WrongArgument2;
	}

	// Check 2. arg
	if (!StrCmp(argv[1], "on"))
	{
		switch (Arg3Num)
		{
			case 1:	LED_BLUE_ON();	break;
			case 2:	LED_GREEN_ON();	break;
			case 3:	LED_RED_ON();	break;
		}
		uprintf("Led on: %d\r\n",Arg3Num);
	}
	else if (!StrCmp(argv[1], "off"))
	{
		switch (Arg3Num)
		{
			case 1:	LED_BLUE_OFF();		break;
			case 2:	LED_GREEN_OFF();	break;
			case 3:	LED_RED_OFF();		break;
		}
		uprintf("Led off: %d\r\n",Arg3Num);
	}
	else if (!StrCmp(argv[1], "toggle"))
	{
		switch (Arg3Num)
		{
			case 1: LED_BLUE_TOGGLE();	break;
			case 2: LED_GREEN_TOGGLE(); break;
			case 3: LED_RED_TOGGLE();	break;
		}
		uprintf("Led toggle: %d\r\n",Arg3Num);
	}
	else if (!StrCmp(argv[1], "status"))
	{
		uprintf("Led status: %d %d %d\r\n",LED_BLUE_STATUS(), LED_GREEN_STATUS(), LED_RED_STATUS());
	}
	else
	{
		USART_SendString("Wrong command!\r\n");
		return CommandResult_Error_WrongArgument1;
	}

#endif
	
	return CommandResult_Ok;

}



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
	
	USART_SendLine("Test start");


	USART_SendSoundBeep();


	FormattedMessage_UnitTest();
	STRING_UnitTest();


	// GlobalVar

	//GlobalVarHandler_ListAllVariables();

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

	USART_SendLine("Test end");

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
			argv[1],argv[2],
			SetGet_Set,Source_DebugSerial,
			resultBuffer,30);

	uprintf("%s\r\n",resultBuffer);


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
			argv[1],argv[2],
			SetGet_Get,Source_DebugSerial,
			resultBuffer,30);

	uprintf("%s\r\n",resultBuffer);


	return CommandResult_Ok;

}



/**
 * \brief	Global variable help
 * 			Use: '? <globalvar>'
 */
CommandResult_t CommandFunction_GlobalVariableHelp ( uint32_t argc, char** argv )
{

	char resultBuffer[30];

	GlobalVarHandler_ProcessCommand(
			argv[1],argv[2],
			SetGet_Help,Source_DebugSerial,
			resultBuffer,30);

	uprintf("%s\r\n",resultBuffer);


	return CommandResult_Ok;

}



/**
 * \brief	List global variables
 */
CommandResult_t CommandFunction_GlobalVariableList ( uint32_t argc, char** argv )
{

	GlobalVarHandler_ListAllVariables();

	return CommandResult_Ok;

}



#ifdef CONFIG_MODULE_FLASH_ENABLE
/**
 * \brief	Flash erase
 * 			Use: 'flashdel <address> <block/sector>'
 */
CommandResult_t CommandFunction_flashdel	( uint32_t argc, char** argv )
{

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
			
	uprintf("address erased: 0x%h\r\n",
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
	

	// Convert arg2 hex
	if ( !StringHexToNum(argv[1],&Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}


	uint8_t Buffer[1];
	FLASH_Read(Arg2Num,Buffer,1,5000);
	
	
	uprintf("address: 0x%w\r\n"
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

	// Convert arg2 hex
	if ( !StringHexToNum(argv[1],&Arg2Num))
	{
		return CommandResult_Error_WrongArgument1;
	}
	
	// Convert arg3, decimal
	if (!UnsignedDecimalStringToNum(argv[2],&Arg3Num))
	{
		return CommandResult_Error_WrongArgument2;
	}


	uint8_t Buffer[1];
	
	Buffer[0] = (uint8_t)Arg3Num;
		
	FLASH_Write(Arg2Num,Buffer,1,5000);
	

	uprintf("address: 0x%w\r\n"
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
		if (!UnsignedDecimalStringToNum(argv[2],&Arg3Num))
		{
			USART_SendString("Wrong 1. argument!\r\n");
			return CommandResult_Error_WrongArgument1;
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

	uprintf("Wrong parameter");

	return CommandResult_Error_Unknown;
}
#endif



#ifdef CONFIG_MODULE_DAC_ENABLE
/**
 * \brief	DAC function
 * 			Use: 'dac <channel> <voltage>'
 */
CommandResult_t CommandFunction_dac (uint32_t argc, char** argv)
{

	float voltage = 0.0f;

	if (!UnsignedDecimalStringToNum(argv[1],&Arg2Num))
	{
		USART_SendLine("Wrong 1. argument!");
		return CommandResult_Error_WrongArgument1;
	}

	if (!StringToFloat(argv[2],&voltage))
	{
		USART_SendLine("Wrong 2. argument!");
		return CommandResult_Error_WrongArgument2;
	}

	DAC_SetValue(Arg2Num,voltage);


	return CommandResult_Ok;
}
#endif


CommandResult_t CommandFunction_unittest (uint32_t argc, char** argv)
{

#ifdef MODULE_STRING_UNIT_TEST_ENABLED
	STRING_UnitTest();
#endif

	return CommandResult_Ok;
}


#if 0
// Function: dl (download)
// dl <destination> <size>
// Download to <destination> address <size> bytes from USART
CommandResult_t CommandFunction_dl ( uint32_t argc, char** argv )
{

	unsigned char * destination;
	uint32_t size;
	//uint32_t i;
	//unsigned char byte;

	if ( argc < 3 ) { USART_SendString("Too few arguments!\r\n"); return 0; }
	if ( argc > 3 ) { USART_SendString("Too many arguments!\r\n"); return 0; }

	if ( StringIsHexadecimalString(argv[1]) ) Arg2Num = StringHexToNum(argv[1]);	// <destination> 	Convert hex to num
	else { USART_SendString("Wrong 1. argument!\r\n"); return 0; }
	if ( StringIsUnsignedDecimalString(argv[2]) ) Arg3Num = UnsignedDecimalStringToNum(argv[2]);	// <size>			Convert dec to num
	else { USART_SendString("Wrong 2. argument!\r\n"); return 0; }

	size = Arg3Num;
	destination = (unsigned char *)Arg2Num;


	// Ellenorizni kell, hogy uint16_t -e
	if ( size > UINT16_MAX )
	{
		USART_SendString("The <size> is too large. Only work with maximum UINT16_MAX\r\n");
		return RETURN_FALSE;
	}

	uprintf("Destination: 0x%w\r\n"
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

	uprintf("Jelenleg teszteles alatt, a FLASH programozassal gond van.\r\n");
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

	// TODO: USART_SendLine();
	// TODO: MONITOR_SendMessage(); ez irányítódjon át egy USART_SendString()-re

	// Convert hex
	if ( !StringHexToNum(argv[1],&Arg2Num,0))
	{
		return CommandResult_Error_WrongArgument1;
	}

	destination = Arg2Num;
	uprintf("Go destination: 0x%w\r\n",
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


	if ( argc < 3 ) {	USART_SendString("Too few arguments!\r\n");	return 0;	}
	if ( argc > 3 ) {	USART_SendString("Too many arguments!\r\n");	return 0;	}

	if ( StringIsHexadecimalString(argv[1]) ) Arg2Num = StringHexToNum(argv[1]);	// Convert arg2, source to hex
	else { USART_SendString("Wrong 1. argument!"); return 2; }
	if ( StringIsUnsignedDecimalString(argv[2]) ) Arg3Num = StringDecToNum(argv[2]);	// Convert arg3, size to dec
	else { USART_SendString("Wrong 2. argument!"); return 2; }

	source = ( uint32_t *) Arg2Num;		// casting for valid numbers
	size = ( unsigned short int ) Arg3Num;	// <size> max 256
	// TODO: checking the correct address

	uprintf("Source: 0x%w\r\n"
			"Size: %d\r\n",
			source,size);

	i=0;
	for (p = source; p < source+size/4; p++) {
		if ( !(i % 4) ) uprintf("\r\n 0x%w:",p);
		uprintf(" %w",*p);
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


	if ( argc < 3 ) {	USART_SendString("Too few arguments!\r\n");	return 0;	}
	if ( argc > 3 ) {	USART_SendString("Too many arguments!\r\n");	return 0;	}

	if ( StringIsHexadecimalString(argv[1]) ) Arg2Num = StringHexToNum(argv[1]);	// Convert hex
	else { USART_SendString("Wrong 1. argument!"); return 2; }
	if ( StringIsHexadecimalString(argv[2]) ) Arg3Num = StringHexToNum(argv[2]);	// Convert hex
	else { USART_SendString("Wrong 2. Argument!"); return 2; }


	if (!StrCmp(argv[0],"mwb")) {
		destination1 = ( unsigned char *) Arg2Num;
		*destination1 = ( unsigned char ) Arg3Num;
		uprintf("Write: %b to: %w",Arg3Num,Arg2Num);
	}
	else if (!StrCmp(argv[0],"mwh")) {
		destination2 = ( unsigned short int *) Arg2Num;
		*destination2 = ( unsigned short int) Arg3Num;
		uprintf("Write: %h to: %w",Arg3Num,Arg2Num);
	}
	else if (!StrCmp(argv[0],"mww")) {
		destination3 = ( uint32_t *) Arg2Num;
		*destination3 = ( uint32_t ) Arg3Num;
		uprintf("Write: %w to: %w",Arg3Num,Arg2Num);
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

	uprintf("Go to STOP mode...\r\n"
					"Wake up with USER button\r\n"
					);

	LOWPOWER_GotoStopMode();

	LED_ALARM_OFF();
	LCD_Instr_DisplayClear();
	LCD_SendString_2line("RadioAlarm","",2,2);
	uprintf("End of STOP mode\r\n");

	return 1;
}
#endif



#if 0
// Function: EEPROM read
// Syntax: romr <address>
CommandResult_t CommandFunction_romr	( uint32_t argc, char** argv ) {

	Arg2Num = StringDecToNum(argv[1]);

	//uprintf("address: 0x%h\r\n"
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


	Arg2Num = StringHexToNum(argv[1]);
	Arg3Num = StringHexToNum(argv[2]);

	uprintf("address: 0x%h\r\n"
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

	//uprintf("Status register: 0x%h\r\n",
	//				EEPROM_ReadStatusRegister ()
	//				);

	return 1;
}
#endif



#if 0
// Function: EEPROM, Write Enable
CommandResult_t CommandFunction_romwe	( uint32_t argc, char** argv ) {

	//EEPROM_WriteEnable ();

	uprintf("EEPROM write enable\r\n");

	return 1;
}
#endif



#if 0
// Function: initialize EEPROM
CommandResult_t CommandFunction_rominit	( uint32_t argc, char** argv ) {

	//EEPROM_Init ();

	uprintf("EEPROM initialized\r\n");

	return 1;
}
#endif



#if 0
CommandResult_t CommandFunction_standby	( uint32_t argc, char** argv ) {

	LCD_Instr_DisplayClear();
	LCD_SendString_2line("RadioAlarm","in STANDBY mode",2,2);

	uprintf("Go to STANDBY mode. You can wake up(=reset) with RESET and USER button!\r\n");

	LOWPOWER_GotoSTANDBYMode ();

	// Exit from STANDBY mode is same with reset, this point is never reachable

	return 1;
}
#endif



#if 0
CommandResult_t CommandFunction_rtc	( uint32_t argc, char** argv ) {

	Arg2Num = StringDecToNum(argv[1]);

	uprintf("RTC wait %d second(s)\r\n",Arg2Num);

	//RTC_WaitSeconds(Arg2Num);

	uprintf("End\r\n");

	return 1;
}
#endif



#if 0
CommandResult_t CommandFunction_start	( uint32_t argc, char** argv )
{
	(void)argc;
	(void)argv;


	uprintf("start command...\r\n");


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

	uprintf("$ Stop command! $\r\n");

	return CommandResult_Ok;

}
#endif



#if 0
// Function: buzzer switch on/off
CommandResult_t CommandFunction_buzzer	( uint32_t argc, char** argv ) {

	if ( argc < 2 )
	{
		uprintf("Too few arguments!\r\n");
		return RETURN_FALSE;
	}
	if ( argc > 2 )
	{
		uprintf("Too many arguments!\r\n");
		return RETURN_FALSE;
	}

	if (!StrCmp(argv[1],"on"))
	{
		// TODO: Buzzer on
		// Buzzer bekapcsolasa
		//BUZZER_Test_Task ();	// Buzzer taszk, nem a legjobb megoldas!
		uprintf("Buzzer on (Important! This is an infinite loop!)\r\n");
		//BUZZER_On(); // Important!! Before turn on, need BUZZER_Init() function.
	}
	else if (!StrCmp(argv[1],"off"))
	{
		// Buzzer kikapcsolása
		//BUZZER_DeInit();
		uprintf("Buzzer off\r\n");
	}

	return CommandResult_Ok;
}
#endif




#if 0
CommandResult_t CommandFunction_accelerometer	( uint32_t argc, char** argv ) {

	(void)argc;
	(void)argv;

	uprintf("Accelerometer has been started...\r\n");

	// TODO:
	//ACCELEROMETER_Task();
	//vTaskResume(ACCELEROMETER_TaskHandle);

	uprintf("End\r\n");

	return CommandResult_Ok;
}
#endif



#if 0
CommandResult_t CommandFunction_gyroscope	( uint32_t argc, char** argv ) {

	(void)argc;
	(void)argv;

	uprintf("Gyroscope has been started...\r\n");

	// TODO:
	//GYROSCOPE_Task ();
	//vTaskResume(GYROSCOPE_TaskHandle);

	uprintf("End\r\n");

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

	uprintf("Jelenleg nincs funkcioja!\r\n");

//	if ( argc < 2 )
//	{
//		uprintf("Too few arguments!\r\n"
//				" Use: \"prox on\" or \"prox off\"");
//
//		return RETURN_FALSE;
//	}
//	if ( argc > 2 )
//	{
//		uprintf("Too many arguments!\r\n");
//		return RETURN_FALSE;
//	}
//
//
//	if (!StrCmp(argv[1],"on"))
//	{
//
//		// LOG start
//		uprintf("Proximity log on\r\n");
//		LOG_QueueEnable_PROXIMITY = LOG_ENABLE;
//
//	}
//	else if (!StrCmp(argv[1],"off"))
//	{
//
//		// LOG end
//		LOG_QueueEnable_PROXIMITY = LOG_DISABLE;
//		uprintf("Proximity log off\r\n");
//
//	}

	//uprintf("End\r\n");

	return CommandResult_Ok;
}
#endif



#if 0
CommandResult_t CommandFunction_log ( uint32_t argc, char** argv )
{

	//(void)argc;

	if ( argc < 2 )
	{
		uprintf("Too few arguments!\r\n"
				" Use: \"log on\" or \"log off\" or \"log on <task>\"");

		return RETURN_FALSE;
	}
//	if ( argc > 2 )
//	{
//		uprintf("Too many arguments!\r\n");
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
			uprintf("Log off\r\n");
		}
		else
		{
			uprintf("Wrong! Use \"log on <task>\" or \"log off\"\r\n");
			return RETURN_FALSE;
		}

	}

	/*
	if ( argc == 2 )		// "log on", "log off"
	{
		if (!StrCmp(argv[1],"on"))
		{

			uprintf("Log on\r\n");

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

			uprintf("Log off\r\n");
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
			uprintf("2. argumentum rossz! \"on\" vagy \"off\" kell legyen!\r\n");
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


	uprintf("exit\r\n"
			" MONITOR program leallitasa...\r\n");
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

	uprintf("Distance: %s\r\n",String);
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


	uprintf("\r\n"
			"Raw:\r\n");
	#ifdef CONFIG_USE_FREERTOS
	vTaskDelay(1);
	#else
	HAL_Delay(1);
	#endif
	for (i=0; i<200; i++ )
	{
		uprintf("%d\r\n",localDataRaw[i]);
		#ifdef CONFIG_USE_FREERTOS
		vTaskDelay(1);
		#else
		HAL_Delay(1);
		#endif
	}

	uprintf("\r\n"
			"Avg:\r\n");
	for (i=0; i<200; i++ )
	{
		//uprintf("%d\r\n",localDataAvg[i]);	// egyszeru kiiratas, int. De az Avg float!

		pFloat = &localDataAvg[i];
		FloatToString(*pFloat, String);

		uprintf("%s\r\n",String);
	}


	return CommandResult_Ok;

}
#endif



#if 0
// Function: ESP8266 bridge
CommandResult_t CommandFunction_ESP8266	( uint32_t argc, char** argv ) {

	if ( argc > 1 )
	{
		uprintf("Too many arguments!\r\n");
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
