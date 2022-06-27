/*
 *    Reset.c
 *    Created on:   2017-02-27
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     ResetHandler module
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"

#ifdef CONFIG_MODULE_RESETREASON_ENABLE

#include "DebugUart.h"
#include "StringHelper.h"
#include "ResetHandler.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static uint32_t Reset_GetResetReason(void);
static void Reset_GetResetReasonString(uint32_t resetReason, char *resetString, uint8_t strMaxLength);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief       Get reset reason
 */
static uint32_t Reset_GetResetReason(void)
{
    uint32_t resetReason = 0;
    uint8_t i;

    for (i = 0; (1<<i) < ResetReason_Count; i++)
    {
        /* @note Be careful, now it works in STM32F407, because: */
        /*
         *    #define RCC_FLAG_BORRST                  ((uint8_t)0x79U)
         *    #define RCC_FLAG_PINRST                  ((uint8_t)0x7AU)
         *    #define RCC_FLAG_PORRST                  ((uint8_t)0x7BU)
         *    #define RCC_FLAG_SFTRST                  ((uint8_t)0x7CU)
         *    #define RCC_FLAG_IWDGRST                 ((uint8_t)0x7DU)
         *    #define RCC_FLAG_WWDGRST                 ((uint8_t)0x7EU)
         *    #define RCC_FLAG_LPWRRST                 ((uint8_t)0x7FU)
         */
#ifdef CONFIG_PLATFORM_MCU_STM32F4xx
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST + i))
#elif defined(CONFIG_PLATFORM_MCU_STM32F0xx)
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST + i))
#elif defined(CONFIG_PLATFORM_X86)
        if (0)
#else
#error "Unknown Microcontroller family"
#endif
        {
            /* Set flags, if have */
            resetReason |= (1 << i);
        }

    }

    /* Clear reset flags */
    __HAL_RCC_CLEAR_RESET_FLAGS();

    return resetReason;
}



/**
 * @brief       Get reset reason string
 */
static void Reset_GetResetReasonString(uint32_t resetReason, char *resetString, uint8_t strMaxLength)
{
    ResetReason_t reset = resetReason & ResetReason_Mask;
    uint8_t length = 0;
    uint8_t remainLength = strMaxLength;
    uint8_t i;

    if (resetReason)
    {
        /* If has reset reason value */
        for (i = 0; (1<<i) < ResetReason_Count; i++)
        {
            switch (reset & (1 << i))
            {
                case ResetReason_BORRST:
                    length += usnprintf(&resetString[length], remainLength, "Bor RST");
                    break;

                case ResetReason_IWDGRST:
                    length += usnprintf(&resetString[length], remainLength, "IWDG RST");
                    break;

                case ResetReason_LPWRRST:
                    length += usnprintf(&resetString[length], remainLength, "LowPower RST");
                    break;

                case ResetReason_PINRST:
                    length += usnprintf(&resetString[length], remainLength, "Pin RST");
                    break;

                case ResetReason_PORRST:
                    length += usnprintf(&resetString[length], remainLength, "POR RST");
                    break;

                case ResetReason_SFTRST:
                    length += usnprintf(&resetString[length], remainLength, "Software RST");
                    break;

                case ResetReason_WWDGRST:
                    length += usnprintf(&resetString[length], remainLength, "WWDG RST");
                    break;

                default:
                    break;
            }

            remainLength = strMaxLength - length;

            if (reset & (1 << i))
            {
                /* Clear flag */
                reset &= ~(1 << i);
                if (reset)
                {
                    /* If there are some more reset flag, print ', ' */
                    length += usnprintf(&resetString[length], remainLength, ", ");
                    remainLength = strMaxLength - length;
                }
                else
                {
                    /* There is no more reset flag */
                    break;
                }
            }
        }
    }
    else
    {
        /* If hasn't got value of reset */
        usnprintf(resetString, remainLength, "There is no reset reason");
    }
}



/**
 * @brief       Clear reset flags
 */
void Reset_ClearResetFlags(void)
{
    __HAL_RCC_CLEAR_RESET_FLAGS();
}



/**
 * @brief       Print reset reasons to string
 */
size_t Reset_PrintResetReasons(char * str, uint8_t strLength)
{
    const uint8_t resetStrLength = 40;
    char resetStr[resetStrLength];
    uint32_t resetReason = Reset_GetResetReason();

    Reset_GetResetReasonString(resetReason, resetStr, resetStrLength);

    return usnprintf(str, strLength, "Reset reason: %s", resetStr);
}



/* TODO: Add UnitTest */



#endif
