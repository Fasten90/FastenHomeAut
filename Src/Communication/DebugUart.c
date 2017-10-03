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

#include "options.h"
#include <stdarg.h>		// for "..." parameters in printf function
#include "String.h"
#include "Globals.h"
#include "CircularBuffer.h"
#include "Communication.h"
#include "CommandHandler.h"
#include "DebugUart.h"


#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

UART_HandleTypeDef Debug_UartHandle;

volatile char DebugUart_RxBuffer[DEBUGUART_RX_BUFFER_SIZE] = { 0 };
volatile char DebugUart_TxBuffer[DEBUGUART_TX_BUFFER_SIZE] = { 0 };

CircularBufferInfo_t DebugUart_RxBuffStruct =
{
	.buffer = (char *)DebugUart_RxBuffer,
	.name = "DebugUart_RxBuffer",
	.size = DEBUGUART_RX_BUFFER_SIZE
};


///< Sending enable flag
bool DebugUart_SendEnable_flag = false;

///< Enable command receiving
const bool DebugUart_CommandReceiveEnable = true;


#if defined(CONFIG_USE_FREERTOS)
xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
#endif



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static bool DebugUart_WaitForSend(uint16_t timeoutMilliSecond);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	DebugUart init
 */
void DebugUart_Init(void)
{
	CircularBuffer_Init(&DebugUart_RxBuffStruct);
}



/**
 * \brief	Send string on USART
 */
uint8_t DebugUart_SendMessage(const char *message)
{
	uint16_t length = 0;

	length = StringLength(message);

	if (length == 0)
	{
		return 0;
	}

	if (length > DEBUGUART_TX_BUFFER_SIZE)
	{
		length = DEBUGUART_TX_BUFFER_SIZE - 1;
	}

	if (DebugUart_WaitForSend(1000))
	{
		// Take semaphore, can sending

		DebugUart_SendEnable_flag = false;

		StrCpyMax((char *)DebugUart_TxBuffer, message, DEBUGUART_TX_BUFFER_SIZE-1);

		// ComIT
		if (HAL_UART_Transmit_IT(&Debug_UartHandle, (uint8_t *)DebugUart_TxBuffer, length) != HAL_OK)
		{
			// NOTE: !!IMPORTANT!! Not sent message
			//Error_Handler();
#ifdef CONFIG_USE_FREERTOS
			xSemaphoreGive(DEBUG_USART_Tx_Semaphore);
#endif
			DebugUart_SendEnable_flag = true;	// Failed to send, now we can send message

			return 0;
		}
		else
		{
			// Successful sending with IT
			// Semaphore give by IT routine
			return length;
		}
	}
	else
	{
		// Cannot take semaphore, now USART is busy
		return 0;
	}
}



/**
 * \brief Send newline
 */
bool DebugUart_SendNewLine(void)
{
	return DebugUart_SendMessage("\r\n");
}



/**
 * \brief Send message with newline
 */
bool DebugUart_SendLine(const char *message)
{
	bool isSuccessful = true;

	isSuccessful &= DebugUart_SendMessage(message);
	isSuccessful &= DebugUart_SendNewLine();

	return isSuccessful;
}



/**
 * \brief	Send a char on USART
 */
bool DebugUart_SendChar(char c)
{
	char buf[2];

	buf[0] = c;
	buf[1] = '\0';

	if (DebugUart_WaitForSend(100))
	{
		// Successful take USART semaphore
		DebugUart_SendEnable_flag = false;

		StrCpy((char *)DebugUart_TxBuffer, buf);

		if (HAL_UART_Transmit_IT(&Debug_UartHandle, (uint8_t *)DebugUart_TxBuffer, 1) != HAL_OK)
		{
			// NOTE: !! IMPORTANT!! Not sent message
			//Error_Handler();
#ifdef CONFIG_USE_FREERTOS
			xSemaphoreGive(DEBUG_USART_Tx_Semaphore);
#endif
			DebugUart_SendEnable_flag = true;

			return false;
		}
		else
		{
			// Successful sending on USART
			// Semaphore will give from ISR
			return true;
		}
	}
	else
	{
		return false;	// Failed to take semaphore
	}
}



/**
 * \brief	Receive message with IT
 */
void DebugUart_StartReceive(void)
{
	// USART - Receive Message
#ifdef CONFIG_DEBUGUSART_MODE_ONEPERONERCHARACTER
	HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)&DebugUart_RxBuffer[DebugUart_RxBufferWriteCnt], DEBUGUART_RXBUFFER_WAIT_LENGTH);
#else
	HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)DebugUart_RxBuffer, DEBUGUART_RX_BUFFER_SIZE);
#endif

#ifdef CONFIG_USE_FREERTOS
	// Wait for semaphore
	xSemaphoreTake(DEBUG_USART_Rx_Semaphore, (portTickType) 1000);
#endif
}



/**
 * \brief	Wait for USART sending
 */
static bool DebugUart_WaitForSend(uint16_t timeoutMilliSecond)
{
#ifdef CONFIG_USE_FREERTOS
	if (xSemaphoreTake(DEBUG_USART_Tx_Semaphore, (portTickType)timeoutMilliSecond) == pdPASS)
	{
		return true;
	}
	else
	{
		return false;
	}
#else
	// Wait for flag or timeout
	while ((DebugUart_SendEnable_flag != true) || (timeoutMilliSecond == 0))
	{
		timeoutMilliSecond--;
		DelayMs(1);
	}

	// TODO: Not a good idea...
	//DebugUart_SendEnable_flag = true;

	return true;
#endif
}



#if !defined(CONFIG_MODULE_TERMINAL_ENABLE)
/**
 * \brief	Process received characters
 */
void DebugUart_ProcessReceivedCharacters(void)
{
	if (DebugUart_CommandReceiveEnable)
	{
		// Find new received characters
		CircularBuffer_FindLastMessage(&DebugUart_RxBuffStruct);

		// If WriteCnt not equal with ReadCnt, we have received message
		char receiveBuffer[DEBUGUART_RX_BUFFER_SIZE+1];

		// Received new character?
		if (CircularBuffer_HasNewMessage(&DebugUart_RxBuffStruct))
		{
			// Need copy to receiveBuffer
			CircularBuffer_GetCharacters(
					&DebugUart_RxBuffStruct,
					receiveBuffer,
					true);

			// TODO: Do more beautiful solution
			int16_t newLinePos = STRING_FindString(receiveBuffer, "\r");
			if (newLinePos < 0)
			{
				newLinePos = STRING_FindString(receiveBuffer, "\n");
			}

			if (newLinePos > 0)
			{
				receiveBuffer[newLinePos] = '\0';
					// Search command and run
					CommandHandler_PrepareFindExecuteCommand(
						CommProt_DebugUart, (char *)receiveBuffer);

					// TODO: Create Get&Clear function
					CircularBuffer_ClearLast(&DebugUart_RxBuffStruct);
			}

			// TODO: Do not get all messages, which received. Only which are processed...
		}
	}
}
#endif



/**
 * \brief	Function like printf(); Print on debug serial port
 * 			Copy character to buffer and after that, sending.
 */
uint8_t uprintf(const char *format, ...)
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



#endif	// #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
