/*
 *		usart.h
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		usart communication
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef USART_H_
#define USART_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

// Size of Transmission buffer
#define TXBUFFERSIZE					256UL
// Size of Reception buffer
//#define RXBUFFERSIZE					TXBUFFERSIZE
#define RXBUFFERSIZE					256UL
#define RX_BUFFER_WAIT_LENGTH			1


// TODO!!! For compiler errors

#define RESET	0
#define SET		!RESET


#define	ENABLE	1
#define DISABLE	0



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
extern UART_HandleTypeDef Debug_UartHandle;
#endif

extern volatile char USART_RxBuffer[RXBUFFERSIZE];
extern volatile char USART_TxBuffer[TXBUFFERSIZE];

extern uint8_t USART_SendEnable_flag;
extern volatile uint8_t USART_RxBufferWriteCounter;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/
void USART_Init(UART_HandleTypeDef *UartHandle);

uint8_t USART_WaitForSend(uint16_t timeoutMiliSecond);

void USART_StartReceiveMessage(void);

bool USART_SendChar(char c);
uint8_t USART_SendMessage(const char *aTxBuffer);
bool USART_SendNewLine(void);
bool USART_SendLine(const char *message);
void USART_SendFloat(float value);



#endif /* USART_H_ */
