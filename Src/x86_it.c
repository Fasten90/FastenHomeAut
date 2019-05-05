/*
 *      x86_it.c
 *      Created on:     2019-05-05
 *      Author:         Vizi Gábor
 *      E-mail:         vizi.gabor90@gmail.com
 *      Function:       -
 *      Target:         x86
 *      Version:        -
 *      Last modified:  2019-05-05
 *      Note:           Copied from stm32fx_it.c
 */


/* Includes ------------------------------------------------------------------*/
#include "options.h"

#ifdef CONFIG_PLATFORM_PC_WINDOWS

#include "x86_hal.h"

#include "ErrorHandler.h"
#include "SwWatchDog.h"

/* External variables --------------------------------------------------------*/


/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();

    SW_WATCHDOG_CHECK();
}


#endif /* CONFIG_PLATFORM_PC_WINDOWS */
