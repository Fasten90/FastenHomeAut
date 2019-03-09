/*
 *    SecuredDataTypes.c
 *    Created on:   2017-08-24
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Secure data types and handlers
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "compiler.h"
#include "ErrorHandler.h"
#include "SecuredDataTypes.h"


#ifdef CONFIG_MODULE_SECUREDDATATYPES_UNITTEST_ENABLE
#include "UnitTest.h"

static uint8_t Secured_ErrorCnt = 0;
#define SECURED_ERROR()            Secured_ErrorCnt++

#else

#define SECURED_ERROR()            Error_Handler()

#endif



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
 * @brief    Get secured bool value
 */
bool Get_SecuredBool(SecuredBool_t *data)
{
    /* Check pointer */
    if (data == NULL)
        return false;

    /* Check validity */
    if (data->isEn == !data->negIsEn)
    {
        /* Valid */
        return data->isEn;
    }
    else
    {
        /* Invalid */
        SECURED_ERROR();
        return false;
    }
}



/**
 * @brief    Set secured bool value
 */
void Set_SecuredBool(SecuredBool_t *data, bool value)
{
    /* Check pointer */
    if (data == NULL)
        return;

    /* Set value */
    data->isEn = value;
    data->negIsEn = !value;
}



/**
 * @brief    Get secured uint32_t value
 */
uint32_t Get_SecuredUint32(SecuredUint32_t *data)
{
    /* Check pointer */
    if (data == NULL)
        return 0;

    /* Check validity */
    if (data->data == ~data->negData)
    {
        /* Valid */
        return data->data;
    }
    else
    {
        /* Invalid */
        SECURED_ERROR();
        return 0;
    }
}



/**
 * @brief    Set secured uint32_t value
 */
void Set_SecuredUint32(SecuredUint32_t *data, uint32_t value)
{
    /* Check pointer */
    if (data == NULL)
        return;

    /* Set value */
    data->data = value;
    data->negData = ~value;
}



#ifdef CONFIG_MODULE_SECUREDDATATYPES_UNITTEST_ENABLE
/**
 * @brief    Secured data types Unit test
 */
uint32_t SecuredDataTypes_UnitTest(void)
{
    SecuredBool_t secBool;
    SecuredUint32_t secInt;

    UnitTest_Start("SecuredDataTypes", __FILE__);

    /* Clear error cnt */
    Secured_ErrorCnt = 0;


    /* Test "SecuredBool" */

    Set_SecuredBool(&secBool, false);
    UNITTEST_ASSERT(Get_SecuredBool(&secBool) == false, "Set bool error!");

    Set_SecuredBool(&secBool, true);
    UNITTEST_ASSERT(Get_SecuredBool(&secBool) == true, "Set bool error!");

    UNITTEST_ASSERT(Secured_ErrorCnt == 0, "ErrorCnt is incremented!");

    /* Make wrong */
    secBool.negIsEn = 123;

    /* Wait wrong value: false */
    UNITTEST_ASSERT(Get_SecuredBool(&secBool) == false, "Get bool error!");
    UNITTEST_ASSERT(Secured_ErrorCnt == 1, "ErrorCnt error!");


    /* Clear error cnt */
    Secured_ErrorCnt = 0;


    /* Test "SecuredUint32" */

    Set_SecuredUint32(&secInt, 0);
    UNITTEST_ASSERT(Get_SecuredUint32(&secInt) == 0, "Set uint error!");

    Set_SecuredUint32(&secInt, 1234);
    UNITTEST_ASSERT(Get_SecuredUint32(&secInt) == 1234, "Set uint error!");

    UNITTEST_ASSERT(Secured_ErrorCnt == 0, "ErrorCnt is incremented!");

    /* Make wrong */
    secInt.negData = 0xFA12;

    /* Wait wrong value: 0 */
    UNITTEST_ASSERT(Get_SecuredUint32(&secInt) == 0, "Get uint error!");
    UNITTEST_ASSERT(Secured_ErrorCnt == 1, "ErrorCnt error!");


    return UnitTest_End();
}
#endif    /* #ifdef CONFIG_MODULE_SECUREDDATATYPES_UNITTEST_ENABLE */


