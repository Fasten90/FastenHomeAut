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
#include <stdarg.h>
#include "String.h"
#include "DebugUart.h"
#include "Communication.h"
#include "MEM.h"



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
char Communication_Buffer[COMMUNICATION_PROTOCOL_BUFFER_SIZE];
uint8_t Communication_BufferCnt = 0;
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief	Send message (string) on selected communication protocol
 */
uint8_t COMMUNICATION_SendMessage(CommProtocol_t protocol, const char *message)
{
	uint8_t length = 0;

	switch (protocol)
	{
		case CommProt_Unknown:
			// Unknown, send on debug
			length = DebugUart_SendMessage(message);
			break;
		case CommProt_DebugUart:
			// Send on Usart
			length = DebugUart_SendMessage(message);
			break;
		case CommProt_SWO:
			// Send on SWO
#ifdef CONFIG_SWO_ENABLE
			length = SWO_SendMessage(message);
			break;
#endif
#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
		case CommProt_Buffer:
			length = StrCpyMax(&Communication_Buffer[Communication_BufferCnt], message, COMMUNICATION_PROTOCOL_BUFFER_SIZE-Communication_BufferCnt);
			Communication_BufferCnt += length;
			break;
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
		case CommProt_ESP8266Wifi:
			// Send on wifi (TCP message)
			length = ESP8266_RequestSendTcpMessage(message);
			break;
#endif
		case CommProt_Disable:
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
uint8_t COMMUNICATION_SendChar(CommProtocol_t protocol, char c)
{
	switch (protocol)
	{
		case CommProt_Unknown:
			// Unknown, send on debug
			DebugUart_SendChar(c);
			break;
		case CommProt_DebugUart:
			DebugUart_SendChar(c);
			break;
		case CommProt_SWO:
#ifdef CONFIG_SWO_ENABLE
			SWO_SendChar(c);
			break;
#endif
#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
		case CommProt_Buffer:
			Communication_Buffer[Communication_BufferCnt++] = c;
			break;
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
		case CommProt_ESP8266Wifi:
			DebugUart_SendChar(c);
			break;
#endif
		case CommProt_Disable:
		default:
			break;
	}

	return 1;
}



/**
 * \brief	Send message on xy communication protocol
 * \param	protocol		what peripheral sending
 */
uint8_t COMMUNICATION_Printf(CommProtocol_t protocol, const char *format, ...)
{
	uint8_t length = 0;

	// Working in at:
	char txBuffer[COMMUNICATION_TXBUFFER_SIZE];

	va_list ap;									// argument pointer
	va_start(ap, format); 						// ap on arg
	string_printf(txBuffer, format, ap);			// Separate and process
	va_end(ap);						 			// Cleaning after end

	length = COMMUNICATION_SendMessage(protocol, txBuffer);

	return length;
}



#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
/**
 * \brief	Protocol buffer clear
 */
void COMMUNICATION_ClearProtocolBuffer(void)
{
	memset(Communication_Buffer, 0, COMMUNICATION_PROTOCOL_BUFFER_SIZE);
	Communication_BufferCnt = 0;
}
#endif	// #ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
