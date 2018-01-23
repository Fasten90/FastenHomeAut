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
void ColoredMessage_SendMsgWithBackgroundColor(char * str, const char *msg, MsgColors_t textColor, MsgColors_t backgroundColor)
{
	ColoredMessage_SendBackgroundColor(str, backgroundColor);								// Send background color
	ColoredMessage_SendMsg(str, msg, textColor);											// Send colored message
	ColoredMessage_SendBackgroundColor(str, COLOREDMESSAGE_STANDARD_BACKGROUND_COLOR);		// Restore background color
}



/**
 * \brief	Send message on debug with textcolor
 * \note	After send message, the original text color will restored
 */
void ColoredMessage_SendMsg(char *str, const char *msg, MsgColors_t textColor)
{
	ColoredMessage_SendTextColor(str, textColor);								// Send text color
	StrAppend(str, msg);														// Send message
	ColoredMessage_SendTextColor(str, COLOREDMESSAGE_STANDARD_TEXT_COLOR);		// Restore text color
}



/**
 * \brief	Send textcolor escape sequence
 */
void ColoredMessage_SendTextColor(char *str, MsgColors_t textColor)
{
	StrAppend(str, ESCAPE_TEXT_START);
	CharAppend(str, '0' + textColor);
	StrAppend(str, ESCAPE_TEXT_END);
}



/**
 * \brief	Send backgroundcolor escape sequence
 */
void ColoredMessage_SendBackgroundColor(char *str, MsgColors_t backgroundColor)
{
	StrAppend(str, ESCAPE_BACKGROUND_START);
	CharAppend(str, '0' + backgroundColor);
	StrAppend(str, ESCAPE_BACKGROUND_END);
}



/**
 * \brief	Send error message (black text, with red background)
 */
void ColoredMessage_SendErrorMsg(char *str, const char *msg)
{
	ColoredMessage_SendMsgWithBackgroundColor(str, msg, Color_Black, Color_Red);
}



/**
 * \brief	Formatted messages Unit test
 */
void ColoredMessage_Test(void)
{
	char text[255] = { 0 };

	// Send colored messages
	ColoredMessage_SendMsg(text, "Red text\r\n", Color_Red);
	ColoredMessage_SendMsg(text, "Yellow text\r\n", Color_Yellow);

	ColoredMessage_SendMsgWithBackgroundColor(text, "Black text with Red background color\r\n", Color_Black, Color_Red);
	ColoredMessage_SendMsgWithBackgroundColor(text, "Black text with Yellow background color\r\n", Color_Black, Color_Yellow);

	ColoredMessage_SendErrorMsg(text, "FATAL ERROR EXAMPLE\r\n");

	DebugUart_SendMessage(text);
}



#endif /* CONFIG_MODULE_COLOREDMESSAGE_ENABLE */
