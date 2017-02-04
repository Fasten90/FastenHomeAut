/*
 *		formattedMessage.h
 *
 *		Created on:		2016.09.10
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Formatted (colored / backgroundcolor) message
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef FORMATTEDMESSAGE_H_
#define FORMATTEDMESSAGE_H_



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

// Example:
// USART_SendString("\x1B" "[30m");	= Black text color

#define USART_ESCAPE_TEXT_START		"\x1B" "[3"
#define USART_ESCAPE_TEXT_END		"m"

// Example:
// USART_SendString("\x1B" "[47m"); = White background color
#define USART_ESCAPE_BACKGROUND_START	"\x1B" "[4"
#define USART_ESCAPE_BACKGROUND_END		"m"



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

/**
 * Color enum for Escape sequence
 * Intensity 	0 	1 	2 	3 	4 	5 	6 	7
 * Normal 	Black 	Red 	Green 	Yellow 	Blue 	Magenta 	Cyan 	White
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



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void SendColouredMessageWithBackgroundColor(const char* message,
		FormattedStringColors_t textColor,
		FormattedStringColors_t backgroundColor);
void SendColouredMessage(const char* message, FormattedStringColors_t textColor);
void SendTextColor(FormattedStringColors_t textColor);
void SendBackgroundColor(FormattedStringColors_t backgroundColor);
void SendErrorMessage(const char *message);

void FormattedMessage_Test(void);



#endif /* FORMATTEDMESSAGE_H_ */
