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



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
	CommProt_Unknown	= 0,
	CommProt_DebugUart  = (1 << 0),
	CommProt_OtherUart	= (1 << 1),

	CommProt_Disable	= (1 << 2)
} CommProtocol_t;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

uint8_t COMMUNICATION_SendMessage(CommProtocol_t protocol, const char *message);
uint8_t COMMUNICATION_SendChar(CommProtocol_t protocol, char c);



#endif /* COMMUNICATION_H_ */
