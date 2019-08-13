/*
 *    CommonUART.h
 *    Created on:   2017-12-28
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     HwTester UART function
 *    Target:       STM32Fx
 */

#ifndef COMMONUART_H_
#define COMMONUART_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "UART.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define COMMONUART_TX_BUFFER_SIZE              (255U)
#define COMMONUART_RX_BUFFER_SIZE              (255U)
#define COMMONUART_PROCESS_BUFFER_SIZE         (100U)
#define COMMONUART_RESPONSE_BUFFER_SIZE        (100U)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern UART_HandleTypeDef CommonUART_UartHandle;
extern UART_Handler_t CommonUART;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void CommonUART_Init(void);
size_t CommonUART_SendMessage(const char *msg);
void CommonUART_ProcessReceivedCharacters(void);



#endif /* COMMONUART_H_ */
