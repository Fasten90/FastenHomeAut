/*
 *		CommandHandler.c
 *		Created on:		2013-10-08.
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Command Handler: communication with UART, execute commands
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2017-01-16
 */


#include <stdarg.h>
#include "GenericTypeDefs.h"
#include "String.h"
#include "CommandList.h"
#include "DebugUart.h"
#include "EscapeSequence.h"
#include "GlobalVarHandler.h"
#include "CommandHandler.h"
#include "TaskList.h"
#include "EventHandler.h"
#include "Debug.h"
#if defined(CONFIG_MODULE_GLOBALVARHANDLER_ENABLE) && defined(CONFIG_GLOBALVARHANDLER_CHECK_ENABLE)
#include "GlobalVariables.h"
#endif

#ifdef MODULE_COMMANDHANDLER_UNITTEST_ENABLE
#include "UnitTest.h"
#endif


#ifdef CONFIG_MODULE_COMMANDHANDLER_ENABLE

/*------------------------------------------------------------------------------
 *									Configs
 *----------------------------------------------------------------------------*/

static const char CmdH_DelimiterChar = ' ';



/*------------------------------------------------------------------------------
 *								Global variables
 *----------------------------------------------------------------------------*/

static CommProtocol_t CmdH_CommandSource = CommProt_Unknown;
static char * CmdH_ResponseBuffer = NULL;			///< pointer to a buffer where the command handler can respond
static size_t CmdH_ResponseBufferLength = 0;		///< maximal length of CmdH_ResponseBuffer (to avoid buffer overwriting)

static uint8_t CmdH_CommandArgCount = 0;
static char *CmdH_CommandArguments[CMDH_COMMAND_ARG_MAX_COUNT] = { 0 } ;



/*------------------------------------------------------------------------------
 *								Function prototypes
 *----------------------------------------------------------------------------*/

static uint8_t CmdH_CommandParser(char *command, char **dst);
static CmdH_Result_t CmdH_SearchCommand(void);

static CmdH_Result_t CmdH_RunCommand(CmdH_CommandID_t commandID);
static CmdH_Result_t CmdH_CheckArgumentNumIsGood(uint8_t receivedArgNum, uint8_t commandArgNum);

static void CmdH_PrintCommandHelp(CmdH_CommandID_t commandID);



/*------------------------------------------------------------------------------
 *								Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief	Initialize CommandHandler program
 */
void CmdH_Init(void)
{
	// Check CmdH_CommandList[]
	CmdH_CommandID_t i;
	for (i = 0; i < CmdH_CommandNum; i++)
	{
		ASSERT(CmdH_CommandList[i].commandFunctionPointer != NULL);		// Need command function pointer
		ASSERT(CmdH_CommandList[i].name != NULL);						// Need name
		ASSERT(CmdH_CommandList[i].commandArgNum != 0);					// Need command argument number
		ASSERT(CmdH_CommandList[i].commandArgNum <= CMDH_COMMAND_ARG_MAX_NUM_BITS);

		// example, description, syntax are optional parameters
	}
}



/**
 * \brief	Prepare (Separate) the command and Find and Run it...
 * \note	Be careful! Only one call enabled, because this module use global variables
 */
CmdH_Result_t CmdH_ExecuteCommand(CommProtocol_t source, char *command, char * response, size_t length)
{
	CmdH_Result_t result = CmdH_Result_Unknown;

	// Check parameters
	if (response == NULL)
	{
		Debug_Print(Debug_CommandHandler, "Error! Wrong response buffer!");
		result = CmdH_Result_Error_CallCmdHandlerWithInvalidArgument;
	}
	if (source >= CommProt_Count)
	{
		Debug_Print(Debug_CommandHandler, "Error! Wrong source!");
		result = CmdH_Result_Error_CallCmdHandlerWithInvalidArgument;
	}
	if (command == NULL)
	{
		Debug_Print(Debug_CommandHandler, "Error! Command string pointer is wrong!");
		result = CmdH_Result_Error_CallCmdHandlerWithInvalidArgument;
	}
	if (length == 0)
	{
		Debug_Print(Debug_CommandHandler, "Error! Received 0 length response buffer!");
		result = CmdH_Result_Error_CallCmdHandlerWithInvalidArgument;
	}
	if (StringLength(command) == 0)
	{
		Debug_Print(Debug_CommandHandler, "Error! Received 0 length command!");
		result = CmdH_Result_Error_CallCmdHandlerWithInvalidArgument;
	}

	if (result != CmdH_Result_Error_CallCmdHandlerWithInvalidArgument)
	{
		// Log event
		EventHandler_GenerateEvent(Event_CommandHandler_ProcessCommand, 0, Task_CommandHandlerProcessEvent);

		// Separate command
		CmdH_CommandArgCount = CmdH_CommandParser(command, CmdH_CommandArguments);

		// Check parse result
		if (CmdH_CommandArgCount > 0 && CmdH_CommandArgCount <= CMDH_COMMAND_ARG_MAX_COUNT)
		{
			// Save important values
			CmdH_CommandSource = source;
			CmdH_ResponseBuffer = response;
			CmdH_ResponseBufferLength = length;

			result = CmdH_SearchCommand();		// Find and execute the command
		}
		else
		{
			result = CmdH_Result_Error_WrongArgumentNum;	// 0 or too large argument num
		}

#if defined(CONFIG_COMMANDHANDLER_NOTIFY_COMMAND_RECEIVED_FROM_NOT_DEBUGPORT) && defined(CONFIG_MODULE_DEBUGUART_ENABLE)
		if (source != CommProt_DebugUart)
		{
			uprintf("Received command: \"%s\", from %s\r\n", command, COMMUNICATION_GetProtocolName(source));
		}
#endif
	}

	return result;
}



/**
 * \brief	Preparing the command and its parameters, than find and run it...
 * \command	Command string (must consinst of 1 word!)
 * \param	Parameter list
 * \note	Be careful! Only one call enabled, because this module use global variables
 */
CmdH_Result_t CmdH_ExecuteCommandWithParams(CommProtocol_t source, char *command, char * param, char * response, size_t length)
{
	ASSERT(source < CommProt_Count);
	ASSERT(command != NULL);
	ASSERT(param != NULL);
	ASSERT(response != NULL);

	CmdH_Result_t result = CmdH_Result_Unknown;

	EventHandler_GenerateEvent(Event_CommandHandler_ProcessCommand, 0, Task_CommandHandlerProcessEvent);	// Log event

	// Separate command and its parameteres:
	CmdH_CommandArguments[0] = command;
	CmdH_CommandArgCount = CmdH_CommandParser(param, &CmdH_CommandArguments[1]) + 1;						// +1: command is 1 word

	if (CmdH_CommandArgCount > 0 && CmdH_CommandArgCount <= CMDH_COMMAND_ARG_MAX_COUNT)						// Check parsing result
	{
		// Save important values
		CmdH_CommandSource = source;
		CmdH_ResponseBuffer = response;
		CmdH_ResponseBufferLength = length;

		result = CmdH_SearchCommand();																		// Find and execute the command
	}
	else
	{
		result = (CmdH_CommandArgCount > CMDH_COMMAND_ARG_MAX_COUNT) ? CmdH_Result_Error_TooManyArgument : CmdH_Result_Error_WrongArgumentNum;
	}

#if defined(CONFIG_COMMANDHANDLER_NOTIFY_COMMAND_RECEIVED_FROM_NOT_DEBUGPORT) && defined(CONFIG_MODULE_DEBUGUART_ENABLE)
	if (source != CommProt_DebugUart)
	{
		uprintf("Received command: \"%s\", from %s\r\n", command, COMMUNICATION_GetProtocolName(source));
	}
#endif

	return result;
}



/**
 * \brief	Separate command to parameters/arguments
 * 			: from ActualCommand to COMMAND_Arguments[0], [1], [2]
 * 	\note	Only call from CommandHandler_PrepareFindExecuteCommand()
 * 	\return	argument number
 */
static uint8_t CmdH_CommandParser(char *command, char **dst)
{
	uint8_t commandArgCount;

	commandArgCount = STRING_Splitter(command, CmdH_DelimiterChar, dst, CMDH_COMMAND_ARG_MAX_COUNT);
	// NOTE: Now, ArgCount never will be larger than CMDH_COMMAND_ARG_MAX_COUNT

	return commandArgCount;
}



/**
 * \brief	Find the command (in list)
 */
static CmdH_Result_t CmdH_SearchCommand(void)
{
	CmdH_CommandID_t i;
	CmdH_Result_t result = CmdH_Result_Error_CommandNotFound;

	// Search the command
	for (i = 0; i < CmdH_CommandNum; i++)
	{

		if (!StrCmp(CmdH_CommandArguments[0], CmdH_CommandList[i].name))
		{
			result = CmdH_RunCommand(i);		// Found the command
			EventHandler_GenerateEvent(Event_CommandHandler_ProcessCommand, i, Task_CommandHandlerProcessEvent);	// Event
			break;
		}
	}

	return result;										// Return with validation
}



/**
 * \brief	Check result and write response
 */
void CmdH_PrintResult(CmdH_Result_t result)
{
	const char *pMessage = NULL;

	switch (result)
	{
		case CmdH_Result_Unknown:
			pMessage = "Unknown error";
			break;

		case CmdH_Result_Ok:
			// Do not send response
			break;

		case CmdH_Result_Ok_SendSuccessful:
			pMessage = "Successful";
			break;

		case CmdH_Result_Error_WrongArgument1:
			pMessage = "Wrong argument (1.)";
			break;

		case CmdH_Result_Error_WrongArgument2:
			pMessage = "Wrong argument (2.)";
			break;

		case CmdH_Result_Error_WrongArgument3:
			pMessage = "Wrong argument (3.)";
			break;

		case CmdH_Result_Error_TooFewArgument:
			pMessage = "Too few argument";
			break;

		case CmdH_Result_Error_TooManyArgument:
			pMessage = "Too many argument";
			break;

		case CmdH_Result_Error_WrongArgumentNum:
			pMessage = "Wrong argument num";
			break;

		case CmdH_Result_Error_Unknown:
			pMessage = "Unknown error";
			break;

		case CmdH_Result_Error_CommandNotFound:
			pMessage = "Unknown Command\r\n"
						"Type \"help\" for help";
			break;

		case CmdH_Result_Error_CallCmdHandlerWithInvalidArgument:
			pMessage = "Called CommandHandler with invalid argument";
			break;

		default:
			pMessage = "Unknown command process";
			break;
	}

	CmdH_SendLine(pMessage);
}



/**
 * \brief	Run command
 */
static CmdH_Result_t CmdH_RunCommand(CmdH_CommandID_t commandID)
{
	CmdH_Result_t result;
	bool needWriteHelp = false;

	// Check argument nums
	result = CmdH_CheckArgumentNumIsGood(CmdH_CommandArgCount, CmdH_CommandList[commandID].commandArgNum);

	if (result == CmdH_Result_Ok)
	{
		// Good, found & argument num is ok

		// Call function
		CmdH_FunctionPointer_t thisFunction = (CmdH_FunctionPointer_t)CmdH_CommandList[commandID].commandFunctionPointer;

		// Execute the command function
		result = thisFunction(CmdH_CommandArgCount, (char**)CmdH_CommandArguments);
	}
	else
	{
		needWriteHelp = true;		// if not Ok, write help
	}

	// Write command help, if command run failed
	if (needWriteHelp)
	{
		CmdH_PrintCommandHelp(commandID);
	}

	return result;
}



/**
 * \brief	Print all commands
 */
void CmdH_PrintAllCommands(void)
{
	CmdH_CommandID_t i;

	CmdH_SendLine("Using help:\r\n"
			"help <command>\r\n\r\n"
			"Commands list:");
	for (i = 0; i < CmdH_CommandNum; i++)
	{
		CmdH_SendLine(CmdH_CommandList[i].name);	// Write a command
	}
}



/**
 * \brief	Search command and print help
 */
void CmdH_SearchCommandAndPrintHelp(const char *command)
{
	CmdH_CommandID_t i;
	bool isOk = false;

	for (i = 0; i < CmdH_CommandNum; i++)
	{
		// Find the command
		if (!StrCmp(CmdH_CommandList[i].name, command))
		{
			// Command's describe
			CmdH_PrintCommandHelp(i);
			isOk = true;
		}
	}

	if (!isOk)
	{
		CmdH_Printf("Not find command: %s\r\n", command);
	}
}



/**
 * \brief	Print a command help
 */
static void CmdH_PrintCommandHelp(CmdH_CommandID_t commandID)
{
	// Print command to source
	CmdH_Printf(
			"Command name: %s\r\n"
			"Function: %s\r\n"
			"Syntax: %s %s\r\n"
			"Example: %s %s\r\n"
			"Note: <required parameter> (optional parameter)\r\n",
			CmdH_CommandList[commandID].name,
			CmdH_CommandList[commandID].description,
			CmdH_CommandList[commandID].name, CmdH_CommandList[commandID].syntax,
			CmdH_CommandList[commandID].name, CmdH_CommandList[commandID].example);

	// TODO: Kiírni a szükséges paraméter számot
}



/**
 * \brief	Check actual command argument num from settings
 */
static CmdH_Result_t CmdH_CheckArgumentNumIsGood(uint8_t receivedArgNum, uint8_t commandArgNum)
{
	// Check commandArgNum. bit is set?
	if (receivedArgNum > CMDH_COMMAND_ARG_MAX_COUNT)
	{
		return CmdH_Result_Error_TooManyArgument;
	}
	// commandArgNum not should to check, because checked in Init()

	if (commandArgNum & (1 << (receivedArgNum-1)))
	{
		return CmdH_Result_Ok;		// Good, there is this bit
	}
	else
	{
		// Check min-max
		volatile uint8_t maxRequiredArgNum = 0;
		uint8_t minRequiredArgNum = 0;
		uint8_t i;
		for (i = 0; i < CMDH_COMMAND_ARG_MAX_COUNT; i++)
		{
			if (commandArgNum & (1 << i))
			{
				maxRequiredArgNum = i+1;
				if (minRequiredArgNum == 0)
				{
					minRequiredArgNum = i+1;
				}
			}
		}
		
		// Check min, max
		if (receivedArgNum < minRequiredArgNum)
		{
			return CmdH_Result_Error_TooFewArgument;		// Too few
		}
		else if (receivedArgNum > maxRequiredArgNum)
		{
			return CmdH_Result_Error_TooManyArgument;		// Too many
		}
		else
		{
			return CmdH_Result_Error_WrongArgumentNum;	// Wrong num
		}
	}
}



/**
 * \brief	Send message to source
 */
void CmdH_SendMessage(const char *message)
{
	if ((CmdH_ResponseBuffer != NULL) && (message != NULL))
	{
		size_t length = StrCpyMax(CmdH_ResponseBuffer, message, CmdH_ResponseBufferLength);
		CmdH_ResponseBuffer += length;
		CmdH_ResponseBufferLength -= length;
		// Not need check length, because StrCpyMax is length safe (worst case: length = 0, but never larger than ResponseBufferLength)
	}

	//COMMUNICATION_SendMessage(CmdH_CommandSource, message);
}



/**
 * \brief	Send line (with newline) to source
 */
void CmdH_SendLine(const char *message)
{
	if (CmdH_ResponseBuffer != NULL)
	{
		if (message != NULL)
		{
			CmdH_SendMessage(message);
		}
		CmdH_SendMessage("\r\n");
	}

	/*
	if (message != NULL)
		CmdH_SendMessage(message);
	CmdH_SendMessage("\r\n");
	*/
}



/**
 * \brief	Send character
 */
void CmdH_SendChar(char c)
{
	if (CmdH_ResponseBufferLength > 0)
	{
		*CmdH_ResponseBuffer = c;
		CmdH_ResponseBuffer++;
		*CmdH_ResponseBuffer = '\0';
		CmdH_ResponseBufferLength--;
	}

	//COMMUNICATION_SendChar(CmdH_CommandSource, c);
}



/**
 * \brief	Send message on xy communication protocol
 * \param	format		format string, e.g. "decimal: %d"
 */
void CmdH_Printf(const char *format, ...)
{
	size_t length = 0;

	// Check buffer
	if ((CmdH_ResponseBuffer == NULL) || (CmdH_ResponseBufferLength == 0))
		return;

	// print formatted string to response buffer
	va_list ap;									// argument pointer
	va_start(ap, format); 						// ap on arg
	length = string_printf_safe(CmdH_ResponseBuffer, CmdH_ResponseBufferLength, format, ap);		// Separate and process
	va_end(ap);						 			// Cleaning after end

	CmdH_ResponseBuffer += length;
	CmdH_ResponseBufferLength -= length;
}



/**
 * \brief Set response buffer
 */
void CmdH_SetResponse(CommProtocol_t commandSource, char * respBuffer, size_t respLength)
{
	CmdH_CommandSource = commandSource;
	CmdH_ResponseBuffer = respBuffer;
	CmdH_ResponseBufferLength = respLength;
}



#ifdef MODULE_COMMANDHANDLER_UNITTEST_ENABLE
/**
 * \brief	CommandHandler unit test
 */
void CmdH_UnitTest(void)
{
	// Start of unittest
	UnitTest_Start("CommandHandler", __FILE__);

	CmdH_Result_t result;
#define CMD_UNITTEST_BUFFER_SIZE	(100U)
	char buffer[CMD_UNITTEST_BUFFER_SIZE];
	char cmdSrcBuffer[CMD_UNITTEST_BUFFER_SIZE];
	char cmdParamBuffer[CMD_UNITTEST_BUFFER_SIZE];


	// Check "version" (good) command
	result = CmdH_ExecuteCommand(CommProt_DebugUart, "version", buffer, CMD_UNITTEST_BUFFER_SIZE);

	// Check command execute result
	UNITTEST_ASSERT(result == CmdH_Result_Ok, "CmdH_PrepareFindExecuteCommand error");

	// Check command argument num
	UNITTEST_ASSERT(CmdH_CommandArgCount == 1, "CmdH_PrepareFindExecuteCommand error");

	// Check command source
	UNITTEST_ASSERT(CmdH_CommandSource == CommProt_DebugUart, "CmdH_PrepareFindExecuteCommand error");

	// Check separated / splitted command
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[0], "version"), "CmdH_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(CmdH_CommandArguments[1] == NULL, "CmdH_PrepareFindExecuteCommand error");


	// Check wrong command: not find
	result = CmdH_ExecuteCommand(CommProt_DebugUart, "WrongCommand", buffer, CMD_UNITTEST_BUFFER_SIZE);

	// Check command execute result
	UNITTEST_ASSERT(result == CmdH_Result_Error_CommandNotFound, "CmdH_PrepareFindExecuteCommand error");

	// Check command argument num: will be 1
	UNITTEST_ASSERT(CmdH_CommandArgCount == 1, "CmdH_PrepareFindExecuteCommand error");

	// Check command source
	UNITTEST_ASSERT(CmdH_CommandSource == CommProt_DebugUart, "CmdH_PrepareFindExecuteCommand error");

	// Check separated / splitted command
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[0], "WrongCommand"), "CmdH_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(CmdH_CommandArguments[1] == NULL, "CmdH_PrepareFindExecuteCommand error");


	// Check wrong command: too many arguments ( >3)
	StrCpy(cmdSrcBuffer, "version with lot of arguments");			// command string must reside in RAM!
	result = CmdH_ExecuteCommand(CommProt_DebugUart, cmdSrcBuffer, buffer, CMD_UNITTEST_BUFFER_SIZE);

	// Check command execute result: true/successful, because CommandHandler will find "version" command
	UNITTEST_ASSERT(result == CmdH_Result_Error_TooManyArgument, "CmdH_PrepareFindExecuteCommand error");

	// Check command argument num
	UNITTEST_ASSERT(CmdH_CommandArgCount == CMDH_COMMAND_ARG_MAX_COUNT, "CmdH_PrepareFindExecuteCommand error");

	// Check command source
	UNITTEST_ASSERT(CmdH_CommandSource == CommProt_DebugUart, "CmdH_PrepareFindExecuteCommand error");

	// Check separated / splitted command
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[0], "version"), "CmdH_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[1], "with"), "CmdH_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[2], "lot"), "CmdH_PrepareFindExecuteCommand error");

	//--------------------------------------------------------------------------------------------------
	// CmdH_ExecuteCommandWithParams test:

	//-----------------------------------------------------
	// test with 5 parameter:
	StrCpy(cmdSrcBuffer, "version");			// command string must reside in RAM!
	StrCpy(cmdParamBuffer, "with lot of arguments");			// command string must reside in RAM!
	result = CmdH_ExecuteCommandWithParams(CommProt_DebugUart, cmdSrcBuffer, cmdParamBuffer, buffer, CMD_UNITTEST_BUFFER_SIZE);

	// Check command execute result: true/successful, because CommandHandler will find "version" command
	UNITTEST_ASSERT(result == CmdH_Result_Error_TooManyArgument, "CmdH_PrepareFindExecuteCommand error");

	// Check command argument num
	UNITTEST_ASSERT(CmdH_CommandArgCount == 4, "CmdH_PrepareFindExecuteCommand error");

	// Check command source
	UNITTEST_ASSERT(CmdH_CommandSource == CommProt_DebugUart, "CmdH_PrepareFindExecuteCommand error");

	// Check separated / splitted command
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[0], "version"), "CmdH_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[1], "with"), "CmdH_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[2], "lot"), "CmdH_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[3], "of"), "CmdH_PrepareFindExecuteCommand error");


	//-----------------------------------------------------
	// test with 1 parameter:
	StrCpy(cmdSrcBuffer, "get");			// command string must reside in RAM!
	StrCpy(cmdParamBuffer, "SN");			// command string must reside in RAM!
	result = CmdH_ExecuteCommandWithParams(CommProt_DebugUart, cmdSrcBuffer, cmdParamBuffer, buffer, CMD_UNITTEST_BUFFER_SIZE);

	// Check command execute result: true/successful, because CommandHandler will find "version" command
	UNITTEST_ASSERT(result == CmdH_Result_Ok, "CmdH_PrepareFindExecuteCommand error");

	// Check command argument num
	UNITTEST_ASSERT(CmdH_CommandArgCount == 2, "CmdH_PrepareFindExecuteCommand error");

	// Check command source
	UNITTEST_ASSERT(CmdH_CommandSource == CommProt_DebugUart, "CmdH_PrepareFindExecuteCommand error");

	// Check separated / splitted command
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[0], "get"), "CmdH_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[1], "SN"), "CmdH_PrepareFindExecuteCommand error");


	//-----------------------------------------------------
#ifdef CONFIG_MODULE_LED_ENABLE
	// test with 2 parameters:
	StrCpy(cmdSrcBuffer, "led");			// command string must reside in RAM!
	StrCpy(cmdParamBuffer, "1 on");			// command string must reside in RAM!
	result = CmdH_ExecuteCommandWithParams(CommProt_DebugUart, cmdSrcBuffer, cmdParamBuffer, buffer, CMD_UNITTEST_BUFFER_SIZE);

	// Check command execute result: true/successful, because CommandHandler will find "version" command
	UNITTEST_ASSERT(result == CmdH_Result_Ok, "CmdH_PrepareFindExecuteCommand error");

	// Check command argument num
	UNITTEST_ASSERT(CmdH_CommandArgCount == 3, "CmdH_PrepareFindExecuteCommand error");

	// Check command source
	UNITTEST_ASSERT(CmdH_CommandSource == CommProt_DebugUart, "CmdH_PrepareFindExecuteCommand error");

	// Check separated / splitted command
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[0], "led"), "CmdH_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[1], "1"), "CmdH_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(!StrCmp(CmdH_CommandArguments[2], "on"), "CmdH_PrepareFindExecuteCommand error");
#endif

	// End of unittest
	UnitTest_End();
}

#endif

#endif	// #ifdef CONFIG_MODULE_COMMANDHANDLER_ENABLE
