/*
 *    ColoredMessage.h
 *    Created on:   2016-09-10
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Colored message
 *    Target:       STM32Fx
 */

#ifndef COLOREDMESSAGE_H_
#define COLOREDMESSAGE_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "EscapeSequence.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define COLOREDMESSAGE_STANDARD_TEXT_COLOR                    ((MsgColors_t)Color_Black)
#define COLOREDMESSAGE_STANDARD_BACKGROUND_COLOR            ((MsgColors_t)Color_White)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_COLOREDMESSAGE_ENABLE

void ColoredMessage_SendMsgWithBackgroundColor(char *str, const char *msg,  MsgColors_t textColor, MsgColors_t backgroundColor);
void ColoredMessage_SendMsg(char *str, const char *msg, MsgColors_t textColor);
void ColoredMessage_SendTextColor(char *str, MsgColors_t textColor);
void ColoredMessage_SendTextFormat(char *str, MsgFormat_t textFormat);
void ColoredMessage_SendBackgroundColor(char *str, MsgColors_t backgroundColor);
size_t ColoredMessage_SendBackgroundAndTextColor(char *str, MsgColors_t backgroundColor, MsgColors_t textColor);
void ColoredMessage_SendBackgroundAndTextColorAndFormat(char *str, MsgColors_t backgroundColor, MsgColors_t textColor, MsgFormat_t textFormat);
size_t ColoredMessage_SendDefaultFormat(char *str);
void ColoredMessage_SendErrorMsg(char *str, const char *msg);

void ColoredMessage_Test(void);

#else

#define SendBackgroundColor(_backgroundcolor)
#define SendTextColor(_textcolor)

#endif


#endif /* COLOREDMESSAGE_H_ */
