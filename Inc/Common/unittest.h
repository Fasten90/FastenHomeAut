/*
 *		unittest.h
 *
 *		Created on:		2016. nov. 24.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2016. nov. 24.
 */

#ifndef UNITTEST_H_
#define UNITTEST_H_



/*------------------------------------------------------------------------------
 *  Macros
 *----------------------------------------------------------------------------*/

#define UNITTEST_ASSERT(con, errorstring)			UnitTest_CheckResult(con, errorstring, __LINE__)

#define UNITTEST_ASSERT2(con, errorstring)			UnitTest_CheckResult2(con, #con, errorstring, __LINE__)



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void UnitTest_Start(const char *moduleName, const char *fileName);
void UnitTest_CheckResult(bool isValid, const char *errorString, uint32_t line);
void UnitTest_CheckResult2(bool isValid, const char *conString, const char *errorString, uint32_t line);
void UnitTest_End(void);



#endif /* UNITTEST_H_ */
