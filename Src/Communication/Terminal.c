/*
 *		Terminal.c
 *		Created on:		2017-02-02
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Terminal
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-02
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_MODULE_TERMINAL_ENABLE

#include "String.h"
#include "Globals.h"
#include "CommandList.h"
#include "CommandHandler.h"
#include "CircularBuffer.h"
#include "DebugUart.h"
#include "Reset.h"
#include "Terminal.h"
#include "Communication.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

///< Receive buffer
volatile char Terminal_CommandActual[TERMINAL_MAX_COMMAND_LENGTH] = { 0 };

#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
volatile char Terminal_CommandActualEscape[4] = { 0 };
#endif



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

///< Enable sending back: "Echo mode"
static const bool Terminal_CommandSendBackCharEnable = true;

///< Variables For CommandHandler
static volatile bool Terminal_CommandReceivedEvent = false;
static volatile bool Terminal_CommandReceivedNotLastChar = false;
static volatile bool Terminal_CommandReadable = false;
static volatile bool Terminal_CommandReceivedBackspace = false;

#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
static volatile bool Terminal_CommandReceivedDelete = false;
static volatile bool Terminal_CommandReceivedTabulator = false;
#endif

static volatile uint8_t Terminal_CommandActualLength = 0;
static volatile uint8_t Terminal_CommandSentLength = 0;
static volatile uint8_t Terminal_CommandCursorPosition = 0;

static volatile bool Terminal_CommandEscapeSequenceReceived = false;
static volatile bool Terminal_CommandEscapeSequenceInProgress = false;
static volatile uint8_t Terminal_CommandEscape_cnt = 0;



#ifdef CONFIG_TERMINAL_HISTORY_ENABLE
static uint8_t Terminal_HistorySaveCnt = 0;
static uint8_t Terminal_HistoryLoadCnt = 0;
static char Terminal_HistoryList[TERMINAL_HISTORY_MAX_COUNT][TERMINAL_MAX_COMMAND_LENGTH] = { 0 };

#if (TERMINAL_HISTORY_MAX_COUNT > 0)
static const char * const Terminal_HistoryInitList[] =
{
	#if ( TERMINAL_HISTORY_MAX_COUNT > 0 )
	"help",
	#endif
	#if ( TERMINAL_HISTORY_MAX_COUNT > 1 )
	"led 1 1",
	#endif
	#if ( TERMINAL_HISTORY_MAX_COUNT > 2 )
	"cls",
	#endif
	#if ( TERMINAL_HISTORY_MAX_COUNT > 3 )
	"test",
	#endif
	#if ( TERMINAL_HISTORY_MAX_COUNT > 4 )
	"reset"
	#endif
	#if ( TERMINAL_HISTORY_MAX_COUNT > 5 )
		#warning "Isn't set CommandHandler history commands"
	#endif
};
#else
	#error "CONFIG_COMMANDHANDLER_USE_HISTORY define is defined, but 'COMMANDHANDLER_HISTORY_MAX_COUNT' define is not set valid value."
#endif

#endif


#ifdef CONFIG_TERMINAL_GET_PASSWORD_ENABLE
static const char Terminal_Password[] = "password";
#endif



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void Terminal_ProcessReceivedCharacter(void);

#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
static void Terminal_CommandDelete(void);
static void Terminal_CommandTabulator(void);
static void Terminal_CommandResendLine(bool needRestoreCursor);
static bool Terminal_CommandEscapeCharValidation(void);
#endif

static void Terminal_CommandBackspace(void);


#ifdef CONFIG_TERMINAL_HISTORY_ENABLE
// CommandHandler history
static void Terminal_InitHistory(void);
static void Terminal_HistorySave(void);
static bool Terminal_HistoryFindInList(void);
static void Terminal_HistoryLoad(uint8_t direction);
#endif

static void Terminal_ConvertSmallLetter(void);

#ifdef CONFIG_TERMINAL_GET_PASSWORD_ENABLE
static void Terminal_GetPassword(void);
static bool Terminal_CheckPassword(const char *string);
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Initialize CommandHandler program
 */
void Terminal_Init(void)
{
	Terminal_CommandReceivedEvent = false;
	Terminal_CommandReceivedNotLastChar = false;

	Terminal_CommandActualLength = 0;
	Terminal_CommandSentLength = 0;
	Terminal_CommandCursorPosition = 0;

#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
	Terminal_CommandEscapeSequenceReceived = false;
	Terminal_CommandEscapeSequenceInProgress = false;
	Terminal_CommandEscape_cnt = 0;
#endif

#ifdef CONFIG_TERMINAL_HISTORY_ENABLE
	// Initialize History (fill with default commands)
	Terminal_InitHistory();
#endif


#ifdef CONFIG_USE_FREERTOS
	DelayMs(10);

	// Enable sendings
	xSemaphoreGive(DebugUart_Tx_Semaphore);

	// Terminal Task (FreeRTOS)
	TaskHandle_t Terminal_TaskHandle = NULL;
	//xTaskCreate(vTaskCode, "NAME", STACK_SIZE, &ucParameterToPass, tskIDLE_PRIORITY, &xHandle);
	if (xTaskCreate( (pdTASK_CODE)Terminal_CheckCommand, "TerminalTask", TERMINAL_TASK_STACK_SIZE, 0,
			TERMINAL_TASK_PRIORITY, &Terminal_TaskHandle) != pdPASS)
	{
		Error_Handler();
	}
#endif


	// Start receive
	DebugUart_StartReceive();

	// End of initialization


#ifdef CONFIG_TERMINAL_GET_PASSWORD_ENABLE
	Terminal_SendGetPassword();
#else
	// Welcome message
	Terminal_SendWelcome();
#endif
}



/**
 * \brief	Always run, wait command and execute it
 */
void Terminal_CheckCommand(void)
{
	// \note	If use EventHandler, this loop is not infinite loop,
	//			it will be return after finished
	while (1)
	{
#ifdef CONFIG_TERMINAL_GET_PASSWORD_ENABLE
		if (!Terminal_PasswordIsOk)
		{
			Terminal_GetPassword();
			return;
		}
#endif

		// Always checking the Command
		if (DebugUart_CommandReceiveEnable)
		{
#ifdef CONFIG_USE_FREERTOS
			// Wait for semaphore
			if (xSemaphoreTake(DebugUart_Rx_Semaphore, 1000) == pdTRUE)
			{
				Terminal_ProcessReceivedCharacter();
			}
#else
			// If not used FreeRTOS / EventHandler, always check characters
			Terminal_ProcessReceivedCharacter();
			#endif

			if (Terminal_CommandReceivedEvent)
			{
				// Clear event
				Terminal_CommandReceivedEvent = false;

				// Only one event will receive
				if (Terminal_CommandReceivedBackspace)
				{
					// Backspace
					Terminal_CommandReceivedBackspace = false;
					Terminal_CommandBackspace();
				}
#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
				else if (Terminal_CommandReceivedDelete)
				{
					// Delete
					Terminal_CommandReceivedDelete = false;
					Terminal_CommandDelete();
				}
				else if (Terminal_CommandReceivedNotLastChar)
				{
					// Received inner character

					Terminal_CommandReceivedNotLastChar = false;

					Terminal_SendMessage(ESCAPE_CURSORRIGHT);		// Step right
					Terminal_CommandResendLine(true);					// Not Last char (it is inner character) - Refresh the line
				}
				else if (Terminal_CommandEscapeSequenceReceived)
				{
					// Escape sequence
					Terminal_CommandEscapeSequenceReceived = false;
					Terminal_CommandEscapeCharValidation();
				}
				else if (Terminal_CommandReceivedTabulator)
				{
					// Received tabulator
					Terminal_CommandReceivedTabulator = false;
					Terminal_CommandTabulator();
				}
#endif
				else if (Terminal_CommandReadable)
				{
					// Pressed Enter, EndCommand();
					Terminal_CommandReadable = false;
					if (Terminal_CommandActualLength > 0)
					{
						// There are some char in the line
						// has an command
						Terminal_ConvertSmallLetter();

						TERMINAL_SEND_NEW_LINE();

						#ifdef CONFIG_TERMINAL_HISTORY_ENABLE
						// Save command to History
						Terminal_HistorySave();
						#endif

						// Search command and run
						CommandHandler_PrepareFindExecuteCommand(
								CommProt_DebugUart, (char *)Terminal_CommandActual);

						// Init new command
						TERMINAL_SEND_NEW_LINE();
						TERMINAL_SEND_PROMT_NEW_LINE();
					}
					else
					{
						// There is no char in the line
						TERMINAL_SEND_PROMT_NEW_LINE();
					}
					Terminal_CommandActualLength = 0;
					Terminal_CommandSentLength = 0;
					Terminal_CommandCursorPosition = 0;
				}
			}	// CommandHandler_CommandReceivedEvent
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
			// If we has EventHandler, go out from infinite loop
			else
			{
				return;
			}
#endif
		}
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
		else
		{
			// We must return, if DebugUart is disabled, and TaskHandler used
			return;
		}
#endif
	}

	// Infinite loop, never exit, never reached here, if blocking mode
}



/**
 * \brief	Check received characters and make command (COMMAND_Actual)
 */
static void Terminal_ProcessReceivedCharacter(void)
{
#ifndef CONFIG_DEBUGUSART_MODE_ONEPERONERCHARACTER

	// Find new received characters
	CircularBuffer_FindLastMessage(&DebugUart_RxBuffStruct);

	// If WriteCnt not equal with ReadCnt, we have received message
	char receiveBuffer[DEBUGUART_RX_BUFFER_SIZE+1];
	uint16_t receivedMessageLength = 0;

	// Received new character?
	if (CircularBuffer_HasNewMessage(&DebugUart_RxBuffStruct))
	{
		// Need copy to receiveBuffer
		receivedMessageLength = CircularBuffer_GetCharacters(&DebugUart_RxBuffStruct, receiveBuffer, true);

		// TODO: Create Get&Clear function
		CircularBuffer_ClearLast(&DebugUart_RxBuffStruct);
	}
	else
	{
		// Not received new characters
		return;
	}
#endif


	// Process characters
	uint16_t i;
	for (i = 0; i < receivedMessageLength; i++)
	{
		volatile char receivedChar = receiveBuffer[i];

#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
		// ESCAPE SEQUENCE
		if (Terminal_CommandEscapeSequenceInProgress)
		{
			// Escape sequence in progress
			// Copy escape characters to Terminal_CommandActualEscape[]
			// TODO: Do more beautiful
			if (Terminal_CommandEscape_cnt == 1)
			{
				if (receivedChar == '[')
				{
					Terminal_CommandActualEscape[Terminal_CommandEscape_cnt++] = receivedChar;
				}
				else
				{
					// Wrong escape sequence
					Terminal_CommandEscapeSequenceInProgress = false;
					Terminal_CommandEscape_cnt = 0;
				}
			}
			else if (Terminal_CommandEscape_cnt == 2)
			{
				Terminal_CommandActualEscape[Terminal_CommandEscape_cnt++] = receivedChar;

				// TODO: only works with escape sequence if 3 chars (ESC[A)
				if (Terminal_CommandActualEscape[2] != '3')
				{
					// \e[A / B / C / D
					Terminal_CommandEscapeSequenceInProgress = false;
					Terminal_CommandEscapeSequenceReceived = true;
					Terminal_CommandReceivedEvent = true;

					return;
				}
				else
				{
					// \e[3~ --> delete at zoc
					//MONITOR_CommandEscape_cnt++;
				}
			}
			else if (Terminal_CommandEscape_cnt == 3)
			{
				Terminal_CommandActualEscape[Terminal_CommandEscape_cnt++] = receivedChar;

				if (Terminal_CommandActualEscape[3] == '~')
				{
					// TODO: At ZOC, it is delete char
					// Delete button
					Terminal_CommandEscapeSequenceInProgress = false;
					Terminal_CommandReceivedDelete = true;
					Terminal_CommandReceivedEvent = true;

					return;
				}
			}
		}
		else
		{
			// No escape sequence
			// An character received
			if (receivedChar  == TERMINAL_KEY_ESCAPE)
			{
				// receive an Escape sequence
				Terminal_CommandEscapeSequenceInProgress = true;
				Terminal_CommandActualEscape[0] = receivedChar;
				Terminal_CommandEscape_cnt = 1;
			}
			else
			{
#endif
				if ((receivedChar  == '\r') || (receivedChar == '\n') || (receivedChar == '\0'))
				{
					// TODO: When received 2 new line character, should not print twice the promt
					// Received Enter
					Terminal_CommandReadable = true;
					Terminal_CommandActual[Terminal_CommandActualLength] = '\0';
					Terminal_CommandReceivedEvent = true;

					return;		// TODO: Not need, because after if-else if the code will exit
				}
				else if (receivedChar == TERMINAL_KEY_BACKSPACE)
				{
					// Received backspace
					Terminal_CommandReceivedBackspace = true;
					Terminal_CommandReceivedEvent = true;

					return;
				}
#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
				else if (receivedChar == TERMINAL_KEY_DELETE)
				{
					// Delete button
					// TODO: Not work at ZOC, but work at other terminal?
					Terminal_CommandReceivedDelete = true;
					Terminal_CommandReceivedEvent = true;

					return;
				}
				else if (receivedChar == '\t')
				{
					// TAB
					Terminal_CommandReceivedTabulator = true;
					Terminal_CommandReceivedEvent = true;

					return;
				}
#endif	// #ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
				else
				{
					// Simple char for the command
					// shorted than max length?
					if (Terminal_CommandActualLength < TERMINAL_MAX_COMMAND_LENGTH)
					{
						if (Terminal_CommandCursorPosition == Terminal_CommandActualLength)
						{
							// CursorPosition = CommandLength		(end character)
							Terminal_CommandActual[Terminal_CommandActualLength] = receivedChar;
							Terminal_CommandActualLength++;
							Terminal_CommandCursorPosition++;
							if (Terminal_CommandSendBackCharEnable)
							{
								// Send received character
								// TODO: We need to response on input channel
								DebugUart_SendChar(receivedChar);
							}
						}
						else
						{
							// CursorPosition < CommandLength		(inner character)
							Terminal_CommandActualLength++;
							// Copy
							uint8_t i;
							for (i = Terminal_CommandActualLength; i > Terminal_CommandCursorPosition; i--)
							{
								Terminal_CommandActual[i] = Terminal_CommandActual[i-1];
							}
							Terminal_CommandActual [Terminal_CommandCursorPosition] = receivedChar;
							Terminal_CommandActual [Terminal_CommandActualLength] = '\0';
							Terminal_CommandCursorPosition++;
							Terminal_CommandReceivedNotLastChar = true;
							Terminal_CommandReceivedEvent = true;
						}
					}
					else
					{
						// longer than max length ...
						// Do not copy/print...
					}

#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
				}
			}
#endif
		} // Processed received characters

	} // End of while
}



#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
/**
 * \brief	Process Escape sequence, works with ANSI escape codes
 * @return	valid char, or 0 if invalid
 */
static bool Terminal_CommandEscapeCharValidation(void)
{
	if (Terminal_CommandActualEscape[0] == TERMINAL_KEY_ESCAPE)	// ESC
	{
		if (Terminal_CommandActualEscape[1] == '[' )			// '[', escape sequence 2. letter
		{
			// This is an escape sequence
			// 'A' Up cursor = previous History command
			if (Terminal_CommandActualEscape[2] == 'A')
			{
#ifdef CONFIG_TERMINAL_HISTORY_ENABLE
				Terminal_HistoryLoad(1);
#endif
				return true;
			}
			// 'B' Down cursor		// next History command
			else if (Terminal_CommandActualEscape[2] == 'B')
			{
#ifdef CONFIG_TERMINAL_HISTORY_ENABLE
				Terminal_HistoryLoad(0);
#endif
				return true;
			}
			// 'C' - Right cursor - Step right
			else if (Terminal_CommandActualEscape[2] == 'C')
			{
				// Is cursor at end?
				if (Terminal_CommandCursorPosition < Terminal_CommandActualLength)
				{
					// Cursor within command
					Terminal_SendMessage(ESCAPE_CURSORRIGHT);
					Terminal_CommandCursorPosition++;

					return true;
				}
				else
				{
					// Cursor at end, do nothing
					return true;
				}
			}
			// 'D' Left cursor - Step left
			else if (Terminal_CommandActualEscape[2] == 'D')
			{
				if (Terminal_CommandCursorPosition > 0)				// if not at start
				{
					Terminal_SendMessage(ESCAPE_CURSORLEFT);
					Terminal_CommandCursorPosition--;

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

#endif	// #ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE



/**
 * \brief	Command's letter deleting (backspace)
 */
void Terminal_CommandBackspace(void)
{
	if (Terminal_CommandActualLength > 0)
	{
		// If has command
		// Cursor at end?
		if (Terminal_CommandCursorPosition == Terminal_CommandActualLength)
		{
			// Cursor at end
			// Delete from CommandActual, and Position--
			Terminal_CommandActual[--Terminal_CommandCursorPosition] = '\0';
			Terminal_CommandActualLength--;

			// Delete last character on terminal
#ifdef CONFIG_TERMINAL_USE_ZOC
			TERMINAL_SEND_KEY_BACKSPACE();
			TERMINAL_SEND_KEY_DEL();
			TERMINAL_SEND_KEY_BACKSPACE();
#endif

#ifdef CONFIG_TERMINAL_USE_PUTTY

			// v1
			TERMINAL_SEND_KEY_DEL();
#endif

#ifdef CONFIG_TERMINAL_USE_HYPERTERMINAL
			TERMINAL_SEND_KEY_BACKSPACE();

			// Delete & Resend
			Terminal_CommandResendLine(true);
#endif
		}
#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
		else
		{
			// CursorPosition != CommandLength, we are in command chars
			// Cursor not at end
			// Event: Used backspace at inner command
			// NOTE: Be careful, cursor need to put at original position

			uint8_t i;

			if (Terminal_CommandCursorPosition > 0)
			{
				// not at 0 position

				// Procedure:
				// copy CommandActual
				// delete & resend

				Terminal_CommandActualLength--;
				Terminal_CommandCursorPosition--;

				for (i = Terminal_CommandCursorPosition; i < Terminal_CommandActualLength; i++)
				{
					Terminal_CommandActual[i] = Terminal_CommandActual[i+1];		// copy
				}
				Terminal_CommandActual[i] = '\0';

				// Send backspace = step left
				TERMINAL_SEND_KEY_BACKSPACE();

				// Delete & Resend
				Terminal_CommandResendLine(true);
			}
			else
			{
				// At 0 position
				// Do nothing, cannot backspace
			}
		}
#endif	// #ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
	}

	return;			// not do anything
}



#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
/**
 * \brief	Delete button received
 */
static void Terminal_CommandDelete(void)
{
	if (Terminal_CommandActualLength > 0)
	{
		// If has command
		// Cursor at end?
		if (Terminal_CommandCursorPosition == Terminal_CommandActualLength)
		{
			// Do nothing at end
		}
		else
		{
			// CursorPosition != CommandLength, we are in command chars
			// Cursor not at end

			uint8_t i;

			if (Terminal_CommandCursorPosition > 0)
			{
				// not at 0 position
				// Procedure:
				// - Copy CommandActual after cursor
				// - Resend command with original cursor position

				Terminal_CommandActualLength--;

				// Drop the backspaced character
				for (i = Terminal_CommandCursorPosition; i < Terminal_CommandActualLength; i++)
				{
					Terminal_CommandActual[i] = Terminal_CommandActual[i+1];		// copy
				}
				Terminal_CommandActual[i] = '\0';

				// Resend line with original cursor position
				Terminal_CommandResendLine(true);
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
#endif	// #ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE



#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
/**
 * \brief	Received tabulator command: Complete command
 */
static void Terminal_CommandTabulator(void)
{
	// Find same command
	uint8_t i;

	for (i = 0; i < CommandHandler_CommandNum; i++)
	{
		if (!StrCmpWithLength(CommandList[i].name, (const char *)Terminal_CommandActual, Terminal_CommandActualLength))
		{
			// It is equal
			// We write the first equal
			// TODO: Handle more tabulator?
			StrCpy((char *)Terminal_CommandActual, CommandList[i].name);

			Terminal_CommandActualLength = StringLength(CommandList[i].name);

			Terminal_CommandCursorPosition = Terminal_CommandActualLength;

			Terminal_CommandResendLine(false);

			break;
		}
	}
}
#endif // #ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE



#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
/**
 * \brief		Resend the actual line/command
 * 				NOTE: It save and restore the original cursor position
 */
static void Terminal_CommandResendLine(bool needRestoreCursor)
{
	// Procedure:
	// - Delete line
	// - (opc) Save cursor
	// - Send new command
	// - (opc) Restore cursor

	// Delete line
	Terminal_SendMessage(ESCAPE_DELETELINE);

	if (needRestoreCursor)
	{
		// Save cursor
		Terminal_SendMessage(ESCAPE_SAVECURSOR);
	}

	// Cursor to line start
	Terminal_SendMessage(ESCAPE_CURSOR_TO_LINESTART);
	Terminal_SendMessage(ESCAPE_CURSORLEFTLOTOF);

	// Write new CommandActual
	TERMINAL_SEND_PROMT();
	Terminal_SendMessage((const char *)Terminal_CommandActual);

	if (needRestoreCursor)
	{
		// Restore the position
		Terminal_SendMessage(ESCAPE_RESTORECURSOR);
	}
}
#endif	// #ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE



#ifdef CONFIG_TERMINAL_HISTORY_ENABLE
/**
 * \brief	Initialize History list with default commands
 */
static void Terminal_InitHistory(void)
{
	uint8_t i;

	for (i = 0; i < TERMINAL_HISTORY_MAX_COUNT; i++)
	{
		StrCpyMax(Terminal_HistoryList[i], Terminal_HistoryInitList[i], TERMINAL_MAX_COMMAND_LENGTH);
	}
}



/**
 * \brief	Save actual command to history
 */
static void Terminal_HistorySave(void)
{
	// Has equal command?
	if (Terminal_HistoryFindInList() == true)
	{
		return;
	}

	// Actual save counter
	if (Terminal_HistorySaveCnt >= (TERMINAL_HISTORY_MAX_COUNT-1))
	{
		Terminal_HistorySaveCnt = 0;
	}
	else
	{
		Terminal_HistorySaveCnt++;
	}

	// Actual saved is the "last"
	Terminal_HistoryLoadCnt = Terminal_HistorySaveCnt;

	// Save command
	StrCpyMax(Terminal_HistoryList[Terminal_HistorySaveCnt],
			(char *)Terminal_CommandActual,
			TERMINAL_MAX_COMMAND_LENGTH);
}
#endif



#ifdef CONFIG_TERMINAL_HISTORY_ENABLE
/**
 * \brief	Check, this command is in History?
 * \return	true, if has equal
 * 			false, if not has equal
 */
static bool Terminal_HistoryFindInList(void)
{
	uint8_t i;

	for (i = 0; i < TERMINAL_HISTORY_MAX_COUNT; i++)
	{
		// Check, equal with command?
		if (!StrCmp((const char *)Terminal_HistoryList[i], (const char *)Terminal_CommandActual))
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



#ifdef CONFIG_TERMINAL_HISTORY_ENABLE
/**
 * \brief	Load history from list to actual command
 */
static void Terminal_HistoryLoad(uint8_t direction)
{
	// Down cursor
	if (direction == 0) // direction == 0
	{
		if (Terminal_HistoryLoadCnt >= (TERMINAL_HISTORY_MAX_COUNT-1))
		{
			Terminal_HistoryLoadCnt = 0;
		}
		else
		{
			Terminal_HistoryLoadCnt++;
		}
	}

	// Up cursor
	// if direction == 1, copy actual

	// Copy command and set cursor
	StrCpy((char *)Terminal_CommandActual, (const char *)Terminal_HistoryList[Terminal_HistoryLoadCnt]);

	// cursor, length!
	Terminal_CommandCursorPosition = StringLength((const char *)Terminal_CommandActual);
	Terminal_CommandActualLength = Terminal_CommandCursorPosition;

	Terminal_CommandResendLine(false);

	// Step load cnt
	if (direction == 1) // direction == 0
	{
		if (Terminal_HistoryLoadCnt <= 0)
		{
			Terminal_HistoryLoadCnt = TERMINAL_HISTORY_MAX_COUNT-1;
		}
		else
		{
			Terminal_HistoryLoadCnt--;
		}
	}
}
#endif



/**
 * \brief	Convert Terminal_CommandActual (Actual command) to small letters
 */
static void Terminal_ConvertSmallLetter(void)
{
	StringLower((char *)Terminal_CommandActual);
}



/**
 * \brief	Send welcome message
 */
void Terminal_SendWelcome(void)
{
	DelayMs(1);

#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
	Terminal_SendCls();						// Clean screen
#endif

	TERMINAL_SEND_WELCOME();				// Welcome message
	// One long string are optimized
#ifdef TERMINAL_COMPILER_SIZE_LARGER_1
	Terminal_SendMessage("Version: ");
	Terminal_SendLine(VERSION_DEFINE);
	Terminal_SendMessage("Compile date: ");
	Terminal_SendMessage(DATE_VERSION);
	Terminal_SendMessage(", ");
	Terminal_SendLine(TIME_VERSION);
	Terminal_SendMessage("Used panel: ");
	Terminal_SendLine(BOARD_NAME);
#endif
#ifdef TERMINAL_COMPILER_SIZE_LARGER_2
	CommandHandler_Printf("Version: %s\r\n", VERSION_DEFINE);
	CommandHandler_Printf("Compile date: %s, %s\r\n", DATE_VERSION, TIME_VERSION);
	CommandHandler_Printf("Used panel: %s\r\n", BOARD_NAME);
#endif

	Reset_PrintResetReasons();

	TERMINAL_SEND_NEW_LINE();
	TERMINAL_SEND_PROMT_NEW_LINE();			// New promt
}



/**
 * \brief	Send CLS (Clear Screen)
 */
void Terminal_SendCls(void)
{
	Terminal_SendMessage(ESCAPE_ERASE_CLS);
	Terminal_SendMessage(ESCAPE_CURSOR_TOPLEFT);
}



#ifdef CONFIG_TERMINAL_GET_PASSWORD_ENABLE
/**
 * \brief Get (and wait) Password
 */
static void Terminal_GetPassword(void)
{
	if (!Terminal_PasswordIsOk)
	{
		// Find new received characters
		CircularBuffer_FindLastMessage(&DebugUart_RxBuffStruct);

		// If WriteCnt not equal with ReadCnt, we have received message
		char receiveBuffer[DEBUGUART_RXBUFFERSIZE+1];
		uint16_t receivedMessageLength = 0;

		// Received new character?
		if (CircularBuffer_HasNewMessage(&DebugUart_RxBuffStruct))
		{
			// Need copy to receiveBuffer
			receivedMessageLength = CircularBuffer_GetCharacters(
					&DebugUart_RxBuffStruct,
					receiveBuffer,
					true);
		}
		else
		{
			// Not received new characters
			return;
		}

		static uint16_t PasswordLength = 0;

		if (PasswordLength != receivedMessageLength)
		{
			// Send '*' if pressed character
			while (receivedMessageLength--)
				Terminal_SendMessage("*");	// Optimized version (old: SendChar())
		}
		else
		{
			// There is no change
			return;
		}

		// Check enter
		if ((STRING_FindCharacter(receiveBuffer, '\r') != NULL) || (STRING_FindCharacter(receiveBuffer, '\n') != NULL))
		{
			// Pressed enter, check password
			StrTrim(receiveBuffer);
			Terminal_SendLine("");
			if (Terminal_CheckPassword((const char*)receiveBuffer))
			{
				// Successful password
				Terminal_SendLine("Successful password!");
				Terminal_PasswordIsOk = true;
				Terminal_SendWelcome();
			}
			else
			{
				// Failed password
				Terminal_SendLine("Wrong password!");
			}

			// TODO: Create Get&Clear function
			CircularBuffer_ClearLast(&DebugUart_RxBuffStruct);
			PasswordLength = 0;
		}
	}
}



/**
 * \brief	Send "Get password" text
 */
inline static void Terminal_SendGetPassword(void)
{
	Terminal_SendMessage("\r\nPassword: ");
}



/**
 * \brief Check password
 */
inline static bool Terminal_CheckPassword(const char *string)
{
	return ((!StrCmp(string, Terminal_Password)) ? true : false);
}

#endif	// #ifdef CONFIG_TERMINAL_GET_PASSWORD_ENABLE



/**
 * \brief	loading event
 */
void Terminal_TestLoading(void)
{
	uint8_t i;

	for (i = 0; i <= 100; i++)
	{
		DelayMs(500);
		//Terminal_SendLoadingPercent(i);
		Terminal_SendLoadingPercent2(i);
	}
}



/**
 * \brief	Send a loading line
 */
void Terminal_SendLoadingPercent(uint8_t percent)
{
	char c;

	// Clear line
	CommandHandler_SendMessage(ESCAPE_DELETELINE);
	CommandHandler_SendMessage(ESCAPE_CURSOR_TO_LINESTART);

	// Send "/ 0-100%" - loading line
	switch (percent%4)
	{
		case 0:		c = '|'; 	break;
		case 1:		c = '/';	break;
		case 2: 	c = '-';	break;
		case 3: 	c = '\\';	break;
		default: 	c = '?';	break;
	}

	// Send example: "/ 10%"
	CommandHandler_Printf("%c %3d%%", c, percent);
}



/**
 * \brief	Send a loading line
 */
void Terminal_SendLoadingPercent2(uint8_t percent)
{
	if (percent > 100)
		percent = 100;

	// Clear line
	CommandHandler_SendMessage(ESCAPE_DELETELINE);
	CommandHandler_SendMessage(ESCAPE_CURSOR_TO_LINESTART);

	// Send "/ 0-100%" - loading line

	// Send example: [----       ] 25%
	uint8_t fill = percent / 10;
	uint8_t empty = 10-fill;
	char formatString[20];
	// Need: "%5c%5c %3d%%"
	usprintf(formatString, "[%%%dc%%%dc] %%3d%%%%", fill, empty);
	CommandHandler_Printf(formatString, '-', ' ', percent);
}



void Terminal_SendMessage(const char * message)
{
	DebugUart_SendMessage(message);
}



void Terminal_SendLine(const char * message)
{
	DebugUart_SendLine(message);
}



#endif	// #ifdef CONFIG_MODULE_TERMINAL_ENABLE
