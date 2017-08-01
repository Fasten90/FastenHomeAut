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

char Global_DeviceName[20] 		= BOARD_NAME;



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


	// Stop debugger
	DEBUG_BREAKPOINT();


#ifdef CONFIG_USE_FREERTOS
	// End task scheduling
	vTaskEndScheduler();
#endif

	// Reset...
	NVIC_SystemReset();
	// TODO: It is not the best solution, The user will not detect the reset

	// Infinite loop, do not disable interrupts ...
	uint8_t cnt = 8;
	while(cnt--)
	{
		LED_RED_TOGGLE();
		DelayMs(125);
	}

}
