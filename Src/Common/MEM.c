/*
 *		MEM.c
 *
 *		Created on:		2016.08.31
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		MEM functions
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017.07.15
 */

#include <stdlib.h>	// For size_t
#include <stdint.h>	// For uintx_t
#include "DebugUart.h"
#include "MEM.h"

#ifdef MODULE_MEM_UNITTEST_ENABLE
#include "UnitTest.h"
#endif


#if CONFIG_MEM_CHECK_POINTER_RANGE == 1
#define MEM_CHECK_POINTER_RANGE(_pnt, _size)		if (!mem_CheckPointer(_pnt, _size)) Error_Handler()
#else
#define MEM_CHECK_POINTER_RANGE(_pnt, _size)		(void)_pnt; (void)_size
#endif


/**
 * \brief	Memory copy
 * \param[out]	destination	where to copy
 * \param[in]	source		from copy
 * \param[in]	num			How many length to copy (in bytes)?
 */
void * memcpy(void * destination, const void * source, size_t size)
{
	size_t i;
	uint8_t *dest = destination;
	const uint8_t *src = source;

#if CONFIG_MEM_CHECK_POINTERS == 1
	if (destination == NULL || source == NULL)
	{
		return NULL;
	}
#endif

	for (i = 0; i < size; i++)
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
void * memset(void * ptr, int value, size_t size)
{
	size_t i;
	uint8_t *dest = ptr;

#if CONFIG_MEM_CHECK_POINTERS == 1
	if (ptr == NULL)
	{
		return NULL;
	}
	MEM_CHECK_POINTER_RANGE(ptr, size);
#endif

	for (i = 0; i < size; i++)
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
void * memmove(void * destination, const void * source, size_t size)
{
	size_t i;
	uint8_t *dest = destination;
	uint8_t *src = (uint8_t *)source;

#if CONFIG_MEM_CHECK_POINTERS == 1
	if (destination == NULL || source == NULL)
	{
		return NULL;
	}
	MEM_CHECK_POINTER_RANGE((void *)destination, size);
	MEM_CHECK_POINTER_RANGE((void *)source, size);
#endif

	for (i = 0; i < size; i++)
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
	return memset(ptr, 0, num);
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
int memcmp(const void * ptr1, const void * ptr2, size_t size)
{
	size_t i;
	const uint8_t *buffer1 = ptr1;
	const uint8_t *buffer2 = ptr2;

#if CONFIG_MEM_CHECK_POINTERS == 1
	if (ptr1 == NULL || ptr2 == NULL)
	{
		return -1;
	}
	MEM_CHECK_POINTER_RANGE((void *)ptr1, size);
	MEM_CHECK_POINTER_RANGE((void *)ptr2, size);
#endif

	for (i = 0; i < size; i++)
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
	uint8_t stackOverFlowCheckerVariable[CONFIG_MEM_STACK_GUARD_LENGTH];
	memset(stackOverFlowCheckerVariable,
			CONFIG_MEM_STACK_GUARD_VALUE,
			CONFIG_MEM_STACK_GUARD_LENGTH);
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
	// (cppcheck style) Variable 'stackOverFlowCheckerVariable' is not assigned a value.
	uint8_t stackOverFlowCheckerVariable[CONFIG_MEM_STACK_GUARD_LENGTH];

	for (i = 0; i < CONFIG_MEM_STACK_GUARD_LENGTH; i++)
	{
		if (!guardWasFound && stackOverFlowCheckerVariable[i] == CONFIG_MEM_STACK_GUARD_VALUE)
		{
			guardWasFound = true;
			guardGoodCnt = i;
			uprintf("Guard was found at %d. byte\r\n", i);
		}
		else if (guardWasFound && stackOverFlowCheckerVariable[i] != CONFIG_MEM_STACK_GUARD_VALUE)
		{
			uprintf("Guard was wrong at %d. byte\r\n", i);
			guardWasFound = false;
		}
	}

	uprintf("MEM used: %d / %d, it is %d%%\r\n",
			guardGoodCnt,
			CONFIG_MEM_STACK_GUARD_LENGTH,
			guardGoodCnt * 100/ CONFIG_MEM_STACK_GUARD_LENGTH);
}



/**
 * \brief	Check pointer is in good memory range?
 * \retval	true	is ok
 * \retval	false	wrong
 */
bool mem_CheckPointer(void * pnt, size_t size)
{
#define MEM_FLASH_START		(0x80000000 + 64*1024)
#define MEM_FLASH_END		(0x08000000)
#define MEM_RAM_START		(0x20000000)
#define MEM_RAM_END			(0x20002000)

	if (((uint32_t)pnt >= MEM_RAM_START && (uint32_t)pnt + size < MEM_RAM_END)
		|| ((uint32_t)pnt >= MEM_FLASH_START && (uint32_t)pnt + size < MEM_FLASH_END))
	{
		return true;
	}

	return false;
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
