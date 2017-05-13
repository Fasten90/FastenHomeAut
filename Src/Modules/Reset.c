/*
 *		Reset.c
 *
 *		Created on:		2017. febr. 27.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 27.
 */




/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "options.h"
#include "include.h"
#include "Reset.h"


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
 * \brief Get reset reason
 */
uint32_t Reset_GetResetReason(void)
{
	uint32_t resetReason = 0;
	uint8_t i;

	for (i = 0; (1<<i) < ResetReason_Count; i++)
	{
		// \note Be careful, now it is work in STM32F407, because:
		/*
		 *	#define RCC_FLAG_BORRST                  ((uint8_t)0x79U)
		*	#define RCC_FLAG_PINRST                  ((uint8_t)0x7AU)
		*	#define RCC_FLAG_PORRST                  ((uint8_t)0x7BU)
		*	#define RCC_FLAG_SFTRST                  ((uint8_t)0x7CU)
		*	#define RCC_FLAG_IWDGRST                 ((uint8_t)0x7DU)
		*	#define RCC_FLAG_WWDGRST                 ((uint8_t)0x7EU)
		*	#define RCC_FLAG_LPWRRST                 ((uint8_t)0x7FU)
		 */
#ifdef CONFIG_MICROCONTROLLER_STM32F4xx
		if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST + i))
#elif defined(CONFIG_MICROCONTROLLER_STM32F0xx)
		if (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST + i))
#else
#error "Unknown Microcontroller family"
#endif
		{
			// Set flags, if have
			resetReason |= (1 << i);
		}

	}

	// Clear reset flags
	__HAL_RCC_CLEAR_RESET_FLAGS();

	return resetReason;
}



/**
 * \brief	Get reset reason string
 */
void Reset_GetResetReasonString(uint32_t resetReason, char *resetString)
{
	ResetReason_t reset = resetReason & ResetReason_Mask;
	uint8_t length = 0;
	uint8_t i;

	if (resetReason)
	{
		// If has reset reason value
		for (i = 0; (1<<i) < ResetReason_Count; i++)
		{
			switch (reset & (1 << i))
			{
				case ResetReason_BORRST:
					length += usprintf(&resetString[length], "Bor RST");
					break;

				case ResetReason_IWDGRST:
					length += usprintf(&resetString[length], "IWDG RST");
					break;

				case ResetReason_LPWRRST:
					length += usprintf(&resetString[length], "LowPower RST");
					break;

				case ResetReason_PINRST:
					length += usprintf(&resetString[length], "Pin RST");
					break;

				case ResetReason_PORRST:
					length += usprintf(&resetString[length], "POR RST");
					break;

				case ResetReason_SFTRST:
					length += usprintf(&resetString[length], "Software RST");
					break;

				case ResetReason_WWDGRST:
					length += usprintf(&resetString[length], "WWDG RST");
					break;

				default:
					break;
			}

			if (reset & (1 << i))
			{
				// Clear flag
				reset &= ~(1 << i);
				if (reset)
				{
					// If there are some reset flag, print ', '
					length += usprintf(&resetString[length], ", ");
				}
				else
				{
					// There is no more reset flag
					break;
				}
			}
		}
	}
	else
	{
		// If hasn't got value of reset
		usprintf(resetString, "There is no reset reason");
	}
}



/**
 * \brief	Clear reset flags
 */
void Reset_ClearResetFlags(void)
{
	__HAL_RCC_CLEAR_RESET_FLAGS();
}



/**
 * \brief	Print reset reasons on debug port
 */
void Reset_PrintResetReasons(void)
{
	char str[40];
	uint32_t resetReason = Reset_GetResetReason();

	Reset_GetResetReasonString(resetReason, str);

	uprintf("Reset reason: %s", str);

}
