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
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void UnitTest_Start(const char *moduleName, const char *fileName);
void UnitTest_CheckResult(bool isValid, const char *errorString, const char *fileName, uint32_t line);
void UnitTest_End(const char *fileName);



#endif /* UNITTEST_H_ */
