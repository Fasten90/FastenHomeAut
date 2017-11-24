/*
 *		HardFaultHandler.h
 *		Created on:		2017-11-24
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017-11-24
 */

#ifndef HARDFAULTHANDLER_H_
#define HARDFAULTHANDLER_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/


#ifdef CONFIG_MICROCONTROLLER_STM32F0xx
	// STM32F0
	#include "stm32f0xx_hal.h"
	#include "stm32_hal_legacy.h"
	//#include "core_cm0plus.h"
#endif



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


/*
 * CCR
 * RESERVED 0:3
 * UNALIGN_TRP 3:1
 * RESERVED 4:5
 * STKALIGN 9:1
 * RESERVED 10:22
 */


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MICROCONTROLLER_STM32F0xx
typedef union
{
	struct
	{
		uint32_t Reserved1: 3;
		uint32_t UnalignTrap: 1;
		uint32_t Reserved2: 5;
		uint32_t StkAlign: 1;
		uint32_t Reserved3: 22;
	} regbits;
	uint32_t regvalue;
} CortexM_HardFault_CCR_register_t;
#endif



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void HardFaultHandler_Init(void);
void HardFault_PrintHardFaultReason(void);



#endif /* HARDFAULTHANDLER_H_ */
