/*
 *		ConvertTable.c
 *		Created on:		2017-09-17
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017-09-17
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "include.h"

#ifdef CONFIG_MODULE_CONVERTTABLE_ENABLE

#include "ConvertTable.h"
#include "UnitTest.h"


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
 * \brief	Check convert table (monotonity)
 */
bool ConvertTable_Init(const ConverTable_t * table)
{
	// Check monotonity
	uint16_t i;
	for (i = 1; i < table->size; i++)
	{
		// TODO: With ASSERT() ?
		if (table->recordList[i].adcValue <= table->recordList[i-1].adcValue)
			return false;
		if (table->recordList[i].convertedValue <= table->recordList[i-1].convertedValue)
			return false;
	}

	return true;
}



/**
 * \brief	Get convert value (uint32_t --> float)
 */
float ConvertTable_GetValue(const ConverTable_t * table, uint32_t value)
{
	uint16_t i;
	float retval = 0.0;

	for (i = 0; i < table->size; i++)
	{
		if (value == table->recordList[i].adcValue)
		{
			// Equal with an fix value
			retval = table->recordList[i].convertedValue;
			break;
		}
		else if (value < table->recordList[i].adcValue)
		{
			// Find first larger value
			if (i > 0)
			{
				// TODO: Check i-- is good?
				retval = (float)table->recordList[i-1].convertedValue
					+ ((value - table->recordList[i-1].adcValue)
						* (table->recordList[i].convertedValue - table->recordList[i-1].convertedValue)
						/ (table->recordList[i].adcValue - table->recordList[i-1].adcValue));
			}
			else
			{
				// i == 0 --> first value
				retval = table->recordList[i].convertedValue;
			}
			break;
		}
		else if (i == (table->size - 1))
		{
			// Last value
			retval = table->recordList[i].convertedValue;
			break;
		}
	}

	return retval;
}



#ifdef MODULE_CONVERTTABLE_UNITTEST_ENABLE
/**
 * \brief	Convert table unit test
 */
void ConvertTable_UnitTest(void)
{
	float floatValue;
	float expectedValue;
	const float maxDiff = 0.01f;
	uint32_t testValue;


	UnitTest_Start("ConvertTable", __FILE__);

	const ConvertTableRecord_t testTable[] =
	{
		{
		.adcValue = 10,
		.convertedValue = 0.0
		},
		{
		.adcValue = 20,
		.convertedValue = 1.0
		},
		{
		.adcValue = 30,
		.convertedValue = 2.0
		},
	};


	const ConverTable_t testConvertTableInfo =
	{
		.recordList = testTable,
		.size = NUM_OF(testTable)
	};


	// Test Init()
	UNITTEST_ASSERT(ConvertTable_Init(&testConvertTableInfo), "Init error");


	// Fix cases
	testValue = 10;
	expectedValue = 0.0f;
	floatValue = ConvertTable_GetValue(&testConvertTableInfo, testValue);
	UNITTEST_ASSERT((floatValue < expectedValue + maxDiff) && (floatValue > expectedValue - maxDiff), "GetValue error");

	testValue = 20;
	expectedValue = 1.0f;
	floatValue = ConvertTable_GetValue(&testConvertTableInfo, testValue);
	UNITTEST_ASSERT((floatValue < expectedValue + maxDiff) && (floatValue > expectedValue - maxDiff), "GetValue error");

	testValue = 30;
	expectedValue = 2.0f;
	floatValue = ConvertTable_GetValue(&testConvertTableInfo, testValue);
	UNITTEST_ASSERT((floatValue < expectedValue + maxDiff) && (floatValue > expectedValue - maxDiff), "GetValue error");


	// Inner cases
	testValue = 15;
	expectedValue = 0.5f;
	floatValue = ConvertTable_GetValue(&testConvertTableInfo, testValue);
	UNITTEST_ASSERT((floatValue < expectedValue + maxDiff) && (floatValue > expectedValue - maxDiff), "GetValue error");

	testValue = 25;
	expectedValue = 1.5f;
	floatValue = ConvertTable_GetValue(&testConvertTableInfo, testValue);
	UNITTEST_ASSERT((floatValue < expectedValue + maxDiff) && (floatValue > expectedValue - maxDiff), "GetValue error");

	testValue = 12;
	expectedValue = 0.2f;
	floatValue = ConvertTable_GetValue(&testConvertTableInfo, testValue);
	UNITTEST_ASSERT((floatValue < expectedValue + maxDiff) && (floatValue > expectedValue - maxDiff), "GetValue error");

	testValue = 28;
	expectedValue = 1.8f;
	floatValue = ConvertTable_GetValue(&testConvertTableInfo, testValue);
	UNITTEST_ASSERT((floatValue < expectedValue + maxDiff) && (floatValue > expectedValue - maxDiff), "GetValue error");


	// Too low, too high cases
	testValue = 5;
	expectedValue = 0.0f;
	floatValue = ConvertTable_GetValue(&testConvertTableInfo, testValue);
	UNITTEST_ASSERT((floatValue < expectedValue + maxDiff) && (floatValue > expectedValue - maxDiff), "GetValue error");

	testValue = 35;
	expectedValue = 2.0f;
	floatValue = ConvertTable_GetValue(&testConvertTableInfo, testValue);
	UNITTEST_ASSERT((floatValue < expectedValue + maxDiff) && (floatValue > expectedValue - maxDiff), "GetValue error");


	// Finish
	UnitTest_End();
}
#endif	// #ifdef MODULE_CONVERTTABLE_UNITTEST_ENABLE

#endif	// #ifdef CONFIG_MODULE_CONVERTTABLE_ENABLE
