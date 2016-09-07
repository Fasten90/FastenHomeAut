/*******************************************************************************
 * Purpose:		USART periferia
 ******************************************************************************/


/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "include.h"
#include "board.h"

#include "usart.h"
#include "monitor.h"

#include <stdarg.h>		// for "..." parameters in uprintf function




/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_ESP8266_ENABLE
extern UART_HandleTypeDef ESP8266_UartHandle;
#endif
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
UART_HandleTypeDef Debug_UartHandle;
#endif


volatile char USART_RxBuffer[RXBUFFERSIZE] = { 0 };
volatile char USART_TxBuffer[TXBUFFERSIZE] = { 0 };

volatile uint8_t USART_RxBufferWriteCounter = 0;


#if RXBUFFERSIZE != 256
#warning "RxBufferCounter need to check!"
#endif


uint8_t USART_SendEnable_flag = 0;


//extern uint8_t ESP8266_Uart_ReceivedCharFlag;


/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/
//static uint8_t StrCpy ( const uint8_t *string, uint8_t *dest);	// uint8_t pos


/*------------------------------------------------------------------------------
 *  Local functions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/
extern void Error_Handler( void );




/***************************************************************************//**
 * @brief		-	USART_Init:	Inicializalja az USART periferiat
 ******************************************************************************/
void USART_Init ( UART_HandleTypeDef *UartHandle)
{
	
	// HW init, Port init, etc...
	//HAL_UART_MspInit(UartHandle);
	// TODO: It is called from HAL_UART_Init() function

	//##-1- Configure the UART peripheral ######################################
	// Put the USART peripheral in the Asynchronous mode (UART Mode)
	/* UARTx configured as follow:
	  - Word Length = 8 Bits
	  - Stop Bit = One Stop bit
	  - Parity = None
	  - BaudRate = 9600 baud
	  - Hardware flow control disabled (RTS and CTS signals) */
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	if ( UartHandle == &Debug_UartHandle)
	{
		UartHandle->Instance        = DEBUG_USARTx;
		UartHandle->Init.BaudRate   = 9600;		// Monitor program
	}
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	if ( UartHandle == &ESP8266_UartHandle)
	{
		UartHandle->Instance        = ESP8266_USARTx;
		UartHandle->Init.BaudRate   = 9600;			// ESP8266
	}
#endif
	
	UartHandle->Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle->Init.StopBits   = UART_STOPBITS_1;
	UartHandle->Init.Parity     = UART_PARITY_NONE;
	UartHandle->Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle->Init.Mode       = UART_MODE_TX_RX;
	UartHandle->Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(UartHandle) == HAL_OK)
	{	
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
		if ( UartHandle == &Debug_UartHandle)
		{
			USART_SendEnable_flag = ENABLE;
			__HAL_UART_CLEAR_FLAG(&Debug_UartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);

		}
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
		if ( UartHandle == &ESP8266_UartHandle)
		{
			//USART_SendEnable_flag = ENABLE;
			__HAL_UART_CLEAR_FLAG(&ESP8266_UartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
		}
#endif
	}
	else	// != HAL_OK
	{	
		Error_Handler();
	}
	
	

}




void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	
	
	GPIO_InitTypeDef  GPIO_InitStruct;

	
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE

	if (huart == &Debug_UartHandle)
	{
		// ##-1- Enable peripherals and GPIO Clocks #################################

		// Enable GPIO TX/RX clock
		// Enable USARTx clock
		DEBUG_USART_CLK_ENABLES();

		// ##-2- Configure peripheral GPIO ##########################################
		// UART TX GPIO pin configuration
		GPIO_InitStruct.Pin       = DEBUG_USART_TX_GPIO_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = DEBUG_USART_AF;		// It is initialie alternate function

		HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);

		// UART RX GPIO pin configuration
		GPIO_InitStruct.Pin = DEBUG_USART_RX_GPIO_PIN;
		//GPIO_InitStruct.Alternate = DEBUG_USART_AF;

		HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);


		// ##-3- Configure the NVIC for UART ########################################
		// NVIC for USARTx

		HAL_NVIC_SetPriority(DEBUG_USARTx_IRQn, DEBUG_USART_PREEMT_PRIORITY, DEBUG_USART_SUB_PRIORITY);
		HAL_NVIC_EnableIRQ(DEBUG_USARTx_IRQn);
	}
#endif	// #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	if ( huart == &ESP8266_UartHandle)
	{
		// ##-1- Enable peripherals and GPIO Clocks #################################

		// Enable GPIO TX/RX clock
		// Enable USARTx clock
		ESP8266_USART_CLK_ENABLES();

		
		// ##-2- Configure peripheral GPIO ##########################################
		// UART TX GPIO pin configuration
		GPIO_InitStruct.Pin       = ESP8266_USART_TX_GPIO_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_MEDIUM;
		GPIO_InitStruct.Alternate = ESP8266_USART_AF;

		HAL_GPIO_Init(ESP8266_USART_TX_GPIO_PORT, &GPIO_InitStruct);

		// UART RX GPIO pin configuration
		GPIO_InitStruct.Pin = ESP8266_USART_RX_GPIO_PIN;
		//GPIO_InitStruct.Alternate = ESP8266_USART_AF;

		HAL_GPIO_Init(ESP8266_USART_RX_GPIO_PORT, &GPIO_InitStruct);


		// ##-3- Configure the NVIC for UART ########################################
		// NVIC for USARTx

		HAL_NVIC_SetPriority(ESP8266_USARTx_IRQn, ESP8266_USART_PREEMT_PRIORITY, ESP8266_USART_SUB_PRIORITY);
		HAL_NVIC_EnableIRQ(ESP8266_USARTx_IRQn);
		
	}
#endif
	else
	{
		Error_Handler();
	}

	
}




void USART1_IRQHandler(void)
{

	//__HAL_UART_DISABLE_IT: Disable the specified UART interrupt
	//__HAL_UART_DISABLE_IT(&BluetoothUartHandle, UART_IT_RXNE | UART_IT_TXE);

	// TODO: VG - USART javitani! ha tul sok adat erkezik, akkor itt beragad, lehet hogy valamilyen flaget torolni kene
	#ifdef CONFIG_USE_PANEL_NODESMALL
	HAL_UART_IRQHandler(&ESP8266_UartHandle);
	#endif
	
	#if ( CONFIG_USE_PANEL_NODEMEDIUM || CONFIG_USE_PANEL_CENTERPANEL )
	HAL_UART_IRQHandler(&Debug_UartHandle);
	#endif

	// __HAL_UART_ENABLE_IT: Enable the specified UART interrupt
	//__HAL_UART_ENABLE_IT(&BluetoothUartHandle, UART_IT_RXNE | UART_IT_TXE);

	// __HAL_UART_GET_FLAG : Check whether the specified UART flag is set or not
	// __HAL_UART_CLEAR_FLAG : Clear the specified UART pending flag


}

#ifdef CONFIG_USE_PANEL_DISCOVERY
void USART2_IRQHandler(void)
{

	HAL_UART_IRQHandler(&Debug_UartHandle);

}
#endif	//#ifdef CONFIG_BOARD_DISCOVERY

#ifdef CONFIG_MODULE_ESP8266_ENABLE
void USART2_IRQHandler(void)
{

	HAL_UART_IRQHandler(&ESP8266_UartHandle);

}
#endif



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{

	#ifdef CONFIG_USE_FREERTOS
	taskDISABLE_INTERRUPTS();	// TODO: not need
	#endif
	
	(void)UartHandle;

	
	// Set transmission flag: trasfer complete	
	#ifdef CONFIG_USE_FREERTOS
	xSemaphoreGiveFromISR(DEBUG_USART_Tx_Semaphore,(BaseType_t *)NULL);
	#endif
	
	// Successful sending

	USART_SendEnable_flag = ENABLE;

	#ifdef CONFIG_USE_FREERTOS
	taskENABLE_INTERRUPTS();	// TODO: not need
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

	#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	if ( ( UartHandle == &Debug_UartHandle ) &&
			( MONITOR_CommandReceiveEnable == true ) )
	{
		HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)&USART_RxBuffer[USART_RxBufferWriteCounter], RX_BUFFER_WAIT_LENGTH);
		USART_RxBufferWriteCounter++;

		#ifdef CONFIG_USE_FREERTOS
		// Transmission end semaphore / flag
		// Give semaphore
		xSemaphoreGiveFromISR(DEBUG_USART_Rx_Semaphore,0);
		#endif

	}
	#endif	// #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	
	#ifdef CONFIG_MODULE_ESP8266_ENABLE
	if ( UartHandle->Instance == ESP8266_USARTx && UartHandle == &ESP8266_UartHandle )
	{
		
		#ifdef CONFIG_USE_FREERTOS
		if ( ESP8266_Receive_Mode_FixLength == 1)
		{
			xSemaphoreGiveFromISR(ESP8266_USART_Rx_Semaphore,0);	
		}
		else	// ESP8266_Receive_Mode_FixLength == 0
		{
			// Put to Buffer and receive next char
			//uint8_t receivedChar = Esp8266UartHandle.pRxBuffPtr[0];	// TODO: delete
			//ESP8266_RxBuffer[ESP8266_RxBuffer_Cnt++] = receivedChar;
			if ( ESP8266_ReceiveBuffer[ESP8266_ReceiveBuffer_Cnt] != '\n' || ESP8266_ReceiveBuffer_Cnt <= 4)
			{
				ESP8266_ReceiveBuffer_Cnt++;
				HAL_UART_Receive_IT(&ESP8266_UartHandle,(uint8_t *)&ESP8266_ReceiveBuffer[ESP8266_ReceiveBuffer_Cnt],1);
			}
			else
			{
				// received an '\n'
				xSemaphoreGiveFromISR(ESP8266_USART_Rx_Semaphore,0);
				ESP8266_ReceiveBuffer_Cnt++;
				HAL_UART_Receive_IT(&ESP8266_UartHandle,(uint8_t *)&ESP8266_ReceiveBuffer[ESP8266_ReceiveBuffer_Cnt],1);
			}
		}
	
		#else
		// NOT USED FREERTOS:
		ESP8266_Uart_ReceivedCharFlag = 1;
		
		#endif
		

		
	}
	#endif

	#ifdef CONFIG_USE_FREERTOS
	taskENABLE_INTERRUPTS();
	#endif
	
}



// !! IMPORTANT !! Hiba eseten ebbe az ErrorCallback-be jutunk.
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{

#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	if (huart->Instance == DEBUG_USARTx )
	{

		// TODO:
		// neha kapunk ORE errort.

		//__HAL_UART_RESET_HANDLE_STATE(&BluetoothUartHandle);
		//__HAL_UART_GET_FLAG(huart, UART_FLAG_TXE);
		//__HAL_UART_CLEAR_FLAG(&BluetoothUartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);

		huart->ErrorCode = HAL_UART_ERROR_NONE;
		huart->gState = HAL_UART_STATE_READY;
		huart->TxXferCount = 0;
		huart->TxXferSize = 0;
		huart->RxXferCount = 0;
		huart->RxXferSize = 0;

		//__HAL_UART_FLUSH_DRREGISTER(&BluetoothUartHandle);
		

		HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)&USART_RxBuffer[USART_RxBufferWriteCounter], RX_BUFFER_WAIT_LENGTH);
		HAL_UART_Transmit_IT(&Debug_UartHandle,(uint8_t *)"$",1);
		
		#ifdef CONFIG_USE_FREERTOS
		//xSemaphoreGiveFromISR(DEBUG_USART_Rx_Semaphore,0);
		if ( DEBUG_USART_Tx_Semaphore != NULL )
		{
			xSemaphoreGiveFromISR(DEBUG_USART_Tx_Semaphore,0);
		}
		#endif
		
	}
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	else if (huart->Instance == ESP8266_USARTx )
	{

		// TODO:
		// neha kapunk ORE errort.

		//__HAL_UART_RESET_HANDLE_STATE(&BluetoothUartHandle);
		//__HAL_UART_GET_FLAG(huart, UART_FLAG_TXE);
		__HAL_UART_CLEAR_FLAG(&ESP8266_UartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);

		huart->ErrorCode = HAL_UART_ERROR_NONE;
		huart->gState = HAL_UART_STATE_READY;
		huart->TxXferCount = 0;
		huart->TxXferSize = 0;
		huart->RxXferCount = 0;
		huart->RxXferSize = 0;
		
		if ( ESP8266_USART_Rx_Semaphore != NULL )
		{
			xSemaphoreGiveFromISR(ESP8266_USART_Rx_Semaphore,0);
		}
		

	}	
#endif
	else
	{
		Error_Handler();
	}
	
	return;
}



/**
 * \brief	Send string on USART
 */
// TODO: ReturnType + Check Pointer
uint8_t USART_SendMessage ( const char *aTxBuffer )
{
	#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	uint8_t length = 0;

	length = StringLength(aTxBuffer);

	if ( length == 0 )
	{
		return false;
	}
	if ( length > TXBUFFERSIZE )
	{
		length = TXBUFFERSIZE-1;
	}


	#ifdef CONFIG_USE_FREERTOS
	if ( xSemaphoreTake(DEBUG_USART_Tx_Semaphore, (portTickType) 10000) == pdTRUE )
	#else
	if ( USART_WaitForSend(500))
	#endif
	{
		// Take semaphore, can sending
		
		USART_SendEnable_flag = DISABLE;
		
		StrCpy((char *)USART_TxBuffer,aTxBuffer);

		// ComIT
		if(HAL_UART_Transmit_IT(&Debug_UartHandle, (uint8_t*)USART_TxBuffer, length)!= HAL_OK)
		{
			// NOTE: !!IMPORTANT!! Not sent message
			//Error_Handler();
			#ifdef CONFIG_USE_FREERTOS
			xSemaphoreGive(DEBUG_USART_Tx_Semaphore);
			#endif
			USART_SendEnable_flag = ENABLE;	// Failed to send, now we can send message

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
		// Cannot take semaphore, now usart is busy
		return 0;
	}


	#endif // #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
}



bool USART_SendLine (const char *message)
{
	bool isSuccessful = true;
	isSuccessful &= USART_SendMessage(message);
	isSuccessful &= USART_SendMessage("\r\n");
	return isSuccessful;
}



/**
 * \brief	Send a char on USART
 */
bool USART_SendChar ( char c )
{
	#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	char buf[2];
	buf[0] = c;
	buf[1] = '\0';

	
	#ifdef CONFIG_USE_FREERTOS
	if ( xSemaphoreTake(DEBUG_USART_Tx_Semaphore, (portTickType) 1000) == pdTRUE )
	#else
	if ( USART_WaitForSend(100))
	#endif	
	{
		// Successful take USART semaphore
		USART_SendEnable_flag = DISABLE;

		StrCpy((char *)USART_TxBuffer,buf);

		if(HAL_UART_Transmit_IT(&Debug_UartHandle, (uint8_t *)USART_TxBuffer, 1)!= HAL_OK)
		{
			// NOTE: !! IMPORTANT!! Not sent message
			//Error_Handler();
			#ifdef CONFIG_USE_FREERTOS
			xSemaphoreGive(DEBUG_USART_Tx_Semaphore);
			#endif
			USART_SendEnable_flag = ENABLE;
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
void USART_StartReceiveMessage ( void )
{

	// USART - Receive Message - uzenetvaras
	HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)&USART_RxBuffer[USART_RxBufferWriteCounter], RX_BUFFER_WAIT_LENGTH);
	// Javitott, mert ha valamikor nem fogadnank uzenetet, akkor itt megint beallunk uzenetvaro uzemmodba

	#ifdef CONFIG_USE_FREERTOS
	// Wait for semaphore
	xSemaphoreTake(DEBUG_USART_Rx_Semaphore, (portTickType) 1000);
	#endif
	
	
}
#endif	// #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE



uint8_t USART_WaitForSend (uint16_t timeoutMiliSecond)
{

	while(USART_SendEnable_flag == DISABLE && timeoutMiliSecond != 0)
	{	
		timeoutMiliSecond--;
		HAL_Delay(1);
	}
	
	USART_SendEnable_flag = ENABLE;

	
	return ENABLE;
	
}



/*
void USART_Test ( void )
{

	USART_Init();

	USART_SendMessage ("Blablalba\r\n");

}
*/



// Function: like printf(); C function
// Bemasoljuk egy stringbe a szoveget, es csak azutan kuldjuk ki
void uprintf(char * param, ...) {			// d, w, h, b, c, s, %

	// Working in at:
	char TxBuffer[TXBUFFERSIZE];

	va_list ap;			// argument pointer
	char	*p;			// step on fmt
	char	*sval;		// string
	int		ival;		// int
	unsigned int uival;	// uint
	float	flval;		// float
	char 	cval;		// character

	char *string;
	string = TxBuffer;
	//uint8_t	length = 0;

	va_start(ap, param); 							// ap on arg
	for (p = param; *p; p++) {  	       			// p to EOS
		if (*p != '%')								// copy, if not '%'
		{
			*string = *p;							// copy to string
			string++;
		}
		else            							// there is %, working
		{ switch(*++p)
		  {
			case 'd': ival = va_arg(ap, int);						// Decimal
					  string += SignedDecimalToString(ival,string);
					  break;

			case 'u': uival = va_arg(ap, int);						// unsigned
					  string += UnsignedDecimalToString(uival,string);
					  break;

			case 'w': uival = va_arg(ap, unsigned int);				// Hex // 32 bits	// 8 hex	// 4 byte
					  string += DecimalToHexaString(uival,4,string);// copy to string
					  break;

			case 'h': ival = va_arg(ap, int);						// Hex // 16 bits	// 4 hex	// 2 byte
					  string += DecimalToHexaString(ival,2,string);	// copy to string
					  break;

			case 'b': ival = va_arg(ap, int);						// Hex	// 8 bits	// 2 hex	// 1 byte
					  string += DecimalToHexaString(ival,1,string);	// copy to string
					  break;

			case 'c': cval = va_arg(ap, int);						// Char
					  *string = cval;								// copy to string
					  string++;
					  *string = '\0';
					  break;
			case 'f': //flval = va_arg(ap, float);					// float
					  //string += FloatToString(ival,string,6);
					  //flval = va_arg(ap, float);					// GOOD, but warning
					  flval = va_arg(ap, double);
					  string += FloatToString(flval,string,6);
					  break;
			case 's': for(sval = va_arg(ap,char*); *sval; sval++)	// String
					  {
						*string = *sval;							// copy to string
						string++;
					  }
					  break;

			default:  *string = *p;									// Other, for example: '%'
					  string++;
					  break;
		  }
		}
	}
	va_end(ap);						 							// Cleaning after end

	*string = '\0';												// string's end

	USART_SendMessage(TxBuffer);							// Send on Usart
}



void USART_SendFloat ( float value )
{
	char string[16];
	
	FloatToString(value,string,6);	
	
	USART_SendString(string);
	
	
}




