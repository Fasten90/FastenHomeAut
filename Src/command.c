/*******************************************************************************
 * Purpose:
 ******************************************************************************/

/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
//#include "command.h"
//#include "monitor.h"
//#include "usart.h"
#include "include.h"

#include "GlobalVarHandler.h"

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
	/*
	{
		.name = "stop",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_stop,
		.description = "stop\r\n"
	},
	*/
	
	{
		.name = "help",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_help,
		.description = "Commands's list, or write command's propertities",
		.syntax = "<command>",
		.ArgNum = (1 << 1) | (1 << 0),
	},
	
	
	{
		.name = "reset",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_reset,
		.description = "Software reset",
		.syntax = NULL,
		.ArgNum = (1 << 0),
	},

	{
		.name = "cls",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_cls,
		.description = "Clear Screen",
		.syntax = NULL,
		.ArgNum = (1 << 0),
	},

	{
		.name = "test",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_test,
		.description = "...",
		.syntax = NULL,
		.ArgNum = (1 << 0),
	},

	{
		.name = "set",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_set,
		.description = "set global variable value",
		.syntax = "<varName> <value>",
		.ArgNum = (1 << 2),
	},

	{
		.name = "get",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_get,
		.description = "get global variable value",
		.syntax = "<varName>",
		.ArgNum = (1 << 1),
	},

	/*
	{
		.name = "start",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_start,
		.description = "start\r\n"
	},


	{
		.name = "buzzer",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_buzzer,
		.description = "buzzer\r\n"
		"Syntax: buzzer on/off\r\n"
		"Function: trivial\r\n"
	},
	*/
	
	{
		.name = "led",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_led,
		.description = "Control LED",
		.syntax = "<on/off/toggle/status> <num>",
		.ArgNum = (1 << 2) | (1 << 1),
		/*
		"  <num>: decimal: 1-3\r\n"
		"Function:\r\n"
		"  led on: tun on <num>. led\r\n"
		"  led off: turn off <num>. led\r\n"
		"  led switch: switch <num>. led\r\n"
		"  led status: write leds's status\r\n"
		*/
	},

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
	
	{
		.name = "flashdel",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_flashdel,
		.description = "Delete flash",
		.syntax = "<address> <block/sector>",
		.ArgNum = (1 << 2),
	},

	
	{
		.name = "flashread",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_flashread,
		.description = "Read from flash",
		.syntax = "<address>",
		.ArgNum = (1 << 1),
	},

	
	{
		.name = "flashwrite",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_flashwrite,
		.description = "Write flash",
		.syntax = "<address> <byte>",
		.ArgNum = (1 << 2),
	},

	
	{
		.name = "temperature",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_temp,
		.description = "...",
		.syntax = NULL,
		.ArgNum = (1 << 0),
	},

	/*
	{
		.name = "esp8266",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_ESP8266,
		.description = "ESP8266 wifi modul bridge\r\n"
	},
	*/
	
	/*
	{
		.name = "#raspi",
		.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_raspberrypi,
		.description = "Raspberry Pi HomeAutMessage sending",
		.syntax = "-",
		.ArgNum = (1 << 1)
	}
	*/
	
		
};



/*
new1.name = "remote";
new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_remotecontrol;
new1.description = "remote\r\n"
				"Function: Remote control\r\n";
CommandList[13] = new1;


new1.name = "prox";
new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_proximity;
new1.description = "prox\r\n"
				"Function: Proximities's status\r\n";
CommandList[14] = new1;



new1.name = "go";
new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_go;
new1.description = "go\r\n"
				"Syntax: go <destination>\r\n"
				"  <destination>: hexa\r\n"
				"Function: program jump to destination\r\n";
CommandList[16] = new1;

new1.name = "log";
new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_log;
new1.description = "log\r\n"
				"  log <on/off>\r\n";
CommandList[17] = new1;


new1.name = "exit";
new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_exit;
new1.description = "exit\r\n"
				"Function: Exit from MONITOR program\r\n";
CommandList[18] = new1;


new1.name = "read";
new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_read;
new1.description = "read\r\n"
				"Function: Read datas\r\n";
CommandList[19] = new1;
*/



//	new1.name = "mr";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_mr;
//	new1.description = "mr\r\n"
//					"Syntax: mr <source> <size>\r\n"
//					"  <source>: hexa\r\n"
//					"  <size>: decimal, byte's num\r\n"
//					"Function: read from memory address (source) size byte-s\r\n";
//	CommandList[3] = new1;


//	new1.name = "mwb";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_mw;
//	new1.description = "mwb\r\n"
//					"Syntax: mwb <destination> <data>\r\n"
//					"  <destination>: hexa\r\n"
//					"  <data>: 1 byte, hexa\r\n"
//					"Function: write to memory address (destination) data\r\n";
//	CommandList[4] = new1;


//	new1.name = "mwh";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_mw;
//	new1.description = "mwh\r\n"
//					"Syntax: mwh <destination> <data>\r\n"
//					"  <destination>: hexa\r\n"
//					"  <data>: 2 byte, hexa\r\n"
//					"Function: write to memory address (destination) data\r\n";
//	CommandList[5] = new1;


//	new1.name = "mww";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_mw;
//	new1.description = "mww\r\n"
//					"Syntax: mww <destination> <data>\r\n"
//					"  <destination>: hexa\r\n"
//					"  <data>: 4 byte, hexa\r\n"
//					"Function: write to memory address (destination) data\r\n";
//	CommandList[6] = new1;


//	new1.name = "btn";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_btn;
//	new1.description = "btn"
//									"Write buttons's status\r\n";
//	CommandList[10] = new1;
	
//	new1.name = "tizenegy";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_tizenegy;
//	new1.description = "tizenegy\r\n"
//									"Special function for Jablonkai Sara\r\n";
//	CommandList[12] = new1;

//	new1.name = "temp";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_temp;
//	new1.description = "temp\r\n"
//									"Read temperature and Vref\r\n";
//	CommandList[13] = new1;
	
//	new1.name = "lcd";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_lcd;
//	new1.description = "lcd\r\n"
//									"Write 2 line text on LCD\r\n";
//	CommandList[14] = new1;

	
//	new1.name = "romwe";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_romwe;
//	new1.description = "romwe\r\n"
//					"Syntax: romwe\r\n"
//					"Function: Enable ROM Write\r\n";
//	CommandList[16] = new1;


//	new1.name = "romr";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_romr;
//	new1.description = "romr\n\r"
//					"Syntax: romr <address>\r\n"
//					"  <address>: hexa\r\n"
//					"Function: Read from ROM memory from <address>\r\n";
//	CommandList[17] = new1;


//	new1.name = "romw";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_romw;
//	new1.description = "romw\n\r"
//					"Syntax: romw <address> <data>\r\n"
//					"  <address>: hexa\r\n"
//					"  <data>: hexa, 1  byte\r\n"
//					"Function: Write to ROM memory to <address> the <data> byte\r\n"
//					"Example: \"romw 0 0A\" write 0x00 address 0x0A data\r\n"
//					"Note: You need type first \"rominit\" and after the \"romwe\" command!\r\n";
//	CommandList[18] = new1;


//	new1.name = "romsr";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_romsr;
//	new1.description = "romsr\r\n"
//					"Syntax: romsr\r\n"
//					"Function: Read ROM's status register\r\n";
//	CommandList[19] = new1;


//	new1.name = "rominit";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_rominit;
//	new1.description = "rominit\r\n"
//					"Function: Initialize EEPROM\r\n";
//	CommandList[20] = new1;
	

//new1.name = "romsw";
//new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_romwd;
//new1.description = "romwd\r\n"
//				"Syntax: romwd"
//				"Function: Disable ROM Write";
//CommandList[21] = new1;

//new1.name = "romwd";
//new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_romwd;
//new1.description = "romwd\r\n"
//				"Syntax: romwd"
//				"Function: Disable ROM Write";
//CommandList[22] = new1;


//	new1.name = "rfm12b";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_rfm12b;
//	new1.description = "rfm12b\r\n"
//					"Function: rfm12b test\r\n";
//	CommandList[22] = new1;
	
//	new1.name = "keypad";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_keypad;
//	new1.description = "kepyad\r\n"
//					"Function: read keypad's pressed buttons\r\n";
//	CommandList[23] = new1;
	
//	new1.name = "standby";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_standby;
//	new1.description = "standby\r\n"
//					"Function: go to STANDBY mode\r\n";
//	CommandList[24] = new1;
	
//	new1.name = "rtc";
//	new1.CommandFunctionPointer = ( FunctionPointer *)CommandFunction_rtc;
//	new1.description = "rtc\r\n"
//									"Syntax: rtc <second>\r\n"
//									"Function: wait x second\r\n";
//	CommandList[25] = new1;



const uint8_t MONITOR_CommandNum = MONITOR_MAX_COMMAND_NUM;


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


// Function: cls (clean screen)
uint32_t CommandFunction_cls ( uint32_t argc, char** argv ) {

	//(void)argc;
	//(void)COMMAND_Arguments;
	USART_SEND_CLS();

	return CommandResult_Ok;
}



#if 0
// Function: dl (download)
// dl <destination> <size>
// Download to <destination> address <size> bytes from USART
uint32_t CommandFunction_dl ( uint32_t argc, char** COMMAND_Arguments ) {

	unsigned char * destination;
	uint32_t size;
	//uint32_t i;
	//unsigned char byte;

	if ( argc < 3 ) { USART_SendString("Too few arguments!\r\n"); return 0; }
	if ( argc > 3 ) { USART_SendString("Too many arguments!\r\n"); return 0; }

	if ( StringIsHexadecimalString(COMMAND_Arguments[1]) ) Arg2Num = StringHexToNum(COMMAND_Arguments[1]);	// <destination> 	Convert hex to num
	else { USART_SendString("Wrong 1. argument!\r\n"); return 0; }
	if ( StringIsUnsignedDecimalString(COMMAND_Arguments[2]) ) Arg3Num = UnsignedDecimalStringToNum(COMMAND_Arguments[2]);	// <size>			Convert dec to num
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





// Function: go (jump to an address)
// go <destination>
// jump <destination> address
#if 0
uint32_t CommandFunction_go ( uint32_t argc, char** argv ) {

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



// Function: help (list commands)
// 'help' or 'help <command>'
// Listing commands or write the command's describe
uint32_t CommandFunction_help ( uint32_t argc, char** argv ) {

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

	return CommandResult_Unknown;
}



// Function: reset
uint32_t CommandFunction_reset ( uint32_t argc, char** argv ) {

	//(void)argc;
	//(void)COMMAND_Arguments;
	uint16_t i;

	USART_SendLine("Reset...");
	for(i=0; i<1000; i++);

	NVIC_SystemReset();

	for(i=0; i<1000; i++);

	return CommandResult_Ok;
}



/*
// Function: mr (memory read)
// mr <source> <size>
// source: hex
// size: dec
// read <size> byte-s from <source> address
uint32_t CommandFunction_mr ( uint32_t argc, char** COMMAND_Arguments ) {


	uint32_t *source;
	uint32_t *p;
	unsigned short int size;
	uint32_t i;


	if ( argc < 3 ) {	USART_SendString("Too few arguments!\r\n");	return 0;	}
	if ( argc > 3 ) {	USART_SendString("Too many arguments!\r\n");	return 0;	}

	if ( StringIsHexadecimalString(COMMAND_Arguments[1]) ) Arg2Num = StringHexToNum(COMMAND_Arguments[1]);	// Convert arg2, source to hex
	else { USART_SendString("Wrong 1. argument!"); return 2; }
	if ( StringIsUnsignedDecimalString(COMMAND_Arguments[2]) ) Arg3Num = StringDecToNum(COMMAND_Arguments[2]);	// Convert arg3, size to dec
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
*/



/*
// FunctioN: mw - memory write
// mwb <destination> <data>
// destination: hex
// data: hex
// mwb, mwh, mww commands
uint32_t CommandFunction_mw ( uint32_t argc, char** COMMAND_Arguments ) {

	unsigned char *destination1;
	unsigned short int *destination2;
	uint32_t *destination3;


	if ( argc < 3 ) {	USART_SendString("Too few arguments!\r\n");	return 0;	}
	if ( argc > 3 ) {	USART_SendString("Too many arguments!\r\n");	return 0;	}

	if ( StringIsHexadecimalString(COMMAND_Arguments[1]) ) Arg2Num = StringHexToNum(COMMAND_Arguments[1]);	// Convert hex
	else { USART_SendString("Wrong 1. argument!"); return 2; }
	if ( StringIsHexadecimalString(COMMAND_Arguments[2]) ) Arg3Num = StringHexToNum(COMMAND_Arguments[2]);	// Convert hex
	else { USART_SendString("Wrong 2. Argument!"); return 2; }


	if (!StrCmp(COMMAND_Arguments[0],"mwb")) {
		destination1 = ( unsigned char *) Arg2Num;
		*destination1 = ( unsigned char ) Arg3Num;
		uprintf("Write: %b to: %w",Arg3Num,Arg2Num);
	}
	else if(!StrCmp(COMMAND_Arguments[0],"mwh")) {
		destination2 = ( unsigned short int *) Arg2Num;
		*destination2 = ( unsigned short int) Arg3Num;
		uprintf("Write: %h to: %w",Arg3Num,Arg2Num);
	}
	else if(!StrCmp(COMMAND_Arguments[0],"mww")) {
		destination3 = ( uint32_t *) Arg2Num;
		*destination3 = ( uint32_t ) Arg3Num;
		uprintf("Write: %w to: %w",Arg3Num,Arg2Num);
	}


	return 1;
}
*/




// Function: led turn on, turn off, or switch
// led on <num>
// or led off <num>
// or led toggle <num>
// or led status <dummy>
uint32_t CommandFunction_led ( uint32_t argc, char** argv )	// TODO: !!IMPORTANT!! atirni
{

	// Convert arg2, decimal
	if (!UnsignedDecimalStringToNum(argv[2],&Arg3Num))
	{
		return CommandResult_Error_WrongArgument1;
	}


#ifdef CONFIG_MODULE_LED_ENABLE
	
	if (!StrCmp(COMMAND_Arguments[1],"on")) {
		switch (Arg3Num) {
			case 1:	LED_BLUE_ON();	break;
			case 2:	LED_GREEN_ON();	break;
			case 3:	LED_RED_ON();	break;
		}
		uprintf("Led on: %d\r\n",Arg3Num);
	}
	else if(!StrCmp(COMMAND_Arguments[1],"off")) {
		switch (Arg3Num) {
			case 1:	LED_BLUE_OFF();	break;
			case 2:	LED_GREEN_OFF();	break;
			case 3:	LED_RED_OFF();	break;
		}
		uprintf("Led off: %d\r\n",Arg3Num);
	}
	else if(!StrCmp(COMMAND_Arguments[1],"toggle")) {
		switch (Arg3Num) {
			case 1: LED_BLUE_TOGGLE(); break;
			case 2: LED_GREEN_TOGGLE(); break;
			case 3: LED_RED_TOGGLE(); break;
		}
		uprintf("Led toggle: %d\r\n",Arg3Num);
	}
	else if(!StrCmp(COMMAND_Arguments[1],"status")) {
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



// Function: Read temperature and Vref values
uint32_t CommandFunction_temp	( uint32_t argc, char** argv ) {

	//uprintf("Temperature: %d [C]\r\n",ADC_GetTemp());
	//uprintf("Vref: %d [mV]\r\n",ADC_GetVref());
	
	#ifdef ADC_H_
	ADC_ConvertAllMeasuredValues();
	
	uprintf("Temperature: ");
	USART_SendFloat(ADC_ConvertedValue_InternalTemperature);
	uprintf(" [C]\r\n");
	#endif
	
	return 1;
}




/*
// Function: Go to STOP mode
uint32_t CommandFunction_stop	( uint32_t argc, char** COMMAND_Arguments ) {

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
*/



/*
// Function: EEPROM read
// Syntax: romr <address>
uint32_t CommandFunction_romr	( uint32_t argc, char** COMMAND_Arguments ) {

	Arg2Num = StringDecToNum(COMMAND_Arguments[1]);
		
	//uprintf("address: 0x%h\r\n"
	//				"data:    0x%b\r\n",
	//				Arg2Num,
	//				EEPROM_ReadByte(Arg2Num)
	//				);
	
	return 1;
}
*/


/*
// Function: EEPROM write
// Syntax: romw <address> <data>
uint32_t CommandFunction_romw	( uint32_t argc, char** COMMAND_Arguments ) {


	Arg2Num = StringHexToNum(COMMAND_Arguments[1]);
	Arg3Num = StringHexToNum(COMMAND_Arguments[2]);
	
	uprintf("address: 0x%h\r\n"
					"data:    0x%b\r\n",
					Arg2Num,
					Arg3Num);
	//EEPROM_WriteByte(Arg2Num,Arg3Num);

	return 1;
}
*/


/*
// Function: Read EEPROM's status register
uint32_t CommandFunction_romsr	( uint32_t argc, char** COMMAND_Arguments ) {
		
	//uprintf("Status register: 0x%h\r\n",
	//				EEPROM_ReadStatusRegister ()
	//				);
	
	return 1;
}
*/


/*
// Function: EEPROM, Write Enable
uint32_t CommandFunction_romwe	( uint32_t argc, char** COMMAND_Arguments ) {
		
	//EEPROM_WriteEnable ();

	uprintf("EEPROM write enable\r\n");
	
	return 1;
}
*/


/*
// Function: initialize EEPROM
uint32_t CommandFunction_rominit	( uint32_t argc, char** COMMAND_Arguments ) {
		
	//EEPROM_Init ();

	uprintf("EEPROM initialized\r\n");
	
	return 1;
}
*/


/*
uint32_t CommandFunction_standby	( uint32_t argc, char** COMMAND_Arguments ) {
	
	LCD_Instr_DisplayClear();
	LCD_SendString_2line("RadioAlarm","in STANDBY mode",2,2);
	
	uprintf("Go to STANDBY mode. You can wake up(=reset) with RESET and USER button!\r\n");
	
	LOWPOWER_GotoSTANDBYMode ();
	
	// Exit from STANDBY mode is same with reset, this point is never reachable
	
	return 1;
}
*/


/*
uint32_t CommandFunction_rtc	( uint32_t argc, char** COMMAND_Arguments ) {
	
	Arg2Num = StringDecToNum(COMMAND_Arguments[1]);
	
	uprintf("RTC wait %d second(s)\r\n",Arg2Num);
	
	//RTC_WaitSeconds(Arg2Num);
	
	uprintf("End\r\n");
	
	return 1;
}
*/



// Function: TEST...
uint32_t CommandFunction_test	( uint32_t argc, char** argv ) {
	
	//(void)argc;
	//(void)COMMAND_Arguments;
	//uint8_t i = 0;
	//uint8_t buf[2];
	
	USART_SendLine("Test start");

	// GlobalVar

	GlobalVarHandler_ListAllVariables();

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
		//vTaskDelay(200); 		// TODO: FREERTOS
	}
	*/


	// FLOAT TEST
	/*
	char String[20];
	float AnNum;

	AnNum = StringToFloat ( "3.14" );
	FloatToString ( AnNum , String, 2 );
	uprintf("%s\r\n",String);


	AnNum = StringToFloat ( "3.1234567" );
	FloatToString ( AnNum , String,4 );
	uprintf("%s\r\n",String);


	AnNum = StringToFloat ( "3.0" );
	FloatToString ( AnNum , String,6 );
	uprintf("%s\r\n",String);

	AnNum = StringToFloat ( "3.2" );
	FloatToString ( AnNum , String,4 );
	uprintf("%s\r\n",String);

	AnNum = StringToFloat ( "0.24" );
	FloatToString ( AnNum , String,4 );
	uprintf("%s\r\n",String);
	
	
	uprintf("%f\r\n",1.2);
	uprintf("%f\r\n",3.1234567);
	uprintf("%f\r\n",0.24);
	uprintf("%f\r\n",-0.24);	
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



#if 0
uint32_t CommandFunction_start	( uint32_t argc, char** COMMAND_Arguments )
{
	(void)argc;
	(void)COMMAND_Arguments;


	uprintf("start command...\r\n");


	// Auto elinditasa a GLOBAL_CommandStartXXX() fuggvennyel
	//GLOBAL_CommandStartMonitor();


	return CommandResult_Ok;
}
#endif



#if 0
uint32_t CommandFunction_stop	( uint32_t argc, char** COMMAND_Arguments )
{

	//(void)argc;
	//(void)COMMAND_Arguments;

	//GLOBAL_CommandEmergencyStop();

	uprintf("$ Stop command! $\r\n");

	return CommandResult_Ok;

}
#endif




// Function: Beallitja a globalis valtozo erteket (set fuggvennyel)
// Syntax: set <globalvar> <value>
uint32_t CommandFunction_set ( uint32_t argc, char** COMMAND_Arguments )
{

	char resultBuffer[30];

	GlobalVarHandler_ProcessCommand(
			COMMAND_Arguments[1],COMMAND_Arguments[2],
			SetGet_Set,Source_DebugSerial,
			resultBuffer,30);

	uprintf("%s\r\n",resultBuffer);


	return CommandResult_Ok;

}






// Function: get <globalvar>
uint32_t CommandFunction_get ( uint32_t argc, char** COMMAND_Arguments )
{

	char resultBuffer[30];

	GlobalVarHandler_ProcessCommand(
			COMMAND_Arguments[1],COMMAND_Arguments[2],
			SetGet_Get,Source_DebugSerial,
			resultBuffer,30);

	uprintf("%s\r\n",resultBuffer);


	return CommandResult_Ok;

}



#if 0
// Function: buzzer switch on/off
uint32_t CommandFunction_buzzer	( uint32_t argc, char** COMMAND_Arguments ) {

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

	if (!StrCmp(COMMAND_Arguments[1],"on"))
	{
		// TODO: Buzzer on
		// Buzzer bekapcsolasa
		//BUZZER_Test_Task ();	// Buzzer taszk, nem a legjobb megoldas!
		uprintf("Buzzer on (Important! This is an infinite loop!)\r\n");
		//BUZZER_On(); // Important!! Before turn on, need BUZZER_Init() function.
	}
	else if(!StrCmp(COMMAND_Arguments[1],"off"))
	{
		// Buzzer kikapcsolása
		//BUZZER_DeInit();
		uprintf("Buzzer off\r\n");
	}

	return CommandResult_Ok;
}
#endif




#if 0
uint32_t CommandFunction_accelerometer	( uint32_t argc, char** COMMAND_Arguments ) {

	(void)argc;
	(void)COMMAND_Arguments;

	uprintf("Accelerometer has been started...\r\n");

	// TODO:
	//ACCELEROMETER_Task();
	//vTaskResume(ACCELEROMETER_TaskHandle);

	uprintf("End\r\n");

	return CommandResult_Ok;
}
#endif



#if 0
uint32_t CommandFunction_gyroscope	( uint32_t argc, char** COMMAND_Arguments ) {

	(void)argc;
	(void)COMMAND_Arguments;

	uprintf("Gyroscope has been started...\r\n");

	// TODO:
	//GYROSCOPE_Task ();
	//vTaskResume(GYROSCOPE_TaskHandle);

	uprintf("End\r\n");

	return CommandResult_Ok;
}
#endif




#if 0
uint32_t CommandFunction_remotecontrol	( uint32_t argc, char** COMMAND_Arguments ) {


	return CommandResult_Ok;
}
#endif



#if 0
uint32_t CommandFunction_proximity	( uint32_t argc, char** COMMAND_Arguments ) {

	//(void)argc;
	//(void)COMMAND_Arguments;

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
//	if (!StrCmp(COMMAND_Arguments[1],"on"))
//	{
//
//		// LOG start
//		uprintf("Proximity log on\r\n");
//		LOG_QueueEnable_PROXIMITY = LOG_ENABLE;
//
//	}
//	else if(!StrCmp(COMMAND_Arguments[1],"off"))
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
uint32_t CommandFunction_log ( uint32_t argc, char** COMMAND_Arguments )
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
		if(!StrCmp(COMMAND_Arguments[1],"off"))
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
		if (!StrCmp(COMMAND_Arguments[1],"on"))
		{

			uprintf("Log on\r\n");

			// LOG start
			vTaskResume(LOG_TaskHandle);

		}
		else if(!StrCmp(COMMAND_Arguments[1],"off"))
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
		if (!StrCmp(COMMAND_Arguments[1],"on"))
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
uint32_t CommandFunction_exit ( uint32_t argc, char** COMMAND_Arguments )
{
	(void)argc;
	(void)COMMAND_Arguments;


	uprintf("exit\r\n"
			" MONITOR program leallitasa...\r\n");
	//vTaskSuspend(DEBUG_TaskHandle);				// TODO: FREERTOS

	return CommandResult_Ok;
}
#endif



#if 0
uint32_t CommandFunction_read ( uint32_t argc, char** COMMAND_Arguments )
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
	(void)COMMAND_Arguments;

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


// Function: Flash erase
// Syntax: flashdel <address> <block/sector>
uint32_t CommandFunction_flashdel	( uint32_t argc, char** argv ) {

	// Convert arg2 hex
	if ( !StringHexToNum(argv[1],&Arg2Num,0))
	{
		return CommandResult_Error_WrongArgument1;
	}


	#ifdef FLASH_H_
	if (!StrCmp(COMMAND_Arguments[2],"block"))
	{
		FLASH_BlockErase(Arg2Num,5000);
	}
	else if (!StrCmp(COMMAND_Arguments[2],"sector"))
	{
		FLASH_SectorErase(Arg2Num,5000);
	}
	else
	{
		return RETURN_FALSE;
	}
	#endif		
			
	uprintf("address erased: 0x%h\r\n",
			Arg2Num
			);
	
	return CommandResult_Ok;
}



// Function: FLASH read
// Syntax: flashread <address>
uint32_t CommandFunction_flashread	( uint32_t argc, char** argv ) {
	

	// Convert arg2 hex
	if ( !StringHexToNum(argv[1],&Arg2Num,0))
	{
		return CommandResult_Error_WrongArgument1;
	}

	#ifdef FLASH_H_
	uint8_t Buffer[1];
	FLASH_Read(Arg2Num,Buffer,1,5000);
	
	
	uprintf("address: 0x%w\r\n"
			"data:    0x%b\r\n",
			Arg2Num,
			Buffer[0]
			);
	#endif
	
	return CommandResult_Ok;
}




// Function: FLASH write
// Syntax: flashwrite <address> <data>
uint32_t CommandFunction_flashwrite	( uint32_t argc, char** argv ) {

	// Convert arg2 hex
	if ( !StringHexToNum(argv[1],&Arg2Num,0))
	{
		return CommandResult_Error_WrongArgument1;
	}
	
	// Convert arg3, decimal
	if (!UnsignedDecimalStringToNum(argv[2],&Arg3Num))
	{
		return CommandResult_Error_WrongArgument2;
	}


	#ifdef FLASH_H_
	uint8_t Buffer[1];
	
	Buffer[0] = (uint8_t)Arg3Num;
		
	FLASH_Write(Arg2Num,Buffer,1,5000);
	#endif
	
	uprintf("address: 0x%w\r\n"
			"data:    0x%b\r\n",
			Arg2Num,
			Arg3Num);

	
	return CommandResult_Ok;
}



// Function: Raspberry pi bridge
#if 0
uint32_t CommandFunction_raspberrypi	( uint32_t argc, char** argv ) {

#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
	if ( argc < 3 )
	{
		uprintf("Too few arguments!\r\n");
		return RETURN_FALSE;
	}
	if ( argc > 3 )
	{
		uprintf("Too many arguments!\r\n");
		return RETURN_FALSE;
	}
	
	// Convert arg3, decimal
	if (!UnsignedDecimalStringToNum(COMMAND_Arguments[2],&Arg3Num))
	{
		USART_SendString("Wrong 1. argument!\r\n");
		return RETURN_FALSE;
	}
	
	if (!StrCmp(COMMAND_Arguments[1],"setout"))
	{
		// setout
		HOMEAUTMESSAGE_CreateAndSendHomeAutMessage(
			0,255,
			Function_Command, Command_SetOutput,
			Arg3Num, 1);
	}

	
#endif
	
	
	return CommandResult_Ok;
}
#endif


/*
// Function: ESP8266 bridge
uint32_t CommandFunction_ESP8266	( uint32_t argc, char** COMMAND_Arguments ) {

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
*/


/* END OF COMMAND FUNCTIONS */
