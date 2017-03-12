/*
 *		communication.h
 *
 *		Created on:		2016. dec. 3.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2016. dec. 3.
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_


#include "include.h"


#define COMMUNICATION_TXBUFFER_SIZE				(255U)
#define COMMUNICATION_PROTOCOL_BUFFER_SIZE		(100)


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
	CommProt_Unknown	= 0,
	CommProt_DebugUart  = (1 << 0),
	CommProt_SWO		= (1 << 1),
#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
	CommProt_Buffer		= (1 << 2),
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	CommProt_ESP8266Wifi = (1 << 3),
#endif

	// XXX: Add here new Protocol, and correct the CommProt_Disable (largest bit)
	CommProt_Disable	= (1 << 4)
} CommProtocol_t;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

uint8_t COMMUNICATION_SendMessage(CommProtocol_t protocol, const char *message);
uint8_t COMMUNICATION_SendChar(CommProtocol_t protocol, char c);
uint8_t COMMUNICATION_Printf(CommProtocol_t protocol, const char *format, ...);



#endif /* COMMUNICATION_H_ */
