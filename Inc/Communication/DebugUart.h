/*
 *		DebugUart.h
 *
 *		Created on:		2017. márc. 15.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. márc. 15.
 */

#ifndef COMMUNICATION_DEBUGUART_H_
#define COMMUNICATION_DEBUGUART_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

// Size of Transmission buffer
#define USART_TXBUFFERSIZE				256UL
// Size of Reception buffer
#define USART_RXBUFFERSIZE				256UL
#define RXBUFFER_WAIT_LENGTH			1



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern UART_HandleTypeDef Debug_UartHandle;

extern volatile char USART_RxBuffer[USART_RXBUFFERSIZE];
extern volatile char USART_TxBuffer[USART_TXBUFFERSIZE];

extern volatile uint8_t USART_RxBufferWriteCounter;
extern volatile uint8_t USART_RxBufferReadCounter;

extern bool USART_SendEnable_flag;

/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void DebugUart_StartReceive(void);
bool DebugUart_SendChar(char c);
uint8_t DebugUart_SendMessage(const char *aTxBuffer);
bool DebugUart_SendNewLine(void);
bool DebugUart_SendLine(const char *message);



#endif /* COMMUNICATION_DEBUGUART_H_ */
