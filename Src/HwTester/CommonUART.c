/*
 *    CommonUART.c
 *    Created on:   2017-12-28
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_MODULE_COMMON_UART_ENABLE

#ifdef CONFIG_MODULE_UART_REQUIRE_TASKSCHEDULE_ENABLE
#include "TaskList.h"
#endif
#include "StringHelper.h"
#include "DebugUart.h"
#include "CommonUART.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


static volatile char CommonUART_TxBuffer[COMMONUART_TX_BUFFER_SIZE] = { 0 };
static volatile char CommonUART_RxBuffer[COMMONUART_RX_BUFFER_SIZE] = { 0 };

static CircularBufferInfo_t CommonUART_TxBuffStruct =
{
    .buffer = (char *)CommonUART_TxBuffer,
    .name = "CommonUART_TxBuffer",
    .size = COMMONUART_TX_BUFFER_SIZE
};

static CircularBufferInfo_t CommonUART_RxBuffStruct =
{
    .buffer = (char *)CommonUART_RxBuffer,
    .name = "CommonUART_RxBuffer",
    .size = COMMONUART_RX_BUFFER_SIZE
};

UART_HandleTypeDef CommonUART_UartHandle;

UART_Handler_t CommonUART =
{
    .huart = &CommonUART_UartHandle,
    .tx = &CommonUART_TxBuffStruct,
    .rx = &CommonUART_RxBuffStruct,
    .txIsEnabled = true,
    .rxIsEnalbed = true,
#ifdef CONFIG_MODULE_UART_REQUIRE_TASKSCHEDULE_ENABLE
    .requiredTask = Task_CommonUART,
#endif
};



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static inline void CommonUART_SendEnable(void);
static void CommonUART_ReceiveEnable(void);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief CommonUART initialization
 */
void CommonUART_Init(void)
{
    CircularBuffer_Init(&CommonUART_RxBuffStruct);
    CircularBuffer_Init(&CommonUART_TxBuffStruct);

    UART_Init(&CommonUART_UartHandle);

    /* ommonUART_SendEnable(); */
    CommonUART_ReceiveEnable();
}



/**
 * @brief    Send enable
 */
static inline void CommonUART_SendEnable(void)
{
    UART_SendEnable(&CommonUART);
}



/**
 * @brief    Receive enable
 */
static void CommonUART_ReceiveEnable(void)
{
    UART_ReceiveEnable(&CommonUART);
}



/**
 * @brief    Send string on CommonUART
 */
size_t CommonUART_SendMessage(const char *msg)
{
    size_t length = 0;
    size_t putLength;

    length = StringLength(msg);

    if (length == 0)
    {
        return 0;
    }

    putLength = CircularBuffer_PutString(&CommonUART_TxBuffStruct, msg, length);
    /* not added \r\n */
    putLength += CircularBuffer_PutString(&CommonUART_TxBuffStruct, "\r\n", 2);

    if (putLength > 0)
        CommonUART_SendEnable();

    return putLength;
}



/**
 * @brief    Process received characters (if Terminal is not enabled)
 */
void CommonUART_ProcessReceivedCharacters(void)
{
    char recvBuf[COMMONUART_PROCESS_BUFFER_SIZE];

    /* Received new character? */
    if (CircularBuffer_IsNotEmpty(&CommonUART_RxBuffStruct))
    {
        /* Copy received message to buffer */
        CircularBuffer_GetString(&CommonUART_RxBuffStruct, recvBuf, COMMONUART_PROCESS_BUFFER_SIZE);

        static uint32_t CommonUart_LastReceived = 0;

        /* Received newline character? (End of command) */
        char * newLinePos = (char *)STRING_FindCharacters((const char *)recvBuf, "\r\n\0");

        /* TODO: Beautify! */

        /* Check message "time" */
        if (newLinePos == NULL)
        {
            uint32_t actualTime = HAL_GetTick();

            /* if received one char after a long silent time, it will be printed with alone */
            if (StringLength(recvBuf) == 1)
            {
                /* It is after a long time? */
                /* TODO: Not the best solution... if not receive more char, it will not printed */
                CommonUart_LastReceived = actualTime;
            }
            else
            {
                /* Note: Be careful: Recommend to set "check time" to task period time */
                if ((CommonUart_LastReceived + 1000) <= actualTime)
                {
                    /* Received a long time... */
                    newLinePos = recvBuf + StringLength(recvBuf);
                }

                CommonUart_LastReceived = actualTime;
            }
        }

        if (newLinePos != NULL)
        {
            /* Has newline, process the received command */
            if (*newLinePos == '\r' || *newLinePos == '\n')
                *newLinePos = '\0';

            if (StringLength(recvBuf) != 0)
            {
                /* Send on DebugUart (these are different from DebugUart Process() function) */
                uprintf("Received: \"%s\"\r\n", recvBuf);
            }

            /* Drop processed characters */
            size_t processedLength = (newLinePos - recvBuf) + 1;
            if (newLinePos != &recvBuf[COMMONUART_RESPONSE_BUFFER_SIZE-1])
            {
                /* Check next character is not '\n' or '\r'? */
                if ((*(newLinePos+1) == '\r') || (*(newLinePos+1) == '\n'))
                    processedLength++;
            }
            CircularBuffer_DropCharacters(&CommonUART_RxBuffStruct, processedLength);
        }
    }
}

#else

/* CommonUART module is not used */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic pop

#endif    /* #ifdef CONFIG_MODULE_COMMON_UART_ENABLE */
