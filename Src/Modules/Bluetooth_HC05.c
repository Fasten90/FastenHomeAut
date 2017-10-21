/*
 *		Bluetooth_HC05.c
 *		Created on:		2017-10-21
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017-10-21
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "String.h"
#include "USART.h"
#include "Bluetooth_HC05.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

UART_HandleTypeDef Bluetooth_UartHandle;
bool Bluetooth_SendEnable_flag = 0;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

static char Bluetooth_TxBuffer[BLUETOOTH_TX_BUFFER_SIZE];



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief Bluetooth initialization
 */
void Bluetooth_HC05_Init(void)
{
	USART_Init(&Bluetooth_UartHandle);
	Bluetooth_SendEnable_flag = true;
}



void Bluetooth_SendMessage(const char * msg)
{
	while (!Bluetooth_SendEnable_flag);

	uint8_t length = StringLength(msg);
	StrCpy(Bluetooth_TxBuffer, msg);

	Bluetooth_SendEnable_flag = false;
	HAL_UART_Transmit_IT(&Bluetooth_UartHandle, (uint8_t *)Bluetooth_TxBuffer, length);
}



inline bool Bluetooth_GetSendEnable(void)
{
	return Bluetooth_SendEnable_flag;
}


