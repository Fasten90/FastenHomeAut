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

#ifndef USART_H_
#define USART_H_



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
} UART_Handler_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void UART_Init(UART_HandleTypeDef *UartHandle);
void UART_ResetStatus(UART_HandleTypeDef *huart);



#endif /* USART_H_ */
