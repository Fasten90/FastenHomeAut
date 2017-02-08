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


#include <CommandList.h>
#include "include.h"
#include "stdint.h"
#include "escapesequence.h"
#include "GlobalVarHandler.h"
#include "communication.h"

#include "CommandHandler.h"

#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
#include "homeautmessage.h"
#include "raspberrypi.h"
#endif



/*------------------------------------------------------------------------------
 *									Configs
 *----------------------------------------------------------------------------*/

static const char CommandHandler_DelimiterChar = ' ';



/*------------------------------------------------------------------------------
 *								Global variables
 *----------------------------------------------------------------------------*/


volatile CommProtocol_t CommandHandler_CommandSource = 0;

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

	// Check GlobalVarList[]
	if (GlobalVarHandler_CheckCommandStructAreValid() == false)
	{
		CommandHandler_SendLine("ERROR in GlobalVarList! Should correct that!");
	}

	// Initialize

	CommandHandler_CommandArgCount = 0;

	// End of initialization

	return;
}



/**
 * \brief	Send welcome message
 */
void CommandHandler_SendWelcome(void)
{

	DelayMs(1);

#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
	CommandHandler_SendCls();						// Clean screen
#endif
	
	COMMANDHANDLER_SEND_WELCOME();					// Welcome message

	COMMANDHANDLER_SEND_NEW_LINE();
	COMMANDHANDLER_SEND_PROMT_NEW_LINE();			// New promt

	return;
	
}



/**
 * \brief	Prepare (Separate) the command and Find and Run it...
 */
bool CommandHandler_PrepareFindExecuteCommand(CommProtocol_t source, char *command)
{
	bool isSuccessful = false;

	// Separate command
	StrCpyMax(CommandHandler_ProcessedCommandActual, command, COMMANDHANDLER_MAX_COMMAND_LENGTH);
	CommandHandler_CommandArgCount = CommandHandler_CommandParser();
	CommandHandler_CommandSource = source;

	if (CommandHandler_CommandArgCount > 0)
	{
		// Find and execute the command
		isSuccessful = CommandHandler_SearchCommand();
		COMMANDHANDLER_SEND_NEW_LINE();
	}
	else
	{
		// 0 Argument num, Cannot separated, this is not a command
		isSuccessful = false;
	}

	// Init new command
	COMMANDHANDLER_SEND_PROMT_NEW_LINE();

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

	commandArgCount = STRING_Splitter((char*)CommandHandler_ProcessedCommandActual, CommandHandler_DelimiterChar,
			CommandHandler_CommandArguments, COMMANDHANDLER_COMMAND_ARG_MAX_COUNT);


	if (commandArgCount > COMMANDHANDLER_COMMAND_ARG_MAX_COUNT)
	{
		// Too many arguments
		CommandHandler_SendMessage("Too many arguments!\r\n");
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
	for (i=0; i < CommandHandler_CommandNum; i++)
	{

		if (!StrCmp(CommandHandler_CommandArguments[0],CommandList[i].name))
		{
			// Found the command
			result = CommandHandler_RunCommand(i);

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
			pMessage ="Wrong argument (1.)";
			break;

		case CommandResult_Error_WrongArgument2:
			pMessage ="Wrong argument (2.)";
			break;

		case CommandResult_Error_TooFewArgument:
			pMessage ="Too few argument";
			break;

		case CommandResult_Error_TooManyArgument:
			pMessage ="Too many argument";
			break;

		case CommandResult_Error_WrongArgumentNum:
			pMessage ="Wrong argument num";
			break;

		case CommandResult_Error_CommandArgNumIsWrong:
			pMessage ="Command set is wrong";
			break;

		case CommandResult_Error_Unknown:
			pMessage ="Unknown error";
			break;

		case CommandResult_Error_CommandNotFound:
			pMessage = 	"Unknown Command\r\n"
						"Type \"help\" for help";
			break;

		default:
			pMessage ="Unknown command process";
			break;
	}

	CommandHandler_SendLine(pMessage);
}



/**
 * \brief	Run command
 */
static CommandResult_t CommandHandler_RunCommand(CommandID_t commandID)
{

	CommandResult_t result = CommandResult_Unknown;
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
		FunctionPointer thisFunction = (FunctionPointer)CommandList[commandID].commandFunctionPointer;
#pragma GCC diagnostic pop
		// End of warning suppress

		// Execute the command function
		result = thisFunction(CommandHandler_CommandArgCount,(char**)CommandHandler_CommandArguments);
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
	char txBuffer[TXBUFFERSIZE];

	va_list ap;									// argument pointer
	va_start(ap, format); 						// ap on arg
	string_printf(txBuffer,format,ap);			// Separate and process
	va_end(ap);						 			// Cleaning after end

	CommandHandler_SendMessage(txBuffer);

	return;

}



/**
 * \brief	Send CLS (Clear Screen)
 */
void CommandHandler_SendCls(void)
{
	CommandHandler_SendMessage(ESCAPE_ERASE_CLS);
	CommandHandler_SendMessage(ESCAPE_CURSOR_TOPLEFT);
}


