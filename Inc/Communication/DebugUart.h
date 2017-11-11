/*
 *		DebugUart.h
 *		Created on:		2017-03-15
 *		Author:			Vizi G�bor
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
#include "CircularBuffer.h"

#ifdef CONFIG_USE_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define DEBUGUART_TX_BUFFER_SIZE				(256UL)	// Size of Transmission buffer
#define DEBUGUART_RX_BUFFER_SIZE				(256UL)		// Size of Reception buffer
#define DEBUGUART_RX_BUFFER_WAIT_LENGTH			(1)

#define DEBUGUART_RESPONSE_BUFFER				(256U)


// Variadic macro
#define DebugUart_Printf(...)					uprintf(__VA_ARGS__)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern UART_HandleTypeDef Debug_UartHandle;

extern volatile char DebugUart_RxBuffer[DEBUGUART_RX_BUFFER_SIZE];
extern volatile char DebugUart_TxBuffer[DEBUGUART_TX_BUFFER_SIZE];

extern CircularBufferInfo_t DebugUart_RxBuffStruct;

extern bool DebugUart_SendEnable_flag;
extern const bool DebugUart_CommandReceiveEnable;

#if defined(CONFIG_USE_FREERTOS)
extern xSemaphoreHandle DebugUart_Rx_Semaphore;
extern xSemaphoreHandle DebugUart_Tx_Semaphore;
#endif



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_DEBUGUART_ENABLE

void DebugUart_Init(void);
void DebugUart_StartReceive(void);
bool DebugUart_SendChar(char c);
uint8_t DebugUart_SendMessage(const char *message);
bool DebugUart_SendNewLine(void);
bool DebugUart_SendLine(const char *message);

uint8_t uprintf(const char *format, ...);

void DebugUart_ProcessReceivedCharacters(void);

size_t DebugUart_SendMessageBlocked(const char * str);

#else

// Not used DEBUGUART
#ifndef CONFIG_MICROCONTROLLER_PC
#define DebugUart_SendLine(_msg)				(void)_msg
#define DebugUart_SendMessageBlocked(_msg)		(void)_msg

#define uprintf(...)							(0)
#else
#include <stdio.h>
#define uprintf(...)							printf(__VA_ARGS__)
#define DebugUart_SendLine(_msg)				puts(_msg)
#define DebugUart_SendMessageBlocked(_msg)		puts(_msg)
#endif

#endif



#endif /* DEBUGUART_H_ */
