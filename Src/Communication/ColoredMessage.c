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
void ColoredMessage_SendMsgWithBackgroundColor(char *str, const char *msg, MsgColors_t textColor, MsgColors_t backgroundColor)
{
	ColoredMessage_SendBackgroundAndTextColor(str, backgroundColor, textColor);		// Send background + text color
	StrAppend(str, msg);															// Send message
	ColoredMessage_SendDefaultFormat(str);											// Restore format (bg + text color)
}



/**
 * \brief	Send message on debug with textcolor
 * \note	After send message, the original text color will restored
 */
void ColoredMessage_SendMsg(char *str, const char *msg, MsgColors_t textColor)
{
	ColoredMessage_SendTextColor(str, textColor);								// Send text color
	StrAppend(str, msg);														// Send message
	ColoredMessage_SendDefaultFormat(str);										// Restore format (text color)
}



/**
 * \brief	Send textcolor escape sequence
 */
void ColoredMessage_SendTextColor(char *str, MsgColors_t textColor)
{
	StrAppend(str, ESCAPE_FORMAT_TEXTCOLOR_START);
	CharAppend(str, '0' + textColor);
	StrAppend(str, ESCAPE_FORMAT_END);
}



/**
 * \brief	Send backgroundcolor escape sequence
 */
void ColoredMessage_SendBackgroundColor(char *str, MsgColors_t backgroundColor)
{
	StrAppend(str, ESCAPE_FORMAT_BACKGROUNDCOLOR_START);
	CharAppend(str, '0' + backgroundColor);
	StrAppend(str, ESCAPE_FORMAT_END);
}



/**
 * \brief	Send text format
 */
void ColoredMessage_SendTextFormat(char *str, MsgFormat_t textFormat)
{
	StrAppend(str, ESCAPE_FORMAT_ONLY_START);

	CharAppend(str, '0' + textFormat);

	StrAppend(str, ESCAPE_FORMAT_END);
}



/**
 * \brief	Send BackgroundColor + TextColor
 */
void ColoredMessage_SendBackgroundAndTextColor(char *str, MsgColors_t backgroundColor, MsgColors_t textColor)
{
	StrAppend(str, ESCAPE_FORMAT_ONLY_START);

	StrAppend(str, ESCAPE_FORMAT_ONLY_BACKGROUNDCOLOR);
	CharAppend(str, '0' + backgroundColor);

	StrAppend(str, ESCAPE_FORMAT_SEPARATE);

	StrAppend(str, ESCAPE_FORMAT_ONLY_TEXTROUNDCOLOR);
	CharAppend(str, '0' + textColor);

	StrAppend(str, ESCAPE_FORMAT_END);
}



/**
 * \brief	Send BackgroundColor + TextColor + TextFormat
 */
void ColoredMessage_SendBackgroundAndTextColorAndFormat(char *str, MsgColors_t backgroundColor, MsgColors_t textColor, MsgFormat_t textFormat)
{
	StrAppend(str, ESCAPE_FORMAT_ONLY_START);

	StrAppend(str, ESCAPE_FORMAT_ONLY_BACKGROUNDCOLOR);
	CharAppend(str, '0' + backgroundColor);

	StrAppend(str, ESCAPE_FORMAT_SEPARATE);

	StrAppend(str, ESCAPE_FORMAT_ONLY_TEXTROUNDCOLOR);
	CharAppend(str, '0' + textColor);

	StrAppend(str, ESCAPE_FORMAT_SEPARATE);

	CharAppend(str, '0' + textFormat);

	StrAppend(str, ESCAPE_FORMAT_END);
}



/**
 * \brief	Send default format: restore original background color, text color and text format (e.g. bold)
 */
void ColoredMessage_SendDefaultFormat(char *str)
{
	StrAppend(str, ESCAPE_FORMAT_RESET);
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
	char str[255] = { 0 };

	// Send colored messages
	ColoredMessage_SendMsg(str, "Red text\r\n", Color_Red);
	ColoredMessage_SendMsg(str, "Yellow text\r\n", Color_Yellow);

	ColoredMessage_SendMsgWithBackgroundColor(str, "Black text with Red background color\r\n", Color_Black, Color_Red);
	ColoredMessage_SendMsgWithBackgroundColor(str, "Black text with Yellow background color\r\n", Color_Black, Color_Yellow);

	ColoredMessage_SendErrorMsg(str, "FATAL ERROR EXAMPLE\r\n");

	ColoredMessage_SendTextFormat(str, Format_BoldOn);
	StrAppend(str, "bold\r\n");
	ColoredMessage_SendDefaultFormat(str);

	ColoredMessage_SendTextFormat(str, Format_Underscore);
	StrAppend(str, "underscore\r\n");
	ColoredMessage_SendDefaultFormat(str);

	ColoredMessage_SendBackgroundAndTextColorAndFormat(str, Color_Green, Color_Black, Format_BoldOn);
	StrAppend(str, "Colored bold text\r\n");
	ColoredMessage_SendDefaultFormat(str);


	DebugUart_SendMessage(str);
}

#else

// ColoredMessage module is not used
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic pop

#endif /* CONFIG_MODULE_COLOREDMESSAGE_ENABLE */
