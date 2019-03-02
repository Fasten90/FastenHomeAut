/*
 *        Communication.h
 *        Created on:        2016-12-03
 *        Author:            Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        Communication module
 *        Target:            STM32Fx
 *        Version:        v1
 *        Last modified:    2016-12-03
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_MODULE_COMMUNICATION_ENABLE

#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros
 *----------------------------------------------------------------------------*/

#define COMMUNICATION_TXBUFFER_SIZE                (255U)
#define COMMUNICATION_PROTOCOL_BUFFER_SIZE        (100)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< Protocol enums
typedef enum
{
    CommProt_Unknown,
    CommProt_DebugUart,
#ifdef CONFIG_SWO_ENABLE
    CommProt_SWO,
#endif
#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
    CommProt_Buffer,
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
    CommProt_ESP8266Wifi,
#endif

    /*
     * XXX: Add here new Protocol, and correct the CommProt_Disable (largest bit be set)
     * \note    Do not forget Synchronize with ProtocolNameList
     */
    CommProt_Count
} CommProtocol_t;

///< Protocol enums - bit
typedef enum
{
    CommProtBit_Unknown        = 0,
    CommProtBit_DebugUart    = (1 << CommProt_DebugUart),
#ifdef CONFIG_SWO_ENABLE
    CommProtBit_SWO            = (1 << CommProt_SWO),
#endif
#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
    CommProtBit_Buffer        = (1 << CommProt_Buffer),
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
    CommProtBit_ESP8266Wifi = (1 << CommProt_ESP8266Wifi),
#endif

    /*
     * XXX: Add here new Protocol, and correct the CommProt_Disable (largest bit be set)
     * \note    Do not forget Synchronize with CommProtocol_t
     */
    ComProtBit_All            = ((1 << CommProt_Count) - 1),
    CommProtBit_Disable        = (1 << CommProt_Count)

} CommProtocolBit_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_PROTOCOL_BUFFER_ENABLE
extern char Communication_Buffer[COMMUNICATION_PROTOCOL_BUFFER_SIZE];
extern uint8_t Communication_BufferCnt;
#endif



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

size_t COMMUNICATION_SendMessage(CommProtocol_t protocol, const char *message);
size_t COMMUNICATION_SendChar(CommProtocol_t protocol, char c);
size_t COMMUNICATION_Printf(CommProtocol_t protocol, const char *format, ...);

const char * COMMUNICATION_GetProtocolName(CommProtocol_t protocol);

void COMMUNICATION_ClearProtocolBuffer(void);

#endif    // #ifdef CONFIG_MODULE_COMMUNICATION_ENABLE



#endif /* COMMUNICATION_H_ */
