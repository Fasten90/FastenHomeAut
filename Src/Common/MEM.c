/*
 * MEM.c
 *
 *  Created on: Aug 31, 2016
 *      Author: Vizi Gábor
 */


#include <stdlib.h>	// For size_t
#include <stdint.h>	// For uintx_t
#include "DebugUart.h"
#include "MEM.h"

#ifdef MODULE_MEM_UNITTEST_ENABLE
#include "UnitTest.h"
#endif


/**
 * \brief	Memory copy
 * \param[out]	destination	where to copy
 * \param[in]	source		from copy
 * \param[in]	num			How many length to copy (in bytes)?
 */
void * memcpy(void * destination, const void * source, size_t num)
{
	size_t i;
	uint8_t *dest = destination;
	const uint8_t *src = source;

	for (i=0; i < num; i++)
	{
		dest[i] = src[i];
	}

	return NULL;

}



/**
 * \brief		Set memory
 * \param[out]	*ptr	Which memory area need set
 * \param[in]	value	With which value
 * \param[in]	num		How many length to set (in bytes)?
 */
void * memset(void * ptr, int value, size_t num)
{
	size_t i;
	uint8_t *dest = ptr;

	for (i=0; i < num; i++)
	{
		dest[i] = (uint8_t)value;
	}

	return NULL;

}



/**
 * \brief	Memory move
 * \param[out]	destination	where to copy
 * \param[in]	source		from copy
 * \param[in]	num			How many length to move (in bytes)?
 */
void * memmove(void * destination, const void * source, size_t num)
{
	size_t i;
	uint8_t *dest = destination;
	uint8_t *src = (uint8_t *)source;

	for (i=0; i < num; i++)
	{
		dest[i] = src[i];
		src[i] = 0;
	}

	return NULL;
}



/**
 * \brief		Initialize memory area with 0
 * \param[out]	*ptr	Which area
 * \param[in]	num		How many length (in bytes)?
 */
void * meminit(void * ptr, size_t num)
{
	return memset(ptr,0,num);
}



/**
 * \brief		Compare two memory buffer
 * \param[in]	*ptr1	first buffer
 * \param[in]	*ptr2	second buffer
 * \param[in]	num		buffer length (compare length)
 * \retval		0		if equal
 * \retval		<0		first buffer has lower value
 * \retval		>0		first buffer has greater value
 */
int memcmp(const void * ptr1, const void * ptr2, size_t num)
{
	size_t i;
	const uint8_t *buffer1 = ptr1;
	const uint8_t *buffer2 = ptr2;

	for (i = 0; i < num; i++)
	{
		if (buffer1[i] < buffer2[i])
		{
			return (-1-i);
		}
		else if (buffer1[i] > buffer2[i])
		{
			return (1+i);
		}
	}

	// If equal (there is no different)
	return 0;
}



/**
 * \brief	Stack overflow checker
 * 			Fill large RAM buffer with GUARD values
 * 			Useful for Stack size calculate (StackOverFlow checker)
 */
void mem_StackFillWithGuardValues(void)
{
	uint8_t stackOverFlowCheckerVariable[1000];
	memset(stackOverFlowCheckerVariable, 0xEF, 1000);
}



/**
 * \brief	Check memory with local variable: how many bytes not filled by GUARD values?
 */
void mem_CheckStackGuardValues(void)
{
	uint16_t i = 0;
	uint16_t guardGoodCnt = 0;
	bool guardWasFound = false;
	// Do not initialize buffer with fix values!!!!
	uint8_t stackOverFlowCheckerVariable[1000];

	for (i = 0; i < 1000; i++)
	{
		if (!guardWasFound && stackOverFlowCheckerVariable[i] == 0xEF)
		{
			guardWasFound = true;
			guardGoodCnt = i;
			uprintf("Guard was found at %d. byte\r\n", i);
		}
		else if (guardWasFound && stackOverFlowCheckerVariable[i] != 0xEF)
		{
			uprintf("Guard was wrong at %d. byte\r\n", i);
			guardWasFound = false;
		}
	}

	uprintf("MEM used: %d / %d, it is %d%%\r\n", guardGoodCnt, 1000, guardGoodCnt*100/1000);
}



#ifdef MODULE_MEM_UNITTEST_ENABLE
/**
 * \brief	MEM module Unit Test
 */
void MEM_UnitTest(void)
{
	UnitTest_Start("MEM", __FILE__);



	// Test memcmp
	const uint8_t testBuffer1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	uint8_t testBuffer2[10];
	uint8_t i;
	for (i = 0; i < 10; i++)
	{
		testBuffer2[i] = i;
	}

	// Equal buffer
	UNITTEST_ASSERT(!memcmp(testBuffer1, testBuffer2, 10), "memcmp");

	// Different buffer
	testBuffer2[9] = 0;
	UNITTEST_ASSERT(memcmp(testBuffer1, testBuffer2, 10), "memcmp");

	testBuffer2[9] = 9;
	UNITTEST_ASSERT(!memcmp(testBuffer1, testBuffer2, 10), "memcmp");



	// Test meminit
	testBuffer2[0] = 0xFF;
	testBuffer2[9] = 0xFF;
	meminit(&testBuffer2[1], 8);
	for (i = 1; i < 9; i++)
	{
		UNITTEST_ASSERT(testBuffer2[i] == 0, "meminit");
	}
	UNITTEST_ASSERT((testBuffer2[0] == 0xFF), "meminit");
	UNITTEST_ASSERT((testBuffer2[9] == 0xFF), "meminit");



	// Test memcpy
	uint8_t testBuffer3[10];
	testBuffer3[0] = 0xFF;
	testBuffer3[9] = 0xFF;

	memcpy(&testBuffer3[1], "12345678", 8);

	UNITTEST_ASSERT((!memcmp("12345678", &testBuffer3[1], 8)), "memcpy");
	UNITTEST_ASSERT((testBuffer3[0] == 0xFF), "memcpy");
	UNITTEST_ASSERT((testBuffer3[9] == 0xFF), "memcpy");



	// Test memset
	testBuffer3[0] = 0xFF;
	testBuffer3[9] = 0xFF;
	memset(&testBuffer3[1], 0xAA, 8);
	for (i = 1; i < 9; i++)
	{
		UNITTEST_ASSERT((testBuffer3[i] == 0xAA), "memset");
	}
	UNITTEST_ASSERT((testBuffer3[0] == 0xFF), "memset");
	UNITTEST_ASSERT((testBuffer3[9] == 0xFF), "memset");


	memset(&testBuffer3[1], 0, 8);
	for (i = 1; i < 9; i++)
	{
		UNITTEST_ASSERT((testBuffer3[i] == 0), "memset");
	}
	UNITTEST_ASSERT((testBuffer3[0] == 0xFF), "memset");
	UNITTEST_ASSERT((testBuffer3[9] == 0xFF), "memset");



	// Test memmove
	testBuffer2[0] = 0xFF;
	testBuffer2[9] = 0xFF;
	testBuffer3[0] = 0xFF;
	testBuffer3[9] = 0xFF;
	memset(&testBuffer2[1], 0xAA, 8);
	memset(&testBuffer3[1], 0xFF, 8);

	memmove(&testBuffer3[1], &testBuffer2[1], 8);

	for (i = 1; i < 9; i++)
	{
		UNITTEST_ASSERT((testBuffer3[i] == 0xAA), "memset");
	}
	UNITTEST_ASSERT((testBuffer3[0] == 0xFF), "memset");
	UNITTEST_ASSERT((testBuffer3[9] == 0xFF), "memset");


	// Finish
	UnitTest_End();
}
#endif	// #ifdef MODULE_MEM_UNITTEST_ENABLE
