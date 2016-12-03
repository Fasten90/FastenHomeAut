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


#include "include.h"
#include "stdint.h"
#include "command.h"

#include "escapesequence.h"
#include "CommandHandler.h"
#include "GlobalVarHandler.h"

#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
#include "homeautmessage.h"
#include "raspberrypi.h"
#endif


/////////////////////////////////
//			Configs:
/////////////////////////////////

// Enable monitor
const bool MONITOR_CommandReceiveEnable = true;
// Enable sending back: "Echo mode"
const bool MONITOR_CommandSendBackCharEnable = true;

#ifdef MONITOR_GET_PASSWORD_ENABLE
static const char MONITOR_Password[] = "password";
#endif

const char MONITOR_DelimiterChar = ' ';

/////////////////////////////////
// 		GLOBAL VARIABLES
/////////////////////////////////

volatile char MONITOR_CommandActual[MONITOR_MAX_COMMAND_LENGTH] = { 0 };
volatile char MONITOR_CommandActualEscape[4] = { 0 };
volatile CommProtocol_t MONITOR_CommandSource = 0;

// USART Read cnt
volatile uint8_t USART_RxBufferReadCnt = 0;


// Variables For Monitor
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


/////////////////////////////
///		FUNCTION PROTOTYPES
/////////////////////////////

static void MONITOR_ProcessReceivedCharacter ( void );

#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
static void MONITOR_CommandDelete ( void );
static void MONITOR_CommandTabulator ( void );
#endif

bool MONITOR_PrepareFindExecuteCommand ( CommProtocol_t source );
static uint8_t MONITOR_CommandParser ( void );
static bool MONITOR_SearchCommand ( void );

void MONITOR_CommandBackspace ( void );
void MONITOR_CommandResendLine ( void );
void MONITOR_NewCommandResendLine ( void );
bool MONITOR_CommandEscapeCharValidation ( void );

#ifdef USE_MONITOR_HISTORY
// Monitor history
void MONITOR_HISTORY_Save (void);
bool MONITOR_HISTORY_FindInHistoryList (void);
void MONITOR_HISTORY_Load (uint8_t direction);
#endif

void MONITOR_ConvertSmallLetter (void);

void MONITOR_CheckResultAndRespond (CommandResult_t result);
void MONITOR_RunCommand (CommandID_t commandID);
CommandResult_t MONITOR_ArgumentNumIsGood (uint8_t receivedArgNum, uint8_t commandArgNum);
void MONITOR_SendMessage(const char *message);
void MONITOR_SendLine(const char *message);
void MONITOR_SendChar(char c);

#ifdef MONITOR_GET_PASSWORD_ENABLE
static void MONITOR_GetPassword (void);
static bool MONITOR_CheckPassword (const char *string);
#endif


/////////////////////////////
///		FUNCTIONS
/////////////////////////////


/**
 * \brief	Initialize Monitor program
 */
void MONITOR_Init ( void )
{

	// Init

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

	// End of init

	return;
}



/**
 * \brief	Send welcome message
 */
void MONITOR_SendWelcome ( void )
{

	#ifdef CONFIG_USE_FREERTOS
	vTaskDelay(1);
	#else
	HAL_Delay(1);
	#endif

#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
	MONITOR_SendCls();						// Clean screen
#endif
	
	MONITOR_SEND_WELCOME();					// Welcome message

	MONITOR_SEND_NEW_LINE();
	MONITOR_SEND_PROMT();					// New promt

	return;
	
}



/**
 * \brief	Always run, wait command and execute it
 */
void MONITOR_CheckCommand ( void )
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
		if ( MONITOR_CommandReceiveEnable )
		{

			#ifdef CONFIG_USE_FREERTOS
			// Wait semaphore
			if ( xSemaphoreTake(DEBUG_USART_Tx_Semaphore,1000) == pdTRUE )
			{
				MONITOR_CommandReceivedEvent = true;
			}
			#endif

#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
			// Test for Raspberry Pi
			if (BUTTON_Clicked)
			{
				BUTTON_Clicked = 0;
				RASPBERRYPI_SendMessage(1, Function_Alarm, Alarm_PressedButton , 17);
			}
#endif

			MONITOR_ProcessReceivedCharacter();

			if ( MONITOR_CommandReceivedEvent )
			{
				// Clear event
				MONITOR_CommandReceivedEvent = false;

				// Only one event will receive
				if ( MONITOR_CommandReceivedBackspace )
				{
					// Backspace
					MONITOR_CommandReceivedBackspace = false;
					MONITOR_CommandBackspace();
				}
#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
				else if ( MONITOR_CommandReceivedDelete )
				{
					// Delete
					MONITOR_CommandReceivedDelete = false;
					MONITOR_CommandDelete();
				}
				else if ( MONITOR_CommandReceivedNotLastChar )
				{
					// Received inner character

					MONITOR_CommandReceivedNotLastChar = false;

					// Step right
					MONITOR_SendMessage(USART_ESCAPE_CURSORRIGHT);
					// Not Last char (it is inner character) - Refresh the line
					MONITOR_CommandResendLine();
				}
				else if ( MONITOR_CommandEscapeSequenceReceived )
				{
					// Escape sequence
					MONITOR_CommandEscapeSequenceReceived = false;
					MONITOR_CommandEscapeCharValidation ();
				}
				else if ( MONITOR_CommandReceivedTabulator )
				{
					// Received tabulator
					MONITOR_CommandReceivedTabulator = false;
					MONITOR_CommandTabulator();
				}
#endif
				else if ( MONITOR_CommandReadable )
				{
					// Pressed Enter, EndCommand();
					MONITOR_CommandReadable = false;
					if ( MONITOR_CommandActualLength > 0)
					{
						// There are some char in the line
						// has an command
						MONITOR_ConvertSmallLetter();
						
						MONITOR_SEND_NEW_LINE();
						
						// Search command and run
						MONITOR_PrepareFindExecuteCommand(Source_DebugUart);
						
						#ifdef USE_MONITOR_HISTORY
						// Save command to History
						MONITOR_HISTORY_Save();
						#endif
					}
					else
					{
						// There is no char in the line
						MONITOR_SEND_PROMT();
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
static void MONITOR_ProcessReceivedCharacter ( void )
{

	// While Read cnt not equal than Write cnt
	while (USART_RxBufferReadCnt != USART_RxBufferWriteCounter)
	{
		volatile char USART_ReceivedChar = '\0';

		// TODO: Don't know, why work...
		USART_ReceivedChar = USART_RxBuffer[USART_RxBufferReadCnt-1];
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
				else if ( USART_ReceivedChar  == USART_KEY_BACKSPACE )
				{
					// Received backspace
					MONITOR_CommandReceivedBackspace = true;
					MONITOR_CommandReceivedEvent = true;
					return;
				}
#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
				else if ( USART_ReceivedChar == USART_KEY_DELETE )
				{
					// Delete button
					// TODO: Not work at ZOC, but work at other terminal?
					MONITOR_CommandReceivedDelete = true;
					MONITOR_CommandReceivedEvent = true;
					return;
				}
				else if ( USART_ReceivedChar == '\t' )
				{
					// TAB
					MONITOR_CommandReceivedTabulator = true;
					MONITOR_CommandReceivedEvent = true;
					return;
				}
#endif	// #ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
				else
				{
					// simple char for the command
					// Receive an char
					if ( MONITOR_CommandActualLength < MONITOR_MAX_COMMAND_LENGTH )	// shorted than max length?
					{
						if ( MONITOR_CommandCursorPosition == MONITOR_CommandActualLength )
						{
							// CursorPosition = CommandLength		(end character)
							MONITOR_CommandActual[MONITOR_CommandActualLength] = USART_ReceivedChar;
							MONITOR_CommandActualLength++;
							MONITOR_CommandCursorPosition++;
							if (MONITOR_CommandSendBackCharEnable)
							{
								// TODO: Cserélni
								USART_SendChar( USART_ReceivedChar );
							}
						}
						else
						{
							// CursorPosition < CommandLength		(inner character)
							MONITOR_CommandActualLength++;
							// Copy
							uint8_t i;
							for ( i = MONITOR_CommandActualLength; i > MONITOR_CommandCursorPosition; i-- )
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
bool MONITOR_PrepareFindExecuteCommand ( CommProtocol_t source )
{

	MONITOR_CommandSource = source;

	// Separate command
	COMMAND_ArgCount = MONITOR_CommandParser();
	bool isSuccessful = false;

	if (COMMAND_ArgCount > 0)
	{
		// Find and execute the command
		isSuccessful = MONITOR_SearchCommand();
		MONITOR_SEND_NEW_LINE();
	}
	else
	{
		// Cannot separated, this is not a command
		isSuccessful = false;
	}

	
	// Init new command
	MONITOR_SEND_PROMT();

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
static bool MONITOR_SearchCommand ( void )
{

	CommandID_t i;
	bool CommandValid = false;

	// Search the command
	for (i=0; i < MONITOR_CommandNum; i++)
	{

		if (!StrCmp(COMMAND_Arguments[0],CommandList[i].name))
		{
			// Found the command
			MONITOR_RunCommand(i);

			// Valid Command
			CommandValid = true;
			break;
		}
	}

	// Is valid command?
	if (!CommandValid)
	{
		// Error, wrong command
		MONITOR_SendMessage("Unknown Command\r\n"
				"Type \"help\" for help\r\n");
	}

	// Return with validation
	return !CommandValid;

}



/**
\brief			Command's letter deleting (backspace)
*/
void MONITOR_CommandBackspace ( void )
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

			// Delete this line
			MONITOR_SendMessage(USART_ESCAPE_DELETELINE);

			// Save the cursor ( we need backup this, because the user's cursor is stand this position )
			MONITOR_SendMessage(USART_ESCAPE_SAVECURSOR);

			// Cursor to line start -> we want write the "CommandActual"
			MONITOR_SendMessage(USART_ESCAPE_CURSOR_TO_LINESTART);
			MONITOR_SendMessage(USART_ESCAPE_CURSORLEFTLOTOF);

			// Write new CommandActual
			MONITOR_SendMessage("# %s",MONITOR_CommandActual);

			// restore the position
			MONITOR_SendMessage(USART_ESCAPE_RESTORECURSOR);

#endif

		}
#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
		else
		{
			// CursorPosition != CommandLength, we are in command chars
			// Cursor not at end

			uint8_t i;

			if ( MONITOR_CommandCursorPosition > 0 )
			{
				// not at 0 position

				// Procedure:
				// copy CommandActual
				// delete line
				// save cursor
				// send new command
				// restore cursor

				MONITOR_CommandActualLength--;
				MONITOR_CommandCursorPosition--;

				for ( i = MONITOR_CommandCursorPosition; i < MONITOR_CommandActualLength; i++ )
				{
					MONITOR_CommandActual[i] = MONITOR_CommandActual[i+1];		// copy
				}
				MONITOR_CommandActual[i] = '\0';

				// TODO: Egy függvénybe rakni az újra kiküldést?

				// Send backspace = step left
				USART_SEND_KEY_BACKSPACE();

				// Delete this line
				MONITOR_SendMessage(USART_ESCAPE_DELETELINE);

				// Save the cursor ( we need backup this, because the user's cursor is stand this position
				MONITOR_SendMessage(USART_ESCAPE_SAVECURSOR);

				// Cursor to line start -> we want write the "CommandActual"
				MONITOR_SendMessage(USART_ESCAPE_CURSOR_TO_LINESTART);
				MONITOR_SendMessage(USART_ESCAPE_CURSORLEFTLOTOF);

				// Write new CommandActual
				duprintf(MONITOR_CommandSource, "# %s",MONITOR_CommandActual);

				// Restore the position
				MONITOR_SendMessage(USART_ESCAPE_RESTORECURSOR);
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
static void MONITOR_CommandDelete ( void )
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
				// copy CommandActual after cursor
				// delete line
				// save cursor
				// send new command
				// restore cursor

				MONITOR_CommandActualLength--;

				for (i = MONITOR_CommandCursorPosition; i < MONITOR_CommandActualLength; i++)
				{
					MONITOR_CommandActual[i] = MONITOR_CommandActual[i+1];		// copy
				}
				MONITOR_CommandActual[i] = '\0';

				// Resend line
				MONITOR_CommandResendLine();
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
static void MONITOR_CommandTabulator ( void )
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

			MONITOR_NewCommandResendLine();

			return;
		}
	}
}
#endif // #ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE



#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
/**
\brief		Resend the actual line/command
*/
void MONITOR_CommandResendLine ( void )
{

	// Delete the line
	MONITOR_SendMessage(USART_ESCAPE_DELETELINE);

	// Save cursor
	MONITOR_SendMessage(USART_ESCAPE_SAVECURSOR);

	// Cursor to line start
	MONITOR_SendMessage(USART_ESCAPE_CURSOR_TO_LINESTART);
	MONITOR_SendMessage(USART_ESCAPE_CURSORLEFTLOTOF);

	// Write new CommandActual
	duprintf(MONITOR_CommandSource, "# %s",MONITOR_CommandActual);

	// Restore the position
	MONITOR_SendMessage(USART_ESCAPE_RESTORECURSOR);

	return;
}
#endif	// #ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE



#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
/**
\brief		Resend an new line/command
*/
void MONITOR_NewCommandResendLine ( void )
{

	// Delete the line
	MONITOR_SendMessage(USART_ESCAPE_DELETELINE);

	// Cursor to line start
	MONITOR_SendMessage(USART_ESCAPE_CURSOR_TO_LINESTART);
	MONITOR_SendMessage(USART_ESCAPE_CURSORLEFTLOTOF);

	// Write new CommandActual
	duprintf(MONITOR_CommandSource, "# %s",MONITOR_CommandActual);

	return;
}
#endif	// #ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE



#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
/**
\brief		Process Excape sequence
*/
bool MONITOR_CommandEscapeCharValidation ( void )
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
					MONITOR_SendMessage(USART_ESCAPE_CURSORRIGHT);
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
					MONITOR_SendMessage(USART_ESCAPE_CURSORLEFT);
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
void MONITOR_HISTORY_Save ( void )
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
bool MONITOR_HISTORY_FindInHistoryList ( void )
{
	uint8_t i;
	
	for ( i = 0; i < MONITOR_HISTORY_MAX_COUNT; i++ )
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
void MONITOR_HISTORY_Load ( uint8_t direction )
{

	// down cursor
	if ( direction == 0 ) // direction == 0
	{
		if ( MONITOR_HISTORY_Load_cnt >= ( MONITOR_HISTORY_MAX_COUNT-1 ) )
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
	StrCpy ( (char *)MONITOR_CommandActual, (const char *)MONITOR_HISTORY[MONITOR_HISTORY_Load_cnt]);

	// cursor, length!
	MONITOR_CommandCursorPosition = StringLength((const char *)MONITOR_CommandActual);
	MONITOR_CommandActualLength = MONITOR_CommandCursorPosition;

	MONITOR_NewCommandResendLine ();

	// Step load cnt
	if ( direction == 1 ) // direction == 0
	{
		if ( MONITOR_HISTORY_Load_cnt <= 0 )
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
\brief		Convert MONITOR_CommandActual (Actual command) to small letters
*/
void MONITOR_ConvertSmallLetter ( void )
{
	uint8_t i;
	
	for ( i = 0; MONITOR_CommandActual[i] != '\0'; i++ )
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
void MONITOR_CheckResultAndRespond (CommandResult_t result)
{
	switch (result)
	{
		case CommandResult_Unknown:
			MONITOR_SendLine("Unknown error");
			break;
		case CommandResult_Ok:
			// Not need response
			break;
		case CommandResult_Error_WrongArgument1:
			MONITOR_SendLine("Wrong argument (1.)");
			break;
		case CommandResult_Error_WrongArgument2:
			MONITOR_SendLine("Wrong argument (2.)");
			break;
		case CommandResult_Error_TooFewArgument:
			MONITOR_SendLine("Too few argument");
			break;
		case CommandResult_Error_TooManyArgument:
			MONITOR_SendLine("Too many argument");
			break;
		case CommandResult_Error_CommandArgNumIsWrong:
			MONITOR_SendLine("Command set is wrong");
			break;
		case CommandResult_Error_Unknown:
			MONITOR_SendLine("Unknown error");
			break;
		default:
			MONITOR_SendLine("Unknown command process");
			break;
	}
}



/**
 * \brief	Run command
 */
void MONITOR_RunCommand ( CommandID_t commandID )
{

	uint32_t result = CommandResult_Ok;
	bool needWriteHelp = false;

	// Check argument nums
	result = MONITOR_ArgumentNumIsGood(COMMAND_ArgCount, CommandList[commandID].commandArgNum);

	if (result == CommandResult_Ok)
	{
		// Good

		FunctionPointer thisFunction = ( FunctionPointer )CommandList[commandID].commandFunctionPointer;

		// Execute the command function
		result = thisFunction(COMMAND_ArgCount,(char**)COMMAND_Arguments);
	}
	else
	{
		// if not Ok, write help
		needWriteHelp = true;
	}



	// Write result if need
	MONITOR_CheckResultAndRespond(result);

	if (needWriteHelp)
	{
		MONITOR_WriteCommandHelp (commandID);
	}

}



/**
 * \brief	Write a command help
 */
void MONITOR_WriteCommandHelp ( CommandID_t commandID )
{

	duprintf(MONITOR_CommandSource,
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
CommandResult_t MONITOR_ArgumentNumIsGood ( uint8_t receivedArgNum, uint8_t commandArgNum)
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
	// TODO: használni egy közös függvényt a duprintf()-essel
	//return duprintf(MONITOR_CommandSource, message);
	switch (MONITOR_CommandSource)
	{
		case Source_DebugUart:
			USART_SendMessage(message);
			break;

		default:
			// TODO: átrakni máshova?
			USART_SendMessage(message);
			break;
	}
}



/**
 * \brief	Send line (with newline)
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
	/// TODO:
	switch (MONITOR_CommandSource)
	{
		case Source_DebugUart:
			USART_SendChar(c);
			break;

		default:
			// TODO: átrakni máshova?
			USART_SendChar(c);
			break;
	}
}



/**
 * \brief	Send CLS
 */
void MONITOR_SendCls(void)
{
	MONITOR_SendMessage(USART_ESCAPE_ERASE_CLS);
	MONITOR_SendMessage(USART_ESCAPE_CURSOR_TOPLEFT);
}



#ifdef MONITOR_GET_PASSWORD_ENABLE
/**
 * \brief Get (and wait) Password
 */
static void MONITOR_GetPassword ( void )
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

				// TODO: Don't know, why work...
				USART_ReceivedChar = USART_RxBuffer[USART_RxBufferReadCnt-1];
				USART_RxBufferReadCnt++;
				USART_SendChar('*');

				if (USART_ReceivedChar == '\r')
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
static bool MONITOR_CheckPassword (const char *string)
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
