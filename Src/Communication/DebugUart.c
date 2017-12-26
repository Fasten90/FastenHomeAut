/*
 *		DebugUart.c
 *		Created on:		2017-03-15
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Debug uart functions
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-03-15
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include <stdarg.h>		// for "..." parameters in printf function

#include "options.h"
#include "Communication.h"
#include "StringHelper.h"
#include "Globals.h"
#include "CircularBuffer.h"
#include "CommandHandler.h"
#include "DebugUart.h"
#include "USART.h"

#ifdef CONFIG_USE_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif


#ifdef CONFIG_MODULE_DEBUGUART_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

volatile char DebugUart_RxBuffer[DEBUGUART_RX_BUFFER_SIZE] = { 0 };
volatile char DebugUart_TxBuffer[DEBUGUART_TX_BUFFER_SIZE] = { 0 };

CircularBufferInfo_t DebugUart_TxBuffStruct =
{
	.buffer = (char *)DebugUart_TxBuffer,
	.name = "DebugUart_TxBuffer",
	.size = DEBUGUART_TX_BUFFER_SIZE
};

CircularBufferInfo_t DebugUart_RxBuffStruct =
{
	.buffer = (char *)DebugUart_RxBuffer,
	.name = "DebugUart_RxBuffer",
	.size = DEBUGUART_RX_BUFFER_SIZE
};

UART_HandleTypeDef DebugUart_Handle;

UART_Handler_t DebugUart =
{
	.huart = &DebugUart_Handle,
	.tx = &DebugUart_TxBuffStruct,
	.rx = &DebugUart_RxBuffStruct,
	.txIsEnabled = true,
	.rxIsEnalbed = true
};



#if defined(CONFIG_USE_FREERTOS)
xSemaphoreHandle DebugUart_Rx_Semaphore = NULL;
xSemaphoreHandle DebugUart_Tx_Semaphore = NULL;
#endif



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	DebugUart init
 */
void DebugUart_Init(void)
{
	CircularBuffer_Init(&DebugUart_RxBuffStruct);
	CircularBuffer_Init(&DebugUart_TxBuffStruct);

	// Init UART
	UART_Init(&DebugUart_Handle);

#ifdef CONFIG_USE_FREERTOS
	DebugUart_Rx_Semaphore = xSemaphoreCreateBinary();
	DebugUart_Tx_Semaphore = xSemaphoreCreateBinary();

	if (DebugUart_Rx_Semaphore == NULL || DebugUart_Tx_Semaphore == NULL)
	{
		Error_Handler();
	}
#endif	//#ifdef CONFIG_USE_FREERTOS

	// Start receive
	DebugUart_ReceiveEnable();
}



/**
 * \brief	Send enable
 */
static inline void DebugUart_SendEnable(void)
{
	UART_SendEnable(&DebugUart);
}



/**
 * \brief	Receive enable
 */
void DebugUart_ReceiveEnable(void)
{
	UART_ReceiveEnable(&DebugUart);

#ifdef CONFIG_USE_FREERTOS
	// Wait for semaphore
	xSemaphoreTake(DebugUart_Rx_Semaphore, (portTickType) 1000);
#endif
}



/**
 * \brief	Send a char on USART
 */
bool DebugUart_SendChar(char c)
{
	bool isOk = false;

	if (CircularBuffer_PutChar(&DebugUart_TxBuffStruct, c))
	{
		isOk = true;
		DebugUart_SendEnable();
	}

	return isOk;
}



/**
 * \brief	Send string on USART
 */
size_t DebugUart_SendMessage(const char *message)
{
	size_t length = 0;
	size_t putLength;

	length = StringLength(message);

	if (length == 0)
	{
		return 0;
	}

	// TODO: Need check return length?
	putLength = CircularBuffer_PutString(&DebugUart_TxBuffStruct, message, length);

	if (putLength > 0)
		DebugUart_SendEnable();

	return putLength;
}



/**
 * \brief Send message with newline
 */
size_t DebugUart_SendLine(const char *message)
{
	size_t length = 0;

	length += DebugUart_SendMessage(message);
	length += DebugUart_SendMessage("\r\n");

	return length;
}



/**
 * \brief	Send message with blocking mode
 * 			Use only extreme / important situation!
 */
size_t DebugUart_SendMessageBlocked(const char * str)
{
	size_t length = StringLength(str);

	// TODO: Clear statuses?
	//UART_ResetStatus(&DebugUart_Handle);
	//HAL_UART_AbortTransmit_IT()

	HAL_UART_Transmit(&DebugUart_Handle, (uint8_t *)str, length, 1000);

	return length;
}



/**
 * \brief	Function like printf(); Print on debug serial port
 * 			Copy character to buffer and after that, sending.
 */
size_t uprintf(const char *format, ...)
{
	// Working in at:
	char txBuffer[DEBUGUART_TX_BUFFER_SIZE];

#ifdef CONFIG_DEBUG_MODE
	txBuffer[DEBUGUART_TX_BUFFER_SIZE-1] = 0xEF;
#endif

	va_list ap;									// argument pointer
	va_start(ap, format); 						// ap on arg
	string_printf(txBuffer, format, ap);		// Separate and process
	va_end(ap);						 			// Cleaning after end

#ifdef CONFIG_DEBUG_MODE
	if (txBuffer[DEBUGUART_TX_BUFFER_SIZE-1] != 0xEF) DEBUG_BREAKPOINT();
#endif

	return DebugUart_SendMessage(txBuffer);		// Send on Usart
}



#if !defined(CONFIG_MODULE_TERMINAL_ENABLE)
/**
 * \brief	Process received characters (if Terminal is not enabled)
 */
void DebugUart_ProcessReceivedCharacters(void)
{
	// If WriteCnt not equal with ReadCnt, we have received message
	char receiveBuffer[DEBUGUART_RX_BUFFER_SIZE+1];

	// Received new character?
	if (CircularBuffer_IsNotEmpty(&DebugUart_RxBuffStruct))
	{
		// Need copy to receiveBuffer
		CircularBuffer_GetString(&DebugUart_RxBuffStruct, receiveBuffer, DEBUGUART_RX_BUFFER_SIZE);

		char * newLinePos = (char *)STRING_FindCharacters((const char *)receiveBuffer, "\r\n");
		if (newLinePos != NULL)
		{
			char responseBuffer[DEBUGUART_RESPONSE_BUFFER];
			responseBuffer[0] = '\0';

			*newLinePos = '\0';

			// Search command and run
			CmdH_Result_t cmdResult = CmdH_ExecuteCommand(
				(char *)receiveBuffer,
				responseBuffer, DEBUGUART_RESPONSE_BUFFER);

			CmdH_PrintResult(cmdResult);

			DebugUart_SendMessage(responseBuffer);

			// TODO: Create Get&Clear function
			CircularBuffer_ClearLast(&DebugUart_RxBuffStruct);
		}

		// TODO: Do not get all messages, which received. Only which are processed...
	}
}
#endif



#endif	// #ifdef CONFIG_MODULE_DEBUGUART_ENABLE
