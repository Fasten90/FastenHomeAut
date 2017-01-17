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


#include "commandList.h"
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



/*******************************************************************************
 *									Configs:
 ******************************************************************************/

/// Enable command handler
const bool CommandHandler_CommandReceiveEnable = true;

/// Enable sending back: "Echo mode"
static const bool CommandHandler_CommandSendBackCharEnable = true;

#ifdef CONFIG_COMMANDHANDLER_GET_PASSWORD_ENABLE
static const char CommandHandler_Password[] = "password";
#endif

static const char CommandHandler_DelimiterChar = ' ';



/*******************************************************************************
 *						 			Global variables
 ******************************************************************************/

/// Receive buffers
volatile char CommandHandler_CommandActual[COMMANDHANDLER_MAX_COMMAND_LENGTH] = { 0 };
volatile char CommandHandler_CommandActualEscape[4] = { 0 };
volatile CommProtocol_t CommandHandler_CommandSource = 0;

/// USART Read cnt
volatile uint8_t USART_RxBufferReadCnt = 0;


/// Variables For CommandHandler
static volatile bool CommandHandler_CommandReceivedEvent = false;
static volatile bool CommandHandler_CommandReceivedLastChar = false;
static volatile bool CommandHandler_CommandReceivedNotLastChar = false;
static volatile bool CommandHandler_CommandReadable = false;
static volatile bool CommandHandler_CommandReceivedBackspace = false;

#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
static volatile bool CommandHandler_CommandReceivedDelete = false;
static volatile bool CommandHandler_CommandReceivedTabulator = false;
#endif

static volatile uint8_t CommandHandler_CommandActualLength = 0;
static volatile uint8_t CommandHandler_CommandSentLength = 0;
static volatile uint8_t CommandHandler_CommandCursorPosition = 0;

static volatile bool CommandHandler_CommandEscapeSequenceReceived = false;
static volatile uint8_t CommandHandler_CommandEscapeSequenceInProgress = 0;
static volatile uint8_t CommandHandler_CommandEscape_cnt = 0;

static uint8_t CommandHandler_CommandArgCount = 0;
static char *CommandHandler_CommandArguments[COMMANDHANDLER_COMMAND_ARG_MAX_COUNT] = { 0 } ;


#ifdef CONFIG_USE_FREERTOS
xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
#endif


#ifdef CONFIG_COMMANDHANDLER_USE_HISTORY
static uint8_t CommandHandler_HistorySaveCnt = 0;
static uint8_t CommandHandler_HistoryLoadCnt = 0;
static char CommandHandler_HistoryList[COMMANDHANDLER_HISTORY_MAX_COUNT][COMMANDHANDLER_MAX_COMMAND_LENGTH] =
{

#if defined(CONFIG_COMMANDHANDLER_USE_HISTORY) && (COMMANDHANDLER_HISTORY_MAX_COUNT > 0)

	#if ( COMMANDHANDLER_HISTORY_MAX_COUNT > 0 )
	{
		"help"
	},
	#endif
	#if ( COMMANDHANDLER_HISTORY_MAX_COUNT > 1 )
	{
		"led 1 1"
	},
	#endif
	#if ( COMMANDHANDLER_HISTORY_MAX_COUNT > 2 )
	{
		"cls"
	},
	#endif
	#if ( COMMANDHANDLER_HISTORY_MAX_COUNT > 3 )
	{
		"test"
	},
	#endif
	#if ( COMMANDHANDLER_HISTORY_MAX_COUNT > 4 )
	{
		"reset"
	}
	#endif
	#if ( COMMANDHANDLER_HISTORY_MAX_COUNT > 5 )
	#warning "Isn't set CommandHandler history commands"
	#endif
#elif  (!COMMANDHANDLER_HISTORY_MAX_COUNT)
#error "CONFIG_COMMANDHANDLER_USE_HISTORY define is defined, but 'COMMANDHANDLER_HISTORY_MAX_COUNT'"
	"define is not set valid value."
#endif
};
#endif



/*******************************************************************************
 *								Function prototypes
 ******************************************************************************/

static void CommandHandler_ProcessReceivedCharacter(void);

#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
static void CommandHandler_CommandDelete(void);
static void CommandHandler_CommandTabulator(void);
static void CommandHandler_CommandResendLine(bool needRestoreCursor);
static bool CommandHandler_CommandEscapeCharValidation(void);
#endif

static void CommandHandler_CommandBackspace(void);

static bool CommandHandler_PrepareFindExecuteCommand(CommProtocol_t source);
static uint8_t CommandHandler_CommandParser(void);
static bool CommandHandler_SearchCommand(void);



#ifdef CONFIG_COMMANDHANDLER_USE_HISTORY
// CommandHandler history
static void CommandHandler_HistorySave(void);
static bool CommandHandler_HistoryFindInList(void);
static void CommandHandler_HistoryLoad(uint8_t direction);
#endif

static void CommandHandler_ConvertSmallLetter(void);

static void CommandHandler_CheckResultAndRespond(CommandResult_t result);
static CommandResult_t CommandHandler_RunCommand(CommandID_t commandID);
static CommandResult_t CommandHandler_CheckArgumentNumIsGood(uint8_t receivedArgNum,
		uint8_t commandArgNum);

#ifdef CONFIG_COMMANDHANDLER_GET_PASSWORD_ENABLE
static void CommandHandler_GetPassword(void);
static bool CommandHandler_CheckPassword(const char *string);
#endif



/*******************************************************************************
 *									Functions
 ******************************************************************************/


/**
 * \brief	Initialize CommandHandler program
 */
void CommandHandler_Init(void)
{

	// Initialize

	CommandHandler_CommandReceivedEvent = false;
	CommandHandler_CommandReceivedNotLastChar = false;

	CommandHandler_CommandActualLength = 0;
	CommandHandler_CommandSentLength = 0;
	CommandHandler_CommandCursorPosition = 0;

#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
	CommandHandler_CommandEscapeSequenceReceived = false;
	CommandHandler_CommandEscapeSequenceInProgress = false;
	CommandHandler_CommandEscape_cnt = 0;
#endif

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
	COMMANDHANDLER_SEND_PROMT_NEW_LINE();					// New promt

	return;
	
}



/**
 * \brief	Always run, wait command and execute it
 */
void CommandHandler_CheckCommand(void)
{

	// Initialize
	//CommandHandler_Init();	// Init


#if RXBUFFERSIZE != 256
#warning "Ring buffer counter error"
#endif


#ifdef CONFIG_USE_FREERTOS
	vTaskDelay(10);

	xSemaphoreGive(DEBUG_USART_Tx_Semaphore);
	// Enable sendings
#endif

	// Check GlobalVarList[]
	if (GlobalVarHandler_CheckCommandStructAreValid() == false)
	{
		CommandHandler_SendLine("ERROR in GlobalVarList! Should correct that!");
	}

	// Start receive
	USART_StartReceiveMessage();

#ifdef CONFIG_COMMANDHANDLER_GET_PASSWORD_ENABLE
	CommandHandler_GetPassword();
#endif

	// Welcome message
	CommandHandler_SendWelcome();


	// Infinite "task" loop
	while (1)
	{

		// Always checking the Command
		if (CommandHandler_CommandReceiveEnable)
		{

#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
			// Test for Raspberry Pi
			if (BUTTON_Clicked)
			{
				BUTTON_Clicked = 0;
				RASPBERRYPI_SendMessage(1, Function_Alarm, Alarm_PressedButton , 17);
			}
#endif


			#ifdef CONFIG_USE_FREERTOS
			// Wait semaphore
			if (xSemaphoreTake(DEBUG_USART_Rx_Semaphore,1000) == pdTRUE)
			{
				CommandHandler_ProcessReceivedCharacter();
			}
			#else
			// If not used FreeRTOS, always check characters
			CommandHandler_ProcessReceivedCharacter();
			#endif

			if (CommandHandler_CommandReceivedEvent)
			{
				// Clear event
				CommandHandler_CommandReceivedEvent = false;

				// Only one event will receive
				if (CommandHandler_CommandReceivedBackspace)
				{
					// Backspace
					CommandHandler_CommandReceivedBackspace = false;
					CommandHandler_CommandBackspace();
				}
#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
				else if (CommandHandler_CommandReceivedDelete)
				{
					// Delete
					CommandHandler_CommandReceivedDelete = false;
					CommandHandler_CommandDelete();
				}
				else if (CommandHandler_CommandReceivedNotLastChar)
				{
					// Received inner character

					CommandHandler_CommandReceivedNotLastChar = false;

					// Step right
					CommandHandler_SendMessage(ESCAPE_CURSORRIGHT);
					// Not Last char (it is inner character) - Refresh the line
					CommandHandler_CommandResendLine(true);
				}
				else if (CommandHandler_CommandEscapeSequenceReceived)
				{
					// Escape sequence
					CommandHandler_CommandEscapeSequenceReceived = false;
					CommandHandler_CommandEscapeCharValidation();
				}
				else if (CommandHandler_CommandReceivedTabulator)
				{
					// Received tabulator
					CommandHandler_CommandReceivedTabulator = false;
					CommandHandler_CommandTabulator();
				}
#endif
				else if (CommandHandler_CommandReadable)
				{
					// Pressed Enter, EndCommand();
					CommandHandler_CommandReadable = false;
					if (CommandHandler_CommandActualLength > 0)
					{
						// There are some char in the line
						// has an command
						CommandHandler_ConvertSmallLetter();
						
						COMMANDHANDLER_SEND_NEW_LINE();
						
						#ifdef CONFIG_COMMANDHANDLER_USE_HISTORY
						// Save command to History
						CommandHandler_HistorySave();
						#endif

						// Search command and run
						CommandHandler_PrepareFindExecuteCommand(CommProt_DebugUart);
					}
					else
					{
						// There is no char in the line
						COMMANDHANDLER_SEND_PROMT_NEW_LINE();
					}
					CommandHandler_CommandActualLength = 0;
					CommandHandler_CommandSentLength = 0;
					CommandHandler_CommandCursorPosition = 0;
				}
			}
		}
	}
	
	// Infinite loop, never exit, never reached here.

}



/**
 * \brief	Check received characters and make command (COMMAND_Actual)
 */
static void CommandHandler_ProcessReceivedCharacter(void)
{

	// While Read cnt not equal than Write cnt
	while (USART_RxBufferReadCnt != USART_RxBufferWriteCounter)
	{
		volatile char USART_ReceivedChar = '\0';

		USART_ReceivedChar = USART_RxBuffer[USART_RxBufferReadCnt];
		USART_RxBufferReadCnt++;

#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
		// ESCAPE SEQUENCE
		if ( CommandHandler_CommandEscapeSequenceInProgress )
		{
			// Escape sequence in progress
			// Copy escape characters to CommandHandler_CommandActualEscape[]
			// TODO: Megcsinálni elegánsabban
			if (CommandHandler_CommandEscape_cnt == 1)
			{
				if (USART_ReceivedChar == '[')
				{
					CommandHandler_CommandActualEscape[CommandHandler_CommandEscape_cnt++] = USART_ReceivedChar;
				}
				else
				{
					// Wrong escape sequence
					CommandHandler_CommandEscapeSequenceInProgress = false;
					CommandHandler_CommandEscape_cnt = 0;
				}
			}
			else if ( CommandHandler_CommandEscape_cnt == 2)
			{
				CommandHandler_CommandActualEscape[CommandHandler_CommandEscape_cnt++] = USART_ReceivedChar;

				// TODO: only work with escape sequence if 3 chars (ESC[A)
				if (CommandHandler_CommandActualEscape[2] != '3')
				{
					// \e[A / B / C / D
					CommandHandler_CommandEscapeSequenceInProgress = false;
					CommandHandler_CommandEscapeSequenceReceived = true;
					CommandHandler_CommandReceivedEvent = true;
					return;
				}
				else
				{
					// \e[3~ --> delete at zoc
					//MONITOR_CommandEscape_cnt++;
				}
			}
			else if ( CommandHandler_CommandEscape_cnt == 3)
			{
				CommandHandler_CommandActualEscape[CommandHandler_CommandEscape_cnt++] = USART_ReceivedChar;

				if (CommandHandler_CommandActualEscape[3] == '~')
				{
					// TODO: At ZOC, it is delete char
					// Delete button
					CommandHandler_CommandEscapeSequenceInProgress = false;
					CommandHandler_CommandReceivedDelete = true;
					CommandHandler_CommandReceivedEvent = true;
					return;
				}
			}
		}
		else
		{
#endif	// #ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
			// No escape sequence
			// An character received

			if ( USART_ReceivedChar  == '\x1B')	// 'ESC'
			{
				// receive an Escape sequence
				CommandHandler_CommandEscapeSequenceInProgress = true;
				CommandHandler_CommandActualEscape[0] = USART_ReceivedChar;
				CommandHandler_CommandEscape_cnt = 1;
			}
			else
			{
				if ( (USART_ReceivedChar  == '\r') || (USART_ReceivedChar == '\n') ||
					(USART_ReceivedChar == '\0'))
				{
					// Received Enter
					CommandHandler_CommandReadable = true;
					CommandHandler_CommandActual[CommandHandler_CommandActualLength] = '\0';
					CommandHandler_CommandReceivedEvent = true;
					return;
				}
				else if (USART_ReceivedChar == USART_KEY_BACKSPACE)
				{
					// Received backspace
					CommandHandler_CommandReceivedBackspace = true;
					CommandHandler_CommandReceivedEvent = true;
					return;
				}
#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
				else if (USART_ReceivedChar == USART_KEY_DELETE)
				{
					// Delete button
					// TODO: Not work at ZOC, but work at other terminal?
					CommandHandler_CommandReceivedDelete = true;
					CommandHandler_CommandReceivedEvent = true;
					return;
				}
				else if (USART_ReceivedChar == '\t')
				{
					// TAB
					CommandHandler_CommandReceivedTabulator = true;
					CommandHandler_CommandReceivedEvent = true;
					return;
				}
#endif	// #ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
				else
				{
					// Simple char for the command
					// shorted than max length?
					if (CommandHandler_CommandActualLength < COMMANDHANDLER_MAX_COMMAND_LENGTH)
					{
						if (CommandHandler_CommandCursorPosition == CommandHandler_CommandActualLength)
						{
							// CursorPosition = CommandLength		(end character)
							CommandHandler_CommandActual[CommandHandler_CommandActualLength] = USART_ReceivedChar;
							CommandHandler_CommandActualLength++;
							CommandHandler_CommandCursorPosition++;
							if (CommandHandler_CommandSendBackCharEnable)
							{
								// Send received character
								CommandHandler_SendChar( USART_ReceivedChar );
							}
						}
						else
						{
							// CursorPosition < CommandLength		(inner character)
							CommandHandler_CommandActualLength++;
							// Copy
							uint8_t i;
							for (i = CommandHandler_CommandActualLength; i > CommandHandler_CommandCursorPosition; i--)
							{
								CommandHandler_CommandActual[i] = CommandHandler_CommandActual[i-1];
							}
							CommandHandler_CommandActual [CommandHandler_CommandCursorPosition] = USART_ReceivedChar;
							CommandHandler_CommandActual [CommandHandler_CommandActualLength] = '\0';
							CommandHandler_CommandCursorPosition++;
							CommandHandler_CommandReceivedNotLastChar = true;
							CommandHandler_CommandReceivedEvent = true;
						}
					}
					else
					{
						// longer than max length ...
						// Do not copy/print...
					}

				}
#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
			}
#endif

		} // Processed received characters

	} // End of while
}



/**
 * \brief	Prepare (Separate) the command and Find and Run it...
 * 			TODO: Átalakítani char* átvevősre
 */
static bool CommandHandler_PrepareFindExecuteCommand(CommProtocol_t source)
{
	bool isSuccessful = false;

	// Separate command
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

	commandArgCount = STRING_Splitter((char*)CommandHandler_CommandActual, CommandHandler_DelimiterChar,
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
 * \brief	Command's letter deleting (backspace)
 */
void CommandHandler_CommandBackspace(void)
{

	if (CommandHandler_CommandActualLength > 0)
	{
		// If has command
		// Cursor at end?
		if ( CommandHandler_CommandCursorPosition == CommandHandler_CommandActualLength)
		{
			// Cursor at end
			// Delete from CommandActual, and Position--
			CommandHandler_CommandActual[--CommandHandler_CommandCursorPosition] = '\0';
			CommandHandler_CommandActualLength--;
			
			// Delete last character on terminal
#ifdef CONFIG_COMMANDHANDLER_USE_TERMINAL_ZOC
			USART_SEND_KEY_BACKSPACE();
			USART_SEND_KEY_DEL();
			USART_SEND_KEY_BACKSPACE();
#endif		
			
#ifdef CONFIG_COMMANDHANDLER_USE_TERMINAL_PUTTY
			
			// v1
			USART_SEND_KEY_DEL();
#endif
			
#ifdef CONFIG_COMMANDHANDLER_USE_TERMINAL_HYPERTERMINAL
			
			USART_SEND_KEY_BACKSPACE();

			// Delete & Resend
			CommandHandler_CommandResendLine(true);

#endif
		}
#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
		else
		{
			// CursorPosition != CommandLength, we are in command chars
			// Cursor not at end
			// Event: Used backspace at inner command
			// NOTE: Be careful, cursor need to put at original position

			uint8_t i;

			if ( CommandHandler_CommandCursorPosition > 0 )
			{
				// not at 0 position

				// Procedure:
				// copy CommandActual
				// delete & resend

				CommandHandler_CommandActualLength--;
				CommandHandler_CommandCursorPosition--;

				for ( i = CommandHandler_CommandCursorPosition; i < CommandHandler_CommandActualLength; i++ )
				{
					CommandHandler_CommandActual[i] = CommandHandler_CommandActual[i+1];		// copy
				}
				CommandHandler_CommandActual[i] = '\0';

				// Send backspace = step left
				USART_SEND_KEY_BACKSPACE();

				// Delete & Resend
				CommandHandler_CommandResendLine(true);

			}
			else
			{
				// At 0 position
				// Do nothing, cannot backspace
			}
		}
#endif	// #ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
	}
	return;			// not do anything
}



#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
/**
 * \brief	Delete button received
 */
static void CommandHandler_CommandDelete(void)
{

	if (CommandHandler_CommandActualLength > 0)
	{
		// If has command
		// Cursor at end?
		if (CommandHandler_CommandCursorPosition == CommandHandler_CommandActualLength)
		{
			// Do nothing at end
		}
		else
		{
			// CursorPosition != CommandLength, we are in command chars
			// Cursor not at end

			uint8_t i;

			if (CommandHandler_CommandCursorPosition > 0)
			{
				// not at 0 position
				// Procedure:
				// - Copy CommandActual after cursor
				// - Resend command with original cursor position

				CommandHandler_CommandActualLength--;

				// Drop the backspaced character
				for (i = CommandHandler_CommandCursorPosition; i < CommandHandler_CommandActualLength; i++)
				{
					CommandHandler_CommandActual[i] = CommandHandler_CommandActual[i+1];		// copy
				}
				CommandHandler_CommandActual[i] = '\0';

				// Resend line with original cursor position
				CommandHandler_CommandResendLine(true);
			}
			else
			{
				// At 0 position:
				// We cannot use backspace, do nothin!
			}
		}
	}

	return;

}
#endif	// #ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE



#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
/**
 * \brief	Received tabulator command: Complete command
 */
static void CommandHandler_CommandTabulator(void)
{
	// Find same command
	uint8_t i;

	for (i=0; i < CommandHandler_CommandNum; i++)
	{
		if (!StrCmpWithLength(CommandList[i].name,
				(const char *)CommandHandler_CommandActual,
				CommandHandler_CommandActualLength))
		{
			// It is equal
			// We write the first equal
			// TODO: Lekezelni az esetleges több Tabulátort?
			// Biztos jó, hogy az első egyezőt kiírjuk?
			StrCpy((char *)CommandHandler_CommandActual, CommandList[i].name);

			CommandHandler_CommandActualLength = StringLength(CommandList[i].name);

			CommandHandler_CommandCursorPosition = CommandHandler_CommandActualLength;

			CommandHandler_CommandResendLine(false);

			return;
		}
	}
}
#endif // #ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE



#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
/**
 * \brief		Resend the actual line/command
 * 				NOTE: It save and restore the original cursor position
 */
static void CommandHandler_CommandResendLine( bool needRestoreCursor)
{
	// Procedure:
	// - Delete line
	// - (opc) Save cursor
	// - Send new command
	// - (opc) Restore cursor

	// Delete line
	CommandHandler_SendMessage(ESCAPE_DELETELINE);

	if (needRestoreCursor)
	{
		// Save cursor
		CommandHandler_SendMessage(ESCAPE_SAVECURSOR);
	}

	// Cursor to line start
	CommandHandler_SendMessage(ESCAPE_CURSOR_TO_LINESTART);
	CommandHandler_SendMessage(ESCAPE_CURSORLEFTLOTOF);

	// Write new CommandActual
	COMMANDHANDLER_SEND_PROMT();
	CommandHandler_SendMessage((const char *)CommandHandler_CommandActual);

	if (needRestoreCursor)
	{
		// Restore the position
		CommandHandler_SendMessage(ESCAPE_RESTORECURSOR);
	}

	return;
}
#endif	// #ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE



#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
/**
 * \brief	Process Escape sequence
 */
static bool CommandHandler_CommandEscapeCharValidation(void)
{
	// return valid char, or 0 if invalid
	// work with ANSI escape codes

	if (CommandHandler_CommandActualEscape[0] == '\x1B')				// ESC
	{
		if ( CommandHandler_CommandActualEscape[1] == '[' )			// '[', escape sequence 2. letter
		{
			// This is an escape sequence
			// 'A' Up cursor = previous History command
			if ( CommandHandler_CommandActualEscape[2] == 'A' )
			{
				#ifdef CONFIG_COMMANDHANDLER_USE_HISTORY
				CommandHandler_HistoryLoad ( 1 );
				#endif
				return true;
			}
			// 'B' Down cursor		// next History command
			else if ( CommandHandler_CommandActualEscape[2] == 'B' )
			{
				#ifdef CONFIG_COMMANDHANDLER_USE_HISTORY
				CommandHandler_HistoryLoad ( 0 );
				#endif
				return true;
			}
			// 'C' - Right cursor - Step right
			else if (  CommandHandler_CommandActualEscape[2] == 'C' )
			{
				// Is cursor at end?
				if ( CommandHandler_CommandCursorPosition < CommandHandler_CommandActualLength )
				{
					// Cursor within command
					CommandHandler_SendMessage(ESCAPE_CURSORRIGHT);
					CommandHandler_CommandCursorPosition++;
					return true;
				}
				else
				{
					// Cursor at end, do nothing
					return true;
				}
			}
			// 'D' Left cursor - Step left
			else if (  CommandHandler_CommandActualEscape[2] == 'D' )
			{
				if ( CommandHandler_CommandCursorPosition > 0 )				// if not at start
				{
					CommandHandler_SendMessage(ESCAPE_CURSORLEFT);
					CommandHandler_CommandCursorPosition--;
					return true;
				}
				else
				{
					// not do anything
					return true;
				}
			}
		}
		else	// This is not escape sequence
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return false;

}
#endif	// #ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE



#ifdef CONFIG_COMMANDHANDLER_USE_HISTORY
/**
 * \brief	Save actual command to history
 */
static void CommandHandler_HistorySave(void)
{

	// Has equal command?
	if ( CommandHandler_HistoryFindInList() == true )
	{
		return;
	}

	// Actual save counter
	if ( CommandHandler_HistorySaveCnt >= ( COMMANDHANDLER_HISTORY_MAX_COUNT-1 ) )
	{
		CommandHandler_HistorySaveCnt = 0;
	}
	else
	{
		CommandHandler_HistorySaveCnt++;
	}

	// Actual saved is the "last"
	CommandHandler_HistoryLoadCnt = CommandHandler_HistorySaveCnt;

	// Save command
	StrCpyMax( CommandHandler_HistoryList[CommandHandler_HistorySaveCnt],
			(char *)CommandHandler_CommandActual,
			COMMANDHANDLER_MAX_COMMAND_LENGTH );

	return;

}
#endif



#ifdef CONFIG_COMMANDHANDLER_USE_HISTORY
/**
 * \brief	Check, this command is in History?
 * \return	true, if has equal
 * 			false, if not has equal
 */
static bool CommandHandler_HistoryFindInList(void)
{
	uint8_t i;
	
	for (i = 0; i < COMMANDHANDLER_HISTORY_MAX_COUNT; i++)
	{
		// Check, equal with command?
		if ( !StrCmp((const char *)CommandHandler_HistoryList[i],(const char * )CommandHandler_CommandActual))
		{
			// If it is equal
			// Has equal command
			return true;
		}
	}

	// There is no equal command
	return false;

}
#endif



#ifdef CONFIG_COMMANDHANDLER_USE_HISTORY
/**
 * \brief	Load history from list to actual command
 */
static void CommandHandler_HistoryLoad(uint8_t direction)
{

	// down cursor
	if (direction == 0) // direction == 0
	{
		if (CommandHandler_HistoryLoadCnt >= (COMMANDHANDLER_HISTORY_MAX_COUNT-1))
		{
			CommandHandler_HistoryLoadCnt = 0;
		}
		else
		{
			CommandHandler_HistoryLoadCnt++;
		}
	}

	// up cursor
	// if direction == 1, copy actual

	// Copy command and set cursor
	StrCpy((char *)CommandHandler_CommandActual, (const char *)CommandHandler_HistoryList[CommandHandler_HistoryLoadCnt]);

	// cursor, length!
	CommandHandler_CommandCursorPosition = StringLength((const char *)CommandHandler_CommandActual);
	CommandHandler_CommandActualLength = CommandHandler_CommandCursorPosition;

	CommandHandler_CommandResendLine(false);

	// Step load cnt
	if (direction == 1) // direction == 0
	{
		if (CommandHandler_HistoryLoadCnt <= 0)
		{
			CommandHandler_HistoryLoadCnt = COMMANDHANDLER_HISTORY_MAX_COUNT-1;
		}
		else
		{
			CommandHandler_HistoryLoadCnt--;
		}
	}

	return;
}
#endif



/**
 * \brief	Convert CommandHandler_CommandActual (Actual command) to small letters
 */
static void CommandHandler_ConvertSmallLetter(void)
{
	uint8_t i;
	
	for (i = 0; CommandHandler_CommandActual[i] != '\0'; i++)
	{
		if ( ( CommandHandler_CommandActual[i] > 'A' ) && ( CommandHandler_CommandActual[i] < 'Z' ) )
		{
			// Need to change to small letter
			// length between Big Letter and small letter
			CommandHandler_CommandActual[i] = CommandHandler_CommandActual[i] - ( 'A' - 'a');
		}
	}

	return;
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
		CommandHandler_WriteCommandHelp (commandID);
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

	for (i=0; i < CommandHandler_CommandNum; i++)
	{
		// Find the command
		if (!StrCmp(CommandList[i].name, command))
		{
			// Command's describe
			CommandHandler_WriteCommandHelp(i);
			isOk = true;
		}
	}

	if (!isOk)
	{
		CommandHandler_Printf("Not find command: %s\r\n", command);
	}
}



/**
 * \brief	Write a command help
 */
void CommandHandler_WriteCommandHelp(CommandID_t commandID)
{

	// Print command to source
	CommandHandler_Printf(
			"Command name: %s\r\n"
			"Function: %s\r\n"
			"Syntax: %s %s\r\n",
			CommandList[commandID].name,
			CommandList[commandID].description,
			CommandList[commandID].name,
			CommandList[commandID].syntax);

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
		for (i=0; i<COMMANDHANDLER_COMMAND_ARG_MAX_COUNT; i++)
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



#ifdef CONFIG_COMMANDHANDLER_GET_PASSWORD_ENABLE
/**
 * \brief Get (and wait) Password
 */
static void CommandHandler_GetPassword(void)
{

	bool passwordIsOk = false;

	// Wait first character
	CommandHandler_SendLine("\r\nType a character:");
	while (USART_RxBufferWriteCounter < 1);
	USART_RxBufferReadCnt = 1;

	while (!passwordIsOk)
	{
		CommandHandler_SendMessage("\r\nPassword:");

		bool isTry = true;
		CommandHandler_CommandActualLength = 0;

		while (isTry)
		{

			// While Read cnt not equal than Write cnt
			if (USART_RxBufferReadCnt != USART_RxBufferWriteCounter)
			{
				volatile char USART_ReceivedChar = '\0';

				USART_ReceivedChar = USART_RxBuffer[USART_RxBufferReadCnt];
				USART_RxBufferReadCnt++;
				CommandHandler_SendChar('*');

				if (USART_ReceivedChar == '\r' || USART_ReceivedChar == '\n')
				{
					// Pressed enter, check password
					isTry = false;
					CommandHandler_CommandActual[CommandHandler_CommandActualLength++] = '\0';
					USART_SendNewLine();
					if (CommandHandler_CheckPassword((const char*)CommandHandler_CommandActual))
					{
						// Successful password
						CommandHandler_SendLine("Successful password!");
						CommandHandler_CommandActualLength=0;
						return;
					}
					else
					{
						// Failed password
						CommandHandler_SendLine("Wrong password!");
					}
				}
				else
				{
					// Copy character
					CommandHandler_CommandActual[CommandHandler_CommandActualLength++] = USART_ReceivedChar;
				}
			}
		}
	}
}



/**
 * \brief Check password
 */
static bool CommandHandler_CheckPassword(const char *string)
{
	if (!StrCmp(string,CommandHandler_Password))
	{
		// Equal
		return true;
	}
	else
	{
		return false;
	}
}

#endif	// #ifdef CONFIG_COMMANDHANDLER_GET_PASSWORD_ENABLE
