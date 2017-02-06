/*
 *		CommandHandler.h
 *
 *		Created on		2013.10.08.
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Command Handler: communication with UART, execute commands
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016. dec. 13
 */

#ifndef COMMANDHANDLER_H_
#define COMMANDHANDLER_H_


#include "include.h"
#include "escapesequence.h"
#include "communication.h"
#include "board.h"
#include "version.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


//	Config defines


#define COMMANDHANDLER_MAX_COMMAND_LENGTH			(255U)
#define COMMANDHANDLER_COMMAND_ARG_MAX_COUNT		(3)
#define COMMANDHANDLER_COMMAND_ARG_MAX_NUM_BITS	(0x07)	// 0b111 <-- warning: "binary constants are a GCC extension

#define COMMANDHANDLER_HISTORY_MAX_COUNT			(5)


#define COMMANDHANDLER_SEND_WELCOME()						\
		CommandHandler_SendMessage(							\
		"\r\n"												\
		"Welcome in CommandHandler!\r\n"					\
		"Version: " VERSION_DEFINE "\r\n"					\
		"Date: " DATE_VERSION "\r\n"						\
		"Time: "TIME_VERSION "\r\n"							\
		"Used panel: " BOARD_NAME "\r\n"					\
		"(c) Copyright, Vizi Gabor\r\n"						\
		"\r\n"												\
		"Try \"help\" command for help!\r\n")


#define COMMANDHANDLER_SEND_PROMT_NEW_LINE()	CommandHandler_SendMessage("\r\n# ")

#define COMMANDHANDLER_SEND_PROMT()				CommandHandler_SendMessage("# ")

#define COMMANDHANDLER_SEND_NEW_LINE()			CommandHandler_SendMessage("\r\n")


///	DEFINES FOR CHARACTERS

CONFIG_COMMANDHANDLER_USE_TERMINAL_ZOC
#if defined(CONFIG_COMMANDHANDLER_USE_TERMINAL_ZOC) || defined(CONFIG_COMMANDHANDLER_USE_TERMINAL_PUTTY)
#define USART_KEY_DELETE			(0x08)
#define USART_KEY_BACKSPACE			(0x7F)
#else
#define USART_KEY_DELETE			(0x7F)
#define USART_KEY_BACKSPACE			(0x08)
#endif


#define USART_KEY_ENTER				('\r')
#define USART_KEY_SPACE				(' ')
#define USART_KEY_ESCAPE			('\x1B')
#define USART_KEY_BELL				('\a')


// NOTE: Changed \e to \x1B
#define USART_ESCAPESEQUENCE_1		('\x1B')
#define USART_ESCAPESEQUENCE_2		('[')


//	BUTTONS


// 8 = BS = Backspace
#define USART_SEND_KEY_BACKSPACE()	\
		CommandHandler_SendChar(8)

// 127 = DEL = Delete
#define USART_SEND_KEY_DEL()		 \
		CommandHandler_SendChar(127)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/// Command results
typedef enum
{
	CommandResult_Unknown = 0,
	CommandResult_Ok,
	CommandResult_Ok_SendSuccessful,
	CommandResult_Error_WrongArgument1,
	CommandResult_Error_WrongArgument2,
	CommandResult_Error_TooFewArgument,
	CommandResult_Error_WrongArgumentNum,
	CommandResult_Error_TooManyArgument,
	CommandResult_Error_CommandArgNumIsWrong,
	CommandResult_Error_CommandNotFound,
	CommandResult_Error_Unknown
} CommandResult_t;


/// Command Function pointer
typedef CommandResult_t ( *FunctionPointer )( uint32_t argc, char** CommandHandler_CommandArguments );


/// CommandID
typedef uint8_t CommandID_t;


/// CommandArgNum type
typedef enum
{
	CommandArgument_0 = (1 << 0),
	CommandArgument_1 = (1 << 1),
	CommandArgument_2 = (1 << 2)
} CommandArgNum_t;


/// Command structure
typedef struct
{
	const char *name;								///< Name of command (which need type) [string]
	const FunctionPointer *commandFunctionPointer;	///< Function pointer (which called)
	const char *description;						///< Command description (for help)
	const char *syntax;								///< Command syntax
	const CommandArgNum_t commandArgNum;			///< Required command argument num
} CommandStruct;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern volatile char CommandHandler_CommandActual[COMMANDHANDLER_MAX_COMMAND_LENGTH];
extern volatile CommProtocol_t CommandHandler_CommandSource;

extern const bool CommandHandler_CommandReceiveEnable;


#ifdef CONFIG_USE_FREERTOS
extern xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
extern xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
#endif



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void CommandHandler_Init(void);
void CommandHandler_SendWelcome(void);

void CommandHandler_CheckCommand(void);

void CommandHandler_WriteCommandHelp(CommandID_t commandID);
void CommandHandler_SendCls(void);

void CommandHandler_SendMessage(const char *message);
void CommandHandler_SendLine(const char *message);
void CommandHandler_SendChar(char c);
void CommandHandler_Printf(const char *format, ...);

void CommandHandler_PrintAllCommands(void);
void CommandHandler_SearchCommandAndPrintHelp(const char *command);

bool CommandHandler_PrepareFindExecuteCommand(CommProtocol_t source, char *command);


#endif /* COMMANDHANDLER_H_ */