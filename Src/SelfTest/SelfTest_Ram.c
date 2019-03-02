/*
 *    SelfTest_Ram.c
 *    Created on:   2018-01-27
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "board.h"
#include "SelfTest_Ram.h"



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


#ifdef CONFIG_MODULE_SELFTEST_ENABLE
/**
 * \brief  SelfTest RAM test
 */
bool SelfTest_Ram_Test(void)
{
    uint8_t * i;
    uint8_t * stackActualAddress;
    bool result = true;
    volatile uint32_t stackLastVariable = 0U;
    stackActualAddress = (uint8_t *)&stackLastVariable;

    // End of BSS address
    extern uint32_t _ebss;

    // It suppose decrementing stack
    for (i = (uint8_t *)&_ebss; i < stackActualAddress; i++)
    {
        // 0xAA
        *i = 0xAAU;
        if (*i != 0xAAU)
        {
            result = false;
            break;
        }
    }

    if (result == true)
    {

        for (i = (uint8_t *)&_ebss; i < stackActualAddress; i++)
        {
            // 0x55
            *i = 0x55U;
            if (*i != 0x55U)
            {
                result = false;
                break;
            }
        }
    }

    return result;
}

#endif    /* CONFIG_MODULE_SELFTEST_ENABLE */

