/*
 *		communication.c
 *
 *		Created on:		2016. dec. 3.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2016. dec. 3.
 */

#include "options.h"
#include "include.h"
#include "communication.h"



/// FUNCTIONS

/**
 * \brief	Send message (string) on selected communication protocol
 */
uint8_t COMMUNICATION_SendMessage (CommProtocol_t protocol, const char *message)
{
	uint8_t length = 0;

	switch (protocol)
	{
		case CommProt_Unknown:
			// Unknown, send on debug
			length = USART_SendMessage(message);
			break;
		case CommProt_DebugUart:
			// Send on Usart
			length = USART_SendMessage(message);
			break;
		case CommProt_OtherUart:
			// TODO: Not implemented
			length = 0;
			break;
		default:
			// Error, do not use
			length = 0;
			break;
	}

	return length;

}



/**
 * \brief	Send character on selected communication protocol
 */
uint8_t COMMUNICATION_SendChar (CommProtocol_t protocol, char c)
{
	switch (MONITOR_CommandSource)
	{
		case CommProt_DebugUart:
			USART_SendChar(c);
			break;

		default:
			USART_SendChar(c);
			break;
	}

	return 1;
}

