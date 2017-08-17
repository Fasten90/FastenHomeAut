/*
 *		CircularBuffer.c
 *
 *		Created on:		2017. márc. 4.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. márc. 4.
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "include.h"
#include "CircularBuffer.h"
#include "MEM.h"

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
 * \brief	Get characters from ReadCnt to WriteCnt
 */
uint8_t CircularBuffer_GetCharacters(char *receiveBuffer, char *str, uint16_t bufferSize, uint16_t writeCnt, uint16_t readCnt, bool putEnd)
{
	uint16_t i = 0;

	if (readCnt < writeCnt)
	{
		// No overflow
		for (i = 0; i < writeCnt-readCnt; i++)
		{
			str[i] = receiveBuffer[readCnt+i];
		}
		// Put end
		if (putEnd)
		{
			str[i] =  '\0';
		}
	}
	else if (readCnt > writeCnt)
	{
		// Buffer to end
		for (i = 0; i < bufferSize-readCnt; i++)
		{
			str[i] = receiveBuffer[readCnt+i];
		}

		uint16_t oldCnt = i;
		// Begin of buffer
		for (i = 0; i < writeCnt; i++)
		{
			str[oldCnt+i] = receiveBuffer[i];
		}
		// Put end
		i += oldCnt;
		if (putEnd)
		{
			str[i] =  '\0';
		}
	}
	// else if (readCnt == writeCnt) - Do nothing

	return i;
}



/**
 * \brief	Clear buffer from readCnt to writeCnt
 */
void CircularBuffer_Clear(char *receiveBuffer, uint16_t bufferSize, uint16_t readCnt, uint16_t writeCnt)
{
	uint16_t i;

	// Check, if writeCnt > bufferSize
	if (writeCnt >= bufferSize)
	{
		// Change to end of buffer (for avoid overflow)
		writeCnt = bufferSize;
	}

	if (readCnt < writeCnt)
	{
		// No overflow
		for (i = readCnt; i < writeCnt; i++)
		{
			receiveBuffer[i] = '\0';
		}
	}
	else if (readCnt > writeCnt)
	{
		// Buffer to end
		for (i = 0; i < bufferSize-readCnt; i++)
		{
			receiveBuffer[readCnt+i] = '\0';
		}

		// Begin of buffer
		for (i = 0; i < writeCnt; i++)
		{
			receiveBuffer[i] = '\0';
		}
	}
	// else if (readCnt == writeCnt) - Do nothing
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
	// TODO: Put some characters to buffer...
	StrCpy(buffer256, "0123456789xx");
	buffer256_writeCnt = sizeof("0123456789") - 1;
	buffer256_readCnt = 0;

	// Test: Get characters
	CircularBuffer_GetCharacters(buffer256, emptyBuffer,
			256, buffer256_writeCnt, buffer256_readCnt, true);

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
	CircularBuffer_GetCharacters(buffer256, emptyBuffer,
			256, buffer256_writeCnt, buffer256_readCnt, true);

	result = !StrCmp(emptyBuffer, "1234567890");
	UNITTEST_ASSERT(result, "ERROR in GetCharacters()");

	// Test: ClearBuffer
	CircularBuffer_Clear(buffer256, 256, buffer256_readCnt, buffer256_writeCnt);
	// Check, buffer is cleared?
	for (i = 0; i < sizeof(1234567890); i++)
	{
		// Check characters
		result = (buffer256[251+i] == '\0');
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
