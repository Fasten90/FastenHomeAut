/*
 *      x86_hal_msp.c
 *      Created on:     2019-05-05
 *      Author:         Vizi Gábor
 *      E-mail:         vizi.gabor90@gmail.com
 *      Function:       -
 *      Target:         x86
 *      Version:        -
 *      Last modified:  2019-05-05
 *      Note:           Copied from stm32fx_hal_msp.c
 */

/* Includes ------------------------------------------------------------------*/
#include "options.h"

#ifdef CONFIG_PLATFORM_X86

#include "x86_hal.h"


/* Functions ------------------------------------------------------------------*/

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
    /* Empty */
    /* TODO: Add suppression for possible warnings/static code analysis report */
}

#endif /* CONFIG_PLATFORM_X86 */
