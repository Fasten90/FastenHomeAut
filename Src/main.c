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


#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
	//HAL_ADC_MspInit(&AdcHandle);	// Called by HAL driver
	ADC_Init();
	// Test, blocking
	//ADC_Test();
#endif


#ifdef CONFIG_MODULE_STL_SELFTEST_ENABLE
	STL_InitRunTimeChecks();
#endif


// DEBUG USART
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
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
#endif


#ifdef CONFIG_MODULE_MONITOR_ENABLE
	// Monitor initialize
	MONITOR_Init();
#endif


#ifdef CONFIG_MODULE_MONITOR_ENABLE
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
#endif	// #ifdef CONFIG_MODULE_MONITOR_ENABLE


#ifdef CONFIG_MODULE_ESP8266_ENABLE
	// ESP8266
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



#ifdef CONFIG_USE_PANEL_NODESMALL
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



#if defined(CONFIG_USE_PANEL_CENTERPANEL) || defined(CONFIG_USE_PANEL_DISCOVERY)
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

/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	// TODO: What is this?
}
/*-----------------------------------------------------------*/

#if 0
static void prvSetupNestedFPUInterruptsTest( void )
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the TIM2 interrupt in the NVIC.  The timer itself is not used,
	just its interrupt vector to force nesting from software.  TIM2 must have
	a lower priority than TIM3, and both must have priorities above
	configMAX_SYSCALL_INTERRUPT_PRIORITY. */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY - 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	/* Enable the TIM3 interrupt in the NVIC.  The timer itself is not used,
	just its interrupt vector to force nesting from software.  TIM2 must have
	a lower priority than TIM3, and both must have priorities above
	configMAX_SYSCALL_INTERRUPT_PRIORITY. */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY - 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
}
/*-----------------------------------------------------------*/

void TIM3_IRQHandler( void )
{
	/* Just to verify that the interrupt nesting behaves as expected, increment
	ulFPUInterruptNesting on entry, and decrement it on exit. */
	ulFPUInterruptNesting++;

	/* This is the highest priority interrupt in the chain of forced nesting
	interrupts, so latch the maximum value reached by ulFPUInterruptNesting.
	This is done purely to allow verification that the nesting depth reaches
	that intended. */
	if( ulFPUInterruptNesting > ulMaxFPUInterruptNesting )
	{
		ulMaxFPUInterruptNesting = ulFPUInterruptNesting;
	}

	/* Fill the FPU registers with 99 to overwrite the values written by
	TIM2_IRQHandler(). */
	vRegTestClearFlopRegistersToParameterValue( 99UL );

	ulFPUInterruptNesting--;
}
/*-----------------------------------------------------------*/

void TIM2_IRQHandler( void )
{
	/* Just to verify that the interrupt nesting behaves as expected, increment
	ulFPUInterruptNesting on entry, and decrement it on exit. */
	ulFPUInterruptNesting++;

	/* Fill the FPU registers with 1. */
	vRegTestClearFlopRegistersToParameterValue( 1UL );

	/* Trigger a timer 3 interrupt, which will fill the registers with a
	different value. */
	NVIC_SetPendingIRQ( TIM3_IRQn );

	/* Ensure that, after returning from the nesting interrupt, all the FPU
	registers contain the value to which they were set by this interrupt
	function. */
	configASSERT( ulRegTestCheckFlopRegistersContainParameterValue( 1UL ) );

	ulFPUInterruptNesting--;
}
/*-----------------------------------------------------------*/

static void prvOptionallyCreateComprehensveTestApplication( void )
{
	#if ( mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY == 0 )
	{
	TimerHandle_t xCheckTimer = NULL;

		/* Configure the interrupts used to test FPU registers being used from
		nested interrupts. */
		prvSetupNestedFPUInterruptsTest();

		/* Start all the other standard demo/test tasks. */
		vStartIntegerMathTasks( tskIDLE_PRIORITY );
		vStartDynamicPriorityTasks();
		vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );
		vCreateBlockTimeTasks();
		vStartCountingSemaphoreTasks();
		vStartGenericQueueTasks( tskIDLE_PRIORITY );
		vStartRecursiveMutexTasks();
		vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
		vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );

		/* Most importantly, start the tasks that use the FPU. */
		vStartMathTasks( mainFLOP_TASK_PRIORITY );

		/* Create the register check tasks, as described at the top of this
		file */
		xTaskCreate( vRegTest1Task, "Reg1", configMINIMAL_STACK_SIZE, ( void * ) NULL, tskIDLE_PRIORITY, NULL );
		xTaskCreate( vRegTest2Task, "Reg2", configMINIMAL_STACK_SIZE, ( void * ) NULL, tskIDLE_PRIORITY, NULL );

		/* Create the semaphore that is used to demonstrate a task being
		synchronised with an interrupt. */
		vSemaphoreCreateBinary( xTestSemaphore );

		/* Create the task that is unblocked by the demonstration interrupt. */
		xTaskCreate( prvButtonTestTask, "BtnTest", configMINIMAL_STACK_SIZE, ( void * ) NULL, tskIDLE_PRIORITY, NULL );

		/* Create the software timer that performs the 'check' functionality,
		as described at the top of this file. */
		xCheckTimer = xTimerCreate( "CheckTimer",					/* A text name, purely to help debugging. */
									( mainCHECK_TIMER_PERIOD_MS ),	/* The timer period, in this case 3000ms (3s). */
									pdTRUE,							/* This is an auto-reload timer, so xAutoReload is set to pdTRUE. */
									( void * ) 0,					/* The ID is not used, so can be set to anything. */
									prvCheckTimerCallback			/* The callback function that inspects the status of all the other tasks. */
								  );

		if( xCheckTimer != NULL )
		{
			xTimerStart( xCheckTimer, mainDONT_BLOCK );
		}

		/* This task has to be created last as it keeps account of the number of
		tasks it expects to see running. */
		vCreateSuicidalTasks( mainCREATOR_TASK_PRIORITY );
	}
	#else /* mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY */
	{
		/* Just to prevent compiler warnings when the configuration options are
		set such that these static functions are not used. */
		( void ) vRegTest1Task;
		( void ) vRegTest2Task;
		( void ) prvCheckTimerCallback;
		( void ) prvSetupNestedFPUInterruptsTest;
	}
	#endif /* mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY */
}
/*-----------------------------------------------------------*/

void EXTI9_5_IRQHandler(void)
{
	long lHigherPriorityTaskWoken = pdFALSE;

	/* Only line 6 is enabled, so there is no need to test which line generated
	the interrupt. */
	EXTI_ClearITPendingBit( EXTI_Line6 );

	/* This interrupt does nothing more than demonstrate how to synchronise a
	task with an interrupt.  First the handler releases a semaphore.
	lHigherPriorityTaskWoken has been initialised to zero. */
	xSemaphoreGiveFromISR( xTestSemaphore, &lHigherPriorityTaskWoken );

	/* If there was a task that was blocked on the semaphore, and giving the
	semaphore caused the task to unblock, and the unblocked task has a priority
	higher than the currently executing task (the task that this interrupt
	interrupted), then lHigherPriorityTaskWoken will have been set to pdTRUE.
	Passing pdTRUE into the following macro call will cause this interrupt to
	return directly to the unblocked, higher priority, task. */
	portEND_SWITCHING_ISR( lHigherPriorityTaskWoken );
}
#endif
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
