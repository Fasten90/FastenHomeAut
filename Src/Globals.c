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

#include "include.h"
#include "options.h"
#include "globals.h"

const char Global_Version[] = "v0.1.1.5";



void DelayMs(uint32_t ms)
{
#ifdef CONFIG_USE_FREERTOS
	vTaskDelay(ms/);
#else
	HAL_Delay(ms);
#endif
}

