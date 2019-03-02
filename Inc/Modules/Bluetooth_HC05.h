/*
 *    Bluetooth_HC05.h
 *    Created on:   2017-10-21
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef BLUETOOTH_HC05_H_
#define BLUETOOTH_HC05_H_




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



#endif /* BLUETOOTH_HC05_H_ */
