/*
 * formattedMessage.c
 *
 *  Created on: Sep 10, 2016
 *      Author: Vizi Gábor
 */


#include "usart.h"
#include "formattedMessage.h"


void SendColouredMessageWithBackgroundColor (const char* message, FormattedStringColors_t textColor, FormattedStringColors_t backgroundColor)
{
	// Send background color
	SendBackgroundColor(backgroundColor);

	// Send coloured message
	SendColouredMessage(message,textColor);

	// Send background color = white
	SendBackgroundColor(Color_White);
}



void SendColouredMessage (const char* message, FormattedStringColors_t textColor)
{
	// Send color escape
	SendTextColor(textColor);

	// Send message
	USART_SendMessage(message);

	// Send color "end
	SendTextColor(Color_Black);
}



void SendTextColor (FormattedStringColors_t textColor)
{
	USART_SendMessage(USART_ESCAPE_TEXT_START);
	USART_SendChar('0' + textColor);
	USART_SendMessage(USART_ESCAPE_TEXT_END);
}



void SendBackgroundColor (FormattedStringColors_t backgroundColor)
{
	USART_SendMessage(USART_ESCAPE_BACKGROUND_START);
	USART_SendChar('0' + backgroundColor);
	USART_SendMessage(USART_ESCAPE_BACKGROUND_END);
}
