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
#include "include.h"
#include "DebugUart.h"


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

UART_HandleTypeDef Debug_UartHandle;

volatile char USART_RxBuffer[USART_RXBUFFERSIZE] = { 0 };
volatile char USART_TxBuffer[USART_TXBUFFERSIZE] = { 0 };

volatile uint8_t USART_RxBufferWriteCounter = 0;

bool USART_SendEnable_flag = false;


#if USART_RXBUFFERSIZE != 256
#warning "RxBufferCounter need to check!"
#endif


#if defined(CONFIG_USE_FREERTOS) && defined(CONFIG_MODULE_DEBUGUSART_ENABLE)
xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
#endif



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static bool USART_WaitForSend(uint16_t timeoutMilliSecond);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * \brief	Send string on USART
 */
uint8_t USART_SendMessage(const char *aTxBuffer)
{
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	uint8_t length = 0;

	length = StringLength(aTxBuffer);

	if (length == 0)
	{
		return 0;
	}
#if USART_TXBUFFERSIZE < 256
	if (length > USART_TXBUFFERSIZE)
	{
		length = USART_TXBUFFERSIZE - 1;
	}
#endif

	if (USART_WaitForSend(1000))
	{
		// Take semaphore, can sending

		USART_SendEnable_flag = false;

		StrCpy((char *)USART_TxBuffer, aTxBuffer);

		// ComIT
		if (HAL_UART_Transmit_IT(&Debug_UartHandle, (uint8_t*)USART_TxBuffer, length)!= HAL_OK)
		{
			// NOTE: !!IMPORTANT!! Not sent message
			//Error_Handler();
			#ifdef CONFIG_USE_FREERTOS
			xSemaphoreGive(DEBUG_USART_Tx_Semaphore);
			#endif
			USART_SendEnable_flag = true;	// Failed to send, now we can send message

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


#else // #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	return 0;
#endif
}



/**
 * \brief Send newline
 */
bool USART_SendNewLine(void)
{
	return USART_SendMessage("\r\n");
}



/**
 * \brief	Send message with newline
 */
bool USART_SendLine(const char *message)
{
	bool isSuccessful = true;
	isSuccessful &= USART_SendMessage(message);
	isSuccessful &= USART_SendNewLine();
	return isSuccessful;
}



/**
 * \brief	Send a char on USART
 */
bool USART_SendChar(char c)
{
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	char buf[2];
	buf[0] = c;
	buf[1] = '\0';


	if (USART_WaitForSend(100))
	{
		// Successful take USART semaphore
		USART_SendEnable_flag = false;

		StrCpy((char *)USART_TxBuffer, buf);

		if (HAL_UART_Transmit_IT(&Debug_UartHandle, (uint8_t *)USART_TxBuffer, 1)!= HAL_OK)
		{
			// NOTE: !! IMPORTANT!! Not sent message
			//Error_Handler();
			#ifdef CONFIG_USE_FREERTOS
			xSemaphoreGive(DEBUG_USART_Tx_Semaphore);
			#endif
			USART_SendEnable_flag = true;
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


#else // #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	return false;
#endif
}



/**
 * \brief	Receive message with IT
 */
 #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
void USART_StartReceiveMessage(void)
{

	// USART - Receive Message
	HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)&USART_RxBuffer[USART_RxBufferWriteCounter], RXBUFFER_WAIT_LENGTH);


	#ifdef CONFIG_USE_FREERTOS
	// Wait for semaphore
	xSemaphoreTake(DEBUG_USART_Rx_Semaphore, (portTickType) 1000);
	#endif

}
#endif	// #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE



/**
 * \brief	Wait for USART sending
 */
static bool USART_WaitForSend(uint16_t timeoutMilliSecond)
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
	while ((USART_SendEnable_flag != true) || (timeoutMilliSecond == 0))
	{
		timeoutMilliSecond--;
		DelayMs(1);
	}

	USART_SendEnable_flag = true;

	return true;
#endif
}
