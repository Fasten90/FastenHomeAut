/*
 *		CommandHandler.c
 *
 *		Created on:		2013.10.08.
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Command Handler: communication with UART, execute commands
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2017.01.16
 */

#include <stdint.h>
#include <stdarg.h>

#include "String.h"
#include "DebugUart.h"
#include "CommandList.h"
#include "EscapeSequence.h"
#include "GlobalVarHandler.h"
#include "CommandHandler.h"
#include "TaskList.h"
#include "EventHandler.h"

#ifdef MODULE_COMMANDHANDLER_UNITTEST_ENABLE
#include "UnitTest.h"
#endif



/*------------------------------------------------------------------------------
 *									Configs
 *----------------------------------------------------------------------------*/

static const char CommandHandler_DelimiterChar = ' ';



/*------------------------------------------------------------------------------
 *								Global variables
 *----------------------------------------------------------------------------*/

volatile CommProtocol_t CommandHandler_CommandSource = CommProt_Unknown;

static char CommandHandler_ProcessedCommandActual[COMMANDHANDLER_MAX_COMMAND_LENGTH] = { 0 };
static uint8_t CommandHandler_CommandArgCount = 0;
static char *CommandHandler_CommandArguments[COMMANDHANDLER_COMMAND_ARG_MAX_COUNT] = { 0 } ;



/*------------------------------------------------------------------------------
 *								Function prototypes
 *----------------------------------------------------------------------------*/

static uint8_t CommandHandler_CommandParser(void);
static bool CommandHandler_SearchCommand(void);

static void CommandHandler_CheckResultAndRespond(CommandResult_t result);
static CommandResult_t CommandHandler_RunCommand(CommandID_t commandID);
static CommandResult_t CommandHandler_CheckArgumentNumIsGood(uint8_t receivedArgNum,
		uint8_t commandArgNum);



/*------------------------------------------------------------------------------
 *								Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Initialize CommandHandler program
 */
void CommandHandler_Init(void)
{

#if defined(CONFIG_MODULE_GLOBALVARHANDLER_ENABLE) && defined(CONFIG_GLOBALVARHANDLER_CHECK_ENABLE)
	// Check GlobalVarList[]
	GlobalVarHandler_CheckCommandStructAreValid();
#endif

	// Check CommandList[]
	CommandID_t i;
	for (i = 0; i < CommandHandler_CommandNum; i++)
	{
		// Need command function pointer
		ASSERT(CommandList[i].commandFunctionPointer != NULL);

		// Need name
		ASSERT(CommandList[i].name != NULL);

		// Need command argument number
		ASSERT(CommandList[i].commandArgNum != 0);

		// example, description, syntax are optional parameters
	}

	return;
}



/**
 * \brief	Prepare (Separate) the command and Find and Run it...
 * \note	Be careful! Only one call enabled, because this module use global variables
 */
bool CommandHandler_PrepareFindExecuteCommand(CommProtocol_t source, char *command)
{
	bool isSuccessful = false;

	EventHandler_GenerateEvent(Event_CommandHandler_ProcessCommand, 0, Task_ProcessDebugUartReceivedCommand);

	// Save command (to buffer)
	StrCpyMax(CommandHandler_ProcessedCommandActual, command, COMMANDHANDLER_MAX_COMMAND_LENGTH);

	// Separate command
	CommandHandler_CommandArgCount = CommandHandler_CommandParser();
	CommandHandler_CommandSource = source;

	if (CommandHandler_CommandArgCount > 0
		&& CommandHandler_CommandArgCount <= COMMANDHANDLER_COMMAND_ARG_MAX_COUNT)
	{
		// Find and execute the command
		isSuccessful = CommandHandler_SearchCommand();
	}
	else
	{
		// 0 Argument num, Cannot separated, this is not a command
		isSuccessful = false;
	}

#if defined(CONFIG_COMMANDHANDLER_NOTIFY_NOT_DEBUG_COMMAND) && defined(CONFIG_MODULE_DEBUGUSART_ENABLE)
	if (source != CommProt_DebugUart)
	{
		uprintf("Received command: \"%s\", from %s\r\n",
			command,
			COMMUNICATION_GetProtocolName(source)
			);
	}
#endif

	return isSuccessful;
}



/**
 * \brief	Separate command to parameters/arguments
 * 			: from ActualCommand to COMMAND_Arguments[0], [1], [2]
 * 	\note	Only call from CommandHandler_PrepareFindExecuteCommand()
 * 	\return	argument number
 */
static uint8_t CommandHandler_CommandParser(void)
{

	uint8_t commandArgCount = 0;

	commandArgCount = STRING_Splitter((char*)CommandHandler_ProcessedCommandActual,
			CommandHandler_DelimiterChar, CommandHandler_CommandArguments,
			COMMANDHANDLER_COMMAND_ARG_MAX_COUNT);
	// NOTE: Now, ArgCount never will be larger than COMMANDHANDLER_COMMAND_ARG_MAX_COUNT

	// Check argument num
	if (commandArgCount > COMMANDHANDLER_COMMAND_ARG_MAX_COUNT)
	{
		// Too many arguments
		CommandHandler_SendLine("Too many arguments!");
		commandArgCount = 0;
	}

	return commandArgCount;
}



/**
 * \brief	Find the command (in list)
 */
static bool CommandHandler_SearchCommand(void)
{

	CommandID_t i;
	bool CommandValid = false;
	CommandResult_t result = CommandResult_Error_CommandNotFound;

	// Search the command
	for (i = 0; i < CommandHandler_CommandNum; i++)
	{

		if (!StrCmp(CommandHandler_CommandArguments[0], CommandList[i].name))
		{
			// Found the command
			result = CommandHandler_RunCommand(i);

			// Event
			EventHandler_GenerateEvent(Event_CommandHandler_ProcessCommand, i, Task_ProcessDebugUartReceivedCommand);

			// Valid Command
			CommandValid = true;
			break;
		}
	}

	// Write result
	CommandHandler_CheckResultAndRespond(result);

	// Return with validation
	return CommandValid;
}



/**
 * \brief	Check result and write response
 */
static void CommandHandler_CheckResultAndRespond(CommandResult_t result)
{
	const char *pMessage = NULL;

	switch (result)
	{
		case CommandResult_Unknown:
			pMessage = "Unknown error";
			break;

		case CommandResult_Ok:
			// Do not send response
			break;

		case CommandResult_Ok_SendSuccessful:
			pMessage = "Successful";
			break;

		case CommandResult_Error_WrongArgument1:
			pMessage = "Wrong argument (1.)";
			break;

		case CommandResult_Error_WrongArgument2:
			pMessage = "Wrong argument (2.)";
			break;

		case CommandResult_Error_TooFewArgument:
			pMessage = "Too few argument";
			break;

		case CommandResult_Error_TooManyArgument:
			pMessage = "Too many argument";
			break;

		case CommandResult_Error_WrongArgumentNum:
			pMessage = "Wrong argument num";
			break;

		case CommandResult_Error_CommandArgNumIsWrong:
			pMessage = "Command set is wrong";
			break;

		case CommandResult_Error_Unknown:
			pMessage = "Unknown error";
			break;

		case CommandResult_Error_CommandNotFound:
			pMessage = 	"Unknown Command\r\n"
						"Type \"help\" for help";
			break;

		default:
			pMessage = "Unknown command process";
			break;
	}

	CommandHandler_SendLine(pMessage);
}



/**
 * \brief	Run command
 */
static CommandResult_t CommandHandler_RunCommand(CommandID_t commandID)
{

	CommandResult_t result;
	bool needWriteHelp = false;

	// Check argument nums
	result = CommandHandler_CheckArgumentNumIsGood(
			CommandHandler_CommandArgCount, CommandList[commandID].commandArgNum);

	if (result == CommandResult_Ok)
	{
		// Good, found & argument num is ok

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
		// Call function
		CommandHandlerFunctionPointer thisFunction = (CommandHandlerFunctionPointer)CommandList[commandID].commandFunctionPointer;
#pragma GCC diagnostic pop
		// End of warning suppress

		// Execute the command function
		result = thisFunction(CommandHandler_CommandArgCount, (char**)CommandHandler_CommandArguments);
	}
	else
	{
		// if not Ok, write help
		needWriteHelp = true;
	}

	// Write command help, if command run failed
	if (needWriteHelp)
	{
		CommandHandler_PrintCommandHelp(commandID);
	}

	return result;
}



/**
 * \brief	Print all commands
 */
void CommandHandler_PrintAllCommands(void)
{
	CommandID_t i;

	CommandHandler_SendLine("Using help:\r\n"
			"help <command>\r\n\r\n"
			"Commands list:");
	for (i=0; i < CommandHandler_CommandNum; i++)
	{
		// Write a command
		CommandHandler_SendLine(CommandList[i].name);
	}
}



/**
 * \brief	Search command and print help
 */
void CommandHandler_SearchCommandAndPrintHelp(const char *command)
{
	CommandID_t i;
	bool isOk = false;

	for (i = 0; i < CommandHandler_CommandNum; i++)
	{
		// Find the command
		if (!StrCmp(CommandList[i].name, command))
		{
			// Command's describe
			CommandHandler_PrintCommandHelp(i);
			isOk = true;
		}
	}

	if (!isOk)
	{
		CommandHandler_Printf("Not find command: %s\r\n", command);
	}
}



/**
 * \brief	Print a command help
 */
void CommandHandler_PrintCommandHelp(CommandID_t commandID)
{

	// Print command to source
	CommandHandler_Printf(
			"Command name: %s\r\n"
			"Function: %s\r\n"
			"Syntax: %s %s\r\n"
			"Example: %s %s\r\n"
			"Note: <required parameter> (optional parameter)\r\n",
			CommandList[commandID].name,
			CommandList[commandID].description,
			CommandList[commandID].name, CommandList[commandID].syntax,
			CommandList[commandID].name, CommandList[commandID].example);
}



/**
 * \brief	Check actual command argument num from settings
 */
static CommandResult_t CommandHandler_CheckArgumentNumIsGood(uint8_t receivedArgNum,
		uint8_t commandArgNum)
{
	// Check commandArgNum. bit is set?
	if (receivedArgNum > COMMANDHANDLER_COMMAND_ARG_MAX_COUNT)
	{
		return CommandResult_Error_TooManyArgument;
	}
	if (commandArgNum > COMMANDHANDLER_COMMAND_ARG_MAX_NUM_BITS)
	{
		return CommandResult_Error_CommandArgNumIsWrong;
	}

	if (commandArgNum & (1 << (receivedArgNum-1)))
	{
		// Good, there is this bit
		return CommandResult_Ok;
	}
	else
	{
		// Check min-max
		volatile uint8_t maxRequiredArgNum = 0;
		uint8_t minRequiredArgNum = 0;
		uint8_t i;
		for (i = 0; i < COMMANDHANDLER_COMMAND_ARG_MAX_COUNT; i++)
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
			// Too few
			return CommandResult_Error_TooFewArgument;
		}
		else if (receivedArgNum > maxRequiredArgNum)
		{
			// Too many
			return CommandResult_Error_TooManyArgument;
		}
		else
		{
			// Wrong num
			return CommandResult_Error_WrongArgumentNum;
		}
	}
}



/**
 * \brief	Send message to source
 */
void CommandHandler_SendMessage(const char *message)
{
	COMMUNICATION_SendMessage(CommandHandler_CommandSource, message);
}



/**
 * \brief	Send line (with newline) to source
 */
void CommandHandler_SendLine(const char *message)
{
	if (message != NULL)
		CommandHandler_SendMessage(message);
	CommandHandler_SendMessage("\r\n");
}



/**
 * \brief	Send character
 */
void CommandHandler_SendChar(char c)
{
	COMMUNICATION_SendChar(CommandHandler_CommandSource, c);
}



/**
 * \brief	Send message on xy communication protocol
 * \param	protocol		what peripheral sending
 */
void CommandHandler_Printf(const char *format, ...)
{

	// Working in at:
	char txBuffer[COMMANDHANDLER_MAX_MESSAGE_LENGTH];

#ifdef CONFIG_DEBUG_MODE
	txBuffer[COMMANDHANDLER_MAX_MESSAGE_LENGTH-1] = 0xEF;
#endif

	va_list ap;									// argument pointer
	va_start(ap, format); 						// ap on arg
	string_printf(txBuffer, format, ap);		// Separate and process
	va_end(ap);						 			// Cleaning after end

#ifdef CONFIG_DEBUG_MODE
	if (txBuffer[COMMANDHANDLER_MAX_MESSAGE_LENGTH-1] != 0xEF) DEBUG_BREAKPOINT();
#endif

	CommandHandler_SendMessage(txBuffer);

	return;
}



#ifdef MODULE_COMMANDHANDLER_UNITTEST_ENABLE
/**
 * \brief	CommandHandler unit test
 */
void CommandHandler_UnitTest(void)
{

	// Start of unittest
	UnitTest_Start("CommandHandler", __FILE__);

#ifndef CONFIG_PROTOCOL_BUFFER_ENABLE
#error "CommandHandler unit test need buffer protocol!"
#endif

	bool result;

	// Check "version" (good) command
	result = CommandHandler_PrepareFindExecuteCommand(CommProt_Buffer, "version");

	// Check command execute result
	UNITTEST_ASSERT(result,
			"CommandHandler_PrepareFindExecuteCommand error");
	// Check command argument num
	UNITTEST_ASSERT(CommandHandler_CommandArgCount == 1,
			"CommandHandler_PrepareFindExecuteCommand error");
	// Check command source
	UNITTEST_ASSERT(CommandHandler_CommandSource == CommProt_Buffer,
			"CommandHandler_PrepareFindExecuteCommand error");
	// Check separated / splitted command
	UNITTEST_ASSERT(!StrCmp(CommandHandler_CommandArguments[0], "version"),
			"CommandHandler_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(CommandHandler_CommandArguments[1] == NULL,
			"CommandHandler_PrepareFindExecuteCommand error");


	// Check wrong command: not find
	result = CommandHandler_PrepareFindExecuteCommand(CommProt_Buffer, "WrongCommand");
	// Check command execute result
	UNITTEST_ASSERT(!result,
			"CommandHandler_PrepareFindExecuteCommand error");
	// Check command argument num: will be 1
	UNITTEST_ASSERT(CommandHandler_CommandArgCount == 	1,
			"CommandHandler_PrepareFindExecuteCommand error");
	// Check command source
	UNITTEST_ASSERT(CommandHandler_CommandSource == CommProt_Buffer,
			"CommandHandler_PrepareFindExecuteCommand error");
	// Check separated / splitted command
	UNITTEST_ASSERT(!StrCmp(CommandHandler_CommandArguments[0], "WrongCommand"),
			"CommandHandler_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(CommandHandler_CommandArguments[1] == NULL,
			"CommandHandler_PrepareFindExecuteCommand error");


	// Check wrong command: too many arguments ( >3)
	result = CommandHandler_PrepareFindExecuteCommand(CommProt_Buffer, "version with lot of arguments");
	// Check command execute result: true/successful, because CommandHandler will find "version" command
	UNITTEST_ASSERT(result == true,
			"CommandHandler_PrepareFindExecuteCommand error");
	// Check command argument num
	UNITTEST_ASSERT(CommandHandler_CommandArgCount == 3,
			"CommandHandler_PrepareFindExecuteCommand error");
	// Check command source
	UNITTEST_ASSERT(CommandHandler_CommandSource == CommProt_Buffer,
			"CommandHandler_PrepareFindExecuteCommand error");
	// Check separated / splitted command
	UNITTEST_ASSERT(!StrCmp(CommandHandler_CommandArguments[0], "version"),
			"CommandHandler_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(!StrCmp(CommandHandler_CommandArguments[1], "with"),
			"CommandHandler_PrepareFindExecuteCommand error");
	UNITTEST_ASSERT(!StrCmp(CommandHandler_CommandArguments[2], "lot"),
			"CommandHandler_PrepareFindExecuteCommand error");


	// End of unittest
	UnitTest_End();

}

#endif
