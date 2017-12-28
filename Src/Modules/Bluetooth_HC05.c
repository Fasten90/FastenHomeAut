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

#ifdef CONFIG_MODULE_BLUETOOTH_ENABLE

#include "StringHelper.h"
#include "UART.h"
#include "CommandHandler.h"
#include "Bluetooth_HC05.h"
#include "DebugUart.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

static volatile char Bluetooth_TxBuffer[BLUETOOTH_TX_BUFFER_SIZE] = { 0 };
static volatile char Bluetooth_RxBuffer[BLUETOOTH_RX_BUFFER_SIZE] = { 0 };

static CircularBufferInfo_t Bluetooth_TxBuffStruct =
{
	.buffer = (char *)Bluetooth_TxBuffer,
	.name = "Bluetooth_TxBuffer",
	.size = BLUETOOTH_TX_BUFFER_SIZE
};

static CircularBufferInfo_t Bluetooth_RxBuffStruct =
{
	.buffer = (char *)Bluetooth_RxBuffer,
	.name = "Bluetooth_RxBuffer",
	.size = BLUETOOTH_RX_BUFFER_SIZE
};

UART_HandleTypeDef Bluetooth_UartHandle;

UART_Handler_t Bluetooth =
{
	.huart = &Bluetooth_UartHandle,
	.tx = &Bluetooth_TxBuffStruct,
	.rx = &Bluetooth_RxBuffStruct,
	.txIsEnabled = true,
	.rxIsEnalbed = true
};



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static inline void Bluetooth_SendEnable(void);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief Bluetooth initialization
 */
void Bluetooth_HC05_Init(void)
{
	CircularBuffer_Init(&Bluetooth_RxBuffStruct);
	CircularBuffer_Init(&Bluetooth_TxBuffStruct);

	UART_Init(&Bluetooth_UartHandle);

	//Bluetooth_SendEnable();
	Bluetooth_ReceiveEnable();
}



/**
 * \brief	Send enable
 */
static inline void Bluetooth_SendEnable(void)
{
	UART_SendEnable(&Bluetooth);
}



/**
 * \brief	Receive enable
 */
void Bluetooth_ReceiveEnable(void)
{
	UART_ReceiveEnable(&Bluetooth);
}



/**
 * \brief	Send string on Bluetooth
 */
size_t Bluetooth_SendMessage(const char *msg)
{
	size_t length = 0;
	size_t putLength;

	length = StringLength(msg);

	if (length == 0)
	{
		return 0;
	}

	putLength = CircularBuffer_PutString(&Bluetooth_TxBuffStruct, msg, length);

	if (putLength > 0)
		Bluetooth_SendEnable();

	return putLength;
}



/**
 * \brief	Process received characters (if Terminal is not enabled)
 */
void Bluetooth_ProcessReceivedCharacters(void)
{
	char recvBuf[BLUETOOTH_PROCESS_BUFFER_SIZE];

	// Received new character?
	if (CircularBuffer_IsNotEmpty(&Bluetooth_RxBuffStruct))
	{
		// Copy received message to buffer
		CircularBuffer_GetString(&Bluetooth_RxBuffStruct, recvBuf, BLUETOOTH_PROCESS_BUFFER_SIZE);

		// Received newline character? (End of command)
		char * newLinePos = (char *)STRING_FindCharacters((const char *)recvBuf, "\r\n");
		if (newLinePos != NULL)
		{
			// Has newline, process the received command
			char respBuf[BLUETOOTH_RESPONSE_BUFFER_SIZE];
			respBuf[0] = '\0';

			*newLinePos = '\0';

			// Search command and run
			CmdH_Result_t cmdResult = CmdH_ExecuteCommand(recvBuf, respBuf, BLUETOOTH_RESPONSE_BUFFER_SIZE);

			CmdH_PrintResult(cmdResult);

			Bluetooth_SendMessage(respBuf);

			// Send on DebugUart (these are different from DebugUart Process() function)
			uprintf("Received Bluetooth command: \"%s\"\r\n", recvBuf);

			// Drop processed characters
			size_t processedLength = (newLinePos - recvBuf) + 1;
			if (newLinePos != &recvBuf[BLUETOOTH_RESPONSE_BUFFER_SIZE-1])
			{
				// Check next character is not '\n' or '\r'?
				if ((*(newLinePos+1) == '\r') || (*(newLinePos+1) == '\n'))
					processedLength++;
			}
			CircularBuffer_DropCharacters(&Bluetooth_RxBuffStruct, processedLength);
		}
	}
}

#else

// Bluetooth module is not used
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic pop

#endif	// #ifdef CONFIG_MODULE_BLUETOOTH_ENABLE
