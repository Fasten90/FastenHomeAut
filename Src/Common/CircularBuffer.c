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
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define CIRCULARBUFFER_CHECK_CIRCBUFFER(cbuff)			if (cbuff == NULL) return 0;
#define CIRCULARBUFFER_CHECK_WRITE_COUNTER(cbuff)		if (cbuff->writeCnt >= cbuff->size) cbuff->writeCnt = cbuff->size - 1;
#define CIRCULARBUFFER_CHECK_READ_COUNTER(cbuff)		if (cbuff->readCnt >= cbuff->size) cbuff->readCnt = cbuff->size - 1;



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
 * \note	Counter not modified --> Need "drop" characters
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
 * \brief	Clear buffer from readCnt (length count)
 * \note	Not cleared all character from readCnt to writeCnt
 */
uint16_t CircularBuffer_Clear(CircularBufferInfo_t *circBuff, uint16_t length)
{
	uint16_t i;

	// Check, Counter
	CIRCULARBUFFER_CHECK_CIRCBUFFER(circBuff);
	CIRCULARBUFFER_CHECK_WRITE_COUNTER(circBuff);
	CIRCULARBUFFER_CHECK_READ_COUNTER(circBuff);

	if (circBuff->readCnt < circBuff->writeCnt)
	{
		// No overflow
		uint16_t counterLength = circBuff->writeCnt - circBuff->readCnt;
		if (length > counterLength)
		{
			length = counterLength;
		}

		for (i = 0; i < length; i++)
		{
			circBuff->buffer[i] = '\0';
		}

		circBuff->readCnt += length;
	}
	else if (circBuff->readCnt > circBuff->writeCnt)
	{
		// "Overflow"
		uint16_t firstClear = circBuff->size - circBuff->readCnt;
		uint16_t secondClear;
		if (firstClear > length)
		{
			firstClear = length;
			secondClear = 0;
		}
		else
		{
			secondClear = length - firstClear;
		}

		if (secondClear > circBuff->writeCnt)
		{
			secondClear = circBuff->writeCnt;
			length -= secondClear - circBuff->writeCnt;
		}

		// Clear Buffer to end
		for (i = 0; i < firstClear; i++)
		{
			circBuff->buffer[circBuff->readCnt + i] = '\0';
		}

		// Clear from begin of buffer
		for (i = 0; i < secondClear; i++)
		{
			circBuff->buffer[i] = '\0';
		}

		if (firstClear > length)
		{
			circBuff->readCnt = secondClear;
		}
		else
		{
			circBuff->readCnt += length;
		}
	}
	else	// else if (circBuff->readCnt == circBuff->writeCnt)
	{
		length = 0;
	}

	return length;
}


/**
 * \brief	Clear buffer from readCnt to writeCnt
 */
void CircularBuffer_ClearLast(CircularBufferInfo_t *circBuff)
{
	uint16_t length = 0;;

	if (circBuff->readCnt < circBuff->writeCnt)
	{
		length = circBuff->writeCnt - circBuff->readCnt;
	}
	else if (circBuff->writeCnt < circBuff->readCnt)
	{
		length = (circBuff->size - circBuff->readCnt) + circBuff->writeCnt;
	}

	CircularBuffer_Clear(circBuff, length);
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



/**
 * \brief	Put string to CircularBuffer
 */
uint16_t CircularBuffer_PutString(CircularBufferInfo_t *circBuff, char *str, uint16_t needCopyLength)
{
	uint16_t i;

	if (needCopyLength == 0)
		return 0;
	if (circBuff->writeCnt == circBuff->readCnt)		// Cannot put to CircularBuffer
		return 0;

	if (((circBuff->writeCnt + needCopyLength) < circBuff->size)
			&& (circBuff->writeCnt > circBuff->readCnt))
	{
		// Normal situation (Not need handle overflow)
		for (i = 0; i < needCopyLength; i++)
		{
			circBuff->buffer[i + circBuff->writeCnt] = str[i];
		}

		circBuff->writeCnt += needCopyLength;
	}
	else if (((circBuff->writeCnt + needCopyLength) < circBuff->size)
			&& (circBuff->writeCnt < circBuff->readCnt))
	{
		// Normal situation (but read counter after writeCounter)
		needCopyLength = circBuff->readCnt - circBuff->writeCnt;

		for (i = 0; i < needCopyLength; i++)
		{
			circBuff->buffer[i + circBuff->writeCnt] = str[i];
		}

		circBuff->writeCnt += needCopyLength;
	}
	else
	{
		// Overflow:
		uint16_t firstCopy = circBuff->size - circBuff->writeCnt;
		uint16_t secondCopy = needCopyLength - firstCopy;
		if (secondCopy >= circBuff->readCnt)
		{
			secondCopy = circBuff->readCnt - 1;
			needCopyLength = firstCopy + secondCopy;
		}

		for (i = 0; i < firstCopy; i++)
		{
			circBuff->buffer[i + circBuff->writeCnt] = str[i];
		}

		// Second copy from begin of CircularBuffer
		for (i = 0; i < secondCopy; i++)
		{
			circBuff->buffer[i] = str[i + firstCopy];
		}

		circBuff->writeCnt = secondCopy;
	}

	return needCopyLength;
}



#ifdef MODULE_CIRCULARBUFFER_UNITTEST_ENABLE
/**
 * \brief	CircularBuffer UnitTest
 */
void CircularBuffer_UnitTest(void)
{
#define CIRCBUFF_BUFFER_SIZE	(256U)

	char buffer256[CIRCBUFF_BUFFER_SIZE + 1];	// size + 1 "overflow" checker byte

	CircularBufferInfo_t circBufferInfo =
	{
		.buffer = buffer256,
		.name = "TestBuffer",
		.size = CIRCBUFF_BUFFER_SIZE
	};


	char emptyBuffer[20];
	uint16_t i;
	uint16_t length;


	// Start
	UnitTest_Start("CircularBuffer", __FILE__);


	/*
	 * Check Init()
	 */

	// Make wrong cnt-s
	circBufferInfo.readCnt = 1;
	circBufferInfo.writeCnt = 1;
	buffer256[0] = 1;
	// Init
	CircularBuffer_Init(&circBufferInfo);
	// Check results
	UNITTEST_ASSERT(circBufferInfo.readCnt == 0, "Init() error");
	UNITTEST_ASSERT(circBufferInfo.writeCnt == 0, "Init() error");
	UNITTEST_ASSERT(circBufferInfo.buffer[0] == '\0', "Init() error");


	/*
	 * Test PutString()
	 */
	circBufferInfo.writeCnt = 0;
	circBufferInfo.readCnt = 10;
	// Print too long string (free space: 10)
	length = CircularBuffer_PutString(&circBufferInfo, "0123456789xx", 12);
	UNITTEST_ASSERT(length == 10, "PutString() error");
	UNITTEST_ASSERT(!StrCmp(buffer256, "0123456789"), "PutString() error");
	UNITTEST_ASSERT(circBufferInfo.writeCnt == 10, "PutString() error");
	UNITTEST_ASSERT(circBufferInfo.readCnt == 10, "PutString() error");


	circBufferInfo.writeCnt = 10;
	circBufferInfo.readCnt = 0;
	// Print ok
	length = CircularBuffer_PutString(&circBufferInfo, "0123456789xx", 12);
	UNITTEST_ASSERT(length == 12, "PutString() error");
	UNITTEST_ASSERT(!StrCmp(&buffer256[10], "0123456789xx"), "PutString() error");
	UNITTEST_ASSERT(circBufferInfo.writeCnt == 22, "PutString() error");
	UNITTEST_ASSERT(circBufferInfo.readCnt == 0, "PutString() error");


	// Test at end of buffer
	buffer256[256] = 0xEF;
	circBufferInfo.writeCnt = 250;
	circBufferInfo.readCnt = 10;
	// Check overflow
	length = CircularBuffer_PutString(&circBufferInfo, "0123456789xx", 12);
	UNITTEST_ASSERT(length == 12, "PutString() error");
	UNITTEST_ASSERT(!StrCmpWithLength(&buffer256[250], "012345", 6), "PutString() error");
	UNITTEST_ASSERT(!StrCmpWithLength(&buffer256[0], "6789xx", 6), "PutString() error");
	UNITTEST_ASSERT(buffer256[256] == (char)0xEF, "PutString() error");
	UNITTEST_ASSERT(circBufferInfo.writeCnt == 6, "PutString() error");
	UNITTEST_ASSERT(circBufferInfo.readCnt == 10, "PutString() error");


	/*
	 * 		Test data in begin of buffer
	 */
	// Put some characters to buffer...
	StrCpy(buffer256, "0123456789xx");
	circBufferInfo.writeCnt = 10;
	circBufferInfo.readCnt = 0;

	// Test: Get characters
	length = CircularBuffer_GetCharacters(&circBufferInfo, emptyBuffer, true);
	UNITTEST_ASSERT(length == 10, "ERROR in GetCharacters()");
	UNITTEST_ASSERT(!StrCmp(emptyBuffer, "0123456789"), "ERROR in GetCharacters()");
	UNITTEST_ASSERT(circBufferInfo.writeCnt == 10, "PutString() error");
	UNITTEST_ASSERT(circBufferInfo.readCnt == 0, "PutString() error");

	// Test: ClearBuffer
	CircularBuffer_Clear(&circBufferInfo, 10);
	// Check, buffer is cleared?
	for (i = 0; i < 10; i++)
	{
		// Check characters
		UNITTEST_ASSERT(buffer256[i] == '\0', "ERROR in Clear()");
	}
	// Check, do not overflowed by clear
	UNITTEST_ASSERT(!StrCmp("xx", &buffer256[10]), "ERROR: Clear() is overflowed");


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
	circBufferInfo.writeCnt = 5;
	circBufferInfo.readCnt = 251;

	buffer256[256] = 0xEF;	// "After buffer"

	// Test: Get characters
	length = CircularBuffer_GetCharacters(&circBufferInfo, emptyBuffer, true);
	UNITTEST_ASSERT(length == 256-251+5, "ERROR in Clear()");
	UNITTEST_ASSERT(!StrCmp(emptyBuffer, "1234567890"), "ERROR in GetCharacters()");

	// Test: ClearBuffer
	length = CircularBuffer_Clear(&circBufferInfo, 10);
	UNITTEST_ASSERT(length == 256-251+5, "ERROR in Clear()");

	// Check, buffer is cleared?
	for (i = 0; i < 256-251; i++)
	{
		// Check characters
		UNITTEST_ASSERT((buffer256[251+i] == '\0'), "ERROR in Clear()");
	}
	for (i = 0; i < 251-256+sizeof("1234567890"); i++)
	{
		// Check characters
		UNITTEST_ASSERT((buffer256[i] == '\0'), "ERROR in Clear()");
	}
	// Check overflow
	UNITTEST_ASSERT(buffer256[256] == (char)0xEF, "ERROR: Clear() is overflowed()");


	circBufferInfo.readCnt = 256;
	circBufferInfo.writeCnt = 256;
	CircularBuffer_Clear(&circBufferInfo, 10);
	UNITTEST_ASSERT(circBufferInfo.writeCnt == 255, "ERROR: writeCnt wrong");
	UNITTEST_ASSERT(circBufferInfo.readCnt == 255, "ERROR: readCnt wrong");


	// Test: writeCnt > BUFFER_SIZE

	buffer256[255] = 0xEF;	// In buffer end
	buffer256[256] = 0xEF;	// "After buffer"
	circBufferInfo.readCnt = 256;
	circBufferInfo.writeCnt = 200;
	CircularBuffer_Clear(&circBufferInfo, 10);
	UNITTEST_ASSERT(buffer256[255] == '\0', "ERROR: Clear() is not work with too large writeCnt");
	UNITTEST_ASSERT(buffer256[256] == (char)0xEF, "ERROR: Clear() is overflowed()");


	// TODO: Test: CircularBuffer_PutString


	// Finish unittest
	UnitTest_End();
}
#endif
