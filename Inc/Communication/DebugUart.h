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

#include "GenericTypeDefs.h"
#include "CircularBuffer.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

// Size of Transmission buffer
#define DEBUGUART_TX_BUFFER_SIZE				256UL
// Size of Reception buffer
#define DEBUGUART_RX_BUFFER_SIZE				256UL
#define DEBUGUART_RXBUFFER_WAIT_LENGTH		1


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



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void DebugUart_Init(void);
void DebugUart_StartReceive(void);
bool DebugUart_SendChar(char c);
uint8_t DebugUart_SendMessage(const char *message);
bool DebugUart_SendNewLine(void);
bool DebugUart_SendLine(const char *message);
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
uint8_t uprintf(const char *format, ...);
#else
// Not realized
#define uprintf(...)		(0)
#endif

void DebugUart_FindLastMessage(void);
void DebugUart_ClearReceive(bool isFullClear, uint8_t stepLength);
void DebugUart_ProcessReceivedCharacters(void);



#endif /* DEBUGUART_H_ */
