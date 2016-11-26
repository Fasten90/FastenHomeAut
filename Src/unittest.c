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


static uint16_t UnitTest_ValidCnt = 0;
static uint16_t UnitTest_InvalidCnt = 0;


void UnitTest_Start(const char *message, const char *fileName)
{
	uprintf("%s\r\n"
			"File: \"%s\"\r\n"
			"\r\n",
			message, fileName);
	UnitTest_ValidCnt = 0;
	UnitTest_InvalidCnt = 0;
}



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
