/*
 *		command.h
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Command list
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef COMMAND_H_
#define COMMAND_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"
//#include "CommandHandler.h"

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


#define NUM_OF(x) (sizeof(x)/sizeof(x[0]))
	
#define MONITOR_MAX_COMMAND_NUM				(uint8_t)( NUM_OF(CommandList))


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


// Command results
typedef enum
{
	CommandResult_Unknown = 0,
	CommandResult_Ok,
	CommandResult_Error_WrongArgument1,
	CommandResult_Error_WrongArgument2,
	CommandResult_Error_TooFewArgument,
	CommandResult_Error_WrongArgumentNum,
	CommandResult_Error_TooManyArgument,
	CommandResult_Error_CommandArgNumIsWrong,
	CommandResult_Error_Unknown
} CommandResult_t;


// Command Function pointer
typedef CommandResult_t ( *FunctionPointer )( uint32_t argc, char** COMMAND_Arguments );


// CommandID
typedef uint8_t CommandID_t;


// commandArgNum type
typedef enum
{
	CommandArgument_0 = (1 << 0),
	CommandArgument_1 = (1 << 1),
	CommandArgument_2 = (1 << 2)
} CommandArgNum_t;


// Command struct
typedef struct
{
	const char *name;
	const FunctionPointer *commandFunctionPointer;
	const char *description;
	const char *syntax;
	const CommandArgNum_t commandArgNum;
} CommandStruct;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const CommandStruct CommandList[];

extern const CommandID_t MONITOR_CommandNum;



/*------------------------------------------------------------------------------
*  Global function declarations
*----------------------------------------------------------------------------*/
 



// COMMANDS

CommandResult_t CommandFunction_cls	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_help	( uint32_t argc, char** argv );

CommandResult_t CommandFunction_version ( uint32_t argc, char** argv );
CommandResult_t CommandFunction_welcome ( uint32_t argc, char** argv );

CommandResult_t CommandFunction_test	( uint32_t argc, char** argv );

CommandResult_t CommandFunction_set	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_get	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_GlobalVariableHelp ( uint32_t argc, char** argv );
CommandResult_t CommandFunction_GlobalVariableList ( uint32_t argc, char** argv );
CommandResult_t CommandFunction_GlobalVariableValueList ( uint32_t argc, char** argv );


CommandResult_t CommandFunction_reset	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_led	( uint32_t argc, char** argv );

CommandResult_t CommandFunction_flashdel	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_flashread	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_flashwrite	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_temp	( uint32_t argc, char** argv );

CommandResult_t CommandFunction_raspberrypi ( uint32_t argc, char** argv );

CommandResult_t CommandFunction_unittest (uint32_t argc, char** argv);
CommandResult_t CommandFunction_moduletest (uint32_t argc, char** argv);

CommandResult_t CommandFunction_dac (uint32_t argc, char** argv);
CommandResult_t CommandFunction_io (uint32_t argc, char** argv);
CommandResult_t CommandFunction_adc (uint32_t argc, char** argv);

/*
CommandResult_t CommandFunction_buzzer	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_accelerometer	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_gyroscope	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_remotecontrol	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_proximity	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_log 	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_exit	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_read	( uint32_t argc, char** argv );

CommandResult_t CommandFunction_start	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_stop	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_dl		( uint32_t argc, char** argv );
CommandResult_t CommandFunction_go		( uint32_t argc, char** argv );

CommandResult_t CommandFunction_mr		( uint32_t argc, char** argv );
CommandResult_t CommandFunction_mw		( uint32_t argc, char** argv );
CommandResult_t CommandFunction_btn	( uint32_t argc, char** argv );

CommandResult_t CommandFunction_lcd	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_stop	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_romr	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_romw	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_romsr	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_romwe	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_rominit	( uint32_t argc, char** argv );

CommandResult_t CommandFunction_rfm12b	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_keypad	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_standby	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_rtc	( uint32_t argc, char** argv );

CommandResult_t CommandFunction_ESP8266	( uint32_t argc, char** argv );
*/



#endif /* COMMAND_H_ */
