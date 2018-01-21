/*
 *		SwWatchDog.c
 *		Created on:		2017-12-02
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Software WatchDog: The check function should be called by periodical from interrupt (e.g. SysTick),
 *						and the increment (alive) function called from TaskHandler, or while(1), or tasks.
 *						If inc not called enough time ago, the software was frozen, and the CheckState() should call the Error_Handler()
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017-12-02
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_DEBUG_SW_WATCHDOG_ENABLE
#include "GenericTypeDefs.h"
#include "SwWatchDog.h"
#include "ErrorHandler.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

static uint32_t SwWatchDog_Counter = 0;



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
 * \brief	Software WatchDog - Increment counter / alive call
 */
void SwWatchDog_Inc(void)
{
	SwWatchDog_Counter++;
}



/**
 *	\brief	Software WatchDog
 *	\note	Call this function from timer / SysTickHandler every 1 ms
 *			If main software not called SwWatchDog_Inc() (cnt is not increment), we know, the main sw (TaskHandler) was frozen
 */
void SwWatchDog_CheckState(void)
{
	static uint32_t ms = 0;
	static uint32_t lastCnt = 0;

	ms++;
	if (ms >= SW_WATCHDOG_PERIOD)
	{
		ms = 0;
		if (SwWatchDog_Counter == lastCnt)
		{
			// SwWatchDog counter is not changed :(

			// Be careful: Error_Handler is use the SysTick handler...
			Error_Handler();
		}

		lastCnt = SwWatchDog_Counter;
	}
}


#else

// SwWatchDog module is not used
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic pop


#endif
