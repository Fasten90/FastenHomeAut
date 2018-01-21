/*
 *		ErrorHandler.h
 *		Created on:		2016-09-15
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Global variables
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-11-23
 */

#ifndef ERRORHANDLER_H_
#define ERRORHANDLER_H_


#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Error_Handler(void);
void Assert_Function(char *file, uint32_t line, char *exp);

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line);
#endif



#endif /* ERRORHANDLER_H_ */
