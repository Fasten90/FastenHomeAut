/*
 *		Calc.c
 *		Created on:		2016-09-26
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Calculate / Math functions
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-06-02
 */



#include "options.h"
#include "compiler.h"
#include "MathHelper.h"

#ifdef MODULE_MATHHELPER_UNITTEST_ENABLE
#include "UnitTest.h"
#endif


/**
 * \brief	Power(a,b) = a^b
 */
uint32_t power(uint32_t a, uint8_t b)
{
	uint8_t i;
	uint32_t num;

	if (b == 0)
		num = 1;
	else
	{
		num = a;
		for (i = 1; i < b; i++)
		{
			num = num * a;
		}
	}

	return num;
}



/**
 * \brief	Power - secured (check overflow)
 */
uint32_t power_secured(uint32_t a, uint8_t b)
{
	uint8_t i;
	uint32_t num;
	uint32_t prevNum;

	if (b == 0)
	{
		num = 1;
	}
	else
	{
		num = a;
		prevNum = num;
		for (i = 1; i < b; i++)
		{
			num = num * a;
			if (num < prevNum)
			{
				// Overflow! Return with "largest" value
				num = prevNum;

				break;
			}
			else
			{
				prevNum = num;		// Save value
			}
		}
	}

	return num;
}



/**
 * \brief	Calculate how many digit need
 * \param	num		which number
 * \param	radix	radix
 */
uint8_t DigitNum(uint32_t num, uint8_t radix)
{
	uint8_t digit = 1;

	// Check radix is valid?
	if (radix == 0 || radix == 1)
		return 0;

	while (num >= radix)
	{
		num /= radix;
		digit++;
	}

	return digit;
}



/**
 * \brief	Increment a unsigned integer (32bit)
 */
inline uint32_t Increment(uint32_t * i)
{
	if (*i < UINT32_MAX)
	{
		(*i)++;
	}

	return (*i);
}



/**
 * \brief	Decrement a unsigned integer (32bit)
 */
inline uint32_t Decrement(uint32_t * i)
{
	if (*i > 0)
	{
		(*i)--;
	}

	return (*i);
}



/**
 * \brief	Calculate largest 1 bit index (in 32 bit number)
 * \retval	>= 0 - bit index
 * \retval	-1		if there is no set bit (==0
 */
int8_t GetLargestBitIndex(uint32_t value)
{
	int8_t bitIndex = 31;

	if (value == 0)
	{
		return -1;
	}

	while (!(value & ((uint32_t)0x01 << bitIndex)) && (bitIndex > 0))
	{
		bitIndex--;
	}

	return bitIndex;
}



/**
 * \brief	Calculate smallest 1 bit index (in 32 bit number)
 * \retval	>= 0 - bit index
 * \retval	-1		if there is no set bit (==0
 */
int8_t GetSmallestBitIndex(uint32_t value)
{
	int8_t bitIndex = 0;

	if (value == 0)
	{
		return -1;
	}

	while (!(value & ((uint32_t)0x01 << bitIndex)) && (bitIndex < 31))
	{
		bitIndex++;
	}

	return bitIndex;
}



/**
 * \brief	'1' bits count
 */
uint8_t popcount(uint32_t value)
{
	uint8_t onebits = 0;
	uint8_t i;

	if (value == 0)
	{
		// 0 value has zero 1 bits
		return 0;
	}

	for (i = 0; i < 32; i++)
	{
		if (value & ((uint32_t)0x01 << i))
		{
			onebits++;
		}
	}

	return onebits;
}



/**
 * \brief	Clear i. bit
 */
void ClearBit(uint32_t * value, uint8_t index)
{
	if ((value == NULL) || (index > 31))
	{
		return;
	}

	*value &= ~((uint32_t)0x01 << index);
}



/**
 * \brief	Set i. bit
 */
void SetBit(uint32_t * value, uint8_t index)
{
	if ((value == NULL) || (index > 31))
	{
		return;
	}

	*value |= ((uint32_t)0x01 << index);
}



/**
 * \brief	Get i. bit
 */
uint8_t GetBit(uint32_t value, uint8_t index)
{
	if ((value == 0) || (index > 31))
	{
		// 0 or wrong index
		return 0;
	}

	return ((value & ((uint32_t)0x01 << index)) ? (0x01) : (0x00));
}



/**
 * \brief	Random generator
 * 			Generate random number from 0 to RAND_MAX
 */
uint16_t random(void)
{
	// \note	This random is dependent from tick. If you call fastly and often, the value will not change enough
	uint16_t randomValue = HAL_GetTick() % RAND_MAX;
	return randomValue;
}



#ifdef MODULE_MATHHELPER_UNITTEST_ENABLE
/**
 * \brief	MathHelper UnitTest
 */
uint32_t MathHelper_UnitTest(void)
{
	UnitTest_Start("Calc", __FILE__);

	/*		 Test power()		*/
	UNITTEST_ASSERT(power(0, 0)==1, "power error");
	UNITTEST_ASSERT(power(1, 0)==1, "power error");
	UNITTEST_ASSERT(power(1, 1)==1, "power error");
	UNITTEST_ASSERT(power(1, 2)==1, "power error");
	UNITTEST_ASSERT(power(2, 1)==2, "power error");

	UNITTEST_ASSERT(power(2, 2)==4, "power error");
	UNITTEST_ASSERT(power(2, 10)==1024, "power error");
	UNITTEST_ASSERT(power(2, 16)==65536, "power error");
	UNITTEST_ASSERT(power(1024, 0)==1, "power error");
	// Overflow
	UNITTEST_ASSERT(power(2, 32)==0, "power error");


	/*		power_secured()		*/
	UNITTEST_ASSERT(power_secured(0, 0)==1, "power error");
	UNITTEST_ASSERT(power_secured(1, 0)==1, "power error");
	UNITTEST_ASSERT(power_secured(1, 1)==1, "power error");
	UNITTEST_ASSERT(power_secured(1, 2)==1, "power error");
	UNITTEST_ASSERT(power_secured(2, 1)==2, "power error");

	UNITTEST_ASSERT(power_secured(2, 2)==4, "power error");
	UNITTEST_ASSERT(power_secured(2, 10)==1024, "power error");
	UNITTEST_ASSERT(power_secured(2, 16)==65536, "power error");
	UNITTEST_ASSERT(power_secured(1024, 0)==1, "power error");

	// Overflow - but last
	UNITTEST_ASSERT(power_secured(2, 32)==2147483648, "power error");


	/*		DigitNum()			*/
	// Wrong radixes (0, 1)
	UNITTEST_ASSERT(DigitNum(1, 0) == 0, "DigitNum error");
	UNITTEST_ASSERT(DigitNum(1, 1) == 0, "DigitNum error");

	// Good situations
	UNITTEST_ASSERT(DigitNum(10, 10) == 2, "DigitNum error");
	UNITTEST_ASSERT(DigitNum(16, 10) == 2, "DigitNum error");
	UNITTEST_ASSERT(DigitNum(16, 16) == 2, "DigitNum error");
	UNITTEST_ASSERT(DigitNum(100, 10) == 3, "DigitNum error");
	UNITTEST_ASSERT(DigitNum(1000, 10) == 4, "DigitNum error");
	UNITTEST_ASSERT(DigitNum(999, 10) == 3, "DigitNum error");


	/*		Increment()			*/
	uint32_t num = 0;
	// Check with normal example
	UNITTEST_ASSERT(Increment(&num)==1, "Increment error");
	UNITTEST_ASSERT(num==1, "Increment error");
	// Check with overflow possibility
	num = UINT32_MAX;
	UNITTEST_ASSERT(Increment(&num)==UINT32_MAX, "Increment error");
	UNITTEST_ASSERT(num==UINT32_MAX, "Increment error");


	/*		Decrement()			*/
	num = 1;
	// Check with normal example
	UNITTEST_ASSERT(Decrement(&num)==0, "Decrement error");
	UNITTEST_ASSERT(num==0, "Decrement error");
	// Check with underflow possibility
	num = 0;
	UNITTEST_ASSERT(Decrement(&num)==0, "Decrement error");
	UNITTEST_ASSERT(num==0, "Decrement error");


	/*		GetLargestBitIndex()	*/
	UNITTEST_ASSERT(GetLargestBitIndex(0xFFFFFFFF) == 31, "GetLargestBitIndex error");
	UNITTEST_ASSERT(GetLargestBitIndex(0x80000000) == 31, "GetLargestBitIndex error");
	UNITTEST_ASSERT(GetLargestBitIndex(0x00000008) == 3,  "GetLargestBitIndex error");
	UNITTEST_ASSERT(GetLargestBitIndex(0x00000001) == 0,  "GetLargestBitIndex error");
	UNITTEST_ASSERT(GetLargestBitIndex(0x00000000) == -1, "GetLargestBitIndex error");


	/*		GetSmallestBitIndex()	*/
	UNITTEST_ASSERT(GetSmallestBitIndex(0xFFFFFFFF) == 0,  "GetSmallestBitIndex error");
	UNITTEST_ASSERT(GetSmallestBitIndex(0x80000000) == 31, "GetSmallestBitIndex error");
	UNITTEST_ASSERT(GetSmallestBitIndex(0x00000008) == 3,  "GetSmallestBitIndex error");
	UNITTEST_ASSERT(GetSmallestBitIndex(0x00000001) == 0,  "GetSmallestBitIndex error");
	UNITTEST_ASSERT(GetSmallestBitIndex(0x00000000) == -1, "GetSmallestBitIndex error");


	/*		popcount()	*/
	UNITTEST_ASSERT(popcount(0xFFFFFFFF) == 32, "popcount error");
	UNITTEST_ASSERT(popcount(0xF000000F) == 8,  "popcount error");
	UNITTEST_ASSERT(popcount(0x0000000F) == 4,  "popcount error");
	UNITTEST_ASSERT(popcount(0x00000001) == 1,  "popcount error");
	UNITTEST_ASSERT(popcount(0x00000000) == 0,  "popcount error");


	/*		ClearBit()	*/
	uint32_t value;

	value = 0x00;
	ClearBit(&value, 0);
	UNITTEST_ASSERT(value == 0x00000000, "ClearBit error");

	value = 0xFFFFFFFF;
	ClearBit(&value, 0);
	UNITTEST_ASSERT(value == 0xFFFFFFFE, "ClearBit error");

	value = 0xFFFFFFFF;
	ClearBit(&value, 31);
	UNITTEST_ASSERT(value == 0x7FFFFFFF, "ClearBit error");

	value = 0xFFFFFFFF;
	ClearBit(&value, 32);
	UNITTEST_ASSERT(value == 0xFFFFFFFF, "ClearBit error");

	// Do nothing
	ClearBit(NULL, 0);


	/*		SetBit()	*/

	value = 0x00;
	SetBit(&value, 0);
	UNITTEST_ASSERT(value == 0x00000001, "SetBit error");

	value = 0xFFFFFFFF;
	SetBit(&value, 0);
	UNITTEST_ASSERT(value == 0xFFFFFFFF, "SetBit error");

	value = 0x7FFFFFFF;
	SetBit(&value, 31);
	UNITTEST_ASSERT(value == 0xFFFFFFFF, "SetBit error");

	value = 0xFFFFFFFF;
	SetBit(&value, 32);
	UNITTEST_ASSERT(value == 0xFFFFFFFF, "SetBit error");

	// Do nothing
	SetBit(NULL, 0);


	/*		GetBit()	*/
	UNITTEST_ASSERT(GetBit(0xFFFFFFFF, 0) == 1, "GetBit error");
	UNITTEST_ASSERT(GetBit(0xFFFFFFFF, 1) == 1, "GetBit error");
	UNITTEST_ASSERT(GetBit(0xFFFFFFFF, 31) == 1, "GetBit error");
	UNITTEST_ASSERT(GetBit(0x00000000, 0) == 0, "GetBit error");
	UNITTEST_ASSERT(GetBit(0x00000000, 1) == 0, "GetBit error");
	UNITTEST_ASSERT(GetBit(0x00000000, 31) == 0, "GetBit error");
	UNITTEST_ASSERT(GetBit(0x00000000, 32) == 0, "GetBit error");


	// TODO: Test random();


	// Finish
	return UnitTest_End();
}
#endif
