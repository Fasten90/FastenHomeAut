/*
 *		usart.h
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		usart communication
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef USART_H_
#define USART_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/
void USART_Init(UART_HandleTypeDef *UartHandle);

void USART_StartReceiveMessage(void);

void UART_ResetStatus(UART_HandleTypeDef *huart);

bool USART_SendChar(char c);
uint8_t USART_SendMessage(const char *aTxBuffer);
bool USART_SendNewLine(void);
bool USART_SendLine(const char *message);
void USART_SendFloat(float value);



#endif /* USART_H_ */
