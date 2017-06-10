/*
 *		DebugUart.c
 *
 *		Created on:		2017. márc. 15.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. márc. 15.
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include <stdarg.h>		// for "..." parameters in printf function
#include "String.h"
#include "Globals.h"
#include "DebugUart.h"


#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

UART_HandleTypeDef Debug_UartHandle;

volatile char DebugUart_RxBuffer[DEBUGUART_RXBUFFERSIZE] = { 0 };
volatile char DebugUart_TxBuffer[DEBUGUART_TXBUFFERSIZE] = { 0 };

volatile uint8_t DebugUart_RxBufferWriteCnt = 0;
volatile uint8_t DebugUart_RxBufferReadCnt = 0;

bool DebugUart_SendEnable_flag = false;


#if DEBUGUART_RXBUFFERSIZE != 256
#warning "RxBufferCounter need to check!"
#endif


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
 * \brief	Send string on USART
 */
uint8_t DebugUart_SendMessage(const char *aTxBuffer)
{
	uint8_t length = 0;

	length = StringLength(aTxBuffer);

	if (length == 0)
	{
		return 0;
	}
#if DEBUGUART_TXBUFFERSIZE < 256
	if (length > DEBUGUART_TXBUFFERSIZE)
	{
		length = DEBUGUART_TXBUFFERSIZE - 1;
	}
#endif

	if (DebugUart_WaitForSend(1000))
	{
		// Take semaphore, can sending

		DebugUart_SendEnable_flag = false;

		StrCpyMax((char *)DebugUart_TxBuffer, aTxBuffer, DEBUGUART_TXBUFFERSIZE-1);

		// ComIT
		if (HAL_UART_Transmit_IT(&Debug_UartHandle, (uint8_t*)DebugUart_TxBuffer, length)!= HAL_OK)
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
 * \brief	Send message with newline
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

		if (HAL_UART_Transmit_IT(&Debug_UartHandle, (uint8_t *)DebugUart_TxBuffer, 1)!= HAL_OK)
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
		// Failed to take semaphore
		return false;
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
	HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)DebugUart_RxBuffer, DEBUGUART_RXBUFFERSIZE);
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

	DebugUart_SendEnable_flag = true;

	return true;
#endif
}



/**
 * \brief	Function like printf(); Print on debug serial port
 * 			Copy character to buffer and after that, sending.
 */
uint8_t uprintf(const char *format, ...)
{
	// Working in at:
	char TxBuffer[DEBUGUART_TXBUFFERSIZE];

	va_list ap;									// argument pointer
	va_start(ap, format); 						// ap on arg
	string_printf(TxBuffer, format,ap);			// Separate and process
	va_end(ap);						 			// Cleaning after end

	return DebugUart_SendMessage(TxBuffer);		// Send on Usart
}

#endif	// #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
