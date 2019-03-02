/*
 *        Watchdog.c
 *        Created on:        2017-02-06
 *        Author:            Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        Watchdog
 *        Target:            STM32Fx
 *        Version:        v1
 *        Last modified:    2017-02-06
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"
#include "Watchdog.h"



#ifdef CONFIG_MODULE_WATCHDOG_ENABLE


IWDG_HandleTypeDef IWDG_Handle;



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief    Initialize Watchdog
 */
void Watchdog_Init(void)
{
    // Configure IWDG
    IWDG_Handle.Init.Prescaler = IWDG_PRESCALER_32;
    IWDG_Handle.Init.Reload = IWDG_RLR_RL;    // Max value
    IWDG_Handle.Init.Window = IWDG_WINDOW_DISABLE;
    IWDG_Handle.Instance = IWDG;

    if (HAL_IWDG_Init(&IWDG_Handle) != HAL_OK)
    {
        Error_Handler();
    }

#ifdef CONFIG_DEBUG_MODE
    __HAL_DBGMCU_FREEZE_IWDG();
#endif
}



/**
 * \brief    Clear Watchdog
 */
void Watchdog_Clear(void)
{
    HAL_IWDG_Refresh(&IWDG_Handle);
}



#endif    // #ifdef CONFIG_MODULE_WATCHDOG_ENABLE
