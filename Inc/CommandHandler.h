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
#include "command.h"


///////////////////////////// MONITOR


/////////////////////////////
// DEFINES
// TYPEDEFS
// EXTERNED GLOBAL VARIABLES
// EXTERNED PROTOTYPES
/////////////////////////////



/////////////////////////////
///			DEFINES
/////////////////////////////


///	Config defines


#define MONITOR_MAX_COMMAND_LENGTH			(255U)
#define MONITOR_COMMAND_ARG_MAX_COUNT		(3)
#define MONITOR_COMMAND_ARG_MAX_NUM_BITS	(0b111)
#define MONITOR_COMMAND_ARG_MAX_LENGTH		(80)
#define MONITOR_HISTORY_MAX_COUNT			(5)


#define MONITOR_WELCOME_DATE_VERSION	__DATE__
#define MONITOR_WELCOME_TIME_VERSION	__TIME__


#ifdef CONFIG_USE_PANEL_NODEMEDIUM
#define MONITOR_BOARD_TYPE		"Used panel: NodeMedium\r\n"
#endif
#ifdef CONFIG_USE_PANEL_CENTERPANEL
#define MONITOR_BOARD_TYPE		"Used panel: CenterPanel\r\n"
#endif
#ifdef CONFIG_USE_PANEL_DISCOVERY
#define MONITOR_BOARD_TYPE		"Used panel: STM32F4Discovery\r\n"
#endif


#define MONITOR_DUMMY_STRING				"____________________________________________\r\n"


#define MONITOR_SEND_WELCOME()								\
		USART_SendString("\r\n"								\
		"Welcome in Monitor program!\r\n"					\
		MONITOR_WELCOME_DATE_VERSION "\r\n"					\
		MONITOR_WELCOME_TIME_VERSION "\r\n"					\
		MONITOR_BOARD_TYPE									\
		"(c) Copyright, Vizi Gabor\r\n"						\
		"\r\n"												\
		"Try \"help\" command for help!\r\n")


#define MONITOR_SEND_PROMT()		\
		USART_SendString("\r\n# ")


#define USART_SendString(message)		USART_SendMessage((char *)message)


#define USART_SEND_DUMMYSTRING()		USART_SendString(MONITOR_DUMMY_STRING)


#define USART_SEND_NEW_LINE()			\
		USART_SendString("\r\n");


////////////////////////////////////  DEFINES FOR CHARACTERS

CONFIG_USE_TERMINAL_ZOC
#if defined(CONFIG_USE_TERMINAL_ZOC) || defined(CONFIG_USE_TERMINAL_PUTTY)
#define USART_KEY_DELETE			(0x08)
#define USART_KEY_BACKSPACE			(0x7F)
#else
#define USART_KEY_DELETE			(0x7F)
#define USART_KEY_BACKSPACE			(0x08)
#endif

// TODO: clean...
#define USART_KEY_ENTER				('\r')
#define USART_KEY_SPACE				(' ')
#define USART_KEY_ESCAPE			('\x1B')
#define USART_KEY_BELL				('\a')

// NOTE: Changed \e to \x1B
#define USART_ESCAPESEQUENCE_1		('\x1B')
#define USART_ESCAPESEQUENCE_2		('[')


////////////////////////////////////  BUTTONS


// 8 = BS = Backspace
#define USART_SEND_KEY_BACKSPACE()	\
		USART_SendChar(8)

// 127 = DEL = Delete
#define USART_SEND_KEY_DEL()		 \
		USART_SendChar(127)



////////////////////////////////////
//			ESCAPE SEQUENCE
////////////////////////////////////

// 27 = ESCAPE
// 91 = [

// Delete the line
// CSI n K	EL – Erase in Line	Erases part of the line.
// If n is zero (or missing), clear from cursor to the end of the line.
// If n is one, clear from cursor to beginning of the line.
// If n is two, clear entire line. Cursor position does not change.


// CSI 2 K: delete entire line
#define USART_ESCAPE_DELETELINE()	\
	USART_SendString("\x1B" "[2K")


// CSI n G	CHA – Cursor Horizontal Absolute	Moves the cursor to column n.
// CSI 1 G  -> go start of line
#define USART_ESCAPE_CURSOR_TO_LINESTART()	\
	USART_SendString("\x1B" "[1G")


// CSI n A	CUU – Cursor Up	Moves the cursor n (default 1) cells in the given direction.
// If the cursor is already at the edge of the screen, this has no effect.
// CSI n B	CUD – Cursor Down
// CSI n C	CUF – Cursor Forward
// CSI n D	CUB – Cursor Back
#define USART_ESCAPE_CURSORRIGHT()	\
	USART_SendString("\x1B" "[C")


// Cursor step left
#define USART_ESCAPE_CURSORLEFT()	\
	USART_SendString("\x1B" "[D")


// Cursor go left (loft of step)
#define USART_ESCAPE_CURSORLEFTLOTOF()	\
	USART_SendString("\x1B" "[100D")


// CURSOR SAVE/RESTORE
//CSI s	SCP – Save Cursor Position	Saves the cursor position.
#define USART_ESCAPE_SAVECURSOR()	\
	USART_SendString("\x1B" "[s")


//CSI u	RCP – Restore Cursor Position	Restores the cursor position.
#define USART_ESCAPE_RESTORECURSOR()	\
	USART_SendString("\x1B" "[u")


// Erase Functions:
// (CSI in UTF-8 is 0xC2, 0x9B) = ESC+[
// ESC[2J Clear screen and home cursor
#define USART_ESCAPE_ERASE_CLS()		\
	USART_SendString("\x1B" "[2J")


// Goto top left corner
// CSI n ; m H	CUP – Cursor Position	Moves the cursor to row n, column m. The values are 1-based, and default to 1 (top left corner) if omitted.
// A sequence such as CSI ;5H is a synonym for CSI 1;5H as well as CSI 17;H is the same as CSI 17H and CSI 17;1H
// n = 1 = row
// ;
// 1 = column
// H
#define USART_ESCAPE_CURSOR_TOPLEFT()	\
	USART_SendString("\x1B" "[1;1H")


////////////////////////////////////
//		ESCAPE SEQUENCE  - COLORS
////////////////////////////////////

// CSI n m 	SGR – Select Graphic Rendition
// 40–47 	Set background color 	40 + x, where x is from the color table below
#define USART_ESCAPE_BACKGROUND_GREEN() \
		USART_SendString("\x1B" "[42m")

#define USART_ESCAPE_BACKGROUND_RED()	\
		USART_SendString("\x1B" "[41m")

#define USART_ESCAPE_BACKGROUND_WHITE() 	\
		USART_SendString("\x1B" "[47m")

#define USART_ESCAPE_TEXT_BLACK()		\
		USART_SendString("\x1B" "[30m")


// NOTE: Not work in Hyperterminal
#define USART_ESCAPE_BACKGROUND_DEFAULT()	\
		USART_SendString("\x1B" "[49m")


// NOTE: At Hyperterminal the background will be black
#define USART_ESCAPE_RESET() 	\
		USART_SendString("\x1B" "[0m")



// TODO: Sortörlésre?
// CSI n D	CUB – Cursor Back
// CSI n K	EL – Erase in Line	Erases part of the line. If n is zero (or missing), clear from cursor to the end of the line.
// PROBLEM: this is delete everything after cursor
/*
// Cursor back
USART_SendChar(27);	// 27 = ESCAPE
USART_SendChar(91);	// 91 = [
USART_SendChar('1');
USART_SendChar('D');

// Delete from cursor to end of line
USART_SendChar(27);	// 27 = ESCAPE
USART_SendChar(91);	// 91 = [
USART_SendChar('0');
USART_SendChar('K');
*/



#define USART_SEND_CLS()				\
		USART_ESCAPE_ERASE_CLS();		\
		USART_ESCAPE_CURSOR_TOPLEFT()




/////////////////////////////  TYPEDEFS




/////////////////////////////  GLOBAL VARIABLES

extern volatile char MONITOR_CommandActual[MONITOR_MAX_COMMAND_LENGTH];

extern const bool MONITOR_CommandReceiveEnable;


#ifdef CONFIG_USE_FREERTOS
extern xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
extern xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
#endif





/////////////////////////////  PROTOTYPES



void MONITOR_Init (void);
void MONITOR_SendWelcome (void);
void MONITOR_SendPrimitiveWelcome (void);

void MONITOR_CheckCommand (void);

void MONITOR_WriteCommandHelp (CommandID_t commandID);


#endif /* COMMANDHANDLER_H_ */
