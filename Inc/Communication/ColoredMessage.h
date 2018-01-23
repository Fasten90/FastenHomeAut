/*
 *		ColoredMessage.h
 *		Created on:		2016-09-10
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Formatted (colored / backgroundcolor) message
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
 */

#ifndef COLOREDMESSAGE_H_
#define COLOREDMESSAGE_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "EscapeSequence.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define COLOREDMESSAGE_STANDARD_TEXT_COLOR					((FormattedStringColors_t)Color_Black)
#define COLOREDMESSAGE_STANDARD_BACKGROUND_COLOR			((FormattedStringColors_t)Color_White)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_COLOREDMESSAGE_ENABLE

void ColoredMessage_SendMsgWithBackgroundColor(char *str, const char *msg,  FormattedStringColors_t textColor, FormattedStringColors_t backgroundColor);
void ColoredMessage_SendMsg(char *str, const char *msg, FormattedStringColors_t textColor);
void ColoredMessage_SendTextColor(char *str, FormattedStringColors_t textColor);
void ColoredMessage_SendBackgroundColor(char *str, FormattedStringColors_t backgroundColor);
void ColoredMessage_SendErrorMsg(char *str, const char *msg);

void ColoredMessage_Test(void);

#else

#define SendBackgroundColor(_backgroundcolor)
#define SendTextColor(_textcolor)

#endif


#endif /* COLOREDMESSAGE_H_ */
