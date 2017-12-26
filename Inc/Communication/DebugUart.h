/*
 *		DebugUart.h
 *		Created on:		2017-03-15
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Debug uart functions
  *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-03-15
 */

#ifndef DEBUGUART_H_
#define DEBUGUART_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "UART.h"
#include "CircularBuffer.h"

#ifdef CONFIG_USE_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define DEBUGUART_TX_BUFFER_SIZE				(256UL)		///< Size of Transmission buffer
#define DEBUGUART_RX_BUFFER_SIZE				(256UL)		///< Size of Reception buffer

#define DEBUGUART_RESPONSE_BUFFER				(256U)
#define DEBUGUART_PROCESS_BUFFER				(256U)


// Variadic macro
#define DebugUart_Printf(...)					uprintf(__VA_ARGS__)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern UART_Handler_t DebugUart;
extern UART_HandleTypeDef DebugUart_Handle;

#if defined(CONFIG_USE_FREERTOS)
extern xSemaphoreHandle DebugUart_Rx_Semaphore;
extern xSemaphoreHandle DebugUart_Tx_Semaphore;
#endif



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_DEBUGUART_ENABLE

void DebugUart_Init(void);
void DebugUart_ReceiveEnable(void);

bool DebugUart_SendChar(char c);
size_t DebugUart_SendMessage(const char * msg);
size_t DebugUart_SendLine(const char * msg);
size_t DebugUart_SendMessageBlocked(const char * msg);
size_t DebugUart_SendLineBlocked(const char * msg);
size_t uprintf(const char *format, ...);

void DebugUart_ProcessReceivedCharacters(void);


#else

// Not used DEBUGUART

#ifndef CONFIG_MICROCONTROLLER_PC

#define uprintf(...)							(0)
#define DebugUart_SendChar(_c)					(putc)_c
#define DebugUart_SendMessage(_msg)				(void)_msg
#define DebugUart_SendLine(_msg)				(void)_msg
#define DebugUart_SendMessageBlocked(_msg)		(void)_msg

#else

#include <stdio.h>
#define uprintf(...)							printf(__VA_ARGS__)
#define DebugUart_SendChar(_c)					putchar(_c)
#define DebugUart_SendMessage(_msg)				puts(_msg)
#define DebugUart_SendLine(_msg)				puts(_msg)
#define DebugUart_SendMessageBlocked(_msg)		puts(_msg)

#define DebugUart_StartReceive()

#endif

#endif



#endif /* DEBUGUART_H_ */
