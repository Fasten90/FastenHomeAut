/**
  ******************************************************************************
  * @file    stm32fxxx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "options.h"

#ifdef CONFIG_PLATFORM_MCU_STM32Fxxx

#if defined(CONFIG_PLATFORM_MCU_STM32F0xx)
    #include "stm32f0xx_hal.h"
    #include "stm32f0xx.h"
    #include "stm32f0xx_it.h"
#elif defined(CONFIG_PLATFORM_MCU_STM32F4xx)
    #include "stm32f4xx_hal.h"
    #include "stm32f4xx.h"
    #include "stm32f4xx_it.h"
#else
    #error "Not supported STM32Fxxx platform"
#endif

/* TODO: This is need? */
/* include "cmsis_os.h" */

#include "ErrorHandler.h"
#include "SwWatchDog.h"
#include "HardFaultHandler.h"

#ifdef CONFIG_MODULE_HARDFAULTHANDLER_ENABLE
extern void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress);
#endif

/* External variables --------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0-3-4 Processor Interruption and Exception Handlers         */
/******************************************************************************/



/**
 * @brief    HardFault handler
 */
void HardFault_Handler(void)
{
#ifdef CONFIG_MODULE_HARDFAULTHANDLER_ENABLE

#if 0
    /* FreeRTOS style:
     * http://www.freertos.org/Debugging-Hard-Faults-On-Cortex-M-Microcontrollers.html
     * It works on Cortex-M3-4?
     */
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word prvGetRegistersFromStack    \n"
    );
#elif 1
    /* https://blog.feabhas.com/2013/02/developing-a-generic-hard-fault-handler-for-arm-cortex-m3cortex-m4/ */
    /* Not work */
    __asm volatile
    (
        " mrs r0, MSP \n"
        " b prvGetRegistersFromStack \n"
    );
#elif 0
    /* Segger version */
    /* This version is for Cortex M0 */
    /*
    ;
    movs R0, #4
    mov R1, LR
    tst R0, R1 ; Check EXC_RETURN in Link register bit 2.
    bne Uses_PSP
    mrs R0, MSP ;  Stacking was using MSP.
    b Pass_StackPtr
    */
    __asm volatile
    (
    "movs R0, #4 \n"
    "mov R1, LR \n"
    "tst R0, R1 \n" /* Check EXC_RETURN in Link register bit 2. */
    /* bne Uses_PSP \n"        // mrs R0, PSP ;// Stacking was using PSP. */
    "mrs R0, MSP \n" /* Stacking was using MSP. */
    "b prvGetRegistersFromStack  \n"
    );
#elif 1
    /* Own version */
    /* TODO: Not Keil uVision ARMCC compatible */
    __asm volatile
    (
        " mrs r0, MSP \n"
        /* TODO: Check */
        /* Does not work at Optimized compiles: O1, O2, O3, Os... */
        /* Perhaps: F0 not optimized and F4+ versions */
        /*" ldr r1, handler \n"
        " bx r1 \n" */
        " handler: .word prvGetRegistersFromStack \n"
    );
#endif
#endif    /* CONFIG_MODULE_HARDFAULTHANDLER_ENABLE */

    Error_Handler();
}



/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
}



/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
}



/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
    /* TODO: Save */
    #include "DebugUart.h"
    DebugUart_SendMessageBlocked("NMI");
    DEBUG_BREAKPOINT();
}



/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
#ifdef CONFIG_USE_FREERTOS
    osSystickHandler();
#endif
    SW_WATCHDOG_CHECK();
}



/******************************************************************************/
/* STM32Fxxx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32fxxx.s).                    */
/******************************************************************************/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

#endif /* CONFIG_PLATFORM_MCU_STM32Fxxx */
