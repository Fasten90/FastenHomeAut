/*
 *    SelfTest_Errors.h
 *    Created on:   2019-03-03
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef SELFTEST_ERRORS_H_
#define SELFTEST_ERRORS_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void SelfTest_Errors_Constwrite(void);
void SelfTest_Errors_ZeroDivide(void);
void SelfTest_Errors_MemFault(void);
void SelfTest_Erros_StackOverFlow(void);
void SelfTest_Errors_InvalidFunction(void);



#endif /* SELFTEST_ERRORS_H_ */
