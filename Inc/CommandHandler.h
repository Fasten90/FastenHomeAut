/*
 *		CommandHandler.h
 *
 *		Created on		2013.10.08.
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Command Handler: communication with UART, execute commands
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef COMMANDHANDLER_H_
#define COMMANDHANDLER_H_


#include "include.h"
#include "escapesequence.h"
#include "communication.h"
#include "board.h"
#include "version.h"


///////////////////////////// MONITOR


/////////////////////////////
// DEFINES
// TYPEDEFS
// EXTERNED GLOBAL VARIABLES
// EXTERNED PROTOTYPES
/////////////////////////////



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


//	Config defines


#define MONITOR_MAX_COMMAND_LENGTH			(255U)
#define MONITOR_COMMAND_ARG_MAX_COUNT		(3)
#define MONITOR_COMMAND_ARG_MAX_NUM_BITS	(0x07)	// 0b111 <-- warning: "binary constants are a GCC extension

#define MONITOR_HISTORY_MAX_COUNT			(5)


#define MONITOR_DUMMY_STRING				"____________________________________________\r\n"


#define MONITOR_SEND_WELCOME()								\
		MONITOR_SendMessage(								\
		"\r\n"												\
		"Welcome in Monitor program!\r\n"					\
		"Version: " VERSION_DEFINE "\r\n"					\
		DATE_VERSION "\r\n"									\
		TIME_VERSION "\r\n"									\
		"Used panel: " BOARD_NAME "\r\n"					\
		"(c) Copyright, Vizi Gabor\r\n"						\
		"\r\n"												\
		"Try \"help\" command for help!\r\n")


#define MONITOR_SEND_PROMT_NEW_LINE()	\
		MONITOR_SendMessage("\r\n# ")

#define MONITOR_SEND_PROMT()			\
		MONITOR_SendMessage("# ")


#define USART_SEND_DUMMYSTRING()		MONITOR_SendMessage(MONITOR_DUMMY_STRING)


#define MONITOR_SEND_NEW_LINE()			\
		MONITOR_SendMessage("\r\n")


//	DEFINES FOR CHARACTERS

CONFIG_USE_TERMINAL_ZOC
#if defined(CONFIG_USE_TERMINAL_ZOC) || defined(CONFIG_USE_TERMINAL_PUTTY)
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
		MONITOR_SendChar(8)

// 127 = DEL = Delete
#define USART_SEND_KEY_DEL()		 \
		MONITOR_SendChar(127)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/// Command results
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


/// Command Function pointer
typedef CommandResult_t ( *FunctionPointer )( uint32_t argc, char** COMMAND_Arguments );


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

extern volatile char MONITOR_CommandActual[MONITOR_MAX_COMMAND_LENGTH];
extern volatile CommProtocol_t MONITOR_CommandSource;

extern const bool MONITOR_CommandReceiveEnable;


#ifdef CONFIG_USE_FREERTOS
extern xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
extern xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
#endif



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void MONITOR_Init(void);
void MONITOR_SendWelcome(void);

void MONITOR_CheckCommand(void);

void MONITOR_WriteCommandHelp(CommandID_t commandID);
void MONITOR_SendCls(void);

void MONITOR_SendMessage(const char *message);
void MONITOR_SendLine(const char *message);
void MONITOR_SendChar(char c);


#endif /* COMMANDHANDLER_H_ */
