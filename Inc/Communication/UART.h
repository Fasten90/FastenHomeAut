/*
 *		USART.h
 *		Created on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		USART communication
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
 */

#ifndef UART_H_
#define UART_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "CircularBuffer.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef struct
{
	UART_HandleTypeDef * huart;			/* pointer to the HAL_UART struct */
	CircularBufferInfo_t *tx;
	CircularBufferInfo_t *rx;			/* transmit (out of STM32) and receive (in to STM32) FIFOs */
	bool txIsEnabled;
	bool rxIsEnalbed;
} UART_Handler_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void UART_Init(UART_HandleTypeDef *UartHandle);
void UART_SendEnable(UART_Handler_t * handler);
void UART_ReceiveEnable(UART_Handler_t * handler);



#endif /* UART_H_ */
