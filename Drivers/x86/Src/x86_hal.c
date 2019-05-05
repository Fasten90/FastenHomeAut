/*
 *		x86_hal.c
 *		Created on:		2017-11-05
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			x86
 *		Version:		-
 *		Last modified:	2019-05-05
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"

#include <stdio.h>
#include <time.h>

#ifdef CONFIG_PLATFORM_X86_WIN
#include <windows.h>
#include <winbase.h>
#elif defined(CONFIG_PLATFORM_X86_LINUX)
/* Linux */
#include <pthread.h>
#endif

#include <unistd.h>

// for STDIN thread
#include "CommandHandler.h"

#include "x86_hal.h"
#include "x86_hal_def.h"

#ifdef CONFIG_MODULE_BUTTONSIMULATOR_ENABLE
    #ifdef CONFIG_PLATFORM_X86_WIN
    /* conio.h is MS-DOS compiler supported console input/output header. For Linux it does not work! */
    #include <conio.h> /* getch() */
    #endif
#include "ButtonSimulator.h"
#endif



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

///< HAL_Init... Set startup time
//static time_t StartupTime;
static uint32_t uwTick = 0;

///< Thread handles
#ifdef CONFIG_PLATFORM_X86_WIN
// TODO: Rename
HANDLE WindowsHal_SystickThreadHandle;
HANDLE WindowsHal_UartConsoleThreadHandle;
#elif defined(CONFIG_PLATFORM_X86_LINUX)
pthread_t x86_linux_SystickThreadHandle;
pthread_t x86_linux_UartConsoleThreadHandle;
#endif



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

extern void SysTick_Handler(void);

// TODO: Renames
#ifdef CONFIG_PLATFORM_X86_WIN
DWORD WINAPI Windows_SysTickThread(void * args);
DWORD WINAPI Windows_StdinReceiveThread(void * args);
#elif defined(CONFIG_PLATFORM_X86_LINUX)
void * x86_linux_SysTickThread(void * args);
void * x86_linux_StdinReceiveThread(void * args);
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Initialize HAL function
 * 			Windows version: Init SysTick "Thread"
 */
void HAL_Init(void)
{
	// Init Tick
	uwTick = 0;

	// Create Thread
#ifdef CONFIG_PLATFORM_X86_WIN
    WindowsHal_SystickThreadHandle = CreateThread(NULL, 0, Windows_SysTickThread, NULL, 0, NULL);

    if (!WindowsHal_SystickThreadHandle)
    {
        printf("Error with thread!");
    }

#elif defined(CONFIG_PLATFORM_X86_LINUX)
    /*int pthread_create(pthread_t *thread, const
    pthread_attr_t *attr, void
    *(*start_routine) (void *), void *arg);*/
    int result = pthread_create(&x86_linux_SystickThreadHandle, (pthread_attr_t *)NULL, x86_linux_SysTickThread, (void *)NULL);

    if (result != 0)
    {
        printf("Error with thread!");
    }

#endif
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
	uint32_t tickstart = HAL_GetTick();
	while((HAL_GetTick() - tickstart) < Delay)
	{
		// Wait
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
    // Create Thread
#ifdef CONFIG_PLATFORM_X86_WIN
	WindowsHal_UartConsoleThreadHandle = CreateThread(NULL, 0, Windows_StdinReceiveThread, NULL, 0, NULL);

    if (!WindowsHal_SystickThreadHandle)
    {
        printf("Error with thread!");
    }

#elif defined(CONFIG_PLATFORM_X86_LINUX)
    /*int pthread_create(pthread_t *thread, const
    pthread_attr_t *attr, void
    *(*start_routine) (void *), void *arg);*/
    int result = pthread_create(&x86_linux_UartConsoleThreadHandle,  (pthread_attr_t *)NULL, x86_linux_StdinReceiveThread, (void *)NULL);

    if (result != 0)
    {
        printf("Error with thread!");
    }
#endif

	return HAL_OK;
}



#ifdef CONFIG_PLATFORM_X86_WIN
// TODO: Implement more beautiful solution
/**
 * \brief "STDIN --> UART" Receive thread
 */
DWORD WINAPI Windows_StdinReceiveThread(void * args)
{
	// Do stuff.  This will be the first function called on the new thread.
	// When this function returns, the thread goes away.  See MSDN for more details.
    #if !defined(CONFIG_MODULE_BUTTONSIMULATOR_AUTO_ON)
	while (1)
	{
		// Not safe!
		char str[50] = { 0 };
		char respBuffer[2048];
		char * command;

		printf("Type message: ");

		// gets() enabled to use in this situation, because it is only test for windows, not a safety-critical application
		// cppcheck-suppress getsCalled
		command = gets(str);

		if (command != NULL && strlen(str) > 0)
		{
			printf("Received from stdin: \"%s\"\r\n", str);

			CmdH_Result_t res = CmdH_ExecuteCommand(str, respBuffer, 2048);

			CmdH_PrintResult(res);

			printf("CommandHandler answered:\r\n"
					"%s", respBuffer);

    #ifdef CONFIG_MODULE_BUTTONSIMULATOR_ENABLE
			if (ButtonSimulator_IsEnabled)
			{
    #endif /* #ifdef CONFIG_MODULE_BUTTONSIMULATOR_ENABLE */
    #endif /*  #if !defined(CONFIG_MODULE_BUTTONSIMULATOR_AUTO_ON) */
    #if defined(CONFIG_MODULE_BUTTONSIMULATOR_ENABLE) || defined(CONFIG_MODULE_BUTTONSIMULATOR_AUTO_ON)
			    char str[4] = { 0 };
			    int str_pos = 0;
    #if !defined(CONFIG_MODULE_BUTTONSIMULATOR_AUTO_ON)
			    while (ButtonSimulator_IsEnabled)
    #else
                while (1)
    #endif
			    {

			        str[str_pos]= getch();
			        str_pos++;

			        if (str_pos >= 2)
			        {
			            /* Debug print */
			            /* printf("Received: %s\r\n", str); */

			            bool result = ButtonSimulator_ProcessChar(str);
    #if !defined(CONFIG_MODULE_BUTTONSIMULATOR_AUTO_ON)
			            if (!result)
			            {
			                /* Turn off key mode / Buttonsimulator! */
			                ButtonSimulator_Set(false);
			                printf("Exit from buttonsimulator!\r\n");
			            }
    #else
			            UNUSED_VARIABLE(result);
    #endif

			            str_pos = 0;
			        }
			    }
    #endif /* #if defined(CONFIG_MODULE_BUTTONSIMULATOR_ENABLE) || defined(CONFIG_MODULE_BUTTONSIMULATOR_AUTO_ON) */
    #if !defined(CONFIG_MODULE_BUTTONSIMULATOR_AUTO_ON)
			}
		}
		else
		{
			printf("Null message received!\r\n");
		}
	}
    #endif /* !defined(CONFIG_MODULE_BUTTONSIMULATOR_AUTO_ON) */

	return 0;
}
#elif defined(CONFIG_PLATFORM_X86_LINUX)

void * x86_linux_StdinReceiveThread(void * args)
{
    /* TODO: */
    while (1);

    return NULL;
}

#endif



/* TODO: Rename if works with Linux */
void windows_delay_ms(int mseconds)
{
    // Stroing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + mseconds);
}



#ifdef CONFIG_PLATFORM_X86_WIN
/**
 * \brief	Thread for SysTick (increment tick)
 */
DWORD WINAPI Windows_SysTickThread(void * data)
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
	    /* Sleep(1) is not too accurate */
		/* Sleep(1); */

	    /* Known warning: 'usleep' is deprecated [-Wdeprecated-declarations] */
	    /* usleep not enough good too... */
		/*usleep(100); */

	    /* Delay not supported by standard library? */
	    /*delay(1);*/

	    /* Not enough accurate */
	    /*windows_delay_ms(1);*/

	    /* Wrong experiences with these 1ms scheduling */
	    /*SysTick_Handler();*/

	    const int ms = 10;
	    windows_delay_ms(ms);
	    for (int i=0; i < ms; i++)
	    {
	        SysTick_Handler();
	    }
	}

	return 0;
}

#elif defined(CONFIG_PLATFORM_X86_LINUX)

void * x86_linux_SysTickThread(void * args)
{
    /* Note: This code parts was copied from above, from Win */
    while(1)
    {
        const int ms = 10;
        windows_delay_ms(ms);
        for (int i=0; i < ms; i++)
        {
            SysTick_Handler();
        }
    }

    return NULL;
}

#endif
