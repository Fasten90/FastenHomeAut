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
#include "motor.h"
#include "options.h"
#include "include.h"


#ifdef CONFIG_MODULE_MOTOR_ENABLE

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
 *	\brief	Initialize Motor
 */
void MOTOR_Init(void)
{
	MOTOR_DcMotorGpioInit();
	Motor_DcMotorTimerInit(0);
	Motor_ServoTimerInit(0);
}



/**
 * \brief	Motor - Dc motor init
 */
void Motor_DcMotorTimerInit(uint8_t percent)
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
void Motor_ServoTimerInit(int8_t angle)
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
 * \brief	Initialize DC motor GPIO-s
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
void Motor_DcMotor_ChangePercent(TIM_HandleTypeDef *TimHandle, uint32_t timerChannel, uint8_t percent )
{

	/* Timer Output Compare Configuration Structure declaration */
	TIM_OC_InitTypeDef sConfig;


	uint32_t Pulse = PWM_TIMER_PERIOD_VALUE * percent / 100;	// % pecent calculate


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
