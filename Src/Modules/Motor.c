/*
 *		Motor.c
 *		Created on:		2017-02-11
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Motor control
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-11
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"
#include "board.h"
#include "MemHandler.h"
#include "Motor.h"


#ifdef CONFIG_MODULE_MOTOR_ENABLE


// MOTOR - Status machine values

#define MOTOR_STATE_MACHINE_SPEED_CHANGE_LIMIT		(10)
#define MOTOR_SERVO_CHANGE_LIMIT					(10)

#define MOTOR_SLIDE_DCMOTOR_LIMIT_MAX				(50)
#define MOTOR_SLIDE_DCMOTOR_LIMIT_MIN				(0)



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

TIM_HandleTypeDef    TimPWMDcMotor_Handle;	// DC motor

// \note	If we have these two pwm on one timer, need to Handle, with equal content. Cannot merge.

#define MOTOR_MOTORS_PWM_ON_ONE_TIMER

/*#ifdef MOTOR_MOTORS_PWM_ON_ONE_TIMER
#define TimPWMServo_Handle TimPWMDcMotor_Handle
#else*/
TIM_HandleTypeDef    TimPWMServo_Handle;	// Servo motor
//#endif



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

static MotorState_t ActualState = { 0 };
static MotorState_t ControlState = { 0 };

bool MotorTestSlide_Enabled = false;
static uint8_t MotorTestSlide_AngleDir = 0;
static uint8_t MotorTestSlide_DcDir = 0;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static uint32_t Motor_ServoConvertAngleToPeriod(int8_t angle);
static void Motor_DcMotorGpioInit(void);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 *	\brief	Initialize Motor
 */
void Motor_Init(void)
{
	// Init GPIOs
	Motor_DcMotorGpioInit();

	// Init DC motor
	Motor_DcMotorTimerInit(0);

#if defined(MOTOR_MOTORS_PWM_ON_ONE_TIMER)
	memcpy(&TimPWMServo_Handle, &TimPWMDcMotor_Handle, sizeof(TimPWMServo_Handle));
	// Set Servo PWM value, because DC and Servo on one Timer
	Motor_ServoChangeAngle(0);
#else
	// Init servo motor
	Motor_ServoTimerInit(0);
#endif
}



/**
 * \brief	Initialize DC motor GPIO-s
 */
static void Motor_DcMotorGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	MOTOR_DCMOTOR_DIRS_CLK_ENABLES();

	GPIO_InitStruct.Pin = MOTOR_DCMOTOR_DIR1_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

	HAL_GPIO_Init(MOTOR_DCMOTOR_DIR1_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = MOTOR_DCMOTOR_DIR2_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

	HAL_GPIO_Init(MOTOR_DCMOTOR_DIR2_GPIO_PORT, &GPIO_InitStruct);


	// Stop direction
	Motor_DcMotorSeDirection(MotorDir_Stop);
}



/**
 * \brief	Motor - Dc motor init
 */
void Motor_DcMotorTimerInit(uint8_t percent)
{

	/* Counter Prescaler value */
	uint32_t PrescalerValue = 0;
	uint32_t Period = 0;

	// Called by HAL
	//HAL_TIM_PWM_MspInit(NULL);


	/* Compute the prescaler value to have TIM3 counter clock equal to 16000000 Hz */
	//uhPrescalerValue = (uint32_t)(SystemCoreClock / 16000000) - 1;
	//PrescalerValue = (uint32_t)(SystemCoreClock / 400000 ) - 1;
	PrescalerValue = (uint32_t)(SystemCoreClock / MOTOR_DCMOTOR_PWM_TIMER_PRESCALER ) - 1;

	Period = MOTOR_DCMOTOR_PWM_TIMER_PERIOD_VALUE;


	/*##-1- Configure the TIM peripheral #######################################*/

	// PWM1

	TimPWMDcMotor_Handle.Instance = MOTOR_DCMOTOR_PWM_TIMx;

	TimPWMDcMotor_Handle.Init.Prescaler         = PrescalerValue;
	TimPWMDcMotor_Handle.Init.Period            = Period;
	TimPWMDcMotor_Handle.Init.ClockDivision     = 0;
	TimPWMDcMotor_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimPWMDcMotor_Handle.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&TimPWMDcMotor_Handle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	// Set DcMotor PWM value
	Motor_DcMotorChangePercent(percent);
}



/**
 * \brief	Set DC motor PWM percent
 */
void Motor_DcMotorChangePercent(uint8_t percent)
{
	/* Timer Output Compare Configuration Structure declaration */
	TIM_OC_InitTypeDef sConfig;


	uint32_t Pulse = MOTOR_DCMOTOR_PWM_TIMER_PERIOD_VALUE * percent / 100;	// % percent calculate


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


	HAL_TIM_PWM_Stop(&TimPWMDcMotor_Handle, MOTOR_DCMOTOR_PWM_TIMER_CHANNEL);


	// Config ...
	if (HAL_TIM_PWM_ConfigChannel(&TimPWMDcMotor_Handle, &sConfig, MOTOR_DCMOTOR_PWM_TIMER_CHANNEL) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}


	// Need start...
	if (HAL_TIM_PWM_Start(&TimPWMDcMotor_Handle, MOTOR_DCMOTOR_PWM_TIMER_CHANNEL) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}
}



/**
 * \brief	Set DC motor direction
 */
void Motor_DcMotorSeDirection(MotorDir_t dir)
{
	switch (dir)
	{
		case MotorDir_Forward:
			HAL_GPIO_WritePin(MOTOR_DCMOTOR_DIR1_GPIO_PORT, MOTOR_DCMOTOR_DIR1_GPIO_PIN, SET);
			HAL_GPIO_WritePin(MOTOR_DCMOTOR_DIR2_GPIO_PORT, MOTOR_DCMOTOR_DIR2_GPIO_PIN, RESET);
			break;

		case MotorDir_Backward:
			HAL_GPIO_WritePin(MOTOR_DCMOTOR_DIR1_GPIO_PORT, MOTOR_DCMOTOR_DIR1_GPIO_PIN, RESET);
			HAL_GPIO_WritePin(MOTOR_DCMOTOR_DIR2_GPIO_PORT, MOTOR_DCMOTOR_DIR2_GPIO_PIN, SET);
			break;

		case MotorDir_Unknown:
		case MotorDir_Stop:
		default:
			HAL_GPIO_WritePin(MOTOR_DCMOTOR_DIR1_GPIO_PORT, MOTOR_DCMOTOR_DIR1_GPIO_PIN, RESET);
			HAL_GPIO_WritePin(MOTOR_DCMOTOR_DIR2_GPIO_PORT, MOTOR_DCMOTOR_DIR2_GPIO_PIN, RESET);
			break;
	}
}



#ifndef MOTOR_MOTORS_PWM_ON_ONE_TIMER
/**
 * \brief	Init for Servo motor
 */
void Motor_ServoTimerInit(int8_t angle)
{
	/* Counter Prescaler value */
	uint32_t PrescalerValue = 0;
	uint32_t Period = 0;

	/* Compute the prescaler value -> 10000 value / sec */
	PrescalerValue = (uint32_t)(SystemCoreClock / MOTOR_SERVO_PWM_TIMER_PRESCALER ) - 1;

	// ms * 100 (100.000 = 1000 ms = 1sec
	Period = MOTOR_SERVO_PWM_TIMER_PERIOD_VALUE;


	/*##-1- Configure the TIM peripheral #######################################*/

	// PWM2
	TimPWMServo_Handle.Instance = MOTOR_SERVOMOTOR_PWM_TIMx;

	TimPWMServo_Handle.Init.Prescaler         = PrescalerValue;
	TimPWMServo_Handle.Init.Period            = Period;
	TimPWMServo_Handle.Init.ClockDivision     = 0;
	TimPWMServo_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimPWMServo_Handle.Init.RepetitionCounter = 0;

	// One pulse init
	if (HAL_TIM_PWM_Init(&TimPWMServo_Handle) != HAL_OK)
	//if (HAL_TIM_OnePulse_Init(&TimPWMServo_Handle, TIM_OPMODE_SINGLE) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	// Set PWM value
	Motor_ServoChangeAngle(angle);
}
#endif



/**
 * \brief	Change angle for Servo motor
 */
void Motor_ServoChangeAngle(int8_t angle)
{

	/* Timer Output Compare Configuration Structure declaration */
	TIM_OC_InitTypeDef sConfig;

	uint32_t Pulse = Motor_ServoConvertAngleToPeriod(angle);


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

	HAL_TIM_PWM_Stop(&TimPWMServo_Handle, MOTOR_SERVOMOTOR_PWM_TIMER_CHANNEL);

	if (HAL_TIM_PWM_ConfigChannel(&TimPWMServo_Handle, &sConfig, MOTOR_SERVOMOTOR_PWM_TIMER_CHANNEL) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}


	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 2 */
	if (HAL_TIM_PWM_Start(&TimPWMServo_Handle, MOTOR_SERVOMOTOR_PWM_TIMER_CHANNEL) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}
}




/**
 * \brief	Convert angle to ms*10 timer value
 */
static uint32_t Motor_ServoConvertAngleToPeriod(int8_t angle)
{

	// Check values
	if (angle > MOTOR_SERVO_MECHANICAL_MAX_ANGLE)
	{
		angle = MOTOR_SERVO_MECHANICAL_MAX_ANGLE;
	}
	else if (angle < MOTOR_SERVO_MECHANICAL_MIN_ANGLE)
	{
		angle = MOTOR_SERVO_MECHANICAL_MIN_ANGLE;
	}


	// Convert angle to % and timer value
	// time is calculated in ms*10
	float motorServoValue = ((float)(angle+MOTOR_SERVO_OFFSET_ANGLE))/MOTOR_SERVO_ANGLE_INTERVAL*MOTOR_SERVO_TIMER_INTERVAL;

	uint32_t motorServorTimerValue = motorServoValue + MOTOR_SERVO_MIN_TIMER_VALUE;

	return motorServorTimerValue;
}



/**
 * \brief	HAL driver function: PWM MspInit
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	(void)htim;


	GPIO_InitTypeDef GPIO_InitStruct;


	//##-1- Enable peripherals and GPIO Clocks #################################
	// TIMx Peripheral clock enable
	MOTOR_MOTORS_PWM_TIMER_CLK_ENABLES();

	// Enable GPIO Channels Clock
	MOTOR_MOTORS_PWM_GPIO_CLK_ENABLES();


	// PWM1
	//Configure GPIO pin
	GPIO_InitStruct.Pin = MOTOR_DCMOTOR_PWM_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = MOTOR_DCMOTOR_PWM_TIMx_GPIO_AF;
	HAL_GPIO_Init(MOTOR_DCMOTOR_PWM_GPIO_PORT, &GPIO_InitStruct);


	// PWM2 - Servo motor
	//Configure GPIO pin
	GPIO_InitStruct.Pin = MOTOR_SERVOMOTOR_PWM_GPIO_PIN;
	GPIO_InitStruct.Alternate = MOTOR_SERVOMOTOR_PWM_TIMx_GPIO_AF;;
	HAL_GPIO_Init(MOTOR_SERVOMOTOR_PWM_GPIO_PORT, &GPIO_InitStruct);


	// TODO: Added for Error handling... need it?
#if UNUSED
	HAL_NVIC_SetPriority(TIM3_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
#endif

}



/**
 * \brief	Motor state machine
 * \note	Call this "task" periodical and often!
 */
void Motor_StateMachine(void)
{

	if (MotorTestSlide_Enabled)
	{
		// Slide - Angle
		if (MotorTestSlide_AngleDir)
		{
			if (ControlState.angle <= MOTOR_SERVO_MECHANICAL_MAX_ANGLE)
			{
				ControlState.angle += MOTOR_SERVO_CHANGE_LIMIT;
			}
			else
			{
				MotorTestSlide_AngleDir = 0;
			}
		}
		else
		{
			if (ControlState.angle >= MOTOR_SERVO_MECHANICAL_MIN_ANGLE)
			{
				ControlState.angle -= MOTOR_SERVO_CHANGE_LIMIT;
			}
			else
			{
				MotorTestSlide_AngleDir = 1;
			}
		}

		// Slide motor
		if (MotorTestSlide_DcDir)
		{
			if (ControlState.dcPercent <= MOTOR_SLIDE_DCMOTOR_LIMIT_MAX)
			{
				ControlState.dcPercent += MOTOR_STATE_MACHINE_SPEED_CHANGE_LIMIT;
			}
			else
			{
				MotorTestSlide_DcDir = 0;
			}
		}
		else
		{
			if (ControlState.dcPercent > (MOTOR_SLIDE_DCMOTOR_LIMIT_MIN + MOTOR_STATE_MACHINE_SPEED_CHANGE_LIMIT))
			{
				ControlState.dcPercent -= MOTOR_STATE_MACHINE_SPEED_CHANGE_LIMIT;
			}
			else
			{
				MotorTestSlide_DcDir = 1;
			}
		}

		// Check actual direction
		if (ControlState.dcPercent > 0)
		{
			ControlState.dir = MotorDir_Forward;
		}
		else
		{
			// 0
			ControlState.dir = MotorDir_Stop;
		}
	}
	// End of slide


	// DC motor control
	if (ActualState.dir != ControlState.dir)
	{
		// Handle change direction
		// TODO: Too fast stop, do slower?
		Motor_DcMotorChangePercent(0);
		ActualState.dir = ControlState.dir;
		Motor_DcMotorSeDirection(ActualState.dir);
		ActualState.dcPercent = 0;
	}
	else
	{
		// Not need change direction
		if (ActualState.dcPercent != ControlState.dcPercent)
		{
			// Set Dc motor
			if (((ActualState.dcPercent - ControlState.dcPercent) < MOTOR_STATE_MACHINE_SPEED_CHANGE_LIMIT)
				|| ((ControlState.dcPercent - ActualState.dcPercent) < MOTOR_STATE_MACHINE_SPEED_CHANGE_LIMIT))
				{
				// Small different, set to control value
				ActualState.dcPercent =  ControlState.dcPercent;

				}
			else
			{
				// Large different
				if (ActualState.dcPercent < ControlState.dcPercent)
				{
					if ((ControlState.dcPercent - ControlState.dcPercent) > (4 * MOTOR_STATE_MACHINE_SPEED_CHANGE_LIMIT))
					{
						ActualState.dcPercent += 2 * MOTOR_STATE_MACHINE_SPEED_CHANGE_LIMIT;
					}
					else
					{
						ActualState.dcPercent += MOTOR_STATE_MACHINE_SPEED_CHANGE_LIMIT;
					}
				}
				else
				{
					// ActualState.dcPercent > ControlState.dcPercent
					if ((ControlState.dcPercent - ActualState.dcPercent) > (4 *MOTOR_STATE_MACHINE_SPEED_CHANGE_LIMIT))
					{
						ActualState.dcPercent -= 2 * MOTOR_STATE_MACHINE_SPEED_CHANGE_LIMIT;
					}
					else
					{
						ActualState.dcPercent -= MOTOR_STATE_MACHINE_SPEED_CHANGE_LIMIT;
					}
				}
			}

			Motor_DcMotorChangePercent(ActualState.dcPercent);
		}
		else
		{
		// Equal, not need set
		}
	}


	// Control Servo motor
	if (ActualState.angle != ControlState.angle)
	{
		if (ActualState.angle < ControlState.angle)
		{
			if ((ActualState.angle < 0) && (ControlState.angle > 0))
			{
				ActualState.angle += (2 * MOTOR_SERVO_CHANGE_LIMIT);
			}
			else
			{
				ActualState.angle += MOTOR_SERVO_CHANGE_LIMIT;
			}
		}
		else
		{
			if ((ActualState.angle > 0) && (ControlState.angle < 0))
			{
				ActualState.angle -= (2 * MOTOR_SERVO_CHANGE_LIMIT);
			}
			else
			{
				ActualState.angle -= MOTOR_SERVO_CHANGE_LIMIT;
			}
		}
		Motor_ServoChangeAngle(ActualState.angle);
	}

}



/**
 * \brief	Set all motor state to 0 / default
 * \note	Be careful, it is not immediate stop motors, it is only stop "control" states
 */
void Motor_ControlStop(void)
{
	ControlState.dcPercent = 0;
	ControlState.dir = MotorDir_Stop;
	ControlState.angle = 0;
}



void Motor_StateMachine_SetAngle(int8_t angle)
{
	ControlState.angle = angle;
}



void Motor_StateMachine_SetDc(int16_t dc)
{
	if (dc > 0)
	{
		ControlState.dcPercent = dc;
		ControlState.dir = MotorDir_Forward;
	}
	else if (dc < 0)
	{
		ControlState.dcPercent = -dc;
		ControlState.dir = MotorDir_Backward;
	}
	else
	{
		// = 0
		ControlState.dcPercent = 0;
		ControlState.dir = MotorDir_Stop;
	}
}



#endif
