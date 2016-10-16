

#include "board.h"
#include "include.h"
#include "led.h"



#ifdef CONFIG_MODULE_LED_ENABLE

extern void Error_Handler( void );

#ifdef CONFIG_USE_PANEL_NODESMALL
/* Timer handler declaration */
TIM_HandleTypeDef    TimLedBlueHandle;
TIM_HandleTypeDef    TimLedGreenHandle;
#endif





/**
\brief	LED GPIO initialization (without TIMER)
*/
void LED_Init( void )
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

	// GPIO Periph clock enable
	LED_PORT_CLK_ENABLES();
	
	// Configure pin output pushpull mode
	//GPIO_InitStructure.Alternate = GPIO_AF;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;	
	GPIO_InitStructure.Pin = BOARD_LED_RED_PIN;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(BOARD_LED_RED_PORT, &GPIO_InitStructure);
	

	GPIO_InitStructure.Pin = BOARD_LED_BLUE_PIN;
	HAL_GPIO_Init(BOARD_LED_BLUE_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = BOARD_LED_GREEN_PIN;
	HAL_GPIO_Init(BOARD_LED_GREEN_PORT, &GPIO_InitStructure);
	
	
	//GPIO_SetBits();
	// GPIO_ResetBits ( BUZZER_GPIO_PORT, BUZZER_PIN );
	//GPIO_ResetBits(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN);
	//GPIO_ResetBits(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN);
	//GPIO_ResetBits(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN);


	//HAL_GPIO_WritePin(LED_DESIGN_SPI_OE_GPIO_PORT,LED_DESIGN_SPI_OE_GPIO_PIN,GPIO_PIN_RESET)
	LED_RED_OFF();
	LED_BLUE_OFF();
	LED_GREEN_OFF();
	
}



/**
\brief	LED blinkg in infinite loop
*/
void LED_Run ( void )
{
	
	uint16_t delayMs = 200;
	
	while(1)
	{
		
		LED_RED_ON();
		LED_BLUE_ON();
		LED_GREEN_ON();
		
			
		// Delay
		#ifdef CONFIG_USE_FREERTOS
		vTaskDelay(delayMs);
		#else
		HAL_Delay(delayMs);
		#endif
		
		
		LED_RED_OFF();
		LED_BLUE_OFF();
		LED_GREEN_OFF();

		// Delay
		#ifdef CONFIG_USE_FREERTOS
		vTaskDelay(delayMs);
		#else
		HAL_Delay(delayMs);
		#endif	
			
	}


}



/**
\brief	LEDs with timer
*/
#ifdef CONFIG_USE_PANEL_NODESMALL
void LED_InitTimers( void )
{
	
	/* Timer handler declaration */
	//TIM_HandleTypeDef    TimLedBlueHandle;
	//TIM_HandleTypeDef    TimLedGreenHandle;
	// Need to be global variable
	
	/* Timer Output Compare Configuration Structure declaration */
	TIM_OC_InitTypeDef sConfig;

	/* Counter Prescaler value */
	uint32_t PrescalerValue = 0;
	uint32_t Period = 0;
	uint32_t Pulse = 0;
	
	HAL_TIM_PWM_MspInit(NULL);
		

	
	/* Compute the prescaler value to have TIM3 counter clock equal to 16000000 Hz */
	//uhPrescalerValue = (uint32_t)(SystemCoreClock / 16000000) - 1;
	PrescalerValue = (uint32_t)(SystemCoreClock / 400000 ) - 1;
	
	Period = LED_TIMER_PERIOD_VALUE;	
	Pulse = Period * 12.5 / 100;	// = 12.5%
	
	
	/*##-1- Configure the TIM peripheral #######################################*/
	/* -----------------------------------------------------------------------
	TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles.

	In this example TIM3 input clock (TIM3CLK) is set to APB1 clock (PCLK1),
	since APB1 prescaler is equal to 1.
	  TIM3CLK = PCLK1
	  PCLK1 = HCLK
	  => TIM3CLK = HCLK = SystemCoreClock

	To get TIM3 counter clock at 16 MHz, the prescaler is computed as follows:
	   Prescaler = (TIM3CLK / TIM3 counter clock) - 1
	   Prescaler = ((SystemCoreClock) /16 MHz) - 1

	To get TIM3 output clock at 24 KHz, the period (ARR)) is computed as follows:
	   ARR = (TIM3 counter clock / TIM3 output clock) - 1
		   = 665

	TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR + 1)* 100 = 50%
	TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR + 1)* 100 = 37.5%
	TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR + 1)* 100 = 25%
	TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR + 1)* 100 = 12.5%

	Note:
	 SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f0xx.c file.
	 Each time the core clock (HCLK) changes, user had to update SystemCoreClock
	 variable value. Otherwise, any configuration based on this variable will be incorrect.
	 This variable is updated in three ways:
	  1) by calling CMSIS function SystemCoreClockUpdate()
	  2) by calling HAL API function HAL_RCC_GetSysClockFreq()
	  3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
	----------------------------------------------------------------------- */

	/* Initialize TIMx peripheral as follows:
	   + Prescaler = (SystemCoreClock / 16000000) - 1
	   + Period = (666 - 1)
	   + ClockDivision = 0
	   + Counter direction = Up
	*/
	
	// BLUE LED
	
	TimLedBlueHandle.Instance = TIMx_BLUE;

	TimLedBlueHandle.Init.Prescaler         = PrescalerValue;
	TimLedBlueHandle.Init.Period            = Period;
	TimLedBlueHandle.Init.ClockDivision     = 0;
	TimLedBlueHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimLedBlueHandle.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&TimLedBlueHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}


	/*##-2- Configure the PWM channels #########################################*/
	/* Common configuration for all channels */
	sConfig.OCMode       = TIM_OCMODE_PWM1;
	sConfig.OCPolarity   = TIM_OCPOLARITY_LOW;		// TIM_OCPOLARITY_HIGH
	sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_LOW;		// TIM_OCNPOLARITY_HIGH
	sConfig.OCIdleState  = TIM_OCIDLESTATE_SET;		// TIM_OCIDLESTATE_RESET
	sConfig.OCNIdleState = TIM_OCNIDLESTATE_SET;	// TIM_OCNIDLESTATE_RESET	

	/* Set the pulse value for channel 1 */
	sConfig.Pulse = Pulse;
	if (HAL_TIM_PWM_ConfigChannel(&TimLedBlueHandle, &sConfig, LED_BLUE_TIMER_CHANNEL) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}
	
	

	// GREEN LED
	
		
	TimLedGreenHandle.Instance = TIMx_GREEN;

	TimLedGreenHandle.Init.Prescaler         = PrescalerValue;
	TimLedGreenHandle.Init.Period            = Period;
	TimLedGreenHandle.Init.ClockDivision     = 0;
	TimLedGreenHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimLedGreenHandle.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&TimLedGreenHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}


	/*##-2- Configure the PWM channels #########################################*/
	/* Common configuration for all channels */
	sConfig.OCMode       = TIM_OCMODE_PWM1;
	sConfig.OCPolarity   = TIM_OCPOLARITY_LOW;
	sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_LOW;
	sConfig.OCIdleState  = TIM_OCIDLESTATE_SET;
	sConfig.OCNIdleState = TIM_OCNIDLESTATE_SET;

	/* Set the pulse value for channel 1 */
	sConfig.Pulse = Pulse;
	if (HAL_TIM_PWM_ConfigChannel(&TimLedGreenHandle, &sConfig, LED_GREEN_TIMER_CHANNEL) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}
	
	
	
	
	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	if (HAL_TIM_PWM_Start(&TimLedBlueHandle, LED_BLUE_TIMER_CHANNEL) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}
	/* Start channel 2 */
	if (HAL_TIM_PWM_Start(&TimLedGreenHandle, LED_GREEN_TIMER_CHANNEL) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}


}



void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
		
	
	//##-1- Enable peripherals and GPIO Clocks #################################
	// TIMx Peripheral clock enable
	//TIMx_BLUE_CLK_ENABLE();
	//TIMx_GREEN_CLK_ENABLE();
	TIMER_CLK_ENABLES();

	// Enable GPIO Channels Clock 
	LED_PORT_CLK_ENABLES();

	
	// BLUE
	//Configure GPIO pin
	GPIO_InitStruct.Pin = BOARD_LED_BLUE_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = TIMx_LED_BLUE_GPIO_AF;
	HAL_GPIO_Init(BOARD_LED_BLUE_PORT, &GPIO_InitStruct);


	// GREEN
	//Configure GPIO pin
	GPIO_InitStruct.Pin = BOARD_LED_GREEN_PIN;
	GPIO_InitStruct.Alternate = TIMx_LED_GREEN_GPIO_AF;
	HAL_GPIO_Init(BOARD_LED_GREEN_PORT, &GPIO_InitStruct);


}



void LED_PWM_ChangePercent(TIM_HandleTypeDef *TimHandle, uint32_t timerChannel, uint8_t percent )
{
	
	/* Timer Output Compare Configuration Structure declaration */
	TIM_OC_InitTypeDef sConfig;
	
	
	uint32_t Pulse = LED_TIMER_PERIOD_VALUE * percent / 100;	// % számítás
	
	
	/*##-2- Configure the PWM channels #########################################*/
	/* Common configuration for all channels */
	sConfig.OCMode       = TIM_OCMODE_PWM1;
	sConfig.OCPolarity   = TIM_OCPOLARITY_LOW;
	sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_LOW;
	sConfig.OCIdleState  = TIM_OCIDLESTATE_SET;
	sConfig.OCNIdleState = TIM_OCNIDLESTATE_SET;

	/* Set the pulse value for channel 1 */
	sConfig.Pulse = Pulse;
	
	// Config ...
	if (HAL_TIM_PWM_ConfigChannel(TimHandle, &sConfig, timerChannel) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}
	
	
	// Need start... Yes, need...
	if (HAL_TIM_PWM_Start(TimHandle, timerChannel) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}
	
}
#endif	// #ifdef CONFIG_USE_PANEL_NODESMALL

#endif	// #ifdef CONFIG_MODULE_LED_ENABLE


