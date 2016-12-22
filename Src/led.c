/*
 *		led.c
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		LED functions
 *		Target:			STM32Fx
 *		Version:		v2
 *		Last modified:	2016.11.23
 */

#include "options.h"
#include "include.h"
#include "board.h"
#include "led.h"



#ifdef CONFIG_MODULE_LED_ENABLE

extern void Error_Handler( void );

#ifdef CONFIG_USE_PANEL_NODESMALL
/* Timer handler declaration */
TIM_HandleTypeDef    TimLedBlueHandle;
TIM_HandleTypeDef    TimLedGreenHandle;
#endif


/// LED names
const char * const LED_NameList[] =
{
	// NOTE: Be careful, when change the order and num, syncronize with LED_NUM_MAX define
#ifdef CONFIG_USE_PANEL_DISCOVERY
	"red",
	"blue",
	"green",
#elif CONFIG_USE_PANEL_NUCLEOF401RE
	"green",
#else
#warning "Miss CONFIG_USE_PANEL_.. define in LED names"
#endif
	NULL
};				///< LED names


/// LED types
const char * const LED_TypeNameList[] =
{
	// NOTE: Syncronize with LED_SetType
	"-",
	"on",
	"off",
	"toggle",
	"status"
};


// Function prototypes

static bool LED_SetRedLed(LED_SetType ledSet);
static bool LED_SetBlueLed(LED_SetType ledSet);
static bool LED_SetGreenLed(LED_SetType ledSet);


// Functions


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
	GPIO_InitStructure.Pin = BOARD_LED_GREEN_PIN;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(BOARD_LED_GREEN_PORT, &GPIO_InitStructure);
	
#if LED_NUM_MAX > 1
	GPIO_InitStructure.Pin = BOARD_LED_BLUE_PIN;
	HAL_GPIO_Init(BOARD_LED_BLUE_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = BOARD_LED_RED_PIN;
	HAL_GPIO_Init(BOARD_LED_RED_PORT, &GPIO_InitStructure);
#endif

	LED_RED_OFF();
	LED_BLUE_OFF();
	LED_GREEN_OFF();
	
}



/**
\brief	LED blinking in infinite loop
*/
void LED_Test ( void )
{

	while(1)
	{
		// Set LEDs
		LED_RED_ON();
		LED_BLUE_ON();
		LED_GREEN_ON();

		// Delay
		DelayMs(200);

		// Set LEDs
		LED_RED_OFF();
		LED_BLUE_OFF();
		LED_GREEN_OFF();

		// Delay
		DelayMs(200);
	}
}



/**
 * \brief	Set red LED
 */
static bool LED_SetRedLed(LED_SetType ledSet)
{

	switch (ledSet)
	{
		case LED_SET_ON:
			LED_RED_ON();
			break;
		case LED_SET_OFF:
			LED_RED_OFF();
			break;
		case LED_SET_TOGGLE:
			LED_RED_TOGGLE();
			break;
		case LED_GET_STATUS:
			break;
		case LED_SET_DONTCARE:
		case LED_TYPE_COUNT:
			// Do not use
			break;
		default:
			break;
	}

	return LED_RED_STATUS();
}



/**
 * \brief	Set blue LED
 */
static bool LED_SetBlueLed(LED_SetType ledSet)
{

	switch (ledSet)
	{
		case LED_SET_ON:
			LED_BLUE_ON();
			break;
		case LED_SET_OFF:
			LED_BLUE_OFF();
			break;
		case LED_SET_TOGGLE:
			LED_BLUE_TOGGLE();
			break;
		case LED_GET_STATUS:
			break;
		case LED_SET_DONTCARE:
		case LED_TYPE_COUNT:
			// Do not use
			break;
		default:
			break;
	}

	return LED_BLUE_STATUS();
}



/**
 * \brief	Set green LED
 */
static bool LED_SetGreenLed(LED_SetType ledSet)
{

	switch (ledSet)
	{
		case LED_SET_ON:
			LED_GREEN_ON();
			break;
		case LED_SET_OFF:
			LED_GREEN_OFF();
			break;
		case LED_SET_TOGGLE:
			LED_GREEN_TOGGLE();
			break;
		case LED_GET_STATUS:
			break;
		case LED_SET_DONTCARE:
		case LED_TYPE_COUNT:
			// Do not use
			break;
		default:
			break;
	}

	return LED_GREEN_STATUS();
}



/**
 * \brief	Set LED
 * \param	num		LED number
 * \param	ledSet	Which type (on, off, toggle)
 */
bool LED_SetLed(uint8_t num, LED_SetType ledSet)
{
	bool state = false;

	switch (num)
	{
#ifdef CONFIG_USE_PANEL_DISCOVERY
		case 1:
			state = LED_SetRedLed(ledSet);
			break;
		case 2:
			state = LED_SetBlueLed(ledSet);
			break;
		case 3:
			state = LED_SetGreenLed(ledSet);
			break;
#elif CONFIG_USE_PANEL_NUCLEOF401RE
		case 1:
			state = LED_SetGreenLed(ledSet);
			break;
#endif
		default:
			state = false;
			break;
	}

	return state;

}



/**
 * \brief	Get LED status
 * \return	true, if high
 * 			false, if low
 */
bool LED_GetStatus(uint8_t num)
{
	bool status;

	switch (num)
	{
#ifdef CONFIG_USE_PANEL_DISCOVERY
		case 1:
			status = LED_RED_STATUS();
			break;
		case 2:
			status = LED_BLUE_STATUS();
			break;
		case 3:
			status = LED_GREEN_STATUS();
			break;
#elif CONFIG_USE_PANEL_NUCLEOF401RE
		case 1:
			status = LED_GREEN_STATUS();
			break;
#endif
		default:
			status = false;
			break;
	}

	return status;
}



/**
 * \brief	Get LED type from name
 */
uint8_t LED_GetNumFromName (const char*name)
{
	uint8_t i;
	uint8_t ledNum = 0;

	for (i=0; i < LED_NUM_MAX; i++)
	{
		if (!StrCmp(LED_NameList[i], name))
		{
			// LED num = index+1
			ledNum = i + 1;
			break;
		}
	}

	return ledNum;

}



/**
 * \brief	Get type from string
 */
LED_SetType LED_GetTypeFromString (const char*typeString)
{
	uint8_t i;
	LED_SetType ledType = 0;

	// TODO: Lehet, hogy kevesebbet foglalna nem ciklusként

	for (i=0; i < LED_TYPE_COUNT; i++)
	{
		if (!StrCmp(LED_TypeNameList[i], typeString))
		{
			ledType = i;
			break;
		}
	}

	return ledType;
}



/**
 * \brief	LEDs with timer
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
