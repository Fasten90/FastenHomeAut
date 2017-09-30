/*
 *		Globals.h
 *		Created on:		2016-09-15
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Global variables
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-11-23
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_


#include "include.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const char Global_BoardName[];
extern const char Global_Version[];
extern const char Global_BoardMCU[];
extern char Global_DeviceName[];



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void DelayMs(uint32_t ms);
void Error_Handler(void);



#endif /* GLOBALS_H_ */
