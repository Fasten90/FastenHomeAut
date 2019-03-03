/*
 *    SelfTest_Errors.h
 *    Created on:   2019-03-03
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "compiler.h"
#include "DebugUart.h"
#include "SelfTest_Errors.h"


#ifdef CONFIG_MODULE_SELFTEST_ERRORS_ENABLE

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
 * Generate Fault to test FaultHandler()
 * @note    !! Be careful !! It is an error, the SW will crash (go to FaultHandler) !!
 */
void SelfTest_Errors_Constwrite(void)
{
    // Const write
    // cppcheck-suppress stringLiteralWrite
    static const char const buffer[] = "const";
    char * pnt = (char *)buffer;

    uprintf("Buffer: %s\r\n", buffer);

    pnt[2] = 'e';

    uprintf("Buffer: %s\r\n", buffer);
}



/**
 * Test zero dividing
 * @note !! Be careful !! It is an error, the sw will crash (go to FaultHandler) !!
 */
void SelfTest_Errors_ZeroDivide(void)
{
    uint32_t a = 5;
    uint32_t b = 0;
    uint32_t c;

    // Zero division
    // cppcheck-suppress zerodiv
    c = a/b;

    uprintf("ZeroDivide result: %d\r\n", c);
}



/**
 * Test MemFault with invalid pointer
 */
void SelfTest_Errors_MemFault(void)
{
    // Test invalid pointer
    const uint32_t constValue = 0x12345678;
    uint32_t * wrongPointer = (uint32_t *)constValue;
    *wrongPointer = 0;

    uprintf("WrongPointer value: %d\r\n", *wrongPointer);
}



#endif /* CONFIG_MODULE_SELFTEST_ERRORS_ENABLE */
