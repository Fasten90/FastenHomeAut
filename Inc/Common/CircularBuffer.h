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

typedef struct
{
	char * const buffer;			///< Buffer pointer
	const uint16_t size;			///< Size
	volatile uint16_t writeCnt;		///< Write counter
	volatile uint16_t readCnt;		///< Read counter
	const char * name;				///< Buffer name
} CircularBufferInfo_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void CircularBuffer_Init(CircularBufferInfo_t *circBuff);
uint16_t CircularBuffer_GetCharacters(CircularBufferInfo_t *circBuff, char * message, bool putEnd);
void CircularBuffer_ClearLast(CircularBufferInfo_t *circBuff);
void CircularBuffer_FullClear(CircularBufferInfo_t *circBuff);
void CircularBuffer_FindLastMessage(CircularBufferInfo_t *circBuff);
bool CircularBuffer_HasNewMessage(CircularBufferInfo_t *circBuff);

void CircularBuffer_UnitTest(void);



#endif /* COMMON_CIRCULARBUFFER_H_ */
