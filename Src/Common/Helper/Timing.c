/*
 *        Timing.c
 *        Created on:        2018-01-21
 *        Author:            Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        -
 *        Target:            STM32Fx
 *        Version:        -
 *        Last modified:    2018-01-21
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "compiler.h"
#include "Timing.h"



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
 * \brief        Delay (ms)
 * \param[in]    ms    millisecond, which time delay
 */
inline void DelayMs(uint32_t ms)
{
#ifdef CONFIG_USE_FREERTOS
    vTaskDelay((TickType_t)(ms/portTICK_PERIOD_MS));
#else
    HAL_Delay(ms);
#endif
}



// TODO: void DelayUs(uint32_t us);


