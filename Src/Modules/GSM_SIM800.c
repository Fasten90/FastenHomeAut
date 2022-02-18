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
#ifdef CONFIG_MODULE_DEBUG_ENABLE
    #include "Debug.h"
	#include "DebugUart.h"
#endif



#ifdef CONFIG_MODULE_GSM_ENABLE

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define GSM_TX_BUFFER_LENGTH        (255U)
#define GSM_RX_BUFFER_LENGTH        (255U)

#define GSM_DEBUG_ENABLED           (1)



#if (GSM_DEBUG_ENABLED == 1)
    #ifdef CONFIG_MODULE_DEBUG_ENABLE
        #define DEBUG_PRINT(msg)                    Debug_Print(Debug_GSM, msg)
    #else
        #define DEBUG_PRINT(msg)                    uprintf(msg)
    #endif
#else
    #define DEBUG_PRINT(msg)                        NOT_USED(msg)   // Empty
#endif



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
    /* TODO: for Task_GSM it response immediately after one character received. */
    .requiredTask = Task_Count,
#endif
};



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

typedef enum {
    GSM_Unknown,
    GSM_InitStart,
	GSM_InitStart_Reply,
	GSM_Init_GetSignal,
	GSM_Init_GetSignal_Reply,
	GSM_Ready,
} GSM_StatusMachine_t;



GSM_StatusMachine_t gsm_status = GSM_Unknown; /* Zero initialized */



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static size_t GSM_SendMsg(const char *msg);
static void GSM_ClearReceive(bool isFullClear, size_t stepLength);



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



void GSM_TaskFunction(ScheduleSource_t source)
{
    /* TODO: GSM status machine */
    // TaskHandler: Task_GSM

	static uint8_t err_cnt = 0;

    /* If WriteCnt not equal with ReadCnt, we have received message */
    char receiveBuffer[GSM_RX_BUFFER_LENGTH+1] = { 0 };
    uint16_t receivedMessageLength = 0;

    /* Need copy to receiveBuffer */
    receivedMessageLength = CircularBuffer_GetString(
            GSM_Uart.rx,
            receiveBuffer,
            GSM_RX_BUFFER_LENGTH);

    /* Print all received chars: */
#ifdef CONFIG_MODULE_DEBUG_ENABLE
    if (receivedMessageLength > 0)
    {
        DebugUart_Printf("GSM Received: %s", receiveBuffer);
    }
#endif

	switch (gsm_status)
    {
        case GSM_Unknown:
            DEBUG_PRINT("GSM status init");
            GSM_ClearReceive(true, 0);
            /* TODO: Later: GSM reset */
            TaskHandler_SetTaskPeriodicTime(Task_GSM, 1000);
            gsm_status++;
            break;

        case GSM_InitStart:
        	GSM_ClearReceive(true, 0);
        	GSM_SendMsg("AT\r\n"); /* TODO: Generalize: \r\n always needed */
        	TaskHandler_SetTaskPeriodicTime(Task_GSM, 100);
            gsm_status++;
            break;

        case GSM_InitStart_Reply:
        {
        	if (receivedMessageLength > 0) /* TODO: Generalize the receiving */
        	{
        		if (StrCmp("AT\r\r\nOK\r\n", receiveBuffer) == 0)
        		{
					gsm_status++;
        		}
        		else
        		{
            		DEBUG_PRINT("Wrong answer received");
            		gsm_status--;
            		err_cnt++;
        		}
        	}
        	else
        	{
        		DEBUG_PRINT("AT Answer is not received");
        		gsm_status--;
        		err_cnt++;
        	}
        	GSM_ClearReceive(true, 0);
        }
        break;

        case GSM_Init_GetSignal:
        	GSM_ClearReceive(true, 0);
        	GSM_SendMsg("AT+CSQ\r\n");
        	TaskHandler_SetTaskPeriodicTime(Task_GSM, 100);
            gsm_status++;
            break;

        case GSM_Init_GetSignal_Reply:
        {
        	if (receivedMessageLength > 0)
        	{
        		/* AT+CSQ +CSQ: 22,0 */
        		/* AT+CSQ\r\r\n+CSQ: 21,0\r\n\r\nOK\r\n */ /* TODO: Process the signal quality and save to the information struct */
        		if (StrCmpFirst("AT+CSQ\r\r\n+CSQ: ", receiveBuffer) == 0)
				{
        			gsm_status++;
				}
				else
				{
					DEBUG_PRINT("Wrong answer received");
					gsm_status--;
					err_cnt++;
				}
        	}
        	else
        	{
        		DEBUG_PRINT("AT Answer is not received");
        		gsm_status--;
        		err_cnt++;
        	}
        	GSM_ClearReceive(true, 0);
        }
		break;

        case GSM_Ready:
        	/* TODO: Be happy */
        	DEBUG_PRINT("Be happy!");
        	TaskHandler_SetTaskPeriodicTime(Task_GSM, 1000);
        	break;

        default:
            /* Not stable status, go back */
            gsm_status = GSM_Unknown;
            DEBUG_PRINT("GSM status was invalid, restart the status machine");
            TaskHandler_SetTaskPeriodicTime(Task_GSM, 1000);
            GSM_ClearReceive(true, 0);
            break;
    }
}



/* TODO: Generalize */
static size_t GSM_SendMsg(const char *msg)
{
	size_t msgLength = StringLength(msg);
    size_t putLength = CircularBuffer_PutString(GSM_Uart.tx, msg, msgLength);

    if (putLength > 0)
        GSM_SendEnable();

    return putLength;
}



/* TODO: Generalize */
/**
 * @brief       Clear receive buffer
 */
static void GSM_ClearReceive(bool isFullClear, size_t stepLength)
{
    if (isFullClear)
    {
        /* Clear all buffer */
        CircularBuffer_Clear(GSM_Uart.rx);
    }
    else
    {
        /* Not full clear from readCnt to writeCnt */
        CircularBuffer_DropCharacters(GSM_Uart.rx, stepLength);
    }
}


/* TODO: Check Idle messages: Ring, etc */


/* TODO: Print Information struct (e.g. Signal Quality) */


#endif /* #ifdef CONFIG_MODULE_GSM_ENABLE */
