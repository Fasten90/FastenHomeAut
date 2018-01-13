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
#define CMDH_COMMAND_MAX_LENGTH				(255U)
#define CMDH_COMMAND_ARG_MAX_COUNT			(3)
#define CMDH_COMMAND_ARG_MAX_NUM_BITS		(0x0F)	// 0b1111 <-- warning: "binary constants are a GCC extension

#define CMDH_COMMAND_DELIMITER_CHAR			((char)' ')



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

// TODO: These are need?
#ifndef CONFIG_MODULE_COMMUNICATION_ENABLE
typedef uint8_t CommProtocol_t;
typedef uint8_t CommProtocolBit_t;

#ifndef CommProt_DebugUart
	#define CommProt_DebugUart		(0)
#endif

#ifndef CommProt_Count
	#define CommProt_Count			(1)
#endif

#ifndef CommProt_Unknown
	#define CommProt_Unknown		(0)
#endif
#endif

///< Command results
typedef enum
{
	CmdH_Result_Unknown = 0,								///< Unknown result (do not use!)
	CmdH_Result_Ok,											///< Successful - but not sending successful message (e.g. write answer from command)
	CmdH_Result_Ok_SendSuccessful,							///< Successful - CommandHandler send "successful" message
	CmdH_Result_Error_WrongArgument1,						///< First argument is wrong (type or range)
	CmdH_Result_Error_WrongArgument2,						///< Second argument is wrong (type or range)
	CmdH_Result_Error_WrongArgument3,						///< Third argument is wrong (type or range)
	CmdH_Result_Error_TooFewArgument,						///< Fewer argument than needed
	CmdH_Result_Error_WrongArgumentNum,						///< Not good argument number
	CmdH_Result_Error_TooManyArgument,						///< More argument than needed
	CmdH_Result_Error_CommandNotFound,						///< Command not found
	CmdH_Result_Error_CallCmdHandlerWithInvalidArgument,	///< Called Command Handler with invalid argument
	CmdH_Result_Error_Unknown								///< Unknown error (in command processing)
} CmdH_Result_t;


///< Command Function pointer
typedef CmdH_Result_t (*CmdH_FunctionPointer_t)(uint32_t argc, char** args);


///< Command ID
typedef uint8_t CmdH_CommandID_t;


///< Command Argument number
typedef enum
{
	CmdH_CommandArgNum_0 = (1 << 0),
	CmdH_CommandArgNum_1 = (1 << 1),
	CmdH_CommandArgNum_2 = (1 << 2),
	CmdH_CommandArgNum_3 = (1 << 3)

	// Set one by one bit
} CmdH_CommandArgNum_t;


///< Command structure
typedef struct
{
	const char *name;											///< Name of command (which need type) [string]
	const CmdH_FunctionPointer_t commandFunctionPointer;		///< Function pointer (which called)
	const char *description;									///< Command description (for help)
	const char *syntax;											///< Command syntax
	const char *example;										///< Example of parameters
	const CmdH_CommandArgNum_t commandArgNum;					///< Required command argument num

	// TODO: Add optional enabled communication source
} CmdH_Command_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void CmdH_Init(void);

CmdH_Result_t CmdH_ExecuteCommand(char *command, char *response, size_t length);
CmdH_Result_t CmdH_ExecuteCommandWithParams(char *command, char *param, char *response, size_t length);

void CmdH_PrintAllCommands(void);
void CmdH_SearchCommandAndPrintHelp(const char *command);

void CmdH_PrintResult(CmdH_Result_t result);

void CmdH_SetResponse(char *respBuffer, size_t respLength);

void CmdH_SendMessage(const char *message);
void CmdH_SendLine(const char *message);
void CmdH_SendChar(char c);
void CmdH_Printf(const char *format, ...);

void CmdH_UnitTest(void);



#endif /* COMMANDHANDLER_H_ */
