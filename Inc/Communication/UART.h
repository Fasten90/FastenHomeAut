/*
 *    USART.h
 *    Created on:   2016-01-01
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     USART communication
 *    Target:       STM32Fx
 */

#ifndef UART_H_
#define UART_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "CircularBuffer.h"
#ifdef CONFIG_MODULE_UART_REQUIRE_TASKSCHEDULE_ENABLE
#include "TaskHandler.h"
#endif



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef struct
{
    UART_HandleTypeDef * huart;            /* pointer to the HAL_UART struct */
    CircularBufferInfo_t *tx;
    CircularBufferInfo_t *rx;            /* transmit (out of STM32) and receive (in to STM32) FIFOs */
    bool txIsEnabled;
    bool rxIsEnalbed;
#ifdef CONFIG_MODULE_UART_REQUIRE_TASKSCHEDULE_ENABLE
    TaskID_t requiredTask;
#endif
} UART_Handler_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void UART_Init(UART_HandleTypeDef *UartHandle);
void UART_SendEnable(UART_Handler_t * handler);
void UART_ReceiveEnable(UART_Handler_t * handler);



#endif /* UART_H_ */
