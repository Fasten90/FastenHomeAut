/*
 *		CommandHandler.h
 *		Created on:		2013-10-08.
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
  *		Function:		Command Handler: communication with UART, execute commands
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2017-01-16
 */

#ifndef COMMANDHANDLER_H_
#define COMMANDHANDLER_H_

#include "Communication.h"
#include "EscapeSequence.h"
#include "include.h"
#include "board.h"
#include "version.h"

#ifdef CONFIG_USE_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

//	Config defines
#define COMMANDHANDLER_MAX_MESSAGE_LENGTH			(255U)
#define COMMANDHANDLER_MAX_COMMAND_LENGTH			(255U)
#define COMMANDHANDLER_COMMAND_ARG_MAX_COUNT		(3)
#define COMMANDHANDLER_COMMAND_ARG_MAX_NUM_BITS		(0x07)	// 0b111 <-- warning: "binary constants are a GCC extension



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< Command results
typedef enum
{
	CommandResult_Unknown = 0,								///< Unknown result (do not use!)
	CommandResult_Ok,										///< Successful - but not sending successful message (e.g. write answer from command)
	CommandResult_Ok_SendSuccessful,						///< Successful - CommandHandler send "successful" message
	CommandResult_Error_WrongArgument1,						///< First argument is wrong (type or range)
	CommandResult_Error_WrongArgument2,						///< Second argument is wrong (type or range)
	CommandResult_Error_TooFewArgument,						///< Fewer argument than needed
	CommandResult_Error_WrongArgumentNum,					///< Not good argument number
	CommandResult_Error_TooManyArgument,					///< More argument than needed
	CommandResult_Error_CommandNotFound,					///< Command not found
	CommandResult_Error_Unknown								///< Unknown error (in command processing)
} CommandResult_t;


///< Command Function pointer
typedef CommandResult_t ( *CommandHandlerFunctionPointer )( uint32_t argc, char** CommandHandler_CommandArguments );


///< CommandID
typedef uint8_t CommandID_t;


///< CommandArgNum type
typedef enum
{
	CommandArgument_0 = (1 << 0),
	CommandArgument_1 = (1 << 1),
	CommandArgument_2 = (1 << 2)
	// Set one by one bit
} CommandArgNum_t;


///< Command structure
typedef struct
{
	const char *name;											///< Name of command (which need type) [string]
	const CommandHandlerFunctionPointer commandFunctionPointer;	///< Function pointer (which called)
	const char *description;									///< Command description (for help)
	const char *syntax;											///< Command syntax
	const char *example;										///< Example of parameters
	const CommandArgNum_t commandArgNum;						///< Required command argument num
} CommandStruct;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern volatile CommProtocol_t CommandHandler_CommandSource;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void CommandHandler_Init(void);

void CommandHandler_PrintCommandHelp(CommandID_t commandID);

void CommandHandler_SendMessage(const char *message);
void CommandHandler_SendLine(const char *message);
void CommandHandler_SendChar(char c);
void CommandHandler_Printf(const char *format, ...);

void CommandHandler_PrintAllCommands(void);
void CommandHandler_SearchCommandAndPrintHelp(const char *command);

bool CommandHandler_PrepareFindExecuteCommand(CommProtocol_t source, char *command, char * response, size_t length);

void CommandHandler_UnitTest(void);



#endif /* COMMANDHANDLER_H_ */
