/*
 *    GSM_SIM800.h
 *    Created on:   2022-02-17
 *    Author:       faste
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef GSM_SIM800_H_
#define GSM_SIM800_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef struct {
	bool isValid;
	uint8_t csq; 				///< Signal Qulity 0-3x?
	uint8_t creg_1;				///< Creg value
	uint8_t creg_2;				///< Creg value - 2.
	bool connectionIsActive;	///< Connection is active?
	bool callIsOngoing;			///< Call is ongoing?
} GSM_InformationStruct;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern UART_HandleTypeDef GSM_UartHandle;
extern UART_Handler_t     GSM_Uart;

extern GSM_InformationStruct GSM_Information;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void GSM_SIM800_Init(void);

void GSM_TaskFunction(ScheduleSource_t source);

size_t GSM_SendMsg(const char *msg);



#endif /* GSM_SIM800_H_ */
