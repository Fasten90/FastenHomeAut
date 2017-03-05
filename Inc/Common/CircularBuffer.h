/*
 *		CircularBuffer.h
 *
 *		Created on:		2017. márc. 4.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. márc. 4.
 */

#ifndef COMMON_CIRCULARBUFFER_H_
#define COMMON_CIRCULARBUFFER_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

uint8_t CircularBuffer_GetCharacters(char *receiveBuffer, char *str,
		uint16_t bufferSize, uint16_t writeCnt, uint16_t readCnt, bool putEnd);
void CircularBuffer_Clear(char *receiveBuffer,
		uint16_t bufferSize, uint16_t readCnt, uint16_t writeCnt);

void CircularBuffer_UnitTest(void);



#endif /* COMMON_CIRCULARBUFFER_H_ */
