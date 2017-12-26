/*
 *		FormattedMessage.c
 *		Created on:		2016-09-10
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Formatted (colored / backgroundcolor) message
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
 */


#include "DebugUart.h"
#include "EscapeSequence.h"
#include "ColoredMessage.h"



#ifdef CONFIG_MODULE_COLOREDMESSAGE_ENABLE

/**
 * \brief	Send message on debug with textcolor & backgroundcolor
 */
void SendColouredMessageWithBackgroundColor(const char* message, FormattedStringColors_t textColor, FormattedStringColors_t backgroundColor)
{
	SendBackgroundColor(backgroundColor);		// Send background color
	SendColouredMessage(message, textColor);	// Send colored message
	SendBackgroundColor(Color_White);			// Send background color = white
}



/**
 * \brief	Send message on debug with textcolor
 */
void SendColouredMessage(const char* message, FormattedStringColors_t textColor)
{
	SendTextColor(textColor);					// Send color escape
	DebugUart_SendMessage(message);				// Send message
	SendTextColor(Color_Black);					// Send message
}



/**
 * \brief	Send textcolor escape sequence
 */
void SendTextColor(FormattedStringColors_t textColor)
{
	// TODO: Send with CommandHandler_Send... functions
	DebugUart_SendMessage(USART_ESCAPE_TEXT_START);
	DebugUart_SendChar('0' + textColor);
	DebugUart_SendMessage(USART_ESCAPE_TEXT_END);
}



/**
 * \brief	Send backgroundcolor escape sequence
 */
void SendBackgroundColor(FormattedStringColors_t backgroundColor)
{
	// TODO: Send with CommandHandler_Send... functions
	DebugUart_SendMessage(USART_ESCAPE_BACKGROUND_START);
	DebugUart_SendChar('0' + backgroundColor);
	DebugUart_SendMessage(USART_ESCAPE_BACKGROUND_END);
}



/**
 * \brief	Send error message (black text, with red background)
 */
void SendErrorMessage(const char *message)
{
	SendColouredMessageWithBackgroundColor(message, Color_Black, Color_Red);
}



/**
 * \brief	Formatted messages Unit test
 */
void FormattedMessage_Test(void)
{
	// Send colored messages
	SendColouredMessage("Red text\r\n", Color_Red);
	SendColouredMessage("Yellow text\r\n", Color_Yellow);

	SendColouredMessageWithBackgroundColor("Black text with Red background color\r\n", Color_Black, Color_Red);
	SendColouredMessageWithBackgroundColor("Black text with Yellow background color\r\n", Color_Black, Color_Yellow);

	SendErrorMessage("FATAL ERROR EXAMPLE\r\n");
}



#endif
