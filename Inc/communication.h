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


/// TYPEDEFS

typedef enum
{
	Source_Unknown,
	Source_DebugUart,
	Source_OtherUart,

	// Do not use:
	Source_Count
} CommProtocol_t;



/// FUNCTION PROTOTYPES

uint8_t COMMUNICATION_SendMessage (CommProtocol_t protocol, const char *message);
uint8_t COMMUNICATION_SendChar (CommProtocol_t protocol, char c);



#endif /* COMMUNICATION_H_ */
