/**
  ******************************************************************************
  * File Name          : main.c
  * Date               : 22/06/2015 22:58:07
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "include.h"
#include "Button.h"
#include "LED.h"
#include "CommandHandler.h"
#include "Display_SSD1306.h"
#include "TaskHandler.h"
#include "USART.h"
#include "DebugUart.h"
#include "Terminal.h"
#include "EventLog.h"
#include "Motor.h"
#include "ESP8266.h"


/* Private variables ---------------------------------------------------------*/


/* USER CODE BEGIN 0 */


#if defined(CONFIG_MODULE_DEBUGUSART_ENABLE) && defined(CONFIG_USE_FREERTOS)
extern xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
extern xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
#endif

#if defined(CONFIG_MODULE_ESP8266_ENABLE) && defined(CONFIG_USE_FREERTOS)
extern xQueueHandle ESP8266_SendMessage_Queue;
#endif


/* USER CODE END 0 */


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);



/**
 * \brief	Main
 */
int main(void)
{

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();


	// USER CODE:

#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
	// EventLog
	EventLog_Init();
#endif


#ifdef CONFIG_MODULE_LED_ENABLE
	// LED
	LED_Init();

#ifndef CONFIG_MODULE_TASKHANDLER_ENABLE
	LED_BLUE_ON();
	LED_GREEN_ON();
	LED_RED_OFF();
#endif
#endif


#ifdef CONFIG_MODULE_BUTTON_ENABLE
	// Button
	BUTTON_Init();
#endif


#ifdef CONFIG_MODULE_COMMON_IO_ENABLE
	// Common IO
	CommonIO_Init();
#endif


#ifdef CONFIG_MODULE_IO_ENABLE
	// IO
	IO_Init();
#endif


#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE
	// Common DAC
	CommonDAC_Init();
#endif


#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
	// Common ADC
	CommonADC_Init();
	// Test, blocking
	//CommonADC_Test();
#endif


#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE
	// Common PWM Initialize
	CommonPWM_Init();
#endif


#ifdef CONFIG_MODULE_MOTOR_ENABLE
	// Remote car motors
	Motor_Init();
#endif


#ifdef CONFIG_MODULE_DISPLAY_ENABLE
	// Display
	Display_SSD1306_Init();

	#ifdef CONFIG_MODULE_DISPLAY_SHOW_SCREEN
	// Display start screen
	Display_LoadCarImage();
	#endif
#endif


#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	// Debug UART
	USART_Init(&Debug_UartHandle);
#ifdef CONFIG_USE_FREERTOS
	DEBUG_USART_Rx_Semaphore = NULL;
	DEBUG_USART_Tx_Semaphore = NULL;
	
	DEBUG_USART_Rx_Semaphore = xSemaphoreCreateBinary();
	DEBUG_USART_Tx_Semaphore = xSemaphoreCreateBinary();	
	
	if (DEBUG_USART_Rx_Semaphore == NULL || DEBUG_USART_Tx_Semaphore == NULL)
	{
		Error_Handler();
	}
#endif	//#ifdef CONFIG_USE_FREERTOS
#ifndef CONFIG_MODULE_TERMINAL_ENABLE
	// Start receive
	DebugUart_StartReceive();
#endif
#endif


#ifdef CONFIG_MODULE_COMMANDHANDLER_ENABLE
	// Command Handler + Terminal initialize
	CommandHandler_Init();
#endif
#ifdef CONFIG_MODULE_TERMINAL_ENABLE
	Terminal_Init();
#endif


#ifdef CONFIG_MODULE_TERMINAL_ENABLE
#if defined(CONFIG_USE_FREERTOS)
	// Terminal Task (FreeRTOS)
	TaskHandle_t MONITOR_TaskHandle = NULL;
	//xTaskCreate( vTaskCode, "NAME", STACK_SIZE, &ucParameterToPass, tskIDLE_PRIORITY, &xHandle );
	if (xTaskCreate( (pdTASK_CODE)CommandHandler_CheckCommand, "MonitorTask", MONITOR_TASK_STACK_SIZE, 0,
				MONITOR_TASK_PRIORITY, &MONITOR_TaskHandle ) != pdPASS)
	{
		Error_Handler();
	}
#elif !defined(CONFIG_MODULE_TASKHANDLER_ENABLE)
	// Terminal infinite loop, if not used TaskHandler
	CommandHandler_CheckCommand();
#endif
#endif	// #ifdef CONFIG_MODULE_COMMANDHANDLER_ENABLE


#ifdef CONFIG_MODULE_ESP8266_ENABLE
	// ESP8266
	ESP8266_Init();

#ifdef CONFIG_USE_FREERTOS
	// ESP8266 FreeRTOS queues
	ESP8266_USART_Rx_Semaphore = xSemaphoreCreateBinary();
	ESP8266_SendMessage_Queue = xQueueCreate(
			ESP8266_SENDMESSAGE_QUEUE_LENGTH,
			ESP8266_MESSAGE_QUEUE_ITEM_SIZE);
	ESP8266_ReceivedMessage_Queue = xQueueCreate(
			ESP8266_RECEIVEMESSAGE_QUEUE_LENGTH,
			ESP8266_MESSAGE_QUEUE_ITEM_SIZE);

	// ESP8266 Task initialization
	TaskHandle_t ESP8266_TaskHandle = NULL;
	if (xTaskCreate((pdTASK_CODE)ESP8266_Task, "ESP8266Task", ESP8266_TASK_STACK_SIZE, 0,
					ESP8266_TASK_PRIORITY, &ESP8266_TaskHandle) != pdPASS)
	{
		Error_Handler();
	}
#endif
#endif
	
	
#if defined(CONFIG_MODULE_SYSMANAGER_ENABLE) && defined(CONFIG_USE_FREERTOS)
	// SysManager
	TaskHandle_t SYSMANAGER_TaskHandle = NULL;
	if ( xTaskCreate( (pdTASK_CODE)SYSMANAGER_Task, "SysManagerTask", SYSMANAGER_TASK_STACK_SIZE, 0,
					SYSMANAGER_TASK_PRIORITY, &SYSMANAGER_TaskHandle ) != pdPASS)
	{
		Error_Handler();
	}
#endif
	

#ifdef CONFIG_MODULE_RTC_ENABLE
	// RTC
	RTC_Init();
#endif


#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
	// Watchdog
	Watchdog_Init();
#endif


#if defined(CONFIG_MODULE_TASKHANDLER_ENABLE)
	// Task handler
	TaskHandler_Init();

	uint32_t actualTick = 0;
	uint32_t oldTick = HAL_GetTick();
	uint32_t elapsedTick = 0;

	// Run TaskHandler - Infinite loop
	while (1)
	{
		actualTick = HAL_GetTick();
		elapsedTick = actualTick - oldTick;
		oldTick = actualTick;

		TaskHandler_Scheduler(elapsedTick);
	}
#endif


#ifdef CONFIG_USE_FREERTOS
	// FreeRTOS scheduler
	vTaskStartScheduler();
#endif


	// !! Never reach this code
	Error_Handler();

}	// End of main



#if defined(CONFIG_USE_PANEL_NODESMALL) || defined(CONFIG_USE_PANEL_FASTENNODE)
/**
 * System Clock Configuration
 */
void SystemClock_Config(void)
{

  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  __SYSCFG_CLK_ENABLE();

}
#endif //#ifdef CONFIG_USE_PANEL_NODESMALL



#ifdef CONFIG_USE_PANEL_NODEMEDIUM
/**
 * System Clock Configuration
 */
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	__PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);


	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);


	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}
#endif	// #ifdef CONFIG_USE_PANEL_NODEMEDIUM



#if defined(CONFIG_USE_PANEL_HOMEAUTCENTERPANEL) \
	|| defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) \
	|| defined(CONFIG_USE_PANEL_NUCLEOF401RE)
/**
 * System Clock Configuration
 */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}
#endif	// #ifdef CONFIG_USE_PANEL_...



/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

#ifdef CONFIG_USE_FREERTOS

/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	// TODO: What is this?
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	//for( ;; );
	Error_Handler();
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	//for( ;; );
	Error_Handler();
}
/*-----------------------------------------------------------*/
#endif	// #ifdef CONFIG_USE_FREERTOS

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
