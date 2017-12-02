/*
 *		Globals.c
 *		Created on:		2016-09-15
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Global variables
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-11-23
 */



#include "options.h"
#include "include.h"
#include "version.h"
#include "LED.h"
#include "StringHelper.h"
#include "DebugUart.h"
#include "Globals.h"
#include "TaskHandler.h"

#ifdef CONFIG_MICROCONTROLLER_PC
#include "windows_hal.h"
#endif



/*------------------------------------------------------------------------------
 *								Global variables
 *----------------------------------------------------------------------------*/

const char Global_BoardName[]	= BOARD_NAME;
const char Global_Version[]		= VERSION_STRING;
const char Global_BoardMCU[]	= BOARD_MCU;
char Global_DeviceName[20] 		= BOARD_NAME;



/*------------------------------------------------------------------------------
 *									Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief		Delay (ms)
 * \param[in]	ms	millisecond, which time delay
 */
inline void DelayMs(uint32_t ms)
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
	// Error LED
	LED_SetLed(LED_Red, LED_Set_On);
	LED_SetLed(LED_Green, LED_Set_Off);
	LED_SetLed(LED_Blue, LED_Set_Off);

#ifdef CONFIG_USE_FREERTOS
	// End task scheduling
	vTaskEndScheduler();
#endif

	DebugUart_SendMessageBlocked("ErrorHandler...!!!\r\n");

#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
	char msg[60];
	usprintf(msg, "TaskHandler frozen: %s\r\n", TaskHandler_GetActualRunningTask());
	DebugUart_SendMessageBlocked(msg);
#endif


	// Stop debugger
	DEBUG_BREAKPOINT();

	// Infinite loop, do not disable interrupts ...
	// TODO: But... If there is no interrupt?
	uint8_t cnt = 8;
	while (cnt--)
	{
		LED_SetLed(LED_Red, LED_Set_Toggle);
		DelayMs(125);
	}

	// Reset...
	// TODO: It is not the best solution, The user will not detect the reset
	NVIC_SystemReset();
}



#ifdef USE_FULL_ASSERT
/**
 * \brief Reports the name of the source file and the source line number where the assert_param error has occurred.
 * \param file: pointer to the source file name
 * \param line: assert_param error line source number
 * \retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	// ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)
	Assert_Function(file, line, "assert_failed");
}
#endif



/**
 * \brief	Assert
 */
void Assert_Function(char *file, uint32_t line, char *exp)
{
	// Error LED
	LED_SetLed(LED_Red, LED_Set_On);
	LED_SetLed(LED_Green, LED_Set_Off);
	LED_SetLed(LED_Blue, LED_Set_Off);

	// Send error message
	char errorMsg[255];
	usprintf(errorMsg, "File: %s, %d. line: %s\r\n", file, line, exp);
	DebugUart_SendMessageBlocked(errorMsg);
	DelayMs(100);

	DEBUG_BREAKPOINT();
	Error_Handler();
}

