/*
 *    Communication.c
 *    Created on:   2016-12-03
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Communication module
 *    Target:       STM32Fx
 */



#include "options.h"
#include "GenericTypeDefs.h"

#ifdef CONFIG_MODULE_COMMUNICATION_ENABLE

#include "compiler.h"
#include <stdarg.h>
#include "StringHelper.h"
#include "DebugUart.h"
#include "ESP8266.h"
#include "MemHandler.h"
#include "Communication.h"



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
char Communication_Buffer[COMMUNICATION_PROTOCOL_BUFFER_SIZE];
uint8_t Communication_BufferCnt = 0;
#endif

///< Protocol names
static const char * const ProtocolNameList[] =
{
    "Unknown",
    "DebugUart",
#ifdef CONFIG_SWO_ENABLE
    "SWO"
#endif
#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
    "Buffer",
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
    "ESP8266Wifi",
#endif

    /*
     * XXX: Do not forget Synchronize with CommProtocol_t
     */
};



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief       Initialize Communication (check lists)
 */
void COMMUNICATION_Init()
{
    /* Check Communication list size */
    BUILD_ASSERT(NUM_OF(ProtocolNameList) == CommProt_Count);
}



/**
 * @brief       Send message (string) on selected communication protocol
 */
size_t COMMUNICATION_SendMessage(CommProtocol_t protocol, const char *message)
{
    size_t length = 0;

    switch (protocol)
    {
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
        case CommProt_Unknown:
            /* Unknown, send on debug */
            length = DebugUart_SendMessage(message);
            break;
        case CommProt_DebugUart:
            /* Send on Usart */
            length = DebugUart_SendMessage(message);
            break;
#endif
#ifdef CONFIG_SWO_ENABLE
        case CommProt_SWO:
            /* Send on SWO */
            length = SWO_SendMessage(message);
            break;
#endif
#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
        case CommProt_Buffer:
            length = StrCpyMax(&Communication_Buffer[Communication_BufferCnt], message, COMMUNICATION_PROTOCOL_BUFFER_SIZE-Communication_BufferCnt);
            Communication_BufferCnt += length;
            break;
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
        case CommProt_ESP8266Wifi:
            /* Send on wifi (TCP message) */
            length = ESP8266_RequestSendTcpMessage(message);
            break;
#endif
        case CommProt_Count:
        default:
            /* Error, do not use */
            length = 0;
            break;
    }

    return length;
}



/**
 * @brief       Send character on selected communication protocol
 */
size_t COMMUNICATION_SendChar(CommProtocol_t protocol, char c)
{
    switch (protocol)
    {
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
        case CommProt_Unknown:
            /* Unknown, send on debug */
            DebugUart_SendChar(c);
            break;
        case CommProt_DebugUart:
            DebugUart_SendChar(c);
            break;
#endif
#ifdef CONFIG_SWO_ENABLE
        case CommProt_SWO:
            SWO_SendChar(c);
            break;
#endif
#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
        case CommProt_Buffer:
            Communication_Buffer[Communication_BufferCnt++] = c;
            break;
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
        case CommProt_ESP8266Wifi:
            DebugUart_SendChar(c);
            break;
#endif
        case CommProt_Count:
        default:
            break;
    }

    return 1;
}



/**
 * @brief       Send message on xy communication protocol
 * @param       protocol        what peripheral sending
 */
size_t COMMUNICATION_Printf(CommProtocol_t protocol, const char *format, ...)
{
    size_t length = 0;

    /* Working in at: */
    char txBuffer[COMMUNICATION_TXBUFFER_SIZE];

#ifdef CONFIG_DEBUG_MODE
    txBuffer[COMMUNICATION_TXBUFFER_SIZE-1] = 0xEF;
#endif

    va_list ap;                                    /* argument pointer */
    va_start(ap, format);                         /* ap on arg */
    string_printf(txBuffer, format,ap);            /* Separate and process */
    va_end(ap);                                     /* Cleaning after end */

#ifdef CONFIG_DEBUG_MODE
    if (txBuffer[COMMUNICATION_TXBUFFER_SIZE-1] != 0xEF) DEBUG_BREAKPOINT();
#endif

    length = COMMUNICATION_SendMessage(protocol, txBuffer);

    return length;
}



/**
 * @brief       Get protocol name
 */
const char * COMMUNICATION_GetProtocolName(CommProtocol_t protocol)
{
    const char * str = NULL;

    if (protocol < CommProt_Count)
    {
        str = ProtocolNameList[protocol];
    }

    return str;
}



#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
/**
 * @brief       Protocol buffer clear
 */
void COMMUNICATION_ClearProtocolBuffer(void)
{
    memset(Communication_Buffer, 0, COMMUNICATION_PROTOCOL_BUFFER_SIZE);
    Communication_BufferCnt = 0;
}
#endif    /* #ifdef CONFIG_PROTOCOL_BUFFER_ENABLE */

#else     /* #ifdef CONFIG_MODULE_COMMUNICATION_ENABLE */

/* Communication module is not used */
#if !defined(_MSC_VER)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    #pragma GCC diagnostic pop
#else
    /* MSVC */
    /* Suppress "nonstandard extension used: translation unit is empty" warning */
    /* warning C4206:  nonstandard extension used: translation unit is empty */
    typedef uint32_t Compiler_Communication_TranslationUnitIsEmptySupression_t;
#endif


#endif    /* #ifdef CONFIG_MODULE_COMMUNICATION_ENABLE */
