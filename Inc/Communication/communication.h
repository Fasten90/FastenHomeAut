/*
 *		Communication.h
 *		Created on:		2016-12-03
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Communication module
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2016-12-03
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros
 *----------------------------------------------------------------------------*/

#define COMMUNICATION_TXBUFFER_SIZE				(255U)
#define COMMUNICATION_PROTOCOL_BUFFER_SIZE		(100)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
	CommProt_Unknown	= 0,
	CommProt_DebugUart  = (1 << 0),
#ifdef CONFIG_SWO_ENABLE
	CommProt_SWO		= (1 << 1),
#endif
#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
	CommProt_Buffer		= (1 << 2),
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	CommProt_ESP8266Wifi = (1 << 3),
#endif

	// XXX: Add here new Protocol, and correct the CommProt_Disable (largest bit be set)
	CommProt_Disable	= (1 << 4)
} CommProtocol_t;




/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
extern char Communication_Buffer[COMMUNICATION_PROTOCOL_BUFFER_SIZE];
extern uint8_t Communication_BufferCnt;
#endif



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

uint8_t COMMUNICATION_SendMessage(CommProtocol_t protocol, const char *message);
uint8_t COMMUNICATION_SendChar(CommProtocol_t protocol, char c);
uint8_t COMMUNICATION_Printf(CommProtocol_t protocol, const char *format, ...);

const char * COMMUNICATION_GetProtocolName(CommProtocol_t protocol);

void COMMUNICATION_ClearProtocolBuffer(void);



#endif /* COMMUNICATION_H_ */
