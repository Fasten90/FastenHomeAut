/*
 *    Reset.h
 *    Created on:   2017-02-27
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Reset module
 *    Target:       STM32Fx
 */

#ifndef RESETHANDLER_H_
#define RESETHANDLER_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

/*
 *    @arg RCC_FLAG_BORRST: POR/PDR or BOR reset.
 *    @arg RCC_FLAG_PINRST: Pin reset.
 *    @arg RCC_FLAG_PORRST: POR/PDR reset.
 *    @arg RCC_FLAG_SFTRST: Software reset.
 *    @arg RCC_FLAG_IWDGRST: Independent Watchdog reset.
 *    @arg RCC_FLAG_WWDGRST: Window Watchdog reset.
 *    @arg RCC_FLAG_LPWRRST: Low Power reset.
 */
#define RESET_RCC_ALL_RESET_FLAGS    ( RCC_FLAG_BORRST \
                                    | RCC_FLAG_PINRST \
                                    | RCC_FLAG_PORRST \
                                    | RCC_FLAG_SFTRST \
                                    | RCC_FLAG_IWDGRST \
                                    | RCC_FLAG_WWDGRST \
                                    | RCC_FLAG_LPWRRST \
                                    )



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< Reset reason (Why reseted)
typedef enum
{
    ResetReason_Unknown    = 0x00,    ///< Unknown
    ResetReason_BORRST    = 0x01,    ///< POR/PDR or BOR reset.
    ResetReason_PINRST    = 0x02,    ///< Pin reset.
    ResetReason_PORRST    = 0x04,    ///< POR/PDR reset.
    ResetReason_SFTRST    = 0x08,    ///< Software reset.
    ResetReason_IWDGRST    = 0x10,    ///< Independent Watchdog reset.
    ResetReason_WWDGRST    = 0x20,    ///< Window Watchdog reset.
    ResetReason_LPWRRST    = 0x40,    ///< Low Power reset.
    ResetReason_Count    = 0x80,    ///< Max value of reset reason
    ResetReason_Mask    = 0xFF    ///< Reset reason mask
} ResetReason_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Reset_ClearResetFlags(void);
size_t Reset_PrintResetReasons(char * str, uint8_t strLength);



#endif /* RESETHANDLER_H_ */
