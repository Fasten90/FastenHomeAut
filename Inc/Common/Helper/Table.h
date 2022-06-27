/*
 *    Table.h
 *    Created on:   2017-12-21
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef TABLE_H_
#define TABLE_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "DebugUart.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define TABLE_SEND_BLOCKED            (1)

#if (TABLE_SEND_BLOCKED == 1)
#define Table_SendLine(_str)                DebugUart_SendLineBlocked(_str)
#else
#define Table_SendLine(_str)                DebugUart_SendLine(_str)
#endif



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Table_PrintTableWithBorder(const char * fixheader, char * str, uint8_t strMaxLen, char * header, ...);



#endif /* TABLE_H_ */
