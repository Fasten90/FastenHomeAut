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



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_COLOREDMESSAGE_ENABLE

void ColoredMessage_SendMsgWithBackgroundColor(const char* message,  FormattedStringColors_t textColor, FormattedStringColors_t backgroundColor);
void ColoredMessage_SendMsg(const char* message, FormattedStringColors_t textColor);
void ColoredMessage_SendTextColor(FormattedStringColors_t textColor);
void ColoredMessage_SendBackgroundColor(FormattedStringColors_t backgroundColor);
void ColoredMessage_SendErrorMsg(const char *message);

void ColoredMessage_Test(void);

#else

#define SendBackgroundColor(_backgroundcolor)
#define SendTextColor(_textcolor)

#endif


#endif /* COLOREDMESSAGE_H_ */
