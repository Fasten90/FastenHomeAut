/*
 *        Bluetooth_HC05.h
 *        Created on:        2017-10-21
 *        Author:            Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        -
 *        Target:            STM32Fx
 *        Version:        -
 *        Last modified:    2017-10-21
 */

#ifndef MODULES_BLUETOOTH_HC05_H_
#define MODULES_BLUETOOTH_HC05_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define BLUETOOTH_RX_BUFFER_SIZE            (100)
#define BLUETOOTH_TX_BUFFER_SIZE            (100)

#define BLUETOOTH_PROCESS_BUFFER_SIZE        (100)
#define BLUETOOTH_RESPONSE_BUFFER_SIZE        (100)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern UART_HandleTypeDef Bluetooth_UartHandle;
extern UART_Handler_t Bluetooth;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Bluetooth_HC05_Init(void);
void Bluetooth_ReceiveEnable(void);
size_t Bluetooth_SendMessage(const char *msg);
void Bluetooth_ProcessReceivedCharacters(void);



#endif /* MODULES_BLUETOOTH_HC05_H_ */
