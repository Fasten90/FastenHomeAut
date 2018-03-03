/*
 *		SelfTest_Flag.c
 *		Created on:		2018-01-24
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2018-01-24
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "compiler.h"
#include "SelfTest_Flag.h"
//#include "DebugUart.h"



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

#ifdef CONFIG_MODULE_SELFTEST_ENABLE
/**
 * \brief  SelfTest_Flag Test
 */
bool SelfTest_Flag_Test(void)
{
	APSR_Type apsr;
	uint8_t successfulTest = 0;
	const uint8_t testCount = 5;

	/*
	    core_m0.h
	        APSR_Type
	        cmsis_armcc.h
	            __STATIC_INLINE uint32_t __get_APSR(void)
	*/

	/*		Flag Test		*/
	//uint32_t reg = __get_APSR();

	/*
	volatile uint8_t a = 5;
	volatile uint8_t b = 4;
	b++;
	volatile uint8_t c = a-b;
	*/


	// Wrong:
	/*
	register volatile uint8_t a = 1;
	a--;

	apsr.w = __get_APSR();

	if (apsr.b.Z == 1)
	{
		uprintf("Zero flag is 1\r\n");
	}

	uprintf("a value: %u\r\n", a);
	*/


	/* Signed adding overflow (add two positive signed number, which will overflow (result is a negative value)) */

	volatile int32_t add1 = 0x7FFFFFF0U;
	volatile int32_t add2 = 0x000000FFU;
	// cppcheck-suppress integerOverflow
	volatile uint32_t addres = add1 + add2;

	apsr.w = __get_APSR();

	if (apsr.b.V == 1)
	{
		//uprintf("Overflow flag is 1\r\n");
		successfulTest++;
	}

	//uprintf("Addition value: %u\r\n", addres);
	(void)addres;


	/* Unsigned adding overflow */

	volatile uint32_t overflowval = 0xFFFFFFFFU;
	overflowval++;

	apsr.w = __get_APSR();

	if (apsr.b.C == 1 && apsr.b.Z == 1)
	{
		//uprintf("Carry and Zero flag is 1\r\n");
		successfulTest++;
	}

	if (overflowval == 0)
	{
		successfulTest++;
	}

	//uprintf("overflowval value: %u\r\n", overflowval);


	/* Too large multiplex */

	volatile uint32_t mul1 = 0xF0000000U;
	volatile uint32_t mul2 = 0xF0000000U;
	volatile uint32_t mulres = mul1 * mul2;
	/* not overflowed, but why? result is 0 */

	apsr.w = __get_APSR();

	if (apsr.b.Z == 1)
	{
		//uprintf("Zero flag is 1\r\n");
		successfulTest++;
	}

	//uprintf("Multiplex value: %u\r\n", mulres);
	(void)mulres;


	/* Underflow subtraction */

	volatile uint32_t sub1 = 2;
	volatile uint32_t sub2 = 4;
	volatile uint32_t subres = sub1 - sub2;

	apsr.w = __get_APSR();

	if (apsr.b.N == 1)
	{
		//uprintf("Negative flag is 1\r\n");
		successfulTest++;
	}

	//uprintf("Subtraction value: %u\r\n", subres);
	(void)subres;


	// Return with result (true = successful)
	return (successfulTest == testCount);
}
#endif


