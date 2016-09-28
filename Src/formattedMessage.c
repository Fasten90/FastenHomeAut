/*
 *		formattedMessage.c
 *
 *		Created on:		2016.09.10
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Formatted (colored / backgroundcolor) message
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#include "usart.h"
#include "formattedMessage.h"


/**
 * \brief	Send message on debug with textcolor & backgroundcolor
 */
void SendColouredMessageWithBackgroundColor (const char* message, FormattedStringColors_t textColor, FormattedStringColors_t backgroundColor)
{
	// Send background color
	SendBackgroundColor(backgroundColor);

	// Send colored message
	SendColouredMessage(message,textColor);

	// Send background color = white
	SendBackgroundColor(Color_White);
}



/**
 * \brief	Send message on debug with textcolor
 */
void SendColouredMessage (const char* message, FormattedStringColors_t textColor)
{
	// Send color escape
	SendTextColor(textColor);

	// Send message
	USART_SendMessage(message);

	// Send color "end
	SendTextColor(Color_Black);
}



/**
 * \brief	Send textcolor escape sequence
 */
void SendTextColor (FormattedStringColors_t textColor)
{
	USART_SendMessage(USART_ESCAPE_TEXT_START);
	USART_SendChar('0' + textColor);
	USART_SendMessage(USART_ESCAPE_TEXT_END);
}



/**
 * \brief	Send backgroundcolor escape sequence
 */
void SendBackgroundColor (FormattedStringColors_t backgroundColor)
{
	USART_SendMessage(USART_ESCAPE_BACKGROUND_START);
	USART_SendChar('0' + backgroundColor);
	USART_SendMessage(USART_ESCAPE_BACKGROUND_END);
}



/**
 * \brief	Send error message (black text, with red background)
 */
void SendErrorMessage (const char *message)
{
	// TODO: Will be MACRO()?
	SendColouredMessageWithBackgroundColor(message,Color_Black,Color_Red);
}



/**
 * \brief	Formatted messages Unit test
 */
void FormattedMessage_UnitTest (void)
{
	// Send coloured messages
	SendColouredMessage("Valami pirosan\r\n", Color_Red);
	SendColouredMessage("Valami sárgán\r\n", Color_Yellow);

	SendColouredMessageWithBackgroundColor("Valami piros háttérrel\r\n", Color_Black, Color_Red);
	SendColouredMessageWithBackgroundColor("Valami sárga háttérrel\r\n", Color_Black, Color_Yellow);

	SendErrorMessage ("FATAL ERROR EXAMPLE\r\n");
}

