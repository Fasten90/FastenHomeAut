/*
 *		USART.c
 *		Created on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		USART communication
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "include.h"
#include "board.h"
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
#include "DebugUart.h"
#include "Terminal.h"
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
#include "ESP8266.h"
#endif
#ifdef CONFIG_MODULE_BLUETOOTH_ENABLE
#include "Bluetooth_HC05.h"
#endif
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



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * \brief	Initialize USART peripheral
 */
void UART_Init(UART_HandleTypeDef *UartHandle)
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
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
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
#ifdef CONFIG_MODULE_BLUETOOTH_ENABLE
	if (UartHandle == &Bluetooth_UartHandle)
	{
		UartHandle->Instance      = BLUETOOTH_USARTx;
		UartHandle->Init.BaudRate = BLUETOOTH_USART_BAUDRATE;
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
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
		if (UartHandle == &Debug_UartHandle)
		{
			DebugUart_SendEnable_flag = true;
			__HAL_UART_CLEAR_IT(&Debug_UartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
		}
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
		if (UartHandle == &ESP8266_UartHandle)
		{
			__HAL_UART_CLEAR_FLAG(&ESP8266_UartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
		}
#endif
#ifdef CONFIG_MODULE_BLUETOOTH_ENABLE
		if (UartHandle == &Bluetooth_UartHandle)
		{
			__HAL_UART_CLEAR_IT(&Bluetooth_UartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
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
#if defined(CONFIG_MODULE_DEBUGUART_ENABLE) || defined(CONFIG_MODULE_ESP8266_ENABLE)
	GPIO_InitTypeDef  GPIO_InitStruct;
#endif

#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
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
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
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
#endif	// #ifdef CONFIG_MODULE_DEBUGUART_ENABLE
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
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
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
#endif	// #ifdef CONFIG_MODULE_ESP8266_ENABLE
#ifdef CONFIG_MODULE_BLUETOOTH_ENABLE
	if (huart == &Bluetooth_UartHandle)
	{
		// ##-1- Enable peripherals and GPIO Clocks

		// Enable GPIO TX/RX clock
		// Enable USARTx clock
		BLUETOOTH_USART_CLK_ENABLES();


		// ##-2- Configure peripheral GPIO
		// UART TX GPIO pin configuration
		GPIO_InitStruct.Pin       = BLUETOOTH_USART_TX_GPIO_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = BLUETOOTH_USART_AF;

		HAL_GPIO_Init(BLUETOOTH_USART_TX_GPIO_PORT, &GPIO_InitStruct);

		// UART RX GPIO pin configuration
		GPIO_InitStruct.Pin = BLUETOOTH_USART_RX_GPIO_PIN;
		//GPIO_InitStruct.Alternate = BLUETOOTH_USART_AF;

		HAL_GPIO_Init(BLUETOOTH_USART_RX_GPIO_PORT, &GPIO_InitStruct);


		// ##-3- Configure the NVIC for UART
		// NVIC for USARTx

		HAL_NVIC_SetPriority(BLUETOOTH_USARTx_IRQn, BLUETOOTH_USART_PREEMT_PRIORITY, BLUETOOTH_USART_SUB_PRIORITY);
		HAL_NVIC_EnableIRQ(BLUETOOTH_USARTx_IRQn);

	}
#endif	// #ifdef CONFIG_MODULE_BLUETOOTH_ENABLE
}



#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
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



#ifdef CONFIG_MODULE_BLUETOOTH_ENABLE
void BLUETOOTH_USARTx_IRQHandler(void)
{
	HAL_UART_IRQHandler(&Bluetooth_UartHandle);
}
#endif



/**
 * \brief	HAL driver function - Uart Tx (transmission complete) callback function
 * 			Set send successful flags
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
#if !defined(CONFIG_MODULE_DEBUGUART_ENABLE) && !defined(CONFIG_MODULE_ESP8266_ENABLE) && !defined(CONFIG_MODULE_BLUETOOTH_ENABLE)
	// Suppress warning
	(void)UartHandle;
#endif
	
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
	// Successful sending
	if (UartHandle == &Debug_UartHandle)
	{
		// Set transmission flag: transfer complete
		#ifdef CONFIG_USE_FREERTOS
		xSemaphoreGiveFromISR(DebugUart_Tx_Semaphore, (BaseType_t *)NULL);
		#endif
		DebugUart_SendEnable_flag = true;
	}
#endif

#ifdef CONFIG_MODULE_ESP8266_ENABLE
	if (UartHandle == &ESP8266_UartHandle)
	{
		ESP8266_SendEnable_flag = true;
	}
#endif

#ifdef CONFIG_MODULE_BLUETOOTH_ENABLE
	if (UartHandle == &Bluetooth_UartHandle)
	{
		Bluetooth_SendEnable_flag = true;
	}
#endif
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

	#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
	if ((UartHandle == &Debug_UartHandle) && (DebugUart_CommandReceiveEnable == true))
	{
		#ifdef CONFIG_DEBUGUSART_MODE_ONEPERONERCHARACTER
		// Receive to next index
		HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)&DebugUart_RxBuffer[++USART_RxBufferWriteCounter], DEBUGUART_RXBUFFER_WAIT_LENGTH);

		#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
		// Set flag for TaskHandler
		TaskHandler_RequestTaskScheduling(Task_ProcessDebugUartReceivedCommand);
		#endif

		#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
		EventLog_LogEvent(Event_DebugUartReceive, EventType_UserEvent, USART_RxBufferWriteCounter);
		#endif

		#ifdef CONFIG_USE_FREERTOS
		// Transmission end semaphore / flag: Give semaphore
		xSemaphoreGiveFromISR(DebugUart_Rx_Semaphore, 0);
		#endif
		#else
		HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)DebugUart_RxBuffer, DEBUGUART_RX_BUFFER_SIZE);
		#endif
	}
	#endif	// #ifdef CONFIG_MODULE_DEBUGUART_ENABLE
	

	#ifdef CONFIG_MODULE_ESP8266_ENABLE
	if (UartHandle == &ESP8266_UartHandle)
	{
		// ESP8266 WiFi module

		// Received fix length message
		#ifdef CONFIG_USE_FREERTOS
		xSemaphoreGiveFromISR(ESP8266_USART_Rx_Semaphore, 0);
		#endif

		// Received variable length message
		// Put to Buffer and receive next char

		// Save received character and wait new char
		#if ESP8266_RECEIVE_LENGTH > 1
		HAL_UART_Receive_IT(&ESP8266_UartHandle,
				(uint8_t *)&ESP8266_RxBuffer[0],
				ESP8266_RECEIVE_LENGTH);
		#else
		HAL_UART_Receive_IT(&ESP8266_UartHandle,
				(uint8_t *)&ESP8266_RxBuffer[++ESP8266_RxBuffer_WriteCnt],
				1);
		#endif
	
		#if !defined(CONFIG_USE_FREERTOS) && defined(ESP8266_USE_BLOCK_MODE)
		// Not used FreeRTOS:
		ESP8266_Uart_ReceivedCharFlag = 1;
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
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
	if (huart == &Debug_UartHandle)
	{
		// TODO: sometime receive ORE error.

		UART_ResetStatus(&Debug_UartHandle);

		DebugUart_SendEnable_flag = true;

		// Reinitialize USART
		//UART_Init(&Debug_UartHandle);

		DebugUart_StartReceive();

		HAL_UART_Transmit_IT(&Debug_UartHandle, (uint8_t *)"$", 1);
		
		#ifdef CONFIG_USE_FREERTOS
		//xSemaphoreGiveFromISR(DebugUart_Rx_Semaphore,0);
		if (DebugUart_Tx_Semaphore != NULL)
		{
			xSemaphoreGiveFromISR(DebugUart_Tx_Semaphore, 0);
		}
		#endif
	}
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
	else
	#endif
	if (huart == &ESP8266_UartHandle)
	{
		// TODO: Sad, but sometime receive errors
		UART_ResetStatus(&ESP8266_UartHandle);

		ESP8266_SendEnable_flag = true;

#if ESP8266_RECEIVE_LENGTH > 1
		HAL_UART_Receive_IT(&ESP8266_UartHandle,
				(uint8_t *)&ESP8266_RxBuffer[0],
				ESP8266_RECEIVE_LENGTH);
#else
		HAL_UART_Receive_IT(&ESP8266_UartHandle,
						(uint8_t *)&ESP8266_RxBuffer[ESP8266_RxBuffer_WriteCnt],
						1);
#endif
	}	
#endif
#ifdef CONFIG_MODULE_BLUETOOTH_ENABLE
	else if (huart == &Bluetooth_UartHandle)
	{
		// TODO: Not handled...
		Bluetooth_SendEnable_flag = true;
		UART_ResetStatus(&Bluetooth_UartHandle);
	}
#endif

#if defined(CONFIG_MODULE_DEBUGUART_ENABLE) || defined(CONFIG_MODULE_ESP8266_ENABLE) || defined(CONFIG_MODULE_BLUETOOTH_ENABLE)
	else
	{
		Error_Handler();
	}
#endif
}



/**
 * \brief	Clear UART peripheral status
 * \note	Be careful for use, it will clear the receive-sending status
 */
void UART_ResetStatus(UART_HandleTypeDef *huart)
{
	// Delete previous receive:
	__HAL_UART_FLUSH_DRREGISTER(huart);
	__HAL_UART_CLEAR_IT(huart, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);

	huart->ErrorCode = HAL_UART_ERROR_NONE;
	huart->gState = HAL_UART_STATE_READY;

	huart->TxXferCount = 0;
	huart->TxXferSize = 0;
	huart->RxXferCount = 0;
	huart->RxXferSize = 0;
}
