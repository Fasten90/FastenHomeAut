/*******************************************************************************
 * Purpose:
 
 // Changed \e to \x1B
 
 ******************************************************************************/


#ifndef USART_H_
#define USART_H_





/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


// TODO: normális tx buffer méretet!

// Size of Transmission buffer
#define TXBUFFERSIZE                      255
// Size of Reception buffer
//#define RXBUFFERSIZE                      TXBUFFERSIZE
#define RXBUFFERSIZE                      1



// TODO!!! Error-ok elkerülése véget kellett

#define RESET	0
#define SET		!RESET


#define	ENABLE	1
#define DISABLE	0



/*
typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;
*/


////////////////////////////////////  DEFINES FOR CHARACTERS


//#ifdef CONFIGURE_TERMINAL_USE_PUTTY
#define USART_KEY_BACKSPACE			8
#define USART_KEY_DELETE			127
//#else
//#define USART_KEY_BACKSPACE			127
//#define USART_KEY_DELETE			8
//#endif

#define USART_KEY_ENTER				13
#define USART_KEY_SPACE				32
#define USART_KEY_ESCAPE			27
#define USART_KEY_BELL				7

#define USART_ESCAPESEQUENCE_1		'\x1B'
#define USART_ESCAPESEQUENCE_2		'['


////////////////////////////////////  BUTTONS


// 8 = BS = Backspace
#define USART_SEND_KEY_BACKSPACE()	\
		USART_SendChar(8)

// 127 = DEL = Delete
#define USART_SEND_KEY_DEL()		 \
		USART_SendChar(127)



////////////////////////////////////  ESCAPE SEQUENCE

// Delete the line
// CSI n K	EL – Erase in Line	Erases part of the line.
// If n is zero (or missing), clear from cursor to the end of the line.
// If n is one, clear from cursor to beginning of the line.
// If n is two, clear entire line. Cursor position does not change.

// 27 = ESCAPE
// 91 = [
/*
#define USART_ESCAPE_DELETELINE()	\
	USART_SendChar(27);				\
	USART_SendChar(91);				\
	USART_SendChar('2');			\
	USART_SendChar('K')
*/
#define USART_ESCAPE_DELETELINE()	\
	USART_SendString("\x1B[2K")

// CSI n G	CHA – Cursor Horizontal Absolute	Moves the cursor to column n.
/*
#define USART_ESCAPE_CURSOR_TO_LINESTART()	\
	USART_SendChar(27);						\
	USART_SendChar(91);						\
	USART_SendChar('1');					\
	USART_SendChar('G')
*/
#define USART_ESCAPE_CURSOR_TO_LINESTART()	\
	USART_SendString("\x1B[1G")

// CSI n A	CUU – Cursor Up	Moves the cursor n (default 1) cells in the given direction.
// If the cursor is already at the edge of the screen, this has no effect.
// CSI n B	CUD – Cursor Down
// CSI n C	CUF – Cursor Forward
// CSI n D	CUB – Cursor Back
/*
#define USART_ESCAPE_CURSORRIGHT()	\
	USART_SendChar(27);				\
	USART_SendChar(91);				\
	USART_SendChar('C')
*/
#define USART_ESCAPE_CURSORRIGHT()	\
	USART_SendString("\x1B[C")

/*
#define USART_ESCAPE_CURSORLEFT()	\
	USART_SendChar(27);				\
	USART_SendChar(91);				\
	USART_SendChar('D')
*/
#define USART_ESCAPE_CURSORLEFT()	\
	USART_SendString("\x1B[D")

/*
#define USART_ESCAPE_CURSORLEFTLOTOF()	\
	USART_SendChar(27);					\
	USART_SendChar(91);					\
	USART_SendChar('1');				\
	USART_SendChar('0');				\
	USART_SendChar('0');				\
	USART_SendChar('D')
*/
#define USART_ESCAPE_CURSORLEFTLOTOF()	\
	USART_SendString("\x1B[100D")


// CURSOR SAVE/RESTORE
//CSI s	SCP – Save Cursor Position	Saves the cursor position.
/*
#define USART_ESCAPE_SAVECURSOR()	\
	USART_SendChar(27);				\
	USART_SendChar(91);				\
	USART_SendChar('s')
*/
#define USART_ESCAPE_SAVECURSOR()	\
	USART_SendString("\x1B[s")

//CSI u	RCP – Restore Cursor Position	Restores the cursor position.
/*
#define USART_ESCAPE_RESTORECURSOR()	\
	USART_SendChar(27);					\
	USART_SendChar(91);					\
	USART_SendChar('u')
*/
#define USART_ESCAPE_RESTORECURSOR()	\
	USART_SendString("\x1B[u")

// Erase Functions:
// (CSI in UTF-8 is 0xC2, 0x9B) = ESC+[
// ESC[2J Clear screen and home cursor
/*
#define USART_ESCAPE_ERASE_CLS()		\
	USART_SendChar(27);					\
	USART_SendChar(91);					\
	USART_SendChar('2');				\
	USART_SendChar('J')
*/
#define USART_ESCAPE_ERASE_CLS()		\
	USART_SendString("\x1B[2J")

// Goto top left corner
// CSI n ; m H	CUP – Cursor Position	Moves the cursor to row n, column m. The values are 1-based, and default to 1 (top left corner) if omitted.
// A sequence such as CSI ;5H is a synonym for CSI 1;5H as well as CSI 17;H is the same as CSI 17H and CSI 17;1H
// n = 1 = row
// ;
// 1 = column
// H
/*
#define USART_ESCAPE_CURSOR_TOPLEFT()	\
		USART_SendChar(27);				\
		USART_SendChar(91);				\
		USART_SendChar('1');			\
		USART_SendChar(';');			\
		USART_SendChar('1');			\
		USART_SendChar('H')
*/
#define USART_ESCAPE_CURSOR_TOPLEFT()	\
	USART_SendString("\x1B[1;1H")

////////////////////////////////////  ESCAPE SEQUENCE  - COLORS


// CSI n m 	SGR – Select Graphic Rendition
// 40–47 	Set background color 	40 + x, where x is from the color table below
/*
#define USART_ESCAPE_BACKGROUND_GREEN() \
		USART_SendChar(27);				\
		USART_SendChar(91);				\
		USART_SendChar('4');			\
		USART_SendChar('2');			\
		USART_SendChar('m')
*/
#define USART_ESCAPE_BACKGROUND_GREEN() \
		USART_SendString("\x1B[42m")

/*
#define USART_ESCAPE_BACKGROUND_RED()	\
		USART_SendChar(27);				\
		USART_SendChar(91);				\
		USART_SendChar('4');			\
		USART_SendChar('1');			\
		USART_SendChar('m')
*/
#define USART_ESCAPE_BACKGROUND_RED()	\
		USART_SendString("\x1B[41m")

/*
#define USART_ESCAPE_BACKGROUND_WHITE() 	\
		USART_SendChar(27);					\
		USART_SendChar(91);					\
		USART_SendChar('4');				\
		USART_SendChar('7');				\
		USART_SendChar('m');				\
		USART_SendChar('\0')
		*/
#define USART_ESCAPE_BACKGROUND_WHITE() 	\
				USART_SendString("\x1B[47m")

/*
#define USART_ESCAPE_TEXT_BLACK()		\
		USART_SendChar(27);				\
		USART_SendChar(91);				\
		USART_SendChar('3');			\
		USART_SendChar('0');			\
		USART_SendChar('m')
*/

#define USART_ESCAPE_TEXT_BLACK()		\
		USART_SendString("\x1B[30m")



// 100–109 	Set background color, high intensity 	aixterm (not in standard)
// TODO: Not work
/*
#define USART_ESCAPE_BACKGROUND_WHITE() 	\
		USART_SendChar(27);					\
		USART_SendChar(91);					\
		USART_SendChar('1');				\
		USART_SendChar('0');				\
		USART_SendChar('7');				\
		USART_SendChar('m')
*/


// Hyperterminal alatt nem mûködik:
/*
#define USART_ESCAPE_BACKGROUND_DEFAULT()	\
		USART_SendChar(27);					\
		USART_SendChar(91);					\
		USART_SendChar('4');				\
		USART_SendChar('9');				\
		USART_SendChar('m')
*/
#define USART_ESCAPE_BACKGROUND_DEFAULT()	\
		USART_SendString("\x1B[49m")


// Gond: Fekete lesz az alapháttér Hyperterminalban
/*
#define USART_ESCAPE_RESET() 	\
		USART_SendChar(27);		\
		USART_SendChar(91);		\
		USART_SendChar('0');	\
		USART_SendChar('m')
*/
#define USART_ESCAPE_RESET() 	\
		USART_SendString("\x1B[0m")



//48 	Set xterm-256 background color 	next arguments are 5;x where x is color index (0..255)
// xterm: 009 = red
/*
#define USART_ESCAPE_BACKGROUND_RED()	\
		USART_SendChar(27);				\
		USART_SendChar(91);				\
		USART_SendChar('4');			\
		USART_SendChar('8');			\
		USART_SendChar(';');			\
		USART_SendChar('5');			\
		USART_SendChar(';');			\
		USART_SendChar('0');			\
		USART_SendChar('1');			\
		USART_SendChar('5');			\
		USART_SendChar('m')
*/


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



/*
#define USART_SEND_NEW_LINE()		\
		USART_SendChar('\r');		\
		USART_SendChar('\n')
*/

#define USART_SEND_NEW_LINE()		\
		USART_SendString("\r\n");


#define USART_SEND_CLS()				\
		USART_ESCAPE_ERASE_CLS();		\
		USART_ESCAPE_CURSOR_TOPLEFT()




/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_ESP8266_ENABLE
extern UART_HandleTypeDef ESP8266_UartHandle;
#endif
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
extern UART_HandleTypeDef Debug_UartHandle;
#endif

extern char USART_RxBuffer[RXBUFFERSIZE];
extern char USART_TxBuffer[TXBUFFERSIZE];
extern char USART_ReceivedChar;
extern uint8_t USART_SendEnable_flag;
extern uint8_t USART_ReceiveEnable_flag;

/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/
void USART_Init ( UART_HandleTypeDef *UartHandle );
void USART_SendMessage ( const char *aTxBuffer );
void USART_SendMessageWithLength ( char *aTxBuffer, uint8_t length );
//void USART_ReceiveMessage ( uint8_t *aRxBuffer );
void USART_ReceiveMessage ( void );
void USART_Test ( void );

// !! IMPORTANT !! Send formatted string on USART !!
void uprintf(char * param, ...);


// Send functions
void USART_SendChar ( char c );
void USART_SendCharWithoutWait ( uint8_t c );


void USART_SendFloat ( float value);


uint8_t USART_WaitForSend ( uint16_t timeoutMiliSecond );



#endif /* USART_H_ */
