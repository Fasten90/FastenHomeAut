/*
 *		ColoredMessage.c
 *		Created on:		2016-09-10
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Formatted (colored / backgroundcolor) message
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2018-01-23
 */


#include "options.h"

#ifdef CONFIG_MODULE_COLOREDMESSAGE_ENABLE

#include "StringHelper.h"
#include "EscapeSequence.h"
#include "DebugUart.h"
#include "ColoredMessage.h"



/**
 * \brief	Send message on debug with textcolor & backgroundcolor
 */
void ColoredMessage_SendMsgWithBackgroundColor(const char* message, FormattedStringColors_t textColor, FormattedStringColors_t backgroundColor)
{
	ColoredMessage_SendBackgroundColor(backgroundColor);		// Send background color
	ColoredMessage_SendMsg(message, textColor);	// Send colored message
	ColoredMessage_SendBackgroundColor(Color_White);			// Send background color = white
}



/**
 * \brief	Send message on debug with textcolor
 */
void ColoredMessage_SendMsg(const char* message, FormattedStringColors_t textColor)
{
	ColoredMessage_SendTextColor(textColor);					// Send color escape
	DebugUart_SendMessage(message);				// Send message
	ColoredMessage_SendTextColor(Color_Black);					// Send message
}



/**
 * \brief	Send textcolor escape sequence
 */
void ColoredMessage_SendTextColor(FormattedStringColors_t textColor)
{
	// TODO: Send with CommandHandler_Send... functions
	DebugUart_SendMessage(USART_ESCAPE_TEXT_START);
	DebugUart_SendChar('0' + textColor);
	DebugUart_SendMessage(USART_ESCAPE_TEXT_END);
}



/**
 * \brief	Send backgroundcolor escape sequence
 */
void ColoredMessage_SendBackgroundColor(FormattedStringColors_t backgroundColor)
{
	// TODO: Send with CommandHandler_Send... functions
	DebugUart_SendMessage(USART_ESCAPE_BACKGROUND_START);
	DebugUart_SendChar('0' + backgroundColor);
	DebugUart_SendMessage(USART_ESCAPE_BACKGROUND_END);
}



/**
 * \brief	Send error message (black text, with red background)
 */
void ColoredMessage_SendErrorMsg(const char *message)
{
	ColoredMessage_SendMsgWithBackgroundColor(message, Color_Black, Color_Red);
}



/**
 * \brief	Formatted messages Unit test
 */
void ColoredMessage_Test(void)
{
	// Send colored messages
	ColoredMessage_SendMsg("Red text\r\n", Color_Red);
	ColoredMessage_SendMsg("Yellow text\r\n", Color_Yellow);

	ColoredMessage_SendMsgWithBackgroundColor("Black text with Red background color\r\n", Color_Black, Color_Red);
	ColoredMessage_SendMsgWithBackgroundColor("Black text with Yellow background color\r\n", Color_Black, Color_Yellow);

	ColoredMessage_SendErrorMsg("FATAL ERROR EXAMPLE\r\n");
}



#endif
