/*
 *		swo.h
 *
 *		Created on:		2016. dec. 14.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2016. dec. 14.
 */

#ifndef SWO_H_
#define SWO_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

uint8_t SWO_SendMessage(const char *message);
uint8_t SWO_SendChar(uint8_t ch);



#endif /* SWO_H_ */
