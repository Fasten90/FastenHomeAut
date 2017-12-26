/*
 *		USART.c
 *		Created on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		USART communication
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
 *		Source:			https://www.eevblog.com/forum/microcontrollers/%27best%27-way-to-load-UART_Handler_t-data-to-ring-buffer-with-stm32hal/
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
#include "UART.h"



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
 *  Static function prototypes
 *----------------------------------------------------------------------------*/

static void UART_Handler(UART_Handler_t *handler);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * \brief	Initialize USART peripheral
 */
void UART_Init(UART_HandleTypeDef *UartHandle)
{
	
	// HW init, Port init, etc...
	//HAL_UART_MspInit(UartHandle);			// It is called from HAL_UART_Init() function


	//##-1- Configure the UART peripheral
	// Put the USART peripheral in the Asynchronous mode (UART Mode)
	/* UARTx configured as follow:
	  - Word Length = 8 Bits
	  - Stop Bit = One Stop bit
	  - Parity = None
	  - BaudRate = 9600 baud
	  - Hardware flow control disabled (RTS and CTS signals) */
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
	if (UartHandle == &DebugUart_Handle)
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
		if (UartHandle == &DebugUart_Handle)
		{
			__HAL_UART_CLEAR_IT(&DebugUart_Handle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
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
		//__HAL_UART_ENABLE_IT(UartHandle, UART_IT_RXNE);		/* receiver not empty */
		__HAL_UART_DISABLE_IT(UartHandle, UART_IT_TXE);			/* transmit empty */
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
#if defined(CONFIG_MODULE_DEBUGUART_ENABLE) || defined(CONFIG_MODULE_ESP8266_ENABLE) || defined(CONFIG_MODULE_BLUETOOTH_ENABLE)
	GPIO_InitTypeDef  GPIO_InitStruct;
#endif

#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
	if (huart == &DebugUart_Handle)
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



// TODO: Put to DebugUart ?
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
void DEBUG_USARTx_IRQHandler(void)
{
	UART_Handler(&DebugUart);
}
#endif



#ifdef CONFIG_MODULE_ESP8266_ENABLE
void ESP8266_USARTx_IRQHandler(void)
{
	// TODO:
	HAL_UART_IRQHandler(&ESP8266_UartHandle);
}
#endif



#ifdef CONFIG_MODULE_BLUETOOTH_ENABLE
void BLUETOOTH_USARTx_IRQHandler(void)
{
	// TODO:
	HAL_UART_IRQHandler(&Bluetooth_UartHandle);
}
#endif



/**
 * \brief	UART handler (Handle rx - tx)
 */
static void UART_Handler(UART_Handler_t *handler)
{
	UART_HandleTypeDef *huart;
	uint32_t tmp1, tmp2;
	bool err;

	err = false;
	huart = handler->huart;

	/* UART parity error interrupt occurred ------------------------------------*/
	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);
	if ((tmp1 != RESET) && (tmp2 != RESET))
	{
		__HAL_UART_CLEAR_PEFLAG(huart);
		err = true;
	}

	/* UART frame error interrupt occurred -------------------------------------*/
	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	if ((tmp1 != RESET) && (tmp2 != RESET))
	{
		__HAL_UART_CLEAR_FEFLAG(huart);
		err = true;
	}

	/* UART noise error interrupt occurred -------------------------------------*/
	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	if ((tmp1 != RESET) && (tmp2 != RESET))
	{
		__HAL_UART_CLEAR_NEFLAG(huart);
		err = true;
	}

	/* UART Over-Run interrupt occurred ----------------------------------------*/
	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	if ((tmp1 != RESET) && (tmp2 != RESET))
	{
		__HAL_UART_CLEAR_OREFLAG(huart);
		err = true;
	}

	/* UART in mode Receiver ---------------------------------------------------*/
	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
	if ((tmp1 != RESET) && (tmp2 != RESET))
	{
		uint16_t val;

		val = (uint16_t)(huart->Instance->RDR);

		/* don't put errored data into the FIFO */
		if (!err)
		{
			// TODO: Check rxEnable flag
			CircularBuffer_PutChar(handler->rx, val);
			// TODO: TaskHandler_Request or CallBack, which use semaphore and TaskHandler request?
		}
	}

	/* UART in mode Transmitter ------------------------------------------------*/
	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_TXE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TXE);
	if ((tmp1 != RESET) && (tmp2 != RESET))
	{
		char val;

		/*
		* if there's data to send, send it.
		* otherwise disable the transmit empty interrupt.
		*/
		// TODO: Check txEnable flag
		if (CircularBuffer_GetChar(handler->tx, &val))
		{
			huart->Instance->TDR = val;
		}
		else
		{
			__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
		}
	}
}



/**
 * \brief	Send enable (IT)
 */
void UART_SendEnable(UART_Handler_t * handler)
{
	__HAL_UART_ENABLE_IT(handler->huart, UART_IT_TXE);
}



/**
 * \brief	Receive enable (IT)
 */
void UART_ReceiveEnable(UART_Handler_t * handler)
{
	__HAL_UART_ENABLE_IT(handler->huart, UART_IT_RXNE);		/* receiver not empty */
}
