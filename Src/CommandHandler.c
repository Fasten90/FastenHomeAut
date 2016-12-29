/*
 *		CommandHandler.c
 *
 *		Created on:		2013.10.08.
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Command Handler: communication with UART, execute commands
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
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

/// Enable monitor
const bool MONITOR_CommandReceiveEnable = true;
/// Enable sending back: "Echo mode"
const bool MONITOR_CommandSendBackCharEnable = true;

#ifdef MONITOR_GET_PASSWORD_ENABLE
static const char MONITOR_Password[] = "password";
#endif

const char MONITOR_DelimiterChar = ' ';



/*******************************************************************************
 *						 			Global variables
 ******************************************************************************/

volatile char MONITOR_CommandActual[MONITOR_MAX_COMMAND_LENGTH] = { 0 };
volatile char MONITOR_CommandActualEscape[4] = { 0 };
volatile CommProtocol_t MONITOR_CommandSource = 0;

/// USART Read cnt
volatile uint8_t USART_RxBufferReadCnt = 0;


/// Variables For Monitor
static volatile bool MONITOR_CommandReceivedEvent = false;
static volatile bool MONITOR_CommandReceivedLastChar = false;
static volatile bool MONITOR_CommandReceivedNotLastChar = false;
static volatile bool MONITOR_CommandReadable = false;
static volatile bool MONITOR_CommandReceivedBackspace = false;

#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
static volatile bool MONITOR_CommandReceivedDelete = false;
static volatile bool MONITOR_CommandReceivedTabulator = false;
#endif

static volatile uint8_t MONITOR_CommandActualLength = 0;
static volatile uint8_t MONITOR_CommandSentLength = 0;
static volatile uint8_t MONITOR_CommandCursorPosition = 0;

static volatile bool MONITOR_CommandEscapeSequenceReceived = false;
static volatile uint8_t MONITOR_CommandEscapeSequenceInProgress = 0;
static volatile uint8_t MONITOR_CommandEscape_cnt = 0;

static uint8_t COMMAND_ArgCount = 0;
static char *COMMAND_Arguments[MONITOR_COMMAND_ARG_MAX_COUNT] = { 0 } ;


#ifdef CONFIG_USE_FREERTOS
xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
#endif


#ifdef USE_MONITOR_HISTORY
static uint8_t MONITOR_HISTORY_Save_cnt = 0;
static uint8_t MONITOR_HISTORY_Load_cnt = 0;
static char MONITOR_HISTORY[MONITOR_HISTORY_MAX_COUNT][MONITOR_MAX_COMMAND_LENGTH] =
{

#if defined(USE_MONITOR_HISTORY) && (MONITOR_HISTORY_MAX_COUNT > 0)

	#if ( MONITOR_HISTORY_MAX_COUNT > 0 )
	{
		"help"
	},
	#endif
	#if ( MONITOR_HISTORY_MAX_COUNT > 1 )
	{
		"led 1 1"
	},
	#endif
	#if ( MONITOR_HISTORY_MAX_COUNT > 2 )
	{
		"cls"
	},
	#endif
	#if ( MONITOR_HISTORY_MAX_COUNT > 3 )
	{
		"test"
	},
	#endif
	#if ( MONITOR_HISTORY_MAX_COUNT > 4 )
	{
		"reset"
	}
	#endif
	#if ( MONITOR_HISTORY_MAX_COUNT > 5 )
	#warning "Isn't set monitor history commands"
	#endif
#elif  (!MONITOR_HISTORY_MAX_COUNT)
#error "USE_MONITOR_HISTORY define is defined, but 'MONITOR_HISTORY_MAX_LENGTH' define is not set valid value."
#endif
};
#endif



/*******************************************************************************
 *								Function prototypes
 ******************************************************************************/

static void MONITOR_ProcessReceivedCharacter(void);

#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
static void MONITOR_CommandDelete(void);
static void MONITOR_CommandTabulator(void);
#endif

bool MONITOR_PrepareFindExecuteCommand(CommProtocol_t source);
static uint8_t MONITOR_CommandParser(void);
static bool MONITOR_SearchCommand(void);

void MONITOR_CommandBackspace(void);
void MONITOR_CommandResendLine(bool needRestoreCursor);
bool MONITOR_CommandEscapeCharValidation(void);

#ifdef USE_MONITOR_HISTORY
// Monitor history
void MONITOR_HISTORY_Save(void);
bool MONITOR_HISTORY_FindInHistoryList(void);
void MONITOR_HISTORY_Load(uint8_t direction);
#endif

void MONITOR_ConvertSmallLetter(void);

void MONITOR_CheckResultAndRespond(CommandResult_t result);
CommandResult_t MONITOR_RunCommand(CommandID_t commandID);
CommandResult_t MONITOR_CheckArgumentNumIsGood(uint8_t receivedArgNum,
		uint8_t commandArgNum);

#ifdef MONITOR_GET_PASSWORD_ENABLE
static void MONITOR_GetPassword(void);
static bool MONITOR_CheckPassword(const char *string);
#endif



/*******************************************************************************
 *									Functions
 ******************************************************************************/


/**
 * \brief	Initialize Monitor program
 */
void MONITOR_Init(void)
{

	// Initialize

	MONITOR_CommandReceivedEvent = false;
	MONITOR_CommandReceivedNotLastChar = false;

	MONITOR_CommandActualLength = 0;
	MONITOR_CommandSentLength = 0;
	MONITOR_CommandCursorPosition = 0;

#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
	MONITOR_CommandEscapeSequenceReceived = false;
	MONITOR_CommandEscapeSequenceInProgress = false;
	MONITOR_CommandEscape_cnt = 0;
#endif

	COMMAND_ArgCount = 0;

	// End of initialization

	return;
}



/**
 * \brief	Send welcome message
 */
void MONITOR_SendWelcome(void)
{

	DelayMs(1);

#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
	MONITOR_SendCls();						// Clean screen
#endif
	
	MONITOR_SEND_WELCOME();					// Welcome message

	MONITOR_SEND_NEW_LINE();
	MONITOR_SEND_PROMT_NEW_LINE();					// New promt

	return;
	
}



/**
 * \brief	Always run, wait command and execute it
 */
void MONITOR_CheckCommand(void)
{

	// Initialize
	//MONITOR_InitMonitor();	// Init


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
		MONITOR_SendLine("ERROR in GlobalVarList! Should correct that!");
	}

	// Start receive
	USART_StartReceiveMessage();

#ifdef MONITOR_GET_PASSWORD_ENABLE
	MONITOR_GetPassword();
#endif

	// Welcome message
	MONITOR_SendWelcome();


	// Infinite "task" loop
	while (1)
	{

		// Always checking the Command
		if (MONITOR_CommandReceiveEnable)
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
				MONITOR_ProcessReceivedCharacter();
			}
			#else
			// If not used FreeRTOS, always check characters
			MONITOR_ProcessReceivedCharacter();
			#endif

			if (MONITOR_CommandReceivedEvent)
			{
				// Clear event
				MONITOR_CommandReceivedEvent = false;

				// Only one event will receive
				if (MONITOR_CommandReceivedBackspace)
				{
					// Backspace
					MONITOR_CommandReceivedBackspace = false;
					MONITOR_CommandBackspace();
				}
#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
				else if (MONITOR_CommandReceivedDelete)
				{
					// Delete
					MONITOR_CommandReceivedDelete = false;
					MONITOR_CommandDelete();
				}
				else if (MONITOR_CommandReceivedNotLastChar)
				{
					// Received inner character

					MONITOR_CommandReceivedNotLastChar = false;

					// Step right
					MONITOR_SendMessage(ESCAPE_CURSORRIGHT);
					// Not Last char (it is inner character) - Refresh the line
					MONITOR_CommandResendLine(true);
				}
				else if (MONITOR_CommandEscapeSequenceReceived)
				{
					// Escape sequence
					MONITOR_CommandEscapeSequenceReceived = false;
					MONITOR_CommandEscapeCharValidation();
				}
				else if (MONITOR_CommandReceivedTabulator)
				{
					// Received tabulator
					MONITOR_CommandReceivedTabulator = false;
					MONITOR_CommandTabulator();
				}
#endif
				else if (MONITOR_CommandReadable)
				{
					// Pressed Enter, EndCommand();
					MONITOR_CommandReadable = false;
					if (MONITOR_CommandActualLength > 0)
					{
						// There are some char in the line
						// has an command
						MONITOR_ConvertSmallLetter();
						
						MONITOR_SEND_NEW_LINE();
						
						#ifdef USE_MONITOR_HISTORY
						// Save command to History
						MONITOR_HISTORY_Save();
						#endif

						// Search command and run
						MONITOR_PrepareFindExecuteCommand(CommProt_DebugUart);
					}
					else
					{
						// There is no char in the line
						MONITOR_SEND_PROMT_NEW_LINE();
					}
					MONITOR_CommandActualLength = 0;
					MONITOR_CommandSentLength = 0;
					MONITOR_CommandCursorPosition = 0;
				}
			}
		}
	}
	
	// Infinite loop, never exit, never reached here.

}



/**
 * \brief	Check received characters and make command (COMMAND_Actual)
 */
static void MONITOR_ProcessReceivedCharacter(void)
{

	// While Read cnt not equal than Write cnt
	while (USART_RxBufferReadCnt != USART_RxBufferWriteCounter)
	{
		volatile char USART_ReceivedChar = '\0';

		USART_ReceivedChar = USART_RxBuffer[USART_RxBufferReadCnt];
		USART_RxBufferReadCnt++;

#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
		// ESCAPE SEQUENCE
		if ( MONITOR_CommandEscapeSequenceInProgress )
		{
			// Escape sequence in progress
			// Copy escape characters to MONITOR_CommandActualEscape[]
			// TODO: Megcsinálni elegánsabban
			if (MONITOR_CommandEscape_cnt == 1)
			{
				if (USART_ReceivedChar == '[')
				{
					MONITOR_CommandActualEscape[MONITOR_CommandEscape_cnt++] = USART_ReceivedChar;
				}
				else
				{
					// Wrong escape sequence
					MONITOR_CommandEscapeSequenceInProgress = false;
					MONITOR_CommandEscape_cnt = 0;
				}
			}
			else if ( MONITOR_CommandEscape_cnt == 2)
			{
				MONITOR_CommandActualEscape[MONITOR_CommandEscape_cnt++] = USART_ReceivedChar;

				// TODO: only work with escape sequence if 3 chars (ESC[A)
				if (MONITOR_CommandActualEscape[2] != '3')
				{
					// \e[A / B / C / D
					MONITOR_CommandEscapeSequenceInProgress = false;
					MONITOR_CommandEscapeSequenceReceived = true;
					MONITOR_CommandReceivedEvent = true;
					return;
				}
				else
				{
					// \e[3~ --> delete at zoc
					//MONITOR_CommandEscape_cnt++;
				}
			}
			else if ( MONITOR_CommandEscape_cnt == 3)
			{
				MONITOR_CommandActualEscape[MONITOR_CommandEscape_cnt++] = USART_ReceivedChar;

				if (MONITOR_CommandActualEscape[3] == '~')
				{
					// TODO: At ZOC, it is delete char
					// Delete button
					MONITOR_CommandEscapeSequenceInProgress = false;
					MONITOR_CommandReceivedDelete = true;
					MONITOR_CommandReceivedEvent = true;
					return;
				}
			}
		}
		else
		{
#endif	// #ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
			// No escape sequence
			// An character received

			if ( USART_ReceivedChar  == '\x1B')	// 'ESC'
			{
				// receive an Escape sequence
				MONITOR_CommandEscapeSequenceInProgress = true;
				MONITOR_CommandActualEscape[0] = USART_ReceivedChar;
				MONITOR_CommandEscape_cnt = 1;
			}
			else
			{
				if ( (USART_ReceivedChar  == '\r') || (USART_ReceivedChar == '\n') ||
					(USART_ReceivedChar == '\0'))
				{
					// Received Enter
					MONITOR_CommandReadable = true;
					MONITOR_CommandActual[MONITOR_CommandActualLength] = '\0';
					MONITOR_CommandReceivedEvent = true;
					return;
				}
				else if (USART_ReceivedChar == USART_KEY_BACKSPACE)
				{
					// Received backspace
					MONITOR_CommandReceivedBackspace = true;
					MONITOR_CommandReceivedEvent = true;
					return;
				}
#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
				else if (USART_ReceivedChar == USART_KEY_DELETE)
				{
					// Delete button
					// TODO: Not work at ZOC, but work at other terminal?
					MONITOR_CommandReceivedDelete = true;
					MONITOR_CommandReceivedEvent = true;
					return;
				}
				else if (USART_ReceivedChar == '\t')
				{
					// TAB
					MONITOR_CommandReceivedTabulator = true;
					MONITOR_CommandReceivedEvent = true;
					return;
				}
#endif	// #ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
				else
				{
					// Simple char for the command
					// shorted than max length?
					if (MONITOR_CommandActualLength < MONITOR_MAX_COMMAND_LENGTH)
					{
						if (MONITOR_CommandCursorPosition == MONITOR_CommandActualLength)
						{
							// CursorPosition = CommandLength		(end character)
							MONITOR_CommandActual[MONITOR_CommandActualLength] = USART_ReceivedChar;
							MONITOR_CommandActualLength++;
							MONITOR_CommandCursorPosition++;
							if (MONITOR_CommandSendBackCharEnable)
							{
								// Send received character
								MONITOR_SendChar( USART_ReceivedChar );
							}
						}
						else
						{
							// CursorPosition < CommandLength		(inner character)
							MONITOR_CommandActualLength++;
							// Copy
							uint8_t i;
							for (i = MONITOR_CommandActualLength; i > MONITOR_CommandCursorPosition; i--)
							{
								MONITOR_CommandActual[i] = MONITOR_CommandActual[i-1];
							}
							MONITOR_CommandActual [MONITOR_CommandCursorPosition] = USART_ReceivedChar;
							MONITOR_CommandActual [MONITOR_CommandActualLength] = '\0';
							MONITOR_CommandCursorPosition++;
							MONITOR_CommandReceivedNotLastChar = true;
							MONITOR_CommandReceivedEvent = true;
						}
					}
					else
					{
						// longer than max length ...
						// Do not copy/print...
					}

				}
#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
			}
#endif

		} // Processed received characters

	} // End of while
}



/**
 * \brief	Prepare (Separate) the command and Find and Run it...
 * 			TODO: Átalakítani char* átvevősre
 */
bool MONITOR_PrepareFindExecuteCommand(CommProtocol_t source)
{

	bool isSuccessful = false;

	MONITOR_CommandSource = source;
	// Separate command
	COMMAND_ArgCount = MONITOR_CommandParser();

	if (COMMAND_ArgCount > 0)
	{
		// Find and execute the command
		isSuccessful = MONITOR_SearchCommand();
		MONITOR_SEND_NEW_LINE();
	}
	else
	{
		// 0 Argument num, Cannot separated, this is not a command
		isSuccessful = false;
	}

	// Init new command
	MONITOR_SEND_PROMT_NEW_LINE();

	return isSuccessful;
}



/**
 * \brief	Separate command to parameters/arguments
 * 			: from ActualCommand to COMMAND_Arguments[0], [1], [2]
 * 			NOTE: Only call from MONITOR_PrepareFindExecuteCommand()
 * 	\return	argument number
 */
static uint8_t MONITOR_CommandParser(void)
{

	uint8_t commandArgCount = 0;

	commandArgCount = STRING_Splitter((char*)MONITOR_CommandActual, MONITOR_DelimiterChar,
			COMMAND_Arguments, MONITOR_COMMAND_ARG_MAX_COUNT);


	if (commandArgCount > MONITOR_COMMAND_ARG_MAX_COUNT)
	{
		// Too many arugments
		MONITOR_SendMessage("Too many arguments!\r\n");
		commandArgCount = 0;
	}

	return commandArgCount;
}



/**
 * \brief	Find the command (in list)
 */
static bool MONITOR_SearchCommand(void)
{

	CommandID_t i;
	bool CommandValid = false;
	CommandResult_t result = CommandResult_Error_CommandNotFound;

	// Search the command
	for (i=0; i < MONITOR_CommandNum; i++)
	{

		if (!StrCmp(COMMAND_Arguments[0],CommandList[i].name))
		{
			// Found the command
			result = MONITOR_RunCommand(i);

			// Valid Command
			CommandValid = true;
			break;
		}
	}

	// Write result
	MONITOR_CheckResultAndRespond(result);


	// Return with validation
	return CommandValid;

}



/**
 * \brief	Command's letter deleting (backspace)
 */
void MONITOR_CommandBackspace(void)
{

	if (MONITOR_CommandActualLength > 0)
	{
		// If has command
		// Cursor at end?
		if ( MONITOR_CommandCursorPosition == MONITOR_CommandActualLength)
		{
			// Cursor at end
			// Delete from CommandActual, and Position--
			MONITOR_CommandActual[--MONITOR_CommandCursorPosition] = '\0';
			MONITOR_CommandActualLength--;
			
			// Delete last character on terminal
#ifdef CONFIG_USE_TERMINAL_ZOC
			USART_SEND_KEY_BACKSPACE();
			USART_SEND_KEY_DEL();
			USART_SEND_KEY_BACKSPACE();
#endif		
			
#ifdef CONFIG_USE_TERMINAL_PUTTY
			
			// v1
			USART_SEND_KEY_DEL();
#endif
			
#ifdef CONFIG_USE_TERMINAL_HYPERTERMINAL
			
			USART_SEND_KEY_BACKSPACE();

			// Delete & Resend
			MONITOR_CommandResendLine(true);

#endif
		}
#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
		else
		{
			// CursorPosition != CommandLength, we are in command chars
			// Cursor not at end
			// Event: Used backspace at inner command
			// NOTE: Be careful, cursor need to put at original position

			uint8_t i;

			if ( MONITOR_CommandCursorPosition > 0 )
			{
				// not at 0 position

				// Procedure:
				// copy CommandActual
				// delete & resend

				MONITOR_CommandActualLength--;
				MONITOR_CommandCursorPosition--;

				for ( i = MONITOR_CommandCursorPosition; i < MONITOR_CommandActualLength; i++ )
				{
					MONITOR_CommandActual[i] = MONITOR_CommandActual[i+1];		// copy
				}
				MONITOR_CommandActual[i] = '\0';

				// Send backspace = step left
				USART_SEND_KEY_BACKSPACE();

				// Delete & Resend
				MONITOR_CommandResendLine(true);

			}
			else
			{
				// At 0 position
				// Do nothing, cannot backspace
			}
		}
#endif	// #ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
	}
	return;			// not do anything
}



#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
/**
 * \brief	Delete button received
 */
static void MONITOR_CommandDelete(void)
{

	if (MONITOR_CommandActualLength > 0)
	{
		// If has command
		// Cursor at end?
		if (MONITOR_CommandCursorPosition == MONITOR_CommandActualLength)
		{
			// Do nothing at end
		}
		else
		{
			// CursorPosition != CommandLength, we are in command chars
			// Cursor not at end

			uint8_t i;

			if (MONITOR_CommandCursorPosition > 0)
			{
				// not at 0 position
				// Procedure:
				// - Copy CommandActual after cursor
				// - Resend command with original cursor position

				MONITOR_CommandActualLength--;

				// Drop the backspaced character
				for (i = MONITOR_CommandCursorPosition; i < MONITOR_CommandActualLength; i++)
				{
					MONITOR_CommandActual[i] = MONITOR_CommandActual[i+1];		// copy
				}
				MONITOR_CommandActual[i] = '\0';

				// Resend line with original cursor position
				MONITOR_CommandResendLine(true);
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
#endif	// #ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE



#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
/**
 * \brief	Received tabulator command: Complete command
 */
static void MONITOR_CommandTabulator(void)
{
	// Find same command
	uint8_t i;

	for (i=0; i < MONITOR_CommandNum; i++)
	{
		if (!StrCmpWithLength(CommandList[i].name, (const char *)MONITOR_CommandActual,MONITOR_CommandActualLength))
		{
			// It is equal
			// We write the first equal
			// TODO: Lekezelni az esetleges több Tabulátort? biztos jó, hogy az első egyezőt kiírjuk?
			StrCpy((char *)MONITOR_CommandActual, CommandList[i].name);

			MONITOR_CommandActualLength = StringLength(CommandList[i].name);

			MONITOR_CommandCursorPosition = MONITOR_CommandActualLength;

			MONITOR_CommandResendLine(false);

			return;
		}
	}
}
#endif // #ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE



#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
/**
 * \brief		Resend the actual line/command
 * 				NOTE: It save and restore the original cursor position
 */
void MONITOR_CommandResendLine( bool needRestoreCursor)
{
	// Procedure:
	// - Delete line
	// - (opc) Save cursor
	// - Send new command
	// - (opc) Restore cursor

	// Delete line
	MONITOR_SendMessage(ESCAPE_DELETELINE);

	if (needRestoreCursor)
	{
		// Save cursor
		MONITOR_SendMessage(ESCAPE_SAVECURSOR);
	}

	// Cursor to line start
	MONITOR_SendMessage(ESCAPE_CURSOR_TO_LINESTART);
	MONITOR_SendMessage(ESCAPE_CURSORLEFTLOTOF);

	// Write new CommandActual
	MONITOR_SEND_PROMT();
	MONITOR_SendMessage((const char *)MONITOR_CommandActual);

	if (needRestoreCursor)
	{
		// Restore the position
		MONITOR_SendMessage(ESCAPE_RESTORECURSOR);
	}

	return;
}
#endif	// #ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE



#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
/**
 * \brief	Process Escape sequence
 */
bool MONITOR_CommandEscapeCharValidation(void)
{
	// return valid char, or 0 if invalid
	// work with ANSI escape codes

	if (MONITOR_CommandActualEscape[0] == '\x1B')				// ESC
	{
		if ( MONITOR_CommandActualEscape[1] == '[' )			// '[', escape sequence 2. letter
		{
			// This is an escape sequence
			if ( MONITOR_CommandActualEscape[2] == 'A' )		// Up cursor = previous History command
			{
				#ifdef USE_MONITOR_HISTORY
				MONITOR_HISTORY_Load ( 1 );
				#endif
				return true;
			}
			else if ( MONITOR_CommandActualEscape[2] == 'B' )	// Down cursor		// next History command
			{
				#ifdef USE_MONITOR_HISTORY
				MONITOR_HISTORY_Load ( 0 );
				#endif
				return true;
			}
			else if (  MONITOR_CommandActualEscape[2] == 'C' )	// Right cursor
			{
				if ( MONITOR_CommandCursorPosition < MONITOR_CommandActualLength )	// if not at end
				{
					MONITOR_SendMessage(ESCAPE_CURSORRIGHT);
					MONITOR_CommandCursorPosition++;
					return true;
				}
				else
				{
					// not do anything
					return true;
				}
			}
			else if (  MONITOR_CommandActualEscape[2] == 'D' )			// Left cursor
			{
				if ( MONITOR_CommandCursorPosition > 0 )				// if not at start
				{
					MONITOR_SendMessage(ESCAPE_CURSORLEFT);
					MONITOR_CommandCursorPosition--;
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
#endif	// #ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE



#ifdef USE_MONITOR_HISTORY
/**
 * \brief	Save actual command to history
 */
void MONITOR_HISTORY_Save(void)
{

	// Has equal command?
	if ( MONITOR_HISTORY_FindInHistoryList() == true )
	{
		return;
	}

	// Actual save counter
	if ( MONITOR_HISTORY_Save_cnt >= ( MONITOR_HISTORY_MAX_COUNT-1 ) )
	{
		MONITOR_HISTORY_Save_cnt = 0;
	}
	else
	{
		MONITOR_HISTORY_Save_cnt++;
	}

	// Actual saved is the "last"
	MONITOR_HISTORY_Load_cnt = MONITOR_HISTORY_Save_cnt;

	// Save command
	StrCpyMax( MONITOR_HISTORY[MONITOR_HISTORY_Save_cnt],
			(char *)MONITOR_CommandActual,
			MONITOR_MAX_COMMAND_LENGTH );

	return;

}
#endif



#ifdef USE_MONITOR_HISTORY
/**
 * \brief	Check, this command is in History?
 * \return	true, if has equal
 * 			false, if not has equal
 */
bool MONITOR_HISTORY_FindInHistoryList(void)
{
	uint8_t i;
	
	for (i = 0; i < MONITOR_HISTORY_MAX_COUNT; i++)
	{
		if ( !StrCmp((const char *)MONITOR_HISTORY[i],(const char * )MONITOR_CommandActual))	// If it is equal
		{
			// Has equal command
			return true;
		}
	}

	// There is no equal command
	return false;

}
#endif



#ifdef USE_MONITOR_HISTORY
/**
 * \brief	Load history from list to actual command
 */
void MONITOR_HISTORY_Load(uint8_t direction)
{

	// down cursor
	if (direction == 0) // direction == 0
	{
		if (MONITOR_HISTORY_Load_cnt >= (MONITOR_HISTORY_MAX_COUNT-1))
		{
			MONITOR_HISTORY_Load_cnt = 0;
		}
		else
		{
			MONITOR_HISTORY_Load_cnt++;
		}
	}

	// up cursor
	// if direction == 1, copy actual

	// Copy command and set cursor
	StrCpy((char *)MONITOR_CommandActual, (const char *)MONITOR_HISTORY[MONITOR_HISTORY_Load_cnt]);

	// cursor, length!
	MONITOR_CommandCursorPosition = StringLength((const char *)MONITOR_CommandActual);
	MONITOR_CommandActualLength = MONITOR_CommandCursorPosition;

	MONITOR_CommandResendLine(false);

	// Step load cnt
	if (direction == 1) // direction == 0
	{
		if (MONITOR_HISTORY_Load_cnt <= 0)
		{
			MONITOR_HISTORY_Load_cnt = MONITOR_HISTORY_MAX_COUNT-1;
		}
		else
		{
			MONITOR_HISTORY_Load_cnt--;
		}
	}

	return;
}
#endif



/**
 * \brief	Convert MONITOR_CommandActual (Actual command) to small letters
 */
void MONITOR_ConvertSmallLetter(void)
{
	uint8_t i;
	
	for (i = 0; MONITOR_CommandActual[i] != '\0'; i++)
	{
		if ( ( MONITOR_CommandActual[i] > 'A' ) && ( MONITOR_CommandActual[i] < 'Z' ) )
		{	// need to change to small letter
			MONITOR_CommandActual[i] = MONITOR_CommandActual[i] - ( 'A' - 'a'); // length between Big Letter and small letter
		}
	}

	return;
}



/**
 * \brief	Check result and write response
 */
void MONITOR_CheckResultAndRespond(CommandResult_t result)
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

	MONITOR_SendLine(pMessage);
}



/**
 * \brief	Run command
 */
CommandResult_t MONITOR_RunCommand(CommandID_t commandID)
{

	CommandResult_t result = CommandResult_Unknown;
	bool needWriteHelp = false;

	// Check argument nums
	result = MONITOR_CheckArgumentNumIsGood(COMMAND_ArgCount, CommandList[commandID].commandArgNum);

	if (result == CommandResult_Ok)
	{
		// Good, found & argument num is ok

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
		// Call function
		FunctionPointer thisFunction = ( FunctionPointer )CommandList[commandID].commandFunctionPointer;
#pragma GCC diagnostic pop
		// End of warning suppress

		// Execute the command function
		result = thisFunction(COMMAND_ArgCount,(char**)COMMAND_Arguments);
	}
	else
	{
		// if not Ok, write help
		needWriteHelp = true;
	}

	// Write command help, if command run failed
	if (needWriteHelp)
	{
		MONITOR_WriteCommandHelp (commandID);
	}

	return result;
}



/**
 * \brief	Print all commands
 */
void MONITOR_PrintAllCommands(void)
{
	CommandID_t i;

	MONITOR_SendLine("Using help:\r\n"
			"help <command>\r\n\r\n"
			"Commands list:");
	for (i=0; i < MONITOR_CommandNum; i++)
	{
		// Write a command
		MONITOR_SendLine(CommandList[i].name);
	}
}



/**
 * \brief	Search command and print help
 */
void MONITOR_SearchCommandAndPrintHelp(const char *command)
{
	CommandID_t i;
	bool isOk = false;

	for (i=0; i < MONITOR_CommandNum; i++)
	{
		// Find the command
		if (!StrCmp(CommandList[i].name, command))
		{
			// Command's describe
			MONITOR_WriteCommandHelp(i);
			isOk = true;
		}
	}

	if (!isOk)
	{
		MONITOR_Printf("Not find command: %s\r\n", command);
	}
}



/**
 * \brief	Write a command help
 */
void MONITOR_WriteCommandHelp(CommandID_t commandID)
{

	// Print command to source
	MONITOR_Printf(
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
CommandResult_t MONITOR_CheckArgumentNumIsGood(uint8_t receivedArgNum, uint8_t commandArgNum)
{
	// Check commandArgNum. bit is set?
	if (receivedArgNum > MONITOR_COMMAND_ARG_MAX_COUNT)
	{
		return CommandResult_Error_TooManyArgument;
	}
	if (commandArgNum > MONITOR_COMMAND_ARG_MAX_NUM_BITS)
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
		for (i=0; i<MONITOR_COMMAND_ARG_MAX_COUNT; i++)
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
void MONITOR_SendMessage(const char *message)
{
	COMMUNICATION_SendMessage(MONITOR_CommandSource, message);
}



/**
 * \brief	Send line (with newline) to source
 */
void MONITOR_SendLine(const char *message)
{
	MONITOR_SendMessage(message);
	MONITOR_SendMessage("\r\n");
}



/**
 * \brief	Send character
 */
void MONITOR_SendChar(char c)
{
	COMMUNICATION_SendChar(MONITOR_CommandSource, c);
}



/**
 * \brief	Send message on xy communication protocol
 * \param	protocol		what peripheral sending
 */
void MONITOR_Printf(const char *format, ...)
{

	// Working in at:
	char txBuffer[TXBUFFERSIZE];

	va_list ap;									// argument pointer
	va_start(ap, format); 						// ap on arg
	string_printf(txBuffer,format,ap);			// Separate and process
	va_end(ap);						 			// Cleaning after end

	MONITOR_SendMessage(txBuffer);

	return;

}



/**
 * \brief	Send CLS
 */
void MONITOR_SendCls(void)
{
	MONITOR_SendMessage(ESCAPE_ERASE_CLS);
	MONITOR_SendMessage(ESCAPE_CURSOR_TOPLEFT);
}



#ifdef MONITOR_GET_PASSWORD_ENABLE
/**
 * \brief Get (and wait) Password
 */
static void MONITOR_GetPassword(void)
{

	bool passwordIsOk = false;

	// Wait first character
	MONITOR_SendLine("\r\nType a character:");
	while (USART_RxBufferWriteCounter < 1);
	USART_RxBufferReadCnt = 1;

	while (!passwordIsOk)
	{
		MONITOR_SendMessage("\r\nPassword:");

		bool isTry = true;
		MONITOR_CommandActualLength = 0;

		while (isTry)
		{

			// While Read cnt not equal than Write cnt
			if (USART_RxBufferReadCnt != USART_RxBufferWriteCounter)
			{
				volatile char USART_ReceivedChar = '\0';

				USART_ReceivedChar = USART_RxBuffer[USART_RxBufferReadCnt];
				USART_RxBufferReadCnt++;
				MONITOR_SendChar('*');

				if (USART_ReceivedChar == '\r' || USART_ReceivedChar == '\n')
				{
					// Pressed enter, check password
					isTry = false;
					MONITOR_CommandActual[MONITOR_CommandActualLength++] = '\0';
					USART_SendNewLine();
					if (MONITOR_CheckPassword((const char*)MONITOR_CommandActual))
					{
						// Successful password
						MONITOR_SendLine("Successful password!");
						MONITOR_CommandActualLength=0;
						return;
					}
					else
					{
						// Failed password
						MONITOR_SendLine("Wrong password!");
					}
				}
				else
				{
					// Copy character
					MONITOR_CommandActual[MONITOR_CommandActualLength++] = USART_ReceivedChar;
				}
			}
		}
	}
}



/**
 * \brief Check password
 */
static bool MONITOR_CheckPassword(const char *string)
{
	if (!StrCmp(string,MONITOR_Password))
	{
		// Equal
		return true;
	}
	else
	{
		return false;
	}
}

#endif	// #ifdef MONITOR_GET_PASSWORD_ENABLE
