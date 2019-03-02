/*
 *        Table.h
 *        Created on:        2017-12-21
 *        Author:            Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        -
 *        Target:            STM32Fx
 *        Version:        -
 *        Last modified:    2017-12-21
 */

#ifndef COMMON_HELPER_TABLE_H_
#define COMMON_HELPER_TABLE_H_



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

void Table_PrintTableWithBorder(const char * fixheader, char * str, char * header, ...);



#endif /* COMMON_HELPER_TABLE_H_ */
