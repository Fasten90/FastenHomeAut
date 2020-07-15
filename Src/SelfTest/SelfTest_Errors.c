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

static uint32_t SelfTest_Erros_StackOverFlow_RecursiveInfiniteFunction(uint32_t param);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

/**
 * Generate Fault to test FaultHandler()
 * @note        !! Be careful !! It is an error, the SW will crash (e.g. CortexM go to FaultHandler) !!
 */
void SelfTest_Errors_Constwrite(void)
{
    /* Const write */
    /* cppcheck-suppress stringLiteralWrite */
    static const char buffer[] = "const";
    char * pntr = (char *)buffer;

    uprintf("Buffer: %s\r\n", buffer);

    /* Rewrite the index 2. value of const buffer - which normally stored in FLASH */
    pntr[2] = 'e';

    uprintf("Buffer: %s\r\n", buffer);
}



/**
 * Test zero dividing
 * @note        !! Be careful !! It is an error, the SW will crash (e.g. CortexM go to FaultHandler) !!
 * @note        Not every target support the ZeroDivide "HardFault" flag
 */
void SelfTest_Errors_ZeroDivide(void)
{
    uint32_t a = 5;
    uint32_t b = 0;
    uint32_t c;

    /* Zero division */
    /* NOTE: Be careful! Static analyzers will say, this is possible ZeroDivide - but we want to do that */
    /* cppcheck-suppress zerodiv */
    c = a/b;  /* clang_sa_ignore[core.DivideZero] */

    uprintf("ZeroDivide result: %d\r\n", c);
}



/**
 * Test MemFault with invalid pointer
 */
void SelfTest_Errors_MemFault(void)
{
    /* Test invalid pointer */
    /* TODO: This address normally is an invalid address, but not every target-platform, but shall provide a target dependent invalid address */
    /* TODO: Check constValue with MEM functions */
    const Address_t constValueMemAddress = (const Address_t)0x12345678;
    Address_t * wrongPointer = (Address_t *)constValueMemAddress; /* Set pointer address to an incorrect address */
    *wrongPointer = 0; /* Note: Go to exception / MemFault */

    uprintf("WrongPointer value: %d\r\n", (uint32_t)*wrongPointer); /* Suppress 64 bit --> 32 bit type conversion */
}



/**
 * Test StackOverFlow
 */
static uint32_t SelfTest_Erros_StackOverFlow_RecursiveInfiniteFunction(uint32_t param)
{
    /* Most simple solution if we will re-call (recursive) this function */
    uint32_t dummyVar0 = 0x00 + param;
    uint32_t dummyVar1 = 0x01 + param;
    uint32_t dummyVar2 = 0x02 + param;
    uint32_t dummyVar3 = 0x03 + param;

    UNUSED_ARGUMENT(dummyVar0);
    UNUSED_ARGUMENT(dummyVar1);
    UNUSED_ARGUMENT(dummyVar2);
    UNUSED_ARGUMENT(dummyVar3);

    /* @note: Be careful, recursive, never return... */
    return SelfTest_Erros_StackOverFlow_RecursiveInfiniteFunction(param + 1);
}



/**
 * Test StackOverFlow
 */
void SelfTest_Errors_StackOverFlow(void)
{

    /* @note: Be careful, recursive, never return... */
    SelfTest_Erros_StackOverFlow_RecursiveInfiniteFunction(0);
}



/**
 * Test invalid function / invalid instruction
 */
void SelfTest_Errors_InvalidFunction(void)
{
    /*
     * Idea:
     * function pointer = null;
     * pointer();
     */

    void (*Function)(void);

    /* Note: Static code analyzers will report this! */
    /* STM32F0 --> it goes to the HardFault */
    Function = NULL;

    Function();
}


#endif /* CONFIG_MODULE_SELFTEST_ERRORS_ENABLE */
