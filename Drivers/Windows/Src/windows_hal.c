/*
 *		windows_hal.c
 *		Created on:		2017-11-05
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017-11-05
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#include "windows_hal.h"

#include <windows.h>
#include <time.h>
#include <winbase.h>

// for STDIN thread
#include "CommandHandler.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

///< HAL_Init... Set startup time
//static time_t StartupTime;
static uint32_t uwTick = 0;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

extern void SysTick_Handler(void);



DWORD WINAPI Windows_StdinReceiveThread(void* data);
DWORD WINAPI Windows_SysTickThread(void* data);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/
/*
===============================================================================
                      ##### HAL Control functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Provide a tick value in millisecond
      (+) Provide a blocking delay in millisecond
      (+) Suspend the time base source interrupt
      (+) Resume the time base source interrupt
      (+) Get the HAL API driver version
      (+) Get the device identifier
      (+) Get the device revision identifier
      (+) Enable/Disable Debug module during Sleep mode
      (+) Enable/Disable Debug module during STOP mode
      (+) Enable/Disable Debug module during STANDBY mode

@endverbatim
  * @{
  */


/**
 * \brief	Initialize HAL function
 * 			Windows version: Init SysTick "Thread"
 */
void HAL_Init(void)
{
	// Init Tick
	uwTick = 0;

	// Create Thread
	HANDLE thread = CreateThread(NULL, 0, Windows_SysTickThread, NULL, 0, NULL);

	if (!thread)
	{
		printf("Error with thread!");
	}
}



/**
  * @brief  Provides a tick value in millisecond.
  * @note   This function is declared as __weak  to be overwritten  in case of other
  *       implementations in user file.
  * @retval tick value
  */
uint32_t HAL_GetTick(void)
{
	// Working Windows solution
	//return (uint32_t)GetTickCount();
	return (uint32_t)uwTick;

	// If not work...
	/*
	#include <time.h>

	time(&StartupTime);

	time_t actualTime;
	time(&StartupTime);

	double diff = difftime(StartupTime, actualTime);	// Return with seconds
	uint32_t tick = diff * 1000;
	return tick;
	*/
}



/**
  * @brief This function is called to increment  a global variable "uwTick"
  *        used as application time base.
  * @note In the default implementation, this variable is incremented each 1ms
  *       in Systick ISR.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval None
  */
void HAL_IncTick(void)
{
	uwTick++;
	// TODO: Not need increment, because we get tick from windows, and the Delay works with that
}



/**
  * @brief This function provides accurate delay (in milliseconds) based
  *        on variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note ThiS function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */
void HAL_Delay(__IO uint32_t Delay)
{
	uint32_t tickstart = 0U;
	tickstart = HAL_GetTick();
	while((HAL_GetTick() - tickstart) < Delay)
	{
	}
}



/**
  * @brief Suspend Tick increment.
  * @note In the default implementation , SysTick timer is the source of time base. It is
  *       used to generate interrupts at regular time intervals. Once HAL_SuspendTick()
  *       is called, the the SysTick interrupt will be disabled and so Tick increment
  *       is suspended.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval None
  */
void HAL_SuspendTick(void)
{
	/* Disable SysTick Interrupt */
	//CLEAR_BIT(SysTick->CTRL,SysTick_CTRL_TICKINT_Msk);
}



/**
  * @brief Resume Tick increment.
  * @note In the default implementation , SysTick timer is the source of time base. It is
  *       used to generate interrupts at regular time intervals. Once HAL_ResumeTick()
  *       is called, the the SysTick interrupt will be enabled and so Tick increment
  *       is resumed.
  * @note This function is declared as __weak  to be overwritten  in case of other
  *       implementations in user file.
  * @retval None
  */
void HAL_ResumeTick(void)
{
	/* Enable SysTick Interrupt */
	//SET_BIT(SysTick->CTRL,SysTick_CTRL_TICKINT_Msk);
}



// TODO: Need to put hal_uart.c ...
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
{

	HANDLE thread = CreateThread(NULL, 0, Windows_StdinReceiveThread, NULL, 0, NULL);

	if (!thread)
	{
		printf("Error with thread!");
	}

	return HAL_OK;
}



// TODO: Implement more beautiful solution
/**
 * \brief "STDIN --> UART" Receive thread
 */
DWORD WINAPI Windows_StdinReceiveThread(void* data)
{
	// Do stuff.  This will be the first function called on the new thread.
	// When this function returns, the thread goes away.  See MSDN for more details.
	while (1)
	{
		// Not safe!
		char str[50] = { 0 };
		char respBuffer[2048];
		char * command = NULL;

		printf("Type message: ");

		command = gets(str);

		if (command != NULL && strlen(str) > 0)
		{
			printf("Received from stdin: \"%s\"\r\n", str);

			CmdH_Result_t res = CmdH_ExecuteCommand(str, respBuffer, 2048);

			CmdH_PrintResult(res);

			printf("CommandHandler answered:\r\n"
					"%s", respBuffer);
		}
		else
		{
			printf("Null message received!\r\n");
		}
	}

	return 0;
}



/**
 * \brief	Thread for SysTick (increment tick)
 */
DWORD WINAPI Windows_SysTickThread(void* data)
{
	/*
	void SysTick_Handler(void)
	{
	  HAL_IncTick();
	  HAL_SYSTICK_IRQHandler();
	#ifdef CONFIG_USE_FREERTOS
	  osSystickHandler();
	#endif
	  TASKHANDLER_SW_WATCHDOG();
	}*/
	/*
	VOID WINAPI Sleep(
	_In_ DWORD dwMilliseconds
	);
	*/
	while(1)
	{
		Sleep(1);
		SysTick_Handler();
	}

	return 0;
}

