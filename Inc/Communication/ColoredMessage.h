/*
 *		FormattedMessage.h
 *		Created on:		2016-09-10
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Formatted (colored / backgroundcolor) message
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
 */

#ifndef FORMATTEDMESSAGE_H_
#define FORMATTEDMESSAGE_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

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

#ifdef CONFIG_MODULE_FORMATTEDMESSAGE_ENABLE

void SendColouredMessageWithBackgroundColor(const char* message,  FormattedStringColors_t textColor, FormattedStringColors_t backgroundColor);
void SendColouredMessage(const char* message, FormattedStringColors_t textColor);
void SendTextColor(FormattedStringColors_t textColor);
void SendBackgroundColor(FormattedStringColors_t backgroundColor);
void SendErrorMessage(const char *message);

void FormattedMessage_Test(void);

#else

#define SendBackgroundColor(_backgroundcolor)
#define SendTextColor(_textcolor)

#endif


#endif /* FORMATTEDMESSAGE_H_ */
