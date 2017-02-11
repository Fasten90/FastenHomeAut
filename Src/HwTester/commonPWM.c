/*
 *		commonPWM.c
 *
 *		Created on:		2017. febr. 11.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 11.
 */




/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "options.h"
#include "include.h"
#include "commonPWM.h"


#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
TIM_HandleTypeDef    TimPWM1_Handle;
TIM_HandleTypeDef    TimPWM2_Handle;


/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static uint32_t MotorServo_ConvertAngleToPeriod(int8_t angle);
static void MOTOR_DcMotorGpioInit(void);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	TODO: delete
 */
void CommonPWM_1_2Init(uint32_t percent)
{

	/* Timer Output Compare Configuration Structure declaration */
	TIM_OC_InitTypeDef sConfig;

	/* Counter Prescaler value */
	uint32_t PrescalerValue = 0;
	uint32_t Period = 0;
	uint32_t Pulse = 0;
	//uint32_t Percent = 0;
	uint32_t Percent = percent;


	// it is need? TODO: delete
	//HAL_TIM_PWM_MspInit(NULL);



	/* Compute the prescaler value to have TIM3 counter clock equal to 16000000 Hz */
	//uhPrescalerValue = (uint32_t)(SystemCoreClock / 16000000) - 1;
	PrescalerValue = (uint32_t)(SystemCoreClock / 400000 ) - 1;

	Period = PWM_TIMER_PERIOD_VALUE;
	Pulse = Period * Percent / 100;


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

	// PWM1

	TimPWM1_Handle.Instance = TIMx_PWM1;

	TimPWM1_Handle.Init.Prescaler         = PrescalerValue;
	TimPWM1_Handle.Init.Period            = Period;
	TimPWM1_Handle.Init.ClockDivision     = 0;
	TimPWM1_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimPWM1_Handle.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&TimPWM1_Handle) != HAL_OK)
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
	sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;		// TIM_OCIDLESTATE_RESET
	sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;	// TIM_OCNIDLESTATE_RESET

	/* Set the pulse value for channel 1 */
	sConfig.Pulse = Pulse;
	if (HAL_TIM_PWM_ConfigChannel(&TimPWM1_Handle, &sConfig, TIMER_PWM1_CHANNEL) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}



	// PWM2


	TimPWM2_Handle.Instance = TIMx_PWM2;

	TimPWM2_Handle.Init.Prescaler         = PrescalerValue;
	TimPWM2_Handle.Init.Period            = Period;
	TimPWM2_Handle.Init.ClockDivision     = 0;
	TimPWM2_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimPWM2_Handle.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&TimPWM2_Handle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}


	/*##-2- Configure the PWM channels #########################################*/
	/* Common configuration for all channels */
	sConfig.OCMode       = TIM_OCMODE_PWM1;
	sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_LOW;
	sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
	sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	/* Set the pulse value for channel 1 */
	sConfig.Pulse = Pulse;
	if (HAL_TIM_PWM_ConfigChannel(&TimPWM2_Handle, &sConfig, TIMER_PWM2_CHANNEL) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}


	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	if (HAL_TIM_PWM_Start(&TimPWM1_Handle, TIMER_PWM1_CHANNEL) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}
	/* Start channel 2 */
	if (HAL_TIM_PWM_Start(&TimPWM2_Handle, TIMER_PWM2_CHANNEL) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}
}



void CommonPWM_1Init(uint8_t percent)
{

	/* Timer Output Compare Configuration Structure declaration */
	TIM_OC_InitTypeDef sConfig;

	/* Counter Prescaler value */
	uint32_t PrescalerValue = 0;
	uint32_t Period = 0;
	uint32_t Pulse = 0;


	// Called by HAL
	//HAL_TIM_PWM_MspInit(NULL);



	/* Compute the prescaler value to have TIM3 counter clock equal to 16000000 Hz */
	//uhPrescalerValue = (uint32_t)(SystemCoreClock / 16000000) - 1;
	PrescalerValue = (uint32_t)(SystemCoreClock / 400000 ) - 1;

	Period = PWM_TIMER_PERIOD_VALUE;
	Pulse = Period * percent / 100;


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

	// PWM1

	TimPWM1_Handle.Instance = TIMx_PWM1;

	TimPWM1_Handle.Init.Prescaler         = PrescalerValue;
	TimPWM1_Handle.Init.Period            = Period;
	TimPWM1_Handle.Init.ClockDivision     = 0;
	TimPWM1_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimPWM1_Handle.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&TimPWM1_Handle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}


	/*##-2- Configure the PWM channels #########################################*/
	/* Common configuration for all channels */
	sConfig.OCMode       = TIM_OCMODE_PWM1;
	sConfig.OCPolarity   = TIM_OCNPOLARITY_LOW;		// TIM_OCPOLARITY_HIGH
	sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;		// TIM_OCIDLESTATE_RESET
	//sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;		// TIM_OCNPOLARITY_HIGH
	//sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;	// TIM_OCNIDLESTATE_RESET

	/* Set the pulse value for channel 1 */
	sConfig.Pulse = Pulse;
	if (HAL_TIM_PWM_ConfigChannel(&TimPWM1_Handle, &sConfig, TIMER_PWM1_CHANNEL) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}


	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	if (HAL_TIM_PWM_Start(&TimPWM1_Handle, TIMER_PWM1_CHANNEL) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}

}



/**
 * \brief	Init for Servo motor
 */
void CommonPWM_2Init(int8_t angle)
{

	/* Timer Output Compare Configuration Structure declaration */
	TIM_OC_InitTypeDef sConfig;

	/* Counter Prescaler value */
	uint32_t PrescalerValue = 0;
	uint32_t Period = 0;
	uint32_t Pulse = 0;


	/* Compute the prescaler value -> 10000 value / sec */
	PrescalerValue = (uint32_t)(SystemCoreClock / PWM_MOTOR_SERVO_TIMER_PRESCALER ) - 1;

	// ms * 10 (10.000 = 1000 ms = 1sec
	Period = PWM_MOTOR_SERVO_TIMER_PERIOD_VALUE;

	Pulse = MotorServo_ConvertAngleToPeriod(angle);


	/*##-1- Configure the TIM peripheral #######################################*/


	// PWM2
	TimPWM2_Handle.Instance = TIMx_PWM2;

	TimPWM2_Handle.Init.Prescaler         = PrescalerValue;
	TimPWM2_Handle.Init.Period            = Period;
	TimPWM2_Handle.Init.ClockDivision     = 0;
	TimPWM2_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimPWM2_Handle.Init.RepetitionCounter = 0;

	// One pulse init
	if (HAL_TIM_PWM_Init(&TimPWM2_Handle) != HAL_OK)
	//if (HAL_TIM_OnePulse_Init(&TimPWM2_Handle, TIM_OPMODE_SINGLE) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}



	/*##-2- Configure the PWM channels #########################################*/
	/* Common configuration for all channels */
	sConfig.OCMode       = TIM_OCMODE_PWM1;
	sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_LOW;
	//sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
	//sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	/* Set the pulse value for channel 1 */
	sConfig.Pulse = Pulse;
	if (HAL_TIM_PWM_ConfigChannel(&TimPWM2_Handle, &sConfig, TIMER_PWM2_CHANNEL) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}


	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 2 */
	if (HAL_TIM_PWM_Start(&TimPWM2_Handle, TIMER_PWM2_CHANNEL) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}
}



/**
 * \brief	Convert angle to ms*10 timer value
 */
static uint32_t MotorServo_ConvertAngleToPeriod(int8_t angle)
{

#define MOTOR_SERVO_MIN_ANGLE		(-90)
#define MOTOR_SERVO_OFFSET_ANGLE	(90)
#define MOTOR_SERVO_MAX_ANGLE		(+90)
#define MOTOR_SERVO_ANGLE_INTERVAL	(MOTOR_SERVO_MAX_ANGLE-MOTOR_SERVO_MIN_ANGLE)
#define MOTOR_SERVO_MIN_TIMER_VALUE	(10)		// ms*10
#define MOTOR_SERVO_MAX_TIMER_VALUE (20)		// ms*10
#define MOTOR_SERVO_TIMER_INTERVAL	(MOTOR_SERVO_MAX_TIMER_VALUE-MOTOR_SERVO_MIN_TIMER_VALUE)
#define MOTOR_SERVO_TIME_FULL		(10000)		// ms*10

	// Convert angle to % and timer value
	// time is calculated in ms*10
	float motorServoValue = ((float)(angle+MOTOR_SERVO_OFFSET_ANGLE))/MOTOR_SERVO_ANGLE_INTERVAL*MOTOR_SERVO_TIMER_INTERVAL;

	uint32_t motorServorTimerValue = motorServoValue + MOTOR_SERVO_MIN_TIMER_VALUE;

	return motorServorTimerValue;
}



/**
 * \brief	HAK driver function: PWM MspInit
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	(void)htim;


	GPIO_InitTypeDef GPIO_InitStruct;



	//##-1- Enable peripherals and GPIO Clocks #################################
	// TIMx Peripheral clock enable
	TIMER_CLK_ENABLES();

	// Enable GPIO Channels Clock
	TIMER_PWM_GPIO_CLK_ENABLES();


	// PWM1
	//Configure GPIO pin
	GPIO_InitStruct.Pin = BOARD_PWM1_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = TIMx_PWM1_GPIO_AF;
	HAL_GPIO_Init(BOARD_PWM1_PORT, &GPIO_InitStruct);



	// PWM2 - Servo motor
	//Configure GPIO pin
	GPIO_InitStruct.Pin = BOARD_PWM2_PIN;
	GPIO_InitStruct.Alternate = TIMx_PWM2_GPIO_AF;;
	HAL_GPIO_Init(BOARD_PWM2_PORT, &GPIO_InitStruct);

}



/**
 *
 */
void MOTOR_Init(void)
{
	MOTOR_DcMotorGpioInit();
	CommonPWM_1Init(0);
	CommonPWM_2Init(0);
}



/**
 * \brief	Initialize Dc motor gpios
 */
static void MOTOR_DcMotorGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	MOTOR_DC_DIRECTIONS_CLK_ENABLES();

	GPIO_InitStruct.Pin = MOTOR_DC_DIR1_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

	HAL_GPIO_Init(MOTOR_DC_DIR1_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = MOTOR_DC_DIR2_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

	HAL_GPIO_Init(MOTOR_DC_DIR2_PORT, &GPIO_InitStruct);


	// Forward:
	HAL_GPIO_WritePin(MOTOR_DC_DIR1_PORT, MOTOR_DC_DIR1_PIN, SET);
	HAL_GPIO_WritePin(MOTOR_DC_DIR2_PORT, MOTOR_DC_DIR2_PIN, RESET);

}


/**
 * \brief	TODO
 */
void CommonPWM_ChangePercent(TIM_HandleTypeDef *TimHandle, uint32_t timerChannel, uint8_t percent )
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


	// Need start...
	if (HAL_TIM_PWM_Start(TimHandle, timerChannel) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}

}


#endif
