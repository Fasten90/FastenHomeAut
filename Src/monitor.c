/*
 * monitor.c
 *
 *		Created on:		2013.10.08.
 *    	Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		University:		BME (BUTE)
 *		Project: 		RadioAlarm - Radio Alarm System modules
 *		Function:		Monitor program, communication with UART, execute commands
 *		Target:			STM32F107RC
 *		Version:		v3
 *		Last modified:	2014.08.07
 */




#include "include.h"
#include "stdint.h"
#include "command.h"

#include "monitor.h"

	


///////////////////////////// MONITOR



/////////////////////////////
// GLOBAL VARIABLES
// FUNCTIONS
// COMMAND FUNCTIONS
/////////////////////////////





// GLOBAL VARIABLES


volatile char MONITOR_CommandActual[MONITOR_MAX_COMMAND_LENGTH];
volatile char MONITOR_CommandActualEscape[3];

volatile uint8_t USART_RxBufferReadCnt = 0;

volatile uint8_t MONITOR_CommandEnable = 0;
volatile uint8_t MONITOR_CommandActualLength = 0;
volatile uint8_t MONITOR_CommandSentLength = 0;
volatile uint8_t MONITOR_CommandCursorPosition;
volatile uint8_t MONITOR_CommandEvent;
volatile uint8_t MONITOR_CommandReceivedLastChar;
volatile uint8_t MONITOR_CommandReceivedNotLastChar;
volatile uint8_t MONITOR_CommandReadable;
volatile uint8_t MONITOR_CommandEscapeSequenceReceived;
volatile uint8_t MONITOR_CommandEscapeSequenceInProgress;
volatile uint8_t MONITOR_CommandEscape_cnt;
volatile uint8_t MONITOR_CommandReceivedBackspace;
volatile uint8_t MONITOR_CommandSendBackChar_Enable = 0;


#ifdef CONFIG_USE_FREERTOS
xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
#endif


#ifdef USE_MONITOR_HISTORY
uint8_t MONITOR_HISTORY_Save_cnt = MONITOR_MAX_HISTORY_LENGTH-1;
uint8_t MONITOR_HISTORY_Load_cnt = MONITOR_MAX_HISTORY_LENGTH-1;
char MONITOR_HISTORY[MONITOR_MAX_HISTORY_LENGTH][MONITOR_MAX_COMMAND_LENGTH] =
{

#if defined(USE_MONITOR_HISTORY) && (MONITOR_MAX_HISTORY_LENGTH > 0)

	#if ( MONITOR_MAX_HISTORY_LENGTH > 0 )
	{
		"help"
	},
	#endif
	#if ( MONITOR_MAX_HISTORY_LENGTH > 1 )
	{
		"led 1 1"
	},
	#endif
	#if ( MONITOR_MAX_HISTORY_LENGTH > 2 )
	{
		"cls"
	},
	#endif
	#if ( MONITOR_MAX_HISTORY_LENGTH > 3 )
	{
		"test"
	},
	#endif
	#if ( MONITOR_MAX_HISTORY_LENGTH > 4 )
	{
		"test"
	}
	#endif
	#if ( MONITOR_MAX_HISTORY_LENGTH > 5 )
	#warning isnt set monitor history commands
	#endif
#elif  (!MONITOR_MAX_HISTORY_LENGTH)
#error USE_MONITOR_HISTORY define is defined, but MONITOR_MAX_HISTORY_LENGTH define is not set valid value.
#endif
};
#endif


///////////////////////////// FUNCTION PROTOTYPES

static void ProcessReceivedCharacter(void);


///////////////////////////// FUNCTIONS



// Function: Init Monitor program
// TODO: kiszedve a kikuldes, mert faultot okoz, ha nincs kapcsolat
uint8_t MONITOR_Init ( void ) {

	// INIT									// Init CommandActual

	MONITOR_CommandEvent = 0;
	MONITOR_CommandActualLength = 0;
	MONITOR_CommandSentLength = 0;
	MONITOR_CommandCursorPosition = 0;
	MONITOR_CommandEscapeSequenceReceived = 0;
	MONITOR_CommandEscapeSequenceInProgress = 0;
	MONITOR_CommandReceivedLastChar = 0;
	MONITOR_CommandReceivedNotLastChar = 0;
	MONITOR_CommandEscape_cnt = 0;
	MONITOR_CommandSendBackChar_Enable = 1;	// enable
	USART_TxBuffer[TXBUFFERSIZE-1] = '\0';


	// END OF INIT
	
	// !!IMPORTANT!! Nem szabad kikuldeni uzenetet a kapcsolat letrejotte elott, mert faultba juthatunk.
	
	
	return RETURN_SUCCESS;
}



uint8_t MONITOR_SendWelcome ( void )
{


	//USART_SEND_DUMMYSTRING();

	#ifdef CONFIG_USE_FREERTOS
	vTaskDelay(1);
	#else
	HAL_Delay(1);
	#endif


//	USART_ESCAPE_BACKGROUND_WHITE();		// background: white
//	//USART_ESCAPE_BACKGROUND_DEFAULT();	// Note: White in Hyperterminal is grey colour
//	USART_ESCAPE_TEXT_BLACK();				// text: black

	
	USART_ESCAPE_BACKGROUND_DEFAULT();

	
	USART_SEND_CLS();						// Clean screen
	MONITOR_SEND_WELCOME();					// Üdvözlo üzenet
	USART_SEND_NEW_LINE();					// Új sor

	// USART_ESCAPE_BACKGROUND_DEFAULT()
	// USART_ESCAPE_RESET()


	//USART_SendStringWithBackgroundColor("Színes háttér!");
	USART_SEND_NEW_LINE();

	USART_ESCAPE_TEXT_BLACK();				// Teszt üzenet
	USART_ESCAPE_BACKGROUND_GREEN();
	USART_SendString("Example VALID message.\r\n");

	USART_ESCAPE_BACKGROUND_RED();			// Teszt üzenet
	USART_SendString("Example INVALID message.\r\n");

	USART_ESCAPE_BACKGROUND_WHITE();		// Fehér háttérszín


	USART_SEND_NEW_LINE();
	MONITOR_SEND_PROMT();					// Új promt

	return RETURN_SUCCESS;
}



uint8_t MONITOR_SendPrimitiveWelcome ( void )
{


	#ifdef CONFIG_USE_FREERTOS
	vTaskDelay(1);
	#else
	HAL_Delay(1);
	#endif

	
	USART_SEND_CLS();						// Clean screen
	MONITOR_SEND_WELCOME();					// Welcome message


	USART_SEND_NEW_LINE();
	MONITOR_SEND_PROMT();					// New promt

	return RETURN_SUCCESS;
	
}



// Function: Always run, wait command and execute it
uint8_t MONITOR_CheckCommand ( void ) {

	// Initialize

	//char CommandActual[MONITOR_MAX_COMMAND_LENGTH];	// TODO: kiszedni, ki kell az EndCommand-bol is

	
	// TODO: glob�liss� ttenni oket
	unsigned int argc;
	char *argv[3];

	char CommandArg1[MONITOR_MAX_ARG_LENGTH];
	char CommandArg2[MONITOR_MAX_ARG_LENGTH];
	char CommandArg3[MONITOR_MAX_ARG_LENGTH];

	argv[0] = CommandArg1;
	argv[1] = CommandArg2;
	argv[2] = CommandArg3;


#if RXBUFFERSIZE != 256
#warning "Ring buffer counter error"
#endif

	//MONITOR_InitMonitor();	// Init
		
	argc = 0;
	
	// Initialize - End

	// TODO: �talak�tani, jelenleg nincs �rtelme, mert RxBufferbe m�solunk
	MONITOR_CommandEnable = 1;	// !! IMPORTANT !! Last initialize
	
	
#ifdef CONFIG_USE_FREERTOS
	xSemaphoreGive(DEBUG_USART_Tx_Semaphore);
	// !!IMPORTANT!! Elso szemaforatadas, enged�lyezve az USART-on kiküldés
#endif
	//HAL_UART_Receive_IT(&BluetoothUartHandle, (uint8_t *)USART_RxBuffer, RXBUFFERSIZE);	// TODO: VG - USART - TEST - uzenetvaras

	
	
	// Welcome message
	MONITOR_SendPrimitiveWelcome();
	

#ifdef CONFIG_USE_FREERTOS
	vTaskDelay(10);

	xSemaphoreGive(DEBUG_USART_Tx_Semaphore);
	// Enable sendings
#endif


	// Start receive
	USART_ReceiveMessage();

	while (1)
	{
		// always checking the Command
		if ( MONITOR_CommandEnable )
		{

			// Check and process received characters
			ProcessReceivedCharacter();


			if ( MONITOR_CommandEvent )
			{
				MONITOR_CommandEvent = 0;


				if ( MONITOR_CommandReceivedNotLastChar )
				{
					// Nost Last char - Refresh the line
					MONITOR_CommandEnable = 0;
					MONITOR_CommandReceivedNotLastChar = 0;
					MONITOR_CommandResendLine();
					MONITOR_CommandEnable = 1;
				}
				else if ( MONITOR_CommandEscapeSequenceReceived )
				{
					// Escape sequence
					MONITOR_CommandEnable = 0;
					MONITOR_CommandEscapeSequenceReceived = 0;
					MONITOR_CommandEscapeCharValidation ();
					MONITOR_CommandEnable = 1;
				}
				else if ( MONITOR_CommandReceivedBackspace )
				{
					// Backspace
					MONITOR_CommandEnable = 0;
					MONITOR_CommandReceivedBackspace = 0;
					MONITOR_CommandBackspace();
					MONITOR_CommandEnable = 1;
				}
				else if ( MONITOR_CommandReadable )
				{
					// Pressed Enter, EndCommand();
					MONITOR_CommandEnable = 0;			// Disable
					MONITOR_CommandReadable = 0;
					if ( MONITOR_CommandActualLength > 0)
					{
						// There are some char in the line
						// has an command
						MONITOR_ConvertSmallLetter();
						
						USART_SEND_NEW_LINE();
						
						// TODO: 1. parametert kiszedni
						MONITOR_EndCommand ( (char *)MONITOR_CommandActual, argc, argv );	// Parancs megkeresése és futtatási kísérlet		
						
						#ifdef USE_MONITOR_HISTORY
						MONITOR_HISTORY_Save ();											// History-ba lementés
						#endif
					}
					else {																	// There is no char in the line	
						//USART_SEND_NEW_LINE();											
						MONITOR_SEND_PROMT();
					}
					MONITOR_CommandActualLength = 0;
					MONITOR_CommandSentLength = 0;
					MONITOR_CommandCursorPosition = 0;
					MONITOR_CommandEnable = 1;
				}
			}
		}
	}
	
	// Infinite loop, never exit, never reached here.

}



static void ProcessReceivedCharacter(void)
{

	while (USART_RxBufferReadCnt!=USART_RxBufferWriteCounter)
	{
		volatile char USART_ReceivedChar = '\0';

		// TODO: Don't know, why work...
		USART_ReceivedChar = USART_RxBuffer[USART_RxBufferReadCnt-1];
		USART_RxBufferReadCnt++;

		// ESCAPE SEQUENCE
		if ( MONITOR_CommandEscapeSequenceInProgress == 1 )
		{
			// Escape sequence in progress
			// Copy escape characters to MONITOR_CommandActualEscape[]

			if (MONITOR_CommandEscape_cnt == 1)
			{
				if (USART_ReceivedChar == '[')
				{
					MONITOR_CommandActualEscape[MONITOR_CommandEscape_cnt++] = USART_ReceivedChar;
				}
				else
				{
					// Wrong escape sequence
					MONITOR_CommandEscapeSequenceInProgress = 0;
					MONITOR_CommandEscape_cnt = 0;
				}
			}
			else if ( MONITOR_CommandEscape_cnt == 2)
			{
				// TODO: only work with escape sequence if 3 chars (ESC[A)
				MONITOR_CommandActualEscape[MONITOR_CommandEscape_cnt++] = USART_ReceivedChar;
				MONITOR_CommandEscapeSequenceInProgress = 0;
				MONITOR_CommandEscapeSequenceReceived = 1;
				MONITOR_CommandEvent = 1;
				return;
			}

		}
		else
		{
			// No escape sequence
			// An character received

			if ( USART_ReceivedChar  == '\x1B')	// 'ESC'
			{
				// receive an Escape sequence
				MONITOR_CommandEscapeSequenceInProgress = 1;
				MONITOR_CommandActualEscape[0] = USART_ReceivedChar;
				MONITOR_CommandEscape_cnt = 1;
				//MONITOR_CommandEvent = 1;
			}
			else
			{
				if ( (USART_ReceivedChar  == '\r') || (USART_ReceivedChar == '\n') ||
					(USART_ReceivedChar == '\0'))
				{		// receive Enter
					MONITOR_CommandReadable = 1;
					MONITOR_CommandActual[MONITOR_CommandActualLength] = '\0';
					MONITOR_CommandEvent = 1;
					return;
				}
				else if ( USART_ReceivedChar  == USART_KEY_DELETE )
				{
					// In real world this is backspace	// PuTTy vs ZOC
					MONITOR_CommandReceivedBackspace = 1;
					MONITOR_CommandEvent = 1;
					return;
				}
				else
				{
					// simple char for the command
					// Receive an char
					if ( MONITOR_CommandActualLength < MONITOR_MAX_COMMAND_LENGTH )	// shorted than max length?
					{
						if ( MONITOR_CommandCursorPosition == MONITOR_CommandActualLength )
						{	// CursorPosition = CommandLength		(end character)
							MONITOR_CommandActual[MONITOR_CommandActualLength] = USART_ReceivedChar;
							MONITOR_CommandActualLength++;
							MONITOR_CommandCursorPosition++;
							MONITOR_CommandReceivedLastChar = 1;
							USART_SendChar( USART_ReceivedChar );
						}
						else
						{
							// CursorPosition < CommandLength		(inner character)
							MONITOR_CommandActualLength++;
							// Copy
							for ( uint8_t i = MONITOR_CommandActualLength; i > MONITOR_CommandCursorPosition; i-- )
							{
								MONITOR_CommandActual[i] = MONITOR_CommandActual[i-1];
							}
							MONITOR_CommandActual [MONITOR_CommandCursorPosition] = USART_ReceivedChar;
							MONITOR_CommandActual [MONITOR_CommandActualLength] = '\0';
							MONITOR_CommandCursorPosition++;
							MONITOR_CommandReceivedNotLastChar = 1;
							MONITOR_CommandEvent = 1;
						}
					}
					else
					{
						// longer than max length ...
						// Do not copy/print...
					}

				}
			}

		} // Processed received characters

		// Transmission end semaphore / flag
		// Szemafor beallitasa:
		#ifdef CONFIG_USE_FREERTOS
		//xSemaphoreGive(DEBUG_Rx_Semaphore); // !! IMPORTANT !! ISR-bol nem szabad hasznalni!
		xSemaphoreGiveFromISR(DEBUG_USART_Rx_Semaphore,0);
		#endif
		//MONITOR_CommandEvent = 1;

	} // End of while

}


// Function: Prepare (Separate) the command and Find and Run it...
uint8_t MONITOR_EndCommand ( char *CommandActual, unsigned int argc, char** argv  ) {

	// Separate command
	// CommandActual = MONITOR_CommandActual[]
	argc = MONITOR_CommandSeparate ( argv, CommandActual );

	if ( argc ) {
		
		MONITOR_CommandFind ( argc, argv );		// Find and probing execute the command
		USART_SEND_NEW_LINE();
	}
	//else USART_SEND_NEW_LINE();				// argc == 0

	
	// Init new command
	MONITOR_SEND_PROMT();

	return RETURN_SUCCESS;
}




// Function: Seperate words (command) to command word, and 2 argument
// from char *CommandActul, to argv[0], [1], [2]
uint8_t MONITOR_CommandSeparate ( char** argv, char *CommandActual ) {
	// seperate command to argumentums
	// return arc; = argumentum's num
	// call from void MonitorWaitCommand(void);

	int i;
	int j = 0;
	uint8_t CommandArgCount = 1;	// 1-3	// Arguments num is 1, when we start separate


	for (i=0; CommandActual[i]!='\0'; i++) {
		if ( CommandActual[i] != ' '  ) {						// Not space, copy char to  Argument (CommandArgX[])
			argv[CommandArgCount-1][j] = CommandActual[i];
			j++;
		}
		else {													// It's space or the end, skip the space
			argv[CommandArgCount-1][j] = '\0';
			CommandArgCount++;
			j = 0;
			if ( CommandArgCount >3) { uprintf("Too many arguments!\r\n"); return 0; }
		}
		if ( j >= MONITOR_MAX_ARG_LENGTH ) { uprintf("Too long argument!\r\n"); return 0;}
	} // End of copies

	argv[CommandArgCount-1][j] = '\0';							// Last argument's end

	if ( CommandArgCount < 3 ) argv[2][0] = '\0';				// 3. argument is empty
	if ( CommandArgCount < 2 ) argv[1][0] = '\0';				// 2. argument is empty

	return CommandArgCount;
}



// Function: Find the command
uint8_t MONITOR_CommandFind( unsigned int argc, char** argv ) {

	uint8_t i;
	uint8_t CommandValid = 0;
	FunctionPointer	thisFunction;
	uint8_t commandNum = COMMAND_GetCommandNum();

	
	// Find the command
	//for (i=0; i < MONITOR_MAX_COMMAND_NUM; i++) {		// Need an correct "MAX_COMMAND_NUM" define
	//for (i=0; CommandList[i].name != NULL; i++) {		// If last command = 0
	for (i=0; i < commandNum; i++) {					// get the command.c "MAX_COMMAND_NUM" define
		if (!StrCmp(argv[0],CommandList[i].name)) {						// Found the command
			thisFunction = ( FunctionPointer )CommandList[i].CommandFunctionPointer;	// execute the command function
			thisFunction(argc,argv);
			CommandValid = 1;											// Valid Command
			break;
		}
	}

	// Valid command or not?
	if (!CommandValid) {
		uprintf("Unknown Command.\r\n");	// Has valid command? If not, throw error text
		return RETURN_SUCCESS;
	}
	else {
		return RETURN_FALSE;				// Valid command
	}

}



/*
\brief			Command's letter deleting (backspace)
*/
uint8_t MONITOR_CommandBackspace ( void ) {
	uint8_t i;

	if (MONITOR_CommandActualLength > 0) {

		if ( MONITOR_CommandCursorPosition == MONITOR_CommandActualLength) {
			MONITOR_CommandActual[--MONITOR_CommandCursorPosition] = '\0';	// del from CommandActual, and Position--
			MONITOR_CommandActualLength--;
			
#ifdef CONFIG_USE_TERMINAL_ZOC
			USART_SEND_KEY_BACKSPACE();
			USART_SEND_KEY_DEL();
			USART_SEND_KEY_BACKSPACE();
#endif		
			
#ifdef CONFIG_USE_TERMINAL_PUTTY
			
			// v1
			USART_SEND_KEY_DEL();
			
			// v2
			//USART_SEND_KEY_DEL();
			//USART_SendChar(' ');
			//USART_SEND_KEY_DEL();
			
			// v3
			//USART_SEND_KEY_DEL();
			//USART_SEND_KEY_DEL();
			//USART_SendChar(MONITOR_CommandActual[MONITOR_CommandCursorPosition-1]);
			
			// v4
			//USART_SEND_KEY_DEL();
			//USART_SendChar(' ');
			//USART_SEND_KEY_BACKSPACE();
			
			// v5
			//USART_SEND_KEY_BACKSPACE();
			//USART_SendChar(' ');
			//USART_SEND_KEY_BACKSPACE();
			
			// v6, left, space, left
			//USART_ESCAPE_CURSORLEFT();
			//USART_SendChar(' ');
			//USART_ESCAPE_CURSORLEFT();
			
			// TODO: WRONG!! After backspace an char has been lost (but write to buffer)
			
			/*
			// Delete this line
			USART_ESCAPE_DELETELINE();

			// Save the cursor ( we need backup this, because the user's cursor is stand this position )
			USART_ESCAPE_SAVECURSOR();

			// Cursor to line start -> we want write the "CommandActual"
			USART_ESCAPE_CURSOR_TO_LINESTART();
			USART_ESCAPE_CURSORLEFTLOTOF();

			// Write new CommandActual
			uprintf("# %s",MONITOR_CommandActual);
			*/
#endif
			
#ifdef CONFIG_USE_TERMINAL_HYPERTERMINAL
			
			USART_SEND_KEY_BACKSPACE();

			// Delete this line
			USART_ESCAPE_DELETELINE();

			// Save the cursor ( we need backup this, because the user's cursor is stand this position )
			USART_ESCAPE_SAVECURSOR();

			// Cursor to line start -> we want write the "CommandActual"
			USART_ESCAPE_CURSOR_TO_LINESTART();
			USART_ESCAPE_CURSORLEFTLOTOF();

			// Write new CommandActual
			uprintf("# %s",MONITOR_CommandActual);

			// restore the position
			USART_ESCAPE_RESTORECURSOR();

#endif

		}
		else {										// CursorPosition != CommandLength, we are in command chars

			if ( MONITOR_CommandCursorPosition > 0 ) {			// not at 0 position

				// Procedure:
				// copy CommandActual
				// delete line
				// save cursor
				// send new command
				// restore cursor

				MONITOR_CommandActualLength--;
				MONITOR_CommandCursorPosition--;

				for ( i = MONITOR_CommandCursorPosition; i < MONITOR_CommandActualLength; i++ ) {
					MONITOR_CommandActual[i] = MONITOR_CommandActual[i+1];		// copy
				}
				MONITOR_CommandActual[i] = '\0';

				// Send backspace
				USART_SEND_KEY_BACKSPACE();

				// Delete this line
				USART_ESCAPE_DELETELINE();

				// Save the cursor ( we need backup this, because the user's cursor is stand this position
				USART_ESCAPE_SAVECURSOR();

				// Cursor to line start -> we want write the "CommandActual"
				USART_ESCAPE_CURSOR_TO_LINESTART();
				USART_ESCAPE_CURSORLEFTLOTOF();

				// Write new CommandActual
				uprintf("# %s",MONITOR_CommandActual);

				// restore the position
				USART_ESCAPE_RESTORECURSOR();
			}
		}
	}
	return RETURN_SUCCESS;			// not do anything
}



/*
\brief		Resend the actual line/command
*/
uint8_t MONITOR_CommandResendLine ( void ) {

	//step right
	USART_ESCAPE_CURSORRIGHT();

	// Delete the line
	USART_ESCAPE_DELETELINE();

	// Save cursor
	USART_ESCAPE_SAVECURSOR();

	// Cursor to line start
	USART_ESCAPE_CURSOR_TO_LINESTART();
	USART_ESCAPE_CURSORLEFTLOTOF();

	// Write new CommandActual
	uprintf("# %s",MONITOR_CommandActual);

	// Restore the position
	USART_ESCAPE_RESTORECURSOR();

	return RETURN_SUCCESS;
}



/*
\brief		Resend an new line/command
*/
uint8_t MONITOR_NewCommandResendLine ( void ) {

	// Delete the line
	USART_ESCAPE_DELETELINE();

	// Cursor to line start
	USART_ESCAPE_CURSOR_TO_LINESTART();
	USART_ESCAPE_CURSORLEFTLOTOF();

	// Write new CommandActual
	uprintf("# %s",MONITOR_CommandActual);

	return RETURN_SUCCESS;
}



/*
\brief		Process Excape sequence
*/
uint8_t MONITOR_CommandEscapeCharValidation ( void ) {
	// return valid char, or 0 if invalid
	// work with ANSI escape codes

	if (MONITOR_CommandActualEscape[0] == '\x1B') { 								// ESC
		if ( MONITOR_CommandActualEscape[1] == '[' )							// '[', escape sequence 2. letter
		{
			// This is an escape sequence
			if ( MONITOR_CommandActualEscape[2] == 'A' )						// Up cursor		// previous History command
			{
				#ifdef USE_MONITOR_HISTORY
				MONITOR_HISTORY_Load ( 1 );
				#endif
				return RETURN_SUCCESS;
			}
			else if ( MONITOR_CommandActualEscape[2] == 'B' )					// Down cursor		// next History command
			{
				#ifdef USE_MONITOR_HISTORY
				MONITOR_HISTORY_Load ( 0 );
				#endif
				return RETURN_SUCCESS;
			}
			else if (  MONITOR_CommandActualEscape[2] == 'C' )					// Right cursor
			{
				if ( MONITOR_CommandCursorPosition < MONITOR_CommandActualLength )	// if not at end
				{
					USART_ESCAPE_CURSORRIGHT();
					MONITOR_CommandCursorPosition++;
					return RETURN_SUCCESS;
				}
				else return RETURN_SUCCESS;								// not do anything
			}
			else if (  MONITOR_CommandActualEscape[2] == 'D' )	{		// Left cursor
				if ( MONITOR_CommandCursorPosition > 0 )				// if not at start
				{
					USART_ESCAPE_CURSORLEFT();
					MONITOR_CommandCursorPosition--;
					return RETURN_SUCCESS;
				}
				else return RETURN_SUCCESS;								// not do anything
			}
		}
		else	// This is not escape sequence
		{
			return RETURN_FALSE;
		}
	}
	else
	{
		return RETURN_FALSE;
	}

	return RETURN_FALSE;

}



#ifdef USE_MONITOR_HISTORY
uint8_t MONITOR_HISTORY_Save ( void )
{

	if ( MONITOR_HISTORY_IsInIt() == RETURN_SUCCESS )	// Ha van egyezo
	{
		return RETURN_SUCCESS;
	}

	if ( MONITOR_HISTORY_Save_cnt >= ( MONITOR_MAX_HISTORY_LENGTH-1 ) )
	{
		MONITOR_HISTORY_Save_cnt = 0;
	}
	else
	{
		MONITOR_HISTORY_Save_cnt++;
	}
	MONITOR_HISTORY_Load_cnt = MONITOR_HISTORY_Save_cnt;	// amit most gepeltunk be, az az utolso


	StrCpy ( MONITOR_HISTORY[MONITOR_HISTORY_Save_cnt], (char *)MONITOR_CommandActual );


	return RETURN_SUCCESS;

}
#endif



// Function: van-e a history-ban
// return RETURN_SUCCESS, ha van megegyezo
// return RETURN_FALSE, ha nincs megegyezo
#ifdef USE_MONITOR_HISTORY
uint8_t MONITOR_HISTORY_IsInIt ( void )
{
	int i;
	
	for ( i = 0; i < MONITOR_MAX_HISTORY_LENGTH; i++ )
	{
		if ( !StrCmp((const char *)MONITOR_HISTORY[i],(const char * )MONITOR_CommandActual))	// If it is equal
		{
			return RETURN_SUCCESS; // van megegyezo
		}
	}

	return RETURN_FALSE; // nincs megegyezo

}
#endif



#ifdef USE_MONITOR_HISTORY
uint8_t MONITOR_HISTORY_Load ( uint8_t direction )
{

	// down cursor
	if ( direction == 0 ) // direction == 0
	{
		if ( MONITOR_HISTORY_Load_cnt >= ( MONITOR_MAX_HISTORY_LENGTH-1 ) )
		{
			MONITOR_HISTORY_Load_cnt = 0;
		}
		else
		{
			MONITOR_HISTORY_Load_cnt++;
		}

	}

	// up cursor
	// if direction == 1, masolhato azonnal az aktualis

	StrCpy ( (char *)MONITOR_CommandActual, (const char *)MONITOR_HISTORY[MONITOR_HISTORY_Load_cnt]);

	// cursor, length!
	MONITOR_CommandCursorPosition = StringLength((char *)MONITOR_CommandActual);
	MONITOR_CommandActualLength = MONITOR_CommandCursorPosition;

	MONITOR_NewCommandResendLine ();


	if ( direction == 1 ) // direction == 0
	{
		if ( MONITOR_HISTORY_Load_cnt <= 0 )
		{
			MONITOR_HISTORY_Load_cnt = MONITOR_MAX_HISTORY_LENGTH-1;
		}
		else
		{
			MONITOR_HISTORY_Load_cnt--;
		}

	}


	return RETURN_SUCCESS;

}
#endif



/*
\brief		MONITOR_Actual konvertálása kisbetusre...
*/
uint8_t MONITOR_ConvertSmallLetter( void )
{
	int i;
	
	for ( i = 0; MONITOR_CommandActual[i] != '\0'; i++ )
	{
		if ( ( MONITOR_CommandActual[i] > 'A' ) && ( MONITOR_CommandActual[i] < 'Z' ) )
		{	// need to change to small letter
			MONITOR_CommandActual[i] = MONITOR_CommandActual[i] - ( 'A' - 'a'); // length between Big Letter and small letter
		}
		//else OK;
	}

	return RETURN_SUCCESS;
	
}


