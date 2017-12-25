/*
 *		CircularBuffer.h
 *		Created on:		2017-03-04
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Circular buffer handler
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-03-04
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_



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
	const char * const name;		///< Buffer name
} CircularBufferInfo_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void CircularBuffer_Init(CircularBufferInfo_t *circBuff);
bool CircularBuffer_GetChar(CircularBufferInfo_t *circBuff, char * c);
uint16_t CircularBuffer_GetString(CircularBufferInfo_t *circBuff, char *message, uint16_t maxLen);
uint16_t CircularBuffer_Clear(CircularBufferInfo_t *circBuff, uint16_t length);
void CircularBuffer_ClearLast(CircularBufferInfo_t *circBuff);
void CircularBuffer_FullClear(CircularBufferInfo_t *circBuff);
void CircularBuffer_FindLastMessage(CircularBufferInfo_t *circBuff);
bool CircularBuffer_IsNotEmpty(CircularBufferInfo_t *circBuff);
bool CircularBuffer_PutChar(CircularBufferInfo_t *circBuff, char c);
uint16_t CircularBuffer_PutString(CircularBufferInfo_t *circBuff, const char *str, uint16_t needCopyLength);

void CircularBuffer_UnitTest(void);



#endif /* CIRCULARBUFFER_H_ */
