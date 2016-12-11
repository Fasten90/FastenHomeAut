/*
 *		unittest.c
 *
 *		Created on:		2016. nov. 24.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2016. nov. 24.
 */


#include "string.h"
#include "usart.h"
#include "unittest.h"


/// VARIABLES

static uint16_t UnitTest_ValidCnt = 0;
static uint16_t UnitTest_InvalidCnt = 0;


/// FUNCTIONS



/**
 * \brief	Start unit test
 */
void UnitTest_Start(const char *moduleName, const char *fileName)
{
	uprintf("\r\n"
			"Start %s module unit test\r\n"
			"File: \"%s\"\r\n"
			"\r\n",
			moduleName, fileName);
	UnitTest_ValidCnt = 0;
	UnitTest_InvalidCnt = 0;
}



/**
 * \brief	Check unit test result
 */
void UnitTest_CheckResult(bool isValid, const char *errorString, const char *fileName, uint32_t line)
{
	if (isValid)
	{
		// Valid
		UnitTest_ValidCnt++;
	}
	else
	{
		// Invalid
		UnitTest_InvalidCnt++;

		uprintf("\r\n"
				"Error in \"%s\" at %d. line.\r\n"
				"Case: \"%s\"\r\n"
				"\r\n",
				fileName, line, errorString);
		// TODO: Use SendErrorMessage()
	}
}



/**
 * \brief Finish unit test
 */
void UnitTest_End(const char *fileName)
{
	uprintf("\r\n"
			"In \"%s\" file run unit test:\r\n"
			"Successful: %d\r\n"
			"Error: %d\r\n"
			"\r\n",
			fileName, UnitTest_ValidCnt, UnitTest_InvalidCnt);

	if (UnitTest_InvalidCnt)
	{
		uprintf("UnitTest run failed\r\n");
	}
	else
	{
		uprintf("UnitTest run successfully\r\n");
	}
}
