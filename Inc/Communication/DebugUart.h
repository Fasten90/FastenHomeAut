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
#define DEBUGUART_TXBUFFERSIZE				256UL
// Size of Reception buffer
#define DEBUGUART_RXBUFFERSIZE				256UL
#define DEBUGUART_RXBUFFER_WAIT_LENGTH			1


// Variadic macro
#define DebugUart_Printf(...)		   uprintf(__VA_ARGS__)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern UART_HandleTypeDef Debug_UartHandle;

extern volatile char DebugUart_RxBuffer[DEBUGUART_RXBUFFERSIZE];
extern volatile char DebugUart_TxBuffer[DEBUGUART_TXBUFFERSIZE];

extern volatile uint8_t DebugUart_RxBufferWriteCnt;
extern volatile uint8_t DebugUart_RxBufferReadCnt;

extern bool DebugUart_SendEnable_flag;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void DebugUart_StartReceive(void);
bool DebugUart_SendChar(char c);
uint8_t DebugUart_SendMessage(const char *aTxBuffer);
bool DebugUart_SendNewLine(void);
bool DebugUart_SendLine(const char *message);
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
uint8_t uprintf(const char *format, ...);
#else
// Not realized
#define uprintf(...)		(0)
#endif


#endif /* COMMUNICATION_DEBUGUART_H_ */
