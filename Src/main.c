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
#if CONFIG_USE_PANEL_NODESMALL
	#include "stm32f0xx_hal.h"
#endif
#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	#include "stm32l1xx_hal.h"
#endif
#ifdef CONFIG_USE_PANEL_CENTERPANEL
	#include "stm32f4xx_hal.h"
#endif


#include "include.h"


/* Private variables ---------------------------------------------------------*/


/* USER CODE BEGIN 0 */



#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
extern UART_HandleTypeDef Debug_UartHandle;

	#ifdef CONFIG_USE_FREERTOS
	extern xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
	extern xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
	extern xQueueHandle ESP8266_SendMessage_Queue;
	#endif
#endif


/* USER CODE END 0 */


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);



int main(void)
{

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();


	// USER CODE:
	
	// TEST
#ifdef TEST_CODE
	USART_Init(&Debug_UartHandle);
	USART_SendEnable_flag = ENABLE;
	while(1)
	USART_SendChar('a');
#endif


#ifdef CONFIG_MODULE_LED_ENABLE
	// LED
	LED_Init();
	
	LED_BLUE_ON();
	LED_GREEN_ON();
	LED_RED_OFF();
#endif


#ifdef CONFIG_MODULE_BUTTON_ENABLE
	// BUTTON
	BUTTON_Init();
#endif
	

#ifdef CONFIG_MODULE_IO_ENABLE
	// IO
	IO_Init();
#endif


#ifdef CONFIG_MODULE_DAC_ENABLE
	HAL_DAC_MspInit(&DacHandle);
	DAC_Config();
#endif

	// DEBUG USART
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
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
	
	// Monitor
	USART_Init(&Debug_UartHandle);
	MONITOR_Init();
	
	STRING_UnitTest();

#ifdef CONFIG_USE_FREERTOS
	TaskHandle_t MONITOR_TaskHandle = NULL;
	//xTaskCreate( vTaskCode, "NAME", STACK_SIZE, &ucParameterToPass, tskIDLE_PRIORITY, &xHandle );
	if ( xTaskCreate( (pdTASK_CODE)MONITOR_CheckCommand, "MonitorTask", MONITOR_TASK_STACK_SIZE, 0,
				MONITOR_TASK_PRIORITY, &MONITOR_TaskHandle ) != pdPASS)
	{
		Error_Handler();
	}
#else
	MONITOR_CheckCommand();	// infinite loop
#endif
	// End of DEBUG USART
#endif	// #ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	
	
	// ESP8266
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	ESP8266_USART_Rx_Semaphore = NULL;
	ESP8266_USART_Rx_Semaphore = xSemaphoreCreateBinary();
	
	ESP8266_SendMessage_Queue = NULL;
	ESP8266_SendMessage_Queue = xQueueCreate( ESP8266_HOMEAUTMESSAGE_SENDMESSAGE_QUEUE_LENGTH, ESP8266_HOMEAUTMESSAGE_ITEM_SIZE );
	
	ESP8266_ReceivedMessage_Queue = NULL;
	ESP8266_ReceivedMessage_Queue = xQueueCreate( ESP8266_HOMEAUTMESSAGE_RECEIVEMESSAGE_QUEUE_LENGTH, ESP8266_HOMEAUTMESSAGE_ITEM_SIZE );
	

	ESP8266_Init();
	
	TaskHandle_t ESP8266_TaskHandle = NULL;
	if ( xTaskCreate( (pdTASK_CODE)ESP8266_Task, "ESP8266Task", ESP8266_TASK_STACK_SIZE, 0,
				ESP8266_TASK_PRIORITY, &ESP8266_TaskHandle ) != pdPASS)
	{
		Error_Handler();
	}
	
#endif
	
	
#ifdef CONFIG_MODULE_SYSMANAGER_ENABLE
	TaskHandle_t SYSMANAGER_TaskHandle = NULL;
	if ( xTaskCreate( (pdTASK_CODE)SYSMANAGER_Task, "SysManagerTask", SYSMANAGER_TASK_STACK_SIZE, 0,
				SYSMANAGER_TASK_PRIORITY, &SYSMANAGER_TaskHandle ) != pdPASS)
	{
		Error_Handler();
	}
#endif	// #ifdef CONFIG_MODULE_SYSMANAGER_ENABLE
	
	
#ifdef CONFIG_USE_FREERTOS
	// FreeRTOS scheduler
	vTaskStartScheduler();
#endif
	
	
	// Infinite loop - never reached
	while (1)
	{
		Error_Handler();
	}
	
}


/**
 * \brief	Error_Handler
 */
void Error_Handler( void )
{
	LED_BLUE_OFF();
	LED_GREEN_OFF();
	LED_RED_ON();
	
#ifdef CONFIG_USE_FREERTOS
	// End task scheduling
	vTaskEndScheduler();
#endif
	
	while(1)	// infinite loop
	{
		LED_BLUE_OFF();
		LED_GREEN_OFF();
		LED_RED_ON();			
	}
}


/** System Clock Configuration
*/
#ifdef CONFIG_USE_PANEL_NODESMALL
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
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	__PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/*
	// Original... TODO: miért volt itt?
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	*/

	// for adc... TODO: valóban kell az adc-hez? és ha CSAK ez van?
	//HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
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





#if defined(CONFIG_USE_PANEL_CENTERPANEL) || defined(CONFIG_USE_PANEL_DISCOVERY)

void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}

#endif	// #ifdef CONFIG_USE_PANEL_CENTERPANEL






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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
