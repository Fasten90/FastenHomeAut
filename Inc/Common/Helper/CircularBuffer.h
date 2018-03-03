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

#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

///< Statistics for CircularBuffer	(1 = Enable statistics, 0 = disabled)
#define CIRCULARBUFFER_STATISTICS_ENABLE			(0)

///< When Drop characters, it will clear the buffer ('\0')	(1 = Enable clearing, 0 = disable)
#define CIRCULARBUFFER_DROP_WITH_CLEAR				(0)



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
#if (CIRCULARBUFFER_STATISTICS_ENABLE == 1)
	volatile uint32_t statSuccesfulPutCharCnt;		///< All put characters num
	volatile uint32_t statDroppedCharCnt;			///< All dropped characters num
#endif
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
uint16_t CircularBuffer_DropCharacters(CircularBufferInfo_t *circBuff, uint16_t length);
void CircularBuffer_Clear(CircularBufferInfo_t *circBuff);
bool CircularBuffer_IsNotEmpty(CircularBufferInfo_t *circBuff);
bool CircularBuffer_IsFull(CircularBufferInfo_t *circBuff);
bool CircularBuffer_PutChar(CircularBufferInfo_t *circBuff, char c);
uint16_t CircularBuffer_PutString(CircularBufferInfo_t *circBuff, const char *str, uint16_t needCopyLength);

#if (CIRCULARBUFFER_STATISTICS_ENABLE == 1)
size_t CircularBuffer_PrintStatistics(char *str, size_t len, CircularBufferInfo_t *circBuff);
#endif

uint32_t CircularBuffer_UnitTest(void);



#endif /* CIRCULARBUFFER_H_ */
