/*
 *    GSM_SIM800.c
 *    Created on:   2022-02-17
 *    Author:       faste
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "StringHelper.h"
#include "CircularBuffer.h"
#include "TaskList.h"
#include "CommandHandler.h"
#include "DateTime.h"
#include "ErrorHandler.h"
#include "UART.h"
#include "GSM_SIM800.h"



#ifdef CONFIG_MODULE_GSM_ENABLE

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define GSM_TX_BUFFER_LENGTH        (255U)
#define GSM_RX_BUFFER_LENGTH        (255U)



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

UART_HandleTypeDef GSM_UartHandle;


/* Buffers */
static volatile char GSM_TxBuffer[GSM_TX_BUFFER_LENGTH] = { 0 };
static volatile char GSM_RxBuffer[GSM_RX_BUFFER_LENGTH] = { 0 };

static CircularBufferInfo_t GSM_TxBuffStruct =
{
    .buffer = (char *)GSM_TxBuffer,
    .name = "GSM_TxBuffer",
    .size = GSM_TX_BUFFER_LENGTH
};

static CircularBufferInfo_t GSM_RxBuffStruct =
{
    .buffer = (char *)GSM_RxBuffer,
    .name = "GSM_RxBuffer",
    .size = GSM_RX_BUFFER_LENGTH
};



///< UART + CircularBuffer handler structure
UART_Handler_t GSM_Uart =
{
    .huart = &GSM_UartHandle,
    .tx = &GSM_TxBuffStruct,
    .rx = &GSM_RxBuffStruct,
    .txIsEnabled = true,
    .rxIsEnalbed = true,
#ifdef CONFIG_MODULE_UART_REQUIRE_TASKSCHEDULE_ENABLE
    /* TODO: */
    .requiredTask = Task_Count,
#endif
};



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief  GSM_SIM800 initialization
 */
void GSM_SIM800_Init(void)
{
    /* Circular buffer initialization */
    CircularBuffer_Init(GSM_Uart.tx);
    CircularBuffer_Init(GSM_Uart.rx);


    /* USART TX - RX     */
    UART_Init(&GSM_UartHandle);

    UART_ReceiveEnable(&GSM_Uart);
}



static inline void GSM_SendEnable(void)
{
    UART_SendEnable(&GSM_Uart);
}



#endif /* #ifdef CONFIG_MODULE_GSM_ENABLE */
