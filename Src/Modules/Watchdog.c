/*
 *		Watchdog.c
 *
 *		Created on:		2017. febr. 6.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 6.
 */




/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "Watchdog.h"



#ifdef CONFIG_MODULE_WATCHDOG_ENABLE


IWDG_HandleTypeDef IWDG_Handle;



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Initialize Watchdog
 */
void Watchdog_Init(void)
{
	// Configurate IWDG
	IWDG_Handle.Init.Prescaler = IWDG_PRESCALER_32;
	IWDG_Handle.Init.Reload = 0x0FFF;	// Max value
	IWDG_Handle.Instance = IWDG;

	if (HAL_IWDG_Init(&IWDG_Handle) != HAL_OK)
	{
		Error_Handler();
	}
}



/**
 * \brief	Clear Watchdog
 */
void Watchdog_Clear(void)
{
	HAL_IWDG_Refresh(&IWDG_Handle);
}



#endif	// #ifdef CONFIG_MODULE_WATCHDOG_ENABLE
