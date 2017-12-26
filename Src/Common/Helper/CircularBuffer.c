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
#include "MemHandler.h"

#if (CIRCULARBUFFER_STATISTICS_ENABLE == 1)
#include "StringHelper.h"
#endif

#ifdef MODULE_CIRCULARBUFFER_UNITTEST_ENABLE
	#include "StringHelper.h"
	#include "UnitTest.h"
#endif



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef MODULE_CIRCULARBUFFER_UNITTEST_ENABLE
	#define CIRCBUFF_BUFFER_SIZE						(256U)
	#define CIRCULARBUFFER_ERROR()
#else
	#define CIRCULARBUFFER_ERROR()						DEBUG_BREAKPOINT()
#endif

#define CIRCULARBUFFER_CHECK_CIRCBUFFER(cbuff)			if (cbuff == NULL) return 0;
#define CIRCULARBUFFER_CHECK_WRITE_COUNTER(cbuff)		if (cbuff->writeCnt >= cbuff->size) { cbuff->writeCnt = cbuff->size - 1; CIRCULARBUFFER_ERROR(); }
#define CIRCULARBUFFER_CHECK_READ_COUNTER(cbuff)		if (cbuff->readCnt >= cbuff->size) { cbuff->readCnt = cbuff->size - 1; CIRCULARBUFFER_ERROR(); }



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static bool CircularBuffer_IsFull(CircularBufferInfo_t *circBuff);



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
	// TODO: Check: buffer is in RAM?

	memset(circBuff->buffer, 0, circBuff->size);

	circBuff->readCnt = 0;
	circBuff->writeCnt = 0;
}



/**
 * \brief	Return with there is a new character in the buffer?
 * \retval	true	Has new character
 * \retval	false	Hasn't new character
 */
bool CircularBuffer_IsNotEmpty(CircularBufferInfo_t *circBuff)
{
	return (circBuff->readCnt != circBuff->writeCnt);
}



/**
 * \brief	Buffer is full?
 * \retval	true	full
 * \retval	false	not full (has empty space)
 */
static bool CircularBuffer_IsFull(CircularBufferInfo_t *circBuff)
{
	bool isFull = false;

	if (circBuff->readCnt > 0)
	{
		if (circBuff->writeCnt == circBuff->readCnt - 1)
		{
			isFull = true;
		}
	}
	else
	{
		// readCnt = 0
		if (circBuff->writeCnt == (circBuff->size - 1))
		{
			isFull = true;
		}
	}

	return isFull;
}



/**
 * \brief	Clear buffer (set Circular buffer to empty)
 */
void CircularBuffer_Clear(CircularBufferInfo_t *circBuff)
{
	circBuff->readCnt = circBuff->writeCnt;

#if (CIRCULARBUFFER_DROP_WITH_CLEAR == 1)
	// Clear buffer
	memset(circBuff->buffer, 0, circBuff->size);
#endif
}



/**
 * \brief	Get character and step position (get & drop)
 */
bool CircularBuffer_GetChar(CircularBufferInfo_t *circBuff, char * c)
{
	if (circBuff == NULL || c == NULL)
		return false;

	bool isOk = false;

	if (CircularBuffer_IsNotEmpty(circBuff))
	{
		*c = circBuff->buffer[circBuff->readCnt];

		circBuff->readCnt++;
		isOk = true;

		if (circBuff->readCnt >= circBuff->size)
		{
			circBuff->readCnt = 0;
		}
	}

	return isOk;
}



/**
 * \brief	Get characters (from ReadCnt to WriteCnt)
 * \note	Counter not modified --> Need "drop" characters
 */
uint16_t CircularBuffer_GetString(CircularBufferInfo_t *circBuff, char *message, uint16_t maxLen)
{
	uint16_t i = 0;

	// TODO: Check parameters
	if (circBuff == NULL || message == NULL || maxLen <= 1)
		return 0;

	// Check actual Counters
	CIRCULARBUFFER_CHECK_CIRCBUFFER(circBuff);
	CIRCULARBUFFER_CHECK_WRITE_COUNTER(circBuff);
	CIRCULARBUFFER_CHECK_READ_COUNTER(circBuff);
	// TODO: Check message is in RAM?

	// TODO: With GetChar() ?

	if (circBuff->readCnt < circBuff->writeCnt)
	{
		// No overflow
		for (i = 0; i < (circBuff->writeCnt-circBuff->readCnt) && (i < (maxLen - 1)); i++)
		{
			message[i] = circBuff->buffer[circBuff->readCnt+i];
		}

		message[i] =  '\0';
	}
	else if (circBuff->readCnt > circBuff->writeCnt)
	{
		// Buffer to end
		for (i = 0; (i < circBuff->size-circBuff->readCnt) && (i < (maxLen - 1)); i++)
		{
			message[i] = circBuff->buffer[circBuff->readCnt+i];
		}

		uint16_t oldCnt = i;
		// Begin of buffer
		for (i = 0; (i < circBuff->writeCnt) && ((oldCnt + i) < (maxLen - 1)); i++)
		{
			message[oldCnt+i] = circBuff->buffer[i];
		}
		// Put end
		i += oldCnt;

		message[i] =  '\0';
	}
	// else if (circBuff->readCnt == circBuff->writeCnt) - Do nothing, because buffer is empty

	return i;
}



/**
 * \brief	Clear buffer from readCnt (length count)
 * \note	Not cleared all character from readCnt to writeCnt
 */
uint16_t CircularBuffer_DropCharacters(CircularBufferInfo_t *circBuff, uint16_t length)
{
#if (CIRCULARBUFFER_DROP_WITH_CLEAR == 1)
	uint16_t i;
#endif

	if (circBuff->readCnt < circBuff->writeCnt)
	{
		// No overflow
		uint16_t maxLength = circBuff->writeCnt - circBuff->readCnt;
		if (length > maxLength)
		{
			length = maxLength;
		}

#if (CIRCULARBUFFER_DROP_WITH_CLEAR == 1)
		for (i = 0; i < length; i++)
		{
			circBuff->buffer[circBuff->readCnt+i] = '\0';
		}
#endif

		circBuff->readCnt += length;
	}
	else if (circBuff->readCnt > circBuff->writeCnt)
	{
		// "Overflow"
		uint16_t firstClear = circBuff->size - circBuff->readCnt;
		uint16_t secondClear;

		if (firstClear > length)
		{
			// Buffer is not full
			firstClear = length;
			secondClear = 0;
		}
		else
		{
			// firstClear < length
			// -> Overread
			secondClear = length - firstClear;
		}

		if (secondClear > circBuff->writeCnt)
		{
			secondClear = circBuff->writeCnt;
			length -= secondClear - circBuff->writeCnt;
		}

#if (CIRCULARBUFFER_DROP_WITH_CLEAR == 1)
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
#endif

		if (firstClear <= length)
		{
			// "Overread"
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
 * \brief	Put character to Circular buffer
 */
bool CircularBuffer_PutChar(CircularBufferInfo_t *circBuff, char c)
{
	// TODO: Pointer checking
	if (circBuff == NULL)
		return false;

	bool isOk = false;

	// Put to actual position
	if (!CircularBuffer_IsFull(circBuff))
	{
		circBuff->buffer[circBuff->writeCnt] = c;
		circBuff->writeCnt++;
		isOk = true;

#if (CIRCULARBUFFER_STATISTICS_ENABLE == 1)
		circBuff->statSuccesfulPutCharCnt++;
#endif

		if (circBuff->writeCnt >= circBuff->size)
		{
			circBuff->writeCnt = 0;
		}
	}
#if (CIRCULARBUFFER_STATISTICS_ENABLE == 1)
	else
	{
		// Failed put char
		circBuff->statDroppedCharCnt++;
	}
#endif

	return isOk;
}



/**
 * \brief	Put string to CircularBuffer
 */
uint16_t CircularBuffer_PutString(CircularBufferInfo_t *circBuff, const char *str, uint16_t needCopyLength)
{
	uint16_t i = 0;

	if (circBuff == NULL || str == NULL || needCopyLength == 0)
		return 0;

	while (CircularBuffer_PutChar(circBuff, str[i]) && (i < needCopyLength))
	{
		i++;
	}

	return i;
}



#if (CIRCULARBUFFER_STATISTICS_ENABLE == 1)
/**
 * \brief	Print CircularBuffer info and statistics to string
 */
size_t CircularBuffer_PrintStatistics(char *str, size_t len, CircularBufferInfo_t *circBuff)
{
	return usnprintf(str, len, "CircBuff: %s\r\n"
			"Size: %u\r\n"
			"Put characters num: %u\r\n"
			"Dropped characters num: %u\r\n",
			circBuff->name,
			circBuff->size,
			circBuff->statSuccesfulPutCharCnt,
			circBuff->statDroppedCharCnt
			);
}
#endif



#ifdef MODULE_CIRCULARBUFFER_UNITTEST_ENABLE
/**
 * \brief	CircularBuffer UnitTest
 */
void CircularBuffer_UnitTest(void)
{
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
	circBufferInfo.readCnt = 11;
	// Print too long string (free space: 10)
	length = CircularBuffer_PutString(&circBufferInfo, "0123456789xx", 12);
	UNITTEST_ASSERT(length == 10, "PutString() error");
	UNITTEST_ASSERT(!StrCmp(buffer256, "0123456789"), "PutString() error");
	UNITTEST_ASSERT(circBufferInfo.writeCnt == 10, "PutString() error");
	UNITTEST_ASSERT(circBufferInfo.readCnt == 11, "PutString() error");


	circBufferInfo.writeCnt = 10;
	circBufferInfo.readCnt = 0;
	// Print ok
	length = CircularBuffer_PutString(&circBufferInfo, "0123456789xx", 12);
	UNITTEST_ASSERT(length == 12, "PutString() error");
	UNITTEST_ASSERT(!StrCmp(&buffer256[10], "0123456789xx"), "PutString() error");
	UNITTEST_ASSERT(circBufferInfo.writeCnt == 23, "PutString() error");
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
	UNITTEST_ASSERT(circBufferInfo.writeCnt == 7, "PutString() error");
	UNITTEST_ASSERT(circBufferInfo.readCnt == 10, "PutString() error");


	/*
	 * 		Test data in begin of buffer
	 */
	// Put some characters to buffer...
	StrCpy(buffer256, "0123456789xx");
	circBufferInfo.writeCnt = 10;
	circBufferInfo.readCnt = 0;

	// Test: Get characters
	length = CircularBuffer_GetString(&circBufferInfo, emptyBuffer, 20);
	UNITTEST_ASSERT(length == 10, "ERROR in GetCharacters()");
	UNITTEST_ASSERT(!StrCmp(emptyBuffer, "0123456789"), "ERROR in GetCharacters()");
	UNITTEST_ASSERT(circBufferInfo.writeCnt == 10, "PutString() error");
	UNITTEST_ASSERT(circBufferInfo.readCnt == 0, "PutString() error");

	// Test: ClearBuffer
	CircularBuffer_DropCharacters(&circBufferInfo, 10);
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
	length = CircularBuffer_GetString(&circBufferInfo, emptyBuffer, 20);
	UNITTEST_ASSERT(length == 256-251+5, "ERROR in Clear()");
	UNITTEST_ASSERT(!StrCmp(emptyBuffer, "1234567890"), "ERROR in GetCharacters()");

	// Test: ClearBuffer
	length = CircularBuffer_DropCharacters(&circBufferInfo, 10);
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


	// Test wrong counter
	circBufferInfo.readCnt = 256;
	circBufferInfo.writeCnt = 256;
	CircularBuffer_DropCharacters(&circBufferInfo, 10);
	UNITTEST_ASSERT(circBufferInfo.writeCnt == 255, "ERROR: writeCnt wrong");
	UNITTEST_ASSERT(circBufferInfo.readCnt == 255, "ERROR: readCnt wrong");


	// Test: writeCnt > BUFFER_SIZE

	buffer256[255] = 0xEF;	// In buffer end
	buffer256[256] = 0xEF;	// "After buffer"
	circBufferInfo.readCnt = 256;
	circBufferInfo.writeCnt = 200;
	CircularBuffer_DropCharacters(&circBufferInfo, 10);
	UNITTEST_ASSERT(buffer256[255] == '\0', "ERROR: Clear() is not work with too large writeCnt");
	UNITTEST_ASSERT(buffer256[256] == (char)0xEF, "ERROR: Clear() is overflowed()");


	// TODO: Test: CircularBuffer_PutString


	// TODO: Test GetChar()


	// Finish unittest
	UnitTest_End();
}
#endif
