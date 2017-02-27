/*
 *		usart.c
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		usart communication
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */


/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "include.h"
#include "board.h"
#include "USART.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
UART_HandleTypeDef Debug_UartHandle;
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
extern UART_HandleTypeDef ESP8266_UartHandle;
#endif


volatile char USART_RxBuffer[USART_RXBUFFERSIZE] = { 0 };
volatile char USART_TxBuffer[USART_TXBUFFERSIZE] = { 0 };

volatile uint8_t USART_RxBufferWriteCounter = 0;

bool USART_SendEnable_flag = false;


#if USART_RXBUFFERSIZE != 256
#warning "RxBufferCounter need to check!"
#endif


#ifdef CONFIG_USE_FREERTOS
xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
#endif



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local functions
 *----------------------------------------------------------------------------*/
static bool USART_WaitForSend(uint16_t timeoutMilliSecond);


/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/



/**
 * \brief	Initialize USART peripheral
 */
void USART_Init(UART_HandleTypeDef *UartHandle)
{
	
	// HW init, Port init, etc...
	//HAL_UART_MspInit(UartHandle);
	// TODO: It is called from HAL_UART_Init() function

	//##-1- Configure the UART peripheral
	// Put the USART peripheral in the Asynchronous mode (UART Mode)
	/* UARTx configured as follow:
	  - Word Length = 8 Bits
	  - Stop Bit = One Stop bit
	  - Parity = None
	  - BaudRate = 9600 baud
	  - Hardware flow control disabled (RTS and CTS signals) */
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	if (UartHandle == &Debug_UartHandle)
	{
		UartHandle->Instance      = DEBUG_USARTx;
		UartHandle->Init.BaudRate = DEBUG_USART_BAUDRATE;
	}
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	if (UartHandle == &ESP8266_UartHandle)
	{
		UartHandle->Instance      = ESP8266_USARTx;
		UartHandle->Init.BaudRate = ESP8266_USART_BAUDRATE;
	}
#endif
	
	UartHandle->Init.WordLength   = UART_WORDLENGTH_8B;
	UartHandle->Init.StopBits     = UART_STOPBITS_1;
	UartHandle->Init.Parity       = UART_PARITY_NONE;
	UartHandle->Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	UartHandle->Init.Mode         = UART_MODE_TX_RX;
	UartHandle->Init.OverSampling = UART_OVERSAMPLING_16;

	if (HAL_UART_Init(UartHandle) == HAL_OK)
	{	
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
		if (UartHandle == &Debug_UartHandle)
		{
			USART_SendEnable_flag = true;
			__HAL_UART_CLEAR_FLAG(&Debug_UartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);

		}
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
		if (UartHandle == &ESP8266_UartHandle)
		{
			__HAL_UART_CLEAR_FLAG(&ESP8266_UartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
		}
#endif
	}
	else	// != HAL_OK
	{	
		Error_Handler();
	}
}



/**
 * \brief	UART initialize (port - pins) for HAL driver
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	
	GPIO_InitTypeDef  GPIO_InitStruct;

#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	if (huart == &Debug_UartHandle)
	{
		// ##-1- Enable peripherals and GPIO Clocks

		// Enable GPIO TX/RX clock
		// Enable USARTx clock
		DEBUG_USART_CLK_ENABLES();

		// ##-2- Configure peripheral GPIO
		// UART TX GPIO pin configuration
		GPIO_InitStruct.Pin       = DEBUG_USART_TX_GPIO_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = DEBUG_USART_AF;		// It is initialize alternate function

		HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);

		// UART RX GPIO pin configuration
		GPIO_InitStruct.Pin = DEBUG_USART_RX_GPIO_PIN;
		//GPIO_InitStruct.Alternate = DEBUG_USART_AF;

		HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);


		// ##-3- Configure the NVIC for UART
		// NVIC for USARTx

		HAL_NVIC_SetPriority(DEBUG_USARTx_IRQn, DEBUG_USART_PREEMT_PRIORITY, DEBUG_USART_SUB_PRIORITY);
		HAL_NVIC_EnableIRQ(DEBUG_USARTx_IRQn);
	}
#endif	// #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	if (huart == &ESP8266_UartHandle)
	{
		// ##-1- Enable peripherals and GPIO Clocks

		// Enable GPIO TX/RX clock
		// Enable USARTx clock
		ESP8266_USART_CLK_ENABLES();

		
		// ##-2- Configure peripheral GPIO
		// UART TX GPIO pin configuration
		GPIO_InitStruct.Pin       = ESP8266_USART_TX_GPIO_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = ESP8266_USART_AF;

		HAL_GPIO_Init(ESP8266_USART_TX_GPIO_PORT, &GPIO_InitStruct);

		// UART RX GPIO pin configuration
		GPIO_InitStruct.Pin = ESP8266_USART_RX_GPIO_PIN;
		//GPIO_InitStruct.Alternate = ESP8266_USART_AF;

		HAL_GPIO_Init(ESP8266_USART_RX_GPIO_PORT, &GPIO_InitStruct);


		// ##-3- Configure the NVIC for UART
		// NVIC for USARTx

		HAL_NVIC_SetPriority(ESP8266_USARTx_IRQn, ESP8266_USART_PREEMT_PRIORITY, ESP8266_USART_SUB_PRIORITY);
		HAL_NVIC_EnableIRQ(ESP8266_USARTx_IRQn);
		
	}
#endif

}



#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
void DEBUG_USARTx_IRQHandler(void)
{
	HAL_UART_IRQHandler(&Debug_UartHandle);
}
#endif



#ifdef CONFIG_MODULE_ESP8266_ENABLE
void ESP8266_USARTx_IRQHandler(void)
{
	HAL_UART_IRQHandler(&ESP8266_UartHandle);
}
#endif



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	// For warning
	(void)UartHandle;

	// TODO: Check, which UART...

	// Set transmission flag: trasfer complete	
	#ifdef CONFIG_USE_FREERTOS
	xSemaphoreGiveFromISR(DEBUG_USART_Tx_Semaphore,(BaseType_t *)NULL);
	#endif
	
	// Successful sending

	USART_SendEnable_flag = true;

}



/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{

	#ifdef CONFIG_USE_FREERTOS
	taskDISABLE_INTERRUPTS();
	#endif

	#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	if ( (UartHandle == &Debug_UartHandle) && (Terminal_CommandReceiveEnable == true) )
	{
		// Receive to next index
		HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)&USART_RxBuffer[++USART_RxBufferWriteCounter], RXBUFFER_WAIT_LENGTH);

		#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE
		// Set flag for EventHandler
		EventHandler_SetEventFlag(Event_DebugUartReceivedCommand, EVENT_RUN);
		#endif

		#ifdef CONFIG_USE_FREERTOS
		// Transmission end semaphore / flag: Give semaphore
		xSemaphoreGiveFromISR(DEBUG_USART_Rx_Semaphore,0);
		#endif

	}
	#endif	// #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	
	#ifdef CONFIG_MODULE_ESP8266_ENABLE
	if (UartHandle == &ESP8266_UartHandle)
	{
		
		if (ESP8266_Receive_Mode_FixLength == 1)
		{
			// Received fix length message
			#ifdef CONFIG_USE_FREERTOS
			xSemaphoreGiveFromISR(ESP8266_USART_Rx_Semaphore, 0);
			#endif
		}
		else
		{
			// Received variable length message
			// Put to Buffer and receive next char

			// Save received character and wait new char
#if ESP8266_RECEIVE_LENGTH > 1
			HAL_UART_Receive_IT(&ESP8266_UartHandle,
					(uint8_t *)&ESP8266_ReceiveBuffer[0],
					ESP8266_RECEIVE_LENGTH);
#else
			HAL_UART_Receive_IT(&ESP8266_UartHandle,
					(uint8_t *)&ESP8266_ReceiveBuffer[++ESP8266_ReceiveBuffer_WriteCnt],
					1);

			if (ESP8266_ReceiveBuffer[ESP8266_ReceiveBuffer_WriteCnt-1] == '\n')
			{
				// received an '\n' and not too short message
				#ifdef CONFIG_USE_FREERTOS
				xSemaphoreGiveFromISR(ESP8266_USART_Rx_Semaphore, 0);
				#else
				EventHandler_SetEventFlag(Event_Esp8266ReceivedMessage, EVENT_RUN);
				#endif
			}
#endif
		}
	
		#ifndef CONFIG_USE_FREERTOS
		// Not used FreeRTOS:
		//ESP8266_Uart_ReceivedCharFlag = 1;
		#endif
		
	}
	#endif

	#ifdef CONFIG_USE_FREERTOS
	taskENABLE_INTERRUPTS();
	#endif
	
}



/**
 * \brief	UART error function
 * 			If has error, the HAL driver call this function
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{

#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	if (huart == &Debug_UartHandle)
	{

		// TODO: sometime receive ORE error.

		UART_ResetStatus(&Debug_UartHandle);

		// Reinitialize USART
		//USART_Init(&Debug_UartHandle);

		HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)&USART_RxBuffer[USART_RxBufferWriteCounter], RXBUFFER_WAIT_LENGTH);
		HAL_UART_Transmit_IT(&Debug_UartHandle, (uint8_t *)"$", 1);
		
		#ifdef CONFIG_USE_FREERTOS
		//xSemaphoreGiveFromISR(DEBUG_USART_Rx_Semaphore,0);
		if (DEBUG_USART_Tx_Semaphore != NULL)
		{
			xSemaphoreGiveFromISR(DEBUG_USART_Tx_Semaphore, 0);
		}
		#endif
		
	}
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	else if (huart == &ESP8266_UartHandle)
	{

		// TODO: Sad, but sometime receive errors
		UART_ResetStatus(&ESP8266_UartHandle);

#if ESP8266_RECEIVE_LENGTH > 1
		HAL_UART_Receive_IT(&ESP8266_UartHandle,
				(uint8_t *)&ESP8266_ReceiveBuffer[0],
				ESP8266_RECEIVE_LENGTH);
#else
		HAL_UART_Receive_IT(&ESP8266_UartHandle,
						(uint8_t *)&ESP8266_ReceiveBuffer[ESP8266_ReceiveBuffer_WriteCnt],
						1);
#endif
	}	
#endif
	else
	{
		Error_Handler();
	}
	
	return;
}



/**
 * \brief	Clear UART peripheral status
 * \note	Be careful for use, it will clear the receive-sending status
 */
void UART_ResetStatus(UART_HandleTypeDef *huart)
{

	// Delete previous receive:
	__HAL_UART_FLUSH_DRREGISTER(huart);
	__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);

	huart->ErrorCode = HAL_UART_ERROR_NONE;
	huart->gState = HAL_UART_STATE_READY;

	huart->TxXferCount = 0;
	huart->TxXferSize = 0;
	huart->RxXferCount = 0;
	huart->RxXferSize = 0;

}



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


	#endif // #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
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


	#endif // #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
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



/**
 * \brief	Send float number on usart
 */
void USART_SendFloat(float value)
{
	char string[16];
	
	FloatToString(value, string, 0, 6);
	
	USART_SendMessage(string);
}



/*
void USART_Test (void)
{

	USART_Init();

	USART_SendMessage ("Blablalba\r\n");

}
*/
