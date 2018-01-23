/*
 *		EscapeSequence.h
 *		Created on:		2016-11-29
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Escape sequences
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2016-11-29
 */

#ifndef ESCAPESEQUENCE_H_
#define ESCAPESEQUENCE_H_


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

//		ESCAPE SEQUENCE


// 27 = ESCAPE
// 91 = [

// Delete the line
// CSI n K	EL – Erase in Line	Erases part of the line.
// If n is zero (or missing), clear from cursor to the end of the line.
// If n is one, clear from cursor to beginning of the line.
// If n is two, clear entire line. Cursor position does not change.


// CSI 2 K: delete entire line
#define ESCAPE_DELETELINE				("\x1B" "[2K")


// CSI n G	CHA – Cursor Horizontal Absolute	Moves the cursor to column n.
// CSI 1 G  -> go start of line
#define ESCAPE_CURSOR_TO_LINESTART		("\x1B" "[1G")


// CSI n A	CUU – Cursor Up	Moves the cursor n (default 1) cells in the given direction.
// If the cursor is already at the edge of the screen, this has no effect.
// CSI n B	CUD – Cursor Down
// CSI n C	CUF – Cursor Forward
// CSI n D	CUB – Cursor Back
#define ESCAPE_CURSORRIGHT				("\x1B" "[C")


// Cursor step left
#define ESCAPE_CURSORLEFT				("\x1B" "[D")


// Cursor go left (loft of step)
#define ESCAPE_CURSORLEFTLOTOF			("\x1B" "[100D")


// CURSOR SAVE/RESTORE
//	CSI s	SCP – Save Cursor Position	Saves the cursor position.
#define ESCAPE_SAVECURSOR				("\x1B" "[s")


//CSI u	RCP – Restore Cursor Position	Restores the cursor position.
#define ESCAPE_RESTORECURSOR			("\x1B" "[u")


// Erase Functions:
// (CSI in UTF-8 is 0xC2, 0x9B) = ESC+[
// ESC[2J Clear screen and home cursor
#define ESCAPE_ERASE_CLS				("\x1B" "[2J")


// Goto top left corner
// CSI n ; m H	CUP – Cursor Position	Moves the cursor to row n, column m. The values are 1-based, and default to 1 (top left corner) if omitted.
// A sequence such as CSI ;5H is a synonym for CSI 1;5H as well as CSI 17;H is the same as CSI 17H and CSI 17;1H
// n = 1 = row
// ;
// 1 = column
// H
#define ESCAPE_CURSOR_TOPLEFT			("\x1B" "[1;1H")


/*------------------------------------------------------------------------------
 *							ESCAPE SEQUENCE  - COLORS
 *----------------------------------------------------------------------------*/
 
// CSI n m 	SGR – Select Graphic Rendition
// 40–47 	Set background color 	40 + x, where x is from the color table below
#define ESCAPE_BACKGROUND_GREEN			("\x1B" "[42m")

#define ESCAPE_BACKGROUND_RED			("\x1B" "[41m")

#define ESCAPE_BACKGROUND_WHITE			("\x1B" "[47m")

#define ESCAPE_TEXT_BLACK				("\x1B" "[30m")

#define ESCAPE_BACKGROUND_DEFAULT		("\x1B" "[49m")		// NOTE: Does not work in Hyperterminal

#define ESCAPE_RESET					("\x1B" "[0m")		// NOTE: At Hyperterminal the background will be black

#define ESCAPE_SEND_CLS					ESCAPE_ERASE_CLS ESCAPE_CURSOR_TOPLEFT

// Example:
// "\x1B" "[30m" = Black text color
// TODO: Refactor the names
#define USART_ESCAPE_TEXT_START			"\x1B" "[3"
#define USART_ESCAPE_TEXT_END			"m"

// Example:
// "\x1B" "[47m" = White background color
#define USART_ESCAPE_BACKGROUND_START	"\x1B" "[4"
#define USART_ESCAPE_BACKGROUND_END		"m"


#define ESCAPE_SEQUENCE_STRING_MAX_LENGTH		( 5 + 1 )


/**
 * \brief	Color enum for Escape sequence
 *			Intensity 	0 	1 	2 	3 	4 	5 	6 	7
 *			Normal 	Black 	Red 	Green 	Yellow 	Blue 	Magenta 	Cyan 	White
 * \note	Do not modify! It is depending from escape sequence codes
 */
typedef enum
{
	Color_Black = 0,
	Color_Red,
	Color_Green,
	Color_Yellow,
	Color_Blue,
	Color_Magenta,
	Color_Cyan,
	Color_White
} FormattedStringColors_t;



#endif /* ESCAPESEQUENCE_H_ */
