/*
 *        HardFaultHandler.c
 *        Created on:        2017-11-24
 *        Author:            Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        -
 *        Target:            STM32Fx
 *        Version:        -
 *        Last modified:    2017-11-24
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_MODULE_HARDFAULTHANDLER_ENABLE

#include "HardFaultHandler.h"
#include "GenericTypeDefs.h"
#include "StringHelper.h"
#include "DebugUart.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static const char * HardFault_GetReason(CortexM_HardFault_CCR_register_t reg);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * \brief    Get HardFault reason
 */
static const char * HardFault_GetReason(CortexM_HardFault_CCR_register_t reg)
{
    const char * msg = NULL;

#ifdef CONFIG_PLATFORM_MCU_STM32F0xx
    // TODO: If more bits are set?
    if (reg.regbits.StkAlign)
    {
        msg = "StackAlign";
    }
    else if (reg.regbits.UnalignTrap)
    {
        msg = "UnalignTrap";
    }
#else
#error "HardFault register is not implemented!"
#endif
    else if ((uint32_t)reg.regvalue != 0)
    {
        msg = "Register is not 0";
    }
    else
    {
        msg = "Unknown reason (0)";
    }

    return msg;
}



/**
 * \brief    Print HardFault reason
 */
void HardFault_PrintHardFaultReason(void)
{
    CortexM_HardFault_CCR_register_t reg;
    reg.regvalue = (uint32_t)SCB->CCR;

    const char * msg = HardFault_GetReason(reg);

    DebugUart_SendMessageBlocked("HardFault: ");
    DebugUart_SendMessageBlocked(msg);
    DebugUart_SendMessageBlocked("\r\n");
}



/**
 * \brief    Get registers from stack - called from HardFault ISR
 */
void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress)
{
    /* These are volatile to try and prevent the compiler/linker optimising them
    away as the variables never actually get used.  If the debugger won't show the
    values of the variables, make them global my moving their declaration outside
    of this function. */
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr; /* Link register. */
    volatile uint32_t pc; /* Program counter. */
    volatile uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[0];
    r1 = pulFaultStackAddress[1];
    r2 = pulFaultStackAddress[2];
    r3 = pulFaultStackAddress[3];

    r12 = pulFaultStackAddress[4];
    lr = pulFaultStackAddress[5];
    pc = pulFaultStackAddress[6];
    psr = pulFaultStackAddress[7];


    /* When the following line is hit, the variables contain the register values. */
    char str[30];
    // TODO: print to one large string
    usprintf(str, "%s: 0x%X\r\n", "r0 ", r0);
    DebugUart_SendMessageBlocked(str);
    usprintf(str, "%s: 0x%X\r\n", "r1 ", r1);
    DebugUart_SendMessageBlocked(str);
    usprintf(str, "%s: 0x%X\r\n", "r2 ", r2);
    DebugUart_SendMessageBlocked(str);
    usprintf(str, "%s: 0x%X\r\n", "r3 ", r3);
    DebugUart_SendMessageBlocked(str);

    usprintf(str, "%s: 0x%X\r\n", "r12", r12);
    DebugUart_SendMessageBlocked(str);
    usprintf(str, "%s: 0x%X\r\n", "lr ", lr);
    DebugUart_SendMessageBlocked(str);
    usprintf(str, "%s: 0x%X\r\n", "pc ", pc);
    DebugUart_SendMessageBlocked(str);
    usprintf(str, "%s: 0x%X\r\n", "psr", psr);
    DebugUart_SendMessageBlocked(str);


    // Print HardFault registers
    HardFault_PrintHardFaultReason();

    // Go to Error state
    Error_Handler();
}



#endif    // #ifdef CONFIG_MODULE_HARDFAULTHANDLER_ENABLE
