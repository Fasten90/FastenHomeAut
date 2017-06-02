/*
 *		Globals.c
 *
 *		Created on:		2016.09.15
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Globals
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.11.23
 */

#include "options.h"
#include "include.h"
#include "version.h"
#include "globals.h"



/*------------------------------------------------------------------------------
 *								Global variables
 *----------------------------------------------------------------------------*/

const char Global_BoardName[]	= BOARD_NAME;
const char Global_Version[]		= VERSION_DEFINE;
const char Global_BoardMCU[]	= BOARD_MCU;



/*------------------------------------------------------------------------------
 *									Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief	Delay (ms)
 * \param[in]	ms	millisecond, which time delay
 */
void DelayMs(uint32_t ms)
{
#ifdef CONFIG_USE_FREERTOS
	vTaskDelay((TickType_t)(ms/portTICK_PERIOD_MS));
#else
	HAL_Delay(ms);
#endif
}



/**
 * \brief	Error Handler
 */
void Error_Handler(void)
{
	// Turn off LEDs
	LED_BLUE_OFF();
	LED_GREEN_OFF();
	LED_RED_ON();

#ifdef CONFIG_DEBUG_MODE
	// Stop debugger
	__asm("BKPT #0\n");		// ASM: Break debugger
#endif

#ifdef CONFIG_USE_FREERTOS
	// End task scheduling
	vTaskEndScheduler();
#endif

	// Reset...
	NVIC_SystemReset();

	// Infinite loop, do not disable interrupts ...
	while(1)
	{
		LED_RED_TOGGLE();
		DelayMs(125);
	}
}
