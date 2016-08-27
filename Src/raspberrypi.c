/*******************************************************************************
 * Purpose:
 ******************************************************************************/

/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "raspberrypi.h"
#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE

/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
UART_HandleTypeDef RaspberrypiUartHandle;

/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/
void RASPBERRYPI_Task(void);
void RASPBERRYPI_Init(void);
void HAL_UART_MspInitRASPBERRYPI(UART_HandleTypeDef *huart);


extern void Error_Handler( void );


/*------------------------------------------------------------------------------
 *  Local functions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief
 * @details
 * @param[in]
 * @param[out]
 * @return
 ******************************************************************************/
void RASPBERRYPI_Task(void)
{
	
	//RASPBERRYPI_Init();
	// Put to main.c
	// Not need it, because Rasp uart = debug uart
	
}



// void RASPBERRYPI_SendMessage(uint8_t myAddress, HOMEAUTMESSAGE_FunctionType functionType, HOMEAUTMESSAGE_DataType dataType , float data)
void RASPBERRYPI_SendMessage(uint8_t myAddress, uint8_t functionType, uint8_t dataType , float data)
{
	//myAddress	// ok
	char function = 0;
	switch (functionType)
	{
		case Function_Alarm:
			function = 'a';
		break;
		case Function_Command:
			function = 'c';
		break;
		case Function_Config:
			function = 'c';
		break;
		case Function_End:
			return;
		//break;		// Because: statement is unreachable
		case Function_Invalid:
			return;
		//break;			// Because: statement is unreachable	
		case Function_Login:
			function = 'l';
		break;	
		case Function_Mode:
			function = 'm';
		break;	
		case Function_State:
			function = 's';
		break;
		default:
			return;
		//break;		// Because: statement is unreachable
	}
	
	char type = 0;
	switch (dataType)
	{
		case State_Temperature:
			type = 't';
		break;
		case State_Brightness:
			type = 'b';
		break;
		case State_Sound:
			type = 's';
		break;
		case State_Battery:
			type = 'v';		// Status ~Voltage
		break;
		case State_Input:
			type = 'i';
		break;
		case State_Output:
			type = 'o';
		break;
		case Alarm_PressedButton:
			type = 'b';
		break;
		case Alarm_Moving:
			type = 'm';
		break;		
		case Alarm_SoundImpacted:
			type = 's';
		break;	
		case Alarm_DoorOpened:
			type = 'd';
		break;	
		
		default:
			return;
		//break;		// Because: statement is unreachable
	}
	// STATE
	//State_Button,
	//State_Vin,
	//State_Accelerometer,
	
	// Alarm
	//Alarm_TooHot,
	//Alarm_TooCold,
	//Alarm_TooBright,
	//Alarm_TooDark,

	
	// Sending:
	
	/*
	// Egész
	uint32_t data1 = (uint32_t)data;
	// Tört
	uint32_t data2 = (uint32_t)(data*100) - (data1*100);
	uprintf("%d_%c%c_%d.%d\r\n",myAddress,function,type,data1,data2);
	*/
	uprintf("#%d_%c%c_%f\r\n",myAddress,function,type,data);
	
}



#define RASPBERRYPI_USARTx				USART1

#define RASPBERRYPI_USART_CLK_ENABLES()		\
		__USART1_CLK_ENABLE();			\
		__GPIOA_CLK_ENABLE()


#define RASPBERRYPI_USART_TX_GPIO_PORT			GPIOA
#define RASPBERRYPI_USART_TX_GPIO_CLK			RCC_AHB1Periph_GPIOA
#define RASPBERRYPI_USART_TX_GPIO_PIN			GPIO_PIN_9

#define RASPBERRYPI_USART_RX_GPIO_PORT			GPIOA
#define RASPBERRYPI_USART_RX_GPIO_CLK			RCC_AHB1Periph_GPIOA
#define RASPBERRYPI_USART_RX_GPIO_PIN			GPIO_PIN_10

#define RASPBERRYPI_USART_AF					GPIO_AF7_USART1

#define RASPBERRYPI_USARTx_IRQn              	USART1_IRQn
#define RASPBERRYPI_USARTx_IRQHandler        	USART1_IRQHandler

#define RASPBERRYPI_USART_PREEMT_PRIORITY		14
#define RASPBERRYPI_USART_SUB_PRIORITY			0


void RASPBERRYPI_Init(void)
{
	UART_HandleTypeDef *UartHandle = &RaspberrypiUartHandle;
	
	// HW init, Port init, etc...
	HAL_UART_MspInitRASPBERRYPI(UartHandle);
	

	//##-1- Configure the UART peripheral ######################################
	// Put the USART peripheral in the Asynchronous mode (UART Mode)
	/* UARTx configured as follow:
	  - Word Length = 8 Bits
	  - Stop Bit = One Stop bit
	  - Parity = None
	  - BaudRate = 9600 baud
	  - Hardware flow control disabled (RTS and CTS signals) */
	if ( UartHandle == &RaspberrypiUartHandle)
	{
		UartHandle->Instance        = RASPBERRYPI_USARTx;
		UartHandle->Init.BaudRate   = 115200;
	}

	
	UartHandle->Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle->Init.StopBits   = UART_STOPBITS_1;
	UartHandle->Init.Parity     = UART_PARITY_NONE;
	UartHandle->Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle->Init.Mode       = UART_MODE_TX_RX;

	if(HAL_UART_Init(UartHandle) == HAL_OK)
	{	
		
	}
	else	
	{	
		// != HAL_OK
		Error_Handler();
	}
	
	

}




void HAL_UART_MspInitRASPBERRYPI(UART_HandleTypeDef *huart)
{
	
	GPIO_InitTypeDef  GPIO_InitStruct;

	
	if (huart == &RaspberrypiUartHandle)
	{
		// ##-1- Enable peripherals and GPIO Clocks #################################

		// Enable GPIO TX/RX clock
		// Enable USARTx clock
		RASPBERRYPI_USART_CLK_ENABLES();

		
		// ##-2- Configure peripheral GPIO ##########################################
		// UART TX GPIO pin configuration
		GPIO_InitStruct.Pin       = RASPBERRYPI_USART_TX_GPIO_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_MEDIUM;
		GPIO_InitStruct.Alternate = RASPBERRYPI_USART_AF;

		HAL_GPIO_Init(RASPBERRYPI_USART_TX_GPIO_PORT, &GPIO_InitStruct);

		// UART RX GPIO pin configuration
		GPIO_InitStruct.Pin = RASPBERRYPI_USART_RX_GPIO_PIN;
		//GPIO_InitStruct.Alternate = DEBUG_USART_AF;

		HAL_GPIO_Init(RASPBERRYPI_USART_RX_GPIO_PORT, &GPIO_InitStruct);


		// ##-3- Configure the NVIC for UART ########################################
		// NVIC for USARTx

		HAL_NVIC_SetPriority(RASPBERRYPI_USARTx_IRQn, RASPBERRYPI_USART_PREEMT_PRIORITY, RASPBERRYPI_USART_SUB_PRIORITY);
		HAL_NVIC_EnableIRQ(RASPBERRYPI_USARTx_IRQn);
	}

	
}

#endif	// #ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
