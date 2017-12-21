/*
 *		UnitTest.h
 *		Created on:		2016-11-24
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		UnitTest module
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2016-11-24
 */

#ifndef UNITTEST_H_
#define UNITTEST_H_


#include "GenericTypeDefs.h"


/*------------------------------------------------------------------------------
 *  Macros
 *----------------------------------------------------------------------------*/

// if con == true, is valid
#define UNITTEST_ASSERT(con, errorstring)			UnitTest_CheckResult(con, #con, errorstring, __LINE__)



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void UnitTest_Start(const char *moduleName, const char *fileName);
void UnitTest_CheckResult(bool isValid, const char *conString, const char *errorString, uint32_t line);
void UnitTest_End(void);



#endif /* UNITTEST_H_ */