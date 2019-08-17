/*
 *    x86_it.c
 *    Created on:   2019-05-05
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       x86
 */


/* Includes ------------------------------------------------------------------*/
#include "options.h"

#ifdef CONFIG_PLATFORM_X86

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


#endif /* CONFIG_PLATFORM_X86 */
