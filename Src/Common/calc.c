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



#ifdef MODULE_CALC_UNITTEST_ENABLE
/**
 * \brief	Calc UnitTest
 */
void Calc_UnitTest(void)
{
	UnitTest_Start("Calc", __FILE__);


	/*		 Test power()		*/
	UNITTEST_ASSERT(power(0,0)==1, "power error");
	UNITTEST_ASSERT(power(1,0)==1, "power error");
	UNITTEST_ASSERT(power(1,1)==1, "power error");
	UNITTEST_ASSERT(power(1,2)==1, "power error");
	UNITTEST_ASSERT(power(2,1)==2, "power error");

	UNITTEST_ASSERT(power(2,2)==4, "power error");
	UNITTEST_ASSERT(power(2,10)==1024, "power error");
	UNITTEST_ASSERT(power(2,16)==65536, "power error");
	UNITTEST_ASSERT(power(1024,0)==1, "power error");
	// Overflow
	UNITTEST_ASSERT(power(2,32)==0, "power error");



	/*		power_secured()		*/
	UNITTEST_ASSERT(power_secured(0,0)==1, "power error");
	UNITTEST_ASSERT(power_secured(1,0)==1, "power error");
	UNITTEST_ASSERT(power_secured(1,1)==1, "power error");
	UNITTEST_ASSERT(power_secured(1,2)==1, "power error");
	UNITTEST_ASSERT(power_secured(2,1)==2, "power error");

	UNITTEST_ASSERT(power_secured(2,2)==4, "power error");
	UNITTEST_ASSERT(power_secured(2,10)==1024, "power error");
	UNITTEST_ASSERT(power_secured(2,16)==65536, "power error");
	UNITTEST_ASSERT(power_secured(1024,0)==1, "power error");
	// Overflow - but last
	UNITTEST_ASSERT(power_secured(2,32)==2147483648, "power error");



	UnitTest_End();
}
#endif
