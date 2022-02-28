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
#include "compiler.h"
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
        #define DEBUG_PRINTF(...)                   Debug_Printf(Debug_GSM, __VA_ARGS__)
    #else
        #define DEBUG_PRINT(msg)                    uprintf(msg)
        #define DEBUG_PRINTF(...)                   uprintf(__VA_ARGS__)
    #endif
#else
    #define DEBUG_PRINT(msg)                        NOT_USED(msg)   /*  Empty */
    #define DEBUG_PRINT(...)                        /*  Empty */
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
    GSM_Init_GetCCID,
    GSM_Init_GetCCID_Reply,
    GSM_Settings_CMGF,
    GSM_Settings_CNMI,
    GSM_Settings_CLIP,
    GSM_Init_GetCREG,
    GSM_Init_GetCREG_Reply,
    GSM_Ready,
} GSM_StatusMachine_t;



static GSM_StatusMachine_t gsm_status = GSM_Unknown; /* Zero initialized */


GSM_InformationStruct GSM_Information = { 0 };



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void GSM_ClearReceive(bool isFullClear, size_t stepLength);
static void GSM_CheckIdleMessages(char * receivedMessage);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief       GSM_SIM800 initialization
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
    UNUSED_ARGUMENT(source); /* TODO: use it? */

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
    if (receivedMessageLength > 0)
    {
        DEBUG_PRINTF("GSM Received: %s", receiveBuffer);
    }

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
                    /* AT+CSQ\r\r\n+CSQ: 21,0\r\n\r\nOK\r\n */
                    uint32_t csq_1;
                    uint32_t csq_2;
                    size_t csq_process_retval = string_scanf(receiveBuffer, "AT+CSQ\r\r\n+CSQ: %d,%d", &csq_1, &csq_2);
                    if (csq_process_retval == 0)
                    {
                        gsm_status++;

                        if (csq_1 > 0 && csq_1 < 32)
                        {
                            /* Successful */
                            GSM_Information.isValid = true;
                            GSM_Information.csq = csq_1;
                            DEBUG_PRINTF("CSQ: %d", csq_1);
                            /* TODO: Should we save the csq_2? */
                        }
                        else
                        {
                            /* Failed */
                            DEBUG_PRINT("Wrong CSQ received");
                            gsm_status--;
                            err_cnt++;
                        }
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

        case GSM_Init_GetCCID:
            GSM_ClearReceive(true, 0);
            GSM_SendMsg("AT+CCID\r\n");
            TaskHandler_SetTaskPeriodicTime(Task_GSM, 100);
            gsm_status++;
            break;

        case GSM_Init_GetCCID_Reply:
            {
                if (receivedMessageLength > 0)
                {
                    /*AT+CCID 89367031561940002091*/
                    if (StrCmpFirst("AT+CCID", receiveBuffer) == 0)
                    {
                        /* string_scanf(receiveBuffer, "AT+CCID\r\r\n%s"); */ /* TODO: It is too long for decimal/int. Maybe we can store it in string */
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

        case GSM_Settings_CMGF:
            GSM_ClearReceive(true, 0);
            GSM_SendMsg("AT+CMGF=1\r\n"); /*  Configuring TEXT mode */
            TaskHandler_SetTaskPeriodicTime(Task_GSM, 500);
            gsm_status++;
            break;
            /* TODO: Check reply */

        case GSM_Settings_CNMI:
            GSM_ClearReceive(true, 0);
            GSM_SendMsg("AT+CNMI=1,2,0,0,0\r\n"); /*  Decides how newly arrived SMS messages should be handled */
            TaskHandler_SetTaskPeriodicTime(Task_GSM, 500);
            gsm_status++;
            break;
            /* TODO: Check reply */

        case GSM_Settings_CLIP:
            GSM_ClearReceive(true, 0);
            GSM_SendMsg("AT+CLIP=1\r\n"); /*  Decides how newly arrived SMS messages should be handled */
            TaskHandler_SetTaskPeriodicTime(Task_GSM, 500);
            gsm_status++;
            break;
            /* TODO: Check reply */

        case GSM_Init_GetCREG:
            GSM_ClearReceive(true, 0);
            GSM_SendMsg("AT+CREG?\r\n");
            TaskHandler_SetTaskPeriodicTime(Task_GSM, 100);
            gsm_status++;
            break;

        case GSM_Init_GetCREG_Reply:
            {
                if (receivedMessageLength > 0)
                {
                    /* AT+CREG? +CREG: 0,1 */
                    /* AT+CCREG?\r\r\nERROR\r\n" */
                    /* +CREG: <n>,<stat>[,<lac>,<ci>] */
                    /* n: <n> 0 Disable network registration unsolicited result code
                     * 1 Enable network registration unsolicited result code
                     * +CREG: <stat>
                     * 2 Enable network registration unsolicited result code with
                     * location information +CREG: <stat>[,<lac>,<ci>]
                     * <stat> 0 Not registered, MT is not currently searching a new
                     * operator to register to
                     * 1 Registered, home network
                     * 2 Not registered, but MT is currently searching a ne */
                    if (StrCmpFirst("AT+CREG?\r\r\n+CREG: ", receiveBuffer) == 0)
                    {
                        uint32_t creg_1 = 0;
                        uint32_t creg_2 = 0;
                        size_t convert_reval;
                        convert_reval = string_scanf(receiveBuffer, "AT+CREG?\r\r\n+CREG: %d,%d", &creg_1, &creg_2);

                        if (!convert_reval)
                        {
                            /* Successful */
                            GSM_Information.creg_1 = creg_1;
                            GSM_Information.creg_2 = creg_2;

                            if (creg_2 == 1) /* Registered home network */
                            {
                                DEBUG_PRINT("CCREG is OK");
                                gsm_status++;
                            }
                            else
                            {
                                DEBUG_PRINT("Wrong CCREG received");
                                gsm_status--;
                                err_cnt++;
                            }

                        }
                        else
                        {
                            /* Failed */
                            DEBUG_PRINT("Invalid CCREG received");
                            gsm_status--;
                            err_cnt++;
                        }
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
                    DEBUG_PRINT("CCREG Answer is not received");
                    gsm_status--;
                    err_cnt++;
                }
                GSM_ClearReceive(true, 0);
                TaskHandler_SetTaskPeriodicTime(Task_GSM, 1000);
            }
            break;

        case GSM_Ready:
            /* TODO: Be happy */
            DEBUG_PRINT("Be happy!");
            if (receivedMessageLength > 0)
            {
                GSM_CheckIdleMessages(receiveBuffer);
                GSM_ClearReceive(true, 0);
            }
            TaskHandler_SetTaskPeriodicTime(Task_GSM, 1000);
            break;

        default:
            /* Not stable status, go back */
            gsm_status = GSM_Unknown;
            DEBUG_PRINT("GSM status was invalid, restart the status machine");
            TaskHandler_SetTaskPeriodicTime(Task_GSM, 1000);
            GSM_ClearReceive(true, 0);
            break;
        /* TODO: !!!! Handle error status !!! */
    }
}



/* TODO: Generalize to UART module */
size_t GSM_SendMsg(const char *msg)
{
    size_t msgLength = StringLength(msg);
    size_t putLength = CircularBuffer_PutString(GSM_Uart.tx, msg, msgLength);

    if (putLength > 0)
        GSM_SendEnable();

    return putLength;
}



/* TODO: Generalize to UART module*/
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



/* TODO: Refactor */
static void GSM_CheckIdleMessages(char * receivedMessage)
{
    for (uint8_t i = 0; receivedMessage[i] != '\0'; i++)
    {
        char * recv = &receivedMessage[i];
        /* TODO: Generalize / Move it to a list */
        if (!StrCmpFirst("RING", recv))
        {
            /* Ongoing call */
            DEBUG_PRINT("Ongoing call");
            GSM_Information.callIsOngoing = true;
        }
        else if (!StrCmpFirst("NO CARRIER", recv))
        {
            /* Terminated call */
            DEBUG_PRINT("Terminated call");
            GSM_Information.callIsOngoing = false;

        }
        else if (!StrCmpFirst("+CLIP", recv))
        {
            DEBUG_PRINT("Calling...");
            /* +CLIP: "+36705808642",145,"",0,"",0 */
            /* TODO: !!!!! Implement */
            /* TODO: There is no defend for buffer overwrite */
            //uint32_ clip_int_1;
            //uint32_ clip_int_2;
            //uint32_ clip_int_3;
            //size_t clip_retval = string_scanf(lastCaller, "+CLIP: \"+%s\",%d,\"\",%d,\"\",%d", lastCaller, &csq_1, &csq_2);
            size_t clip_retval = string_scanf(recv, "+CLIP: \"+%s\",%d,\"\",%d,\"\",%d", GSM_Information.lastCaller);
            if (clip_retval > 0)
            {
                /* Self-defense */
                GSM_Information.lastCaller[GSM_TELEPHONE_NUMBER_STRING_LENGTH-1] = '\0';
                DEBUG_PRINTF("Caller found: %s", GSM_Information.lastCaller);
                GSM_Information.lastCallerIsValid = true;
            }

            GSM_Information.callIsOngoing = true;
        }
        else
        {
            /* Unknown - maybe next turn we find somethings*/
            /* DEBUG_PRINT("Unknown message received"); */
        }

        /* Search first \r\n */
        const char * newline = STRING_FindCharacters((const char *)&receivedMessage[i], "\r\n");
        if (newline)
        {
            i += (newline - &receivedMessage[i]); /* Jump to the \r\n */
        }
    }
}


/* TODO: Check Idle messages: Ring, etc */


/* TODO: Print Information struct (e.g. Signal Quality) */


/* TODO: Adding AT commands lists
 *
 * AT+CMGS=+ZZxxxxxxxxxx – Sends SMS to the phone number specified. The text message
 *
 *
 * Set AT+CLTS=1, it means user can receive network time updating
 * and use AT+CCLK to show current time.
 *
 *
 * "AT+CNMI=1,2,0,0,0" // Decides how newly arrived SMS messages should be handled
 *
 *
 *
 * List all SMS messages. AT+CMGL="ALL"
 *
 * AT+CNMI=2,2,0,0,0
 * +CMT: "+36705808642","","22/02/17,20:01:22+04"
 * Newsms
 *
 *
 */


#endif /* #ifdef CONFIG_MODULE_GSM_ENABLE */
