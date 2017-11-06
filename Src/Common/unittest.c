/*
 *		UnitTest.c
 *		Created on:		2016-11-24
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		UnitTest module
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2016-11-24
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "String.h"
#include "DebugUart.h"
#include "FormattedMessage.h"
#include "Terminal.h"
#include "UnitTest.h"


#ifdef CONFIG_MODULE_UNITTEST_ENABLE



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

static uint16_t UnitTest_ValidCnt = 0;
static uint16_t UnitTest_InvalidCnt = 0;

static const char *UnitTest_FileName = NULL;



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


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
	UnitTest_FileName = fileName;
}



/**
 * \brief	Check unit test result (~assert) (+ condition string)
 */
void UnitTest_CheckResult(bool isValid, const char *conString, const char *errorString, uint32_t line)
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
				"Condition: \"%s\"\r\n"
				"Case: \"%s\"\r\n"
				"\r\n",
				UnitTest_FileName, line,
				conString,
				errorString);
		// TODO: Use SendErrorMessage()
		DEBUG_BREAKPOINT();
	}
}



/**
 * \brief Finish unit test
 */
void UnitTest_End(void)
{
	uprintf("\r\n"
			"In \"%s\" file run unit test:\r\n"
			"Successful: %d\r\n"
			"Error: %d\r\n"
			"\r\n",
			UnitTest_FileName,
			UnitTest_ValidCnt,
			UnitTest_InvalidCnt);

	if (UnitTest_InvalidCnt)
	{
#ifdef CONFIG_MODULE_FORMATTEDMESSAGE_ENABLE
		SendErrorMessage("UnitTest run failed\r\n");
		DebugUart_SendChar(TERMINAL_KEY_BELL);
#else
		uprintf("UnitTest run failed\r\n");
#endif
	}
	else
	{
#ifdef CONFIG_MODULE_FORMATTEDMESSAGE_ENABLE
		SendColouredMessage("UnitTest run successfully\r\n", Color_Green);
#else
		uprintf("UnitTest run successfully\r\n");
#endif
	}
}

#endif	// #ifdef CONFIG_MODULE_UNITTEST_ENABLE
