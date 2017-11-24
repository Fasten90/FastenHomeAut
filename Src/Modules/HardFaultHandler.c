/*
 *		HardFaultHandler.c
 *		Created on:		2017-11-24
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017-11-24
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_MODULE_HARDFAULTHANDLER_ENABLE

#include "GenericTypeDefs.h"
#include "HardFaultHandler.h"
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
 * \brief  HardFaultHandler initialization
 */
void HardFaultHandler_Init(void)
{

}



/**
 * \brief	Get HardFault reason
 */
static const char * HardFault_GetReason(CortexM_HardFault_CCR_register_t reg)
{
	const char * msg = NULL;

#ifdef CONFIG_MICROCONTROLLER_STM32F0xx
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
 * \brief	Print HardFault reason
 */
void HardFault_PrintHardFaultReason(void)
{
	CortexM_HardFault_CCR_register_t reg;
	reg.regvalue = (uint32_t)SCB->CCR;

	const char * msg = HardFault_GetReason(reg);

	DebugUart_SendMessageBlocked("HardFault: ");
	DebugUart_SendMessageBlocked(msg);
}



#endif	// #ifdef CONFIG_MODULE_HARDFAULTHANDLER_ENABLE
