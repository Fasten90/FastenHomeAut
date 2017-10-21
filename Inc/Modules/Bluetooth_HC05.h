/*
 *		Bluetooth_HC05.h
 *		Created on:		2017-10-21
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017-10-21
 */

#ifndef MODULES_BLUETOOTH_HC05_H_
#define MODULES_BLUETOOTH_HC05_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define BLUETOOTH_TX_BUFFER_SIZE		(100)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern UART_HandleTypeDef Bluetooth_UartHandle;
extern bool Bluetooth_SendEnable_flag;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Bluetooth_HC05_Init(void);
void Bluetooth_SendMessage(const char * msg);

bool Bluetooth_GetSendEnable(void);



#endif /* MODULES_BLUETOOTH_HC05_H_ */
