/*
 *		Calc.c
 *
 *		Created on:		2016.09.26
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017.06.02.
 */

#include "options.h"
#include "include.h"
#include "Calc.h"

#ifdef MODULE_CALC_UNITTEST_ENABLE
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
		num = 1;
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
				// Save value
				prevNum = num;
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



inline uint32_t Increment(uint32_t * i)
{
	if (*i < UINT32_MAX)
		(*i)++;

	return (*i);
}



inline uint32_t Decrement(uint32_t * i)
{
	if (*i > 0)
		(*i)--;

	return (*i);
}



#ifdef MODULE_CALC_UNITTEST_ENABLE
/**
 * \brief	Calc UnitTest
 */
void Calc_UnitTest(void)
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



	// Finish
	UnitTest_End();

}
#endif
