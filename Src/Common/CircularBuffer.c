/*
 *		CircularBuffer.c
 *		Created on:		2017-03-04
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Circular buffer handler
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-03-04
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "include.h"
#include "CircularBuffer.h"
#include "MEM.h"
#include "DebugUart.h"

#ifdef MODULE_CIRCULARBUFFER_UNITTEST_ENABLE
#include "String.h"
#include "UnitTest.h"
#endif



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Initialize CircularBuffer
 */
void CircularBuffer_Init(CircularBufferInfo_t *circBuff)
{
	// Check buffer parameters
	ASSERT(circBuff->buffer != NULL);
	ASSERT(circBuff->size > 0);
	ASSERT(circBuff->name != NULL);

	CircularBuffer_FullClear(circBuff);

	circBuff->readCnt = 0;
	circBuff->writeCnt = 0;
}



/**
 * \brief	Clear entire buffer
 */
void CircularBuffer_FullClear(CircularBufferInfo_t *circBuff)
{
	memset(circBuff->buffer, 0, circBuff->size);
}



/**
 * \brief	Get characters from ReadCnt to WriteCnt
 */
uint16_t CircularBuffer_GetCharacters(CircularBufferInfo_t *circBuff, char * message, bool putEnd)
{
	uint16_t i = 0;

	if (circBuff->readCnt < circBuff->writeCnt)
	{
		// No overflow
		for (i = 0; i < circBuff->writeCnt-circBuff->readCnt; i++)
		{
			message[i] = circBuff->buffer[circBuff->readCnt+i];
		}
		// Put end
		if (putEnd)
		{
			message[i] =  '\0';
		}
	}
	else if (circBuff->readCnt > circBuff->writeCnt)
	{
		// Buffer to end
		for (i = 0; i < circBuff->size-circBuff->readCnt; i++)
		{
			message[i] = circBuff->buffer[circBuff->readCnt+i];
		}

		uint16_t oldCnt = i;
		// Begin of buffer
		for (i = 0; i < circBuff->writeCnt; i++)
		{
			message[oldCnt+i] = circBuff->buffer[i];
		}
		// Put end
		i += oldCnt;
		if (putEnd)
		{
			message[i] =  '\0';
		}
	}
	// else if (circBuff->readCnt == circBuff->writeCnt) - Do nothing

	return i;
}



/**
 * \brief	Clear buffer from readCnt to writeCnt
 */
void CircularBuffer_ClearLast(CircularBufferInfo_t *circBuff)
{
	uint16_t i;

	// Check, if writeCnt > bufferSize
	if (circBuff->writeCnt >= circBuff->size)
	{
		// Change to end of buffer (for avoid overflow)
		circBuff->writeCnt = circBuff->size;
	}

	if (circBuff->readCnt < circBuff->writeCnt)
	{
		// No overflow
		for (i = circBuff->readCnt; i < circBuff->writeCnt; i++)
		{
			circBuff->buffer[i] = '\0';
		}
	}
	else if (circBuff->readCnt > circBuff->writeCnt)
	{
		// Buffer to end
		for (i = 0; i < circBuff->size-circBuff->readCnt; i++)
		{
			circBuff->buffer[circBuff->readCnt+i] = '\0';
		}

		// Begin of buffer
		for (i = 0; i < circBuff->writeCnt; i++)
		{
			circBuff->buffer[i] = '\0';
		}
	}
	// else if (circBuff->readCnt == circBuff->writeCnt) - Do nothing

	circBuff->readCnt = circBuff->writeCnt;
}



/**
 * \brief	Find last character in the Circular buffer
 * \note	Recommend if haven't "PutCharacter" function
 */
void CircularBuffer_FindLastMessage(CircularBufferInfo_t *circBuff)
{
	// TODO: Not a good solve...
	uint16_t i = 0;

	while (circBuff->buffer[circBuff->writeCnt])
	{
		++circBuff->writeCnt;
		++i;

		if (circBuff->writeCnt >= circBuff->size)
		{
			circBuff->writeCnt = 0;
		}

		if (i > circBuff->size)
		{
			// Buffer "overflow" error
			uprintf("Error: CircularBuffer full, clear it: %s\r\n", circBuff->name);
			// Reinit buffer (counter, buffer content)
			CircularBuffer_Init(circBuff);
			// TODO: Error handling, if buffer overflowing?
			break;
		}
	}
}



/**
 * \brief	Return with there is a new character in the buffer?
 * \retval	true	Has new character
 * \retval	false	Hasn't new character
 */
bool CircularBuffer_HasNewMessage(CircularBufferInfo_t *circBuff)
{
	return (circBuff->readCnt != circBuff->writeCnt);
}



#ifdef MODULE_CIRCULARBUFFER_UNITTEST_ENABLE
/**
 * \brief	CircularBuffer UnitTest
 */
void CircularBuffer_UnitTest(void)
{
	char buffer256[257];	// 256 + 1 "overflow" checker byte
	uint16_t buffer256_writeCnt = 0;
	uint16_t buffer256_readCnt = 0;

	char emptyBuffer[20];
	bool result;
	uint8_t i;

	// Start
	UnitTest_Start("CircularBuffer", __FILE__);


	// Test buffer256
	memset(buffer256, 0, 256);


	/*
	 * 		Test data in begin of buffer
	 */
	// Put some characters to buffer...
	StrCpy(buffer256, "0123456789xx");
	buffer256_writeCnt = sizeof("0123456789") - 1;
	buffer256_readCnt = 0;

	// Test: Get characters
	CircularBuffer_GetCharacters(buffer256, emptyBuffer, 256, buffer256_writeCnt, buffer256_readCnt, true);

	result = !StrCmp(emptyBuffer, "0123456789");
	UNITTEST_ASSERT(result, "ERROR in GetCharacters()");

	// Test: ClearBuffer
	CircularBuffer_Clear(buffer256, 256, buffer256_readCnt, buffer256_writeCnt);
	// Check, buffer is cleared?
	for (i = 0; i < sizeof("0123456789")-1; i++)
	{
		// Check characters
		UNITTEST_ASSERT(buffer256[i] == '\0', "ERROR in Clear()");
	}
	// Check, do not overflowed by clear
	UNITTEST_ASSERT(!StrCmp("xx", &buffer256[buffer256_writeCnt]), "ERROR: Clear() is overflowed");


	/*
	 * 		Test data in end of buffer (overflow!)
	 */

	buffer256[251] = '1';
	buffer256[252] = '2';
	buffer256[253] = '3';
	buffer256[254] = '4';
	buffer256[255] = '5';
	buffer256[0] = '6';
	buffer256[1] = '7';
	buffer256[2] = '8';
	buffer256[3] = '9';
	buffer256[4] = '0';
	buffer256[5] = '\0';
	buffer256_writeCnt = 5;
	buffer256_readCnt = 251;

	buffer256[256] = 0xEF;	// "After buffer"

	// Test: Get characters
	CircularBuffer_GetCharacters(buffer256, emptyBuffer, 256, buffer256_writeCnt, buffer256_readCnt, true);

	result = !StrCmp(emptyBuffer, "1234567890");
	UNITTEST_ASSERT(result, "ERROR in GetCharacters()");

	// Test: ClearBuffer
	CircularBuffer_Clear(buffer256, 256, buffer256_readCnt, buffer256_writeCnt);
	// Check, buffer is cleared?
	for (i = 0; i < 256-251; i++)
	{
		// Check characters
		result = (buffer256[251+i] == '\0');
		UNITTEST_ASSERT(result, "ERROR in Clear()");
	}
	for (i = 0; i < 251-256+sizeof("1234567890"); i++)
	{
		// Check characters
		result = (buffer256[i] == '\0');
		UNITTEST_ASSERT(result, "ERROR in Clear()");
	}
	// Check overflow
	UNITTEST_ASSERT(buffer256[256] == 0xEF, "ERROR: Clear() is overflowed()");


	// Test: writeCnt > BUFFER_SIZE

	buffer256[255] = 0xEF;	// In buffer end
	buffer256[256] = 0xEF;	// "After buffer"
	CircularBuffer_Clear(buffer256, 256, 200, 256);
	UNITTEST_ASSERT(buffer256[255] == 0, "ERROR: Clear() is not work with too large writeCnt");
	UNITTEST_ASSERT(buffer256[256] == 0xEF, "ERROR: Clear() is overflowed()");


	// Finish unittest
	UnitTest_End();
}
#endif
