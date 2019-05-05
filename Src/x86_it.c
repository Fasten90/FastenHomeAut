
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
