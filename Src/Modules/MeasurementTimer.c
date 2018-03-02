/*
 *		MeasurementTimer.c
 *		Created on:		2018-03-02
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		MeasurementTimer for measure events - now only one event can be measured
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2018-03-02
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"
#include "board.h"
#include "MemHandler.h"
#include "MeasurementTimer.h"


#ifdef CONFIG_MODULE_MEASUREMENTTIMER_ENABLE


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

static TIM_HandleTypeDef MeasurementTimer_TimerHandle;
static uint32_t MeasurementTimer_ElapsedSeconds = 0;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * \brief	MeasurementTimer initialization
 */
void MeasurementTimer_Init(void)
{
	/* Counter Prescaler value */
	uint32_t PrescalerValue = 0;
	uint32_t Period = 0;

	/* Called by HAL */
	HAL_TIM_PWM_MspInit(&MeasurementTimer_TimerHandle);

	/* Compute the prescaler value */
	PrescalerValue = (uint32_t)(SystemCoreClock / MEASUREMENTTIMER_TIMER_PRESCALER ) - 1;

	Period = MEASUREMENTTIMER_TIMER_PERIOD_VALUE;

	/* -1- Configure the TIM peripheral */
	MeasurementTimer_TimerHandle.Instance = MEASUREMENTTIMER_TIMx;

	MeasurementTimer_TimerHandle.Init.Prescaler         = PrescalerValue;
	MeasurementTimer_TimerHandle.Init.Period            = Period;
	MeasurementTimer_TimerHandle.Init.ClockDivision     = 0;
	MeasurementTimer_TimerHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	MeasurementTimer_TimerHandle.Init.RepetitionCounter = 0;
	if (HAL_TIM_Base_Init(&MeasurementTimer_TimerHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	/* -2- Start the TIM Base generation in interrupt mode */
	/* Start Channel1 */
	if (HAL_TIM_Base_Start_IT(&MeasurementTimer_TimerHandle) != HAL_OK)
	{
		/* Starting Error */
		Error_Handler();
	}
}



/**
 * \brief	HAL driver function: PWM MspInit
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	(void)htim;

	/* TIMx Peripheral clock enable */
	MEASUREMENTTIMER_TIMER_CLK_ENABLES();

	/* TODO: Added for Error handling... need it? */
#if UNUSED
	HAL_NVIC_SetPriority(TIM3_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
#endif
}



/**
 * \brief	Set DC motor PWM percent
 */
void MeasurementTimer_StartMeasurement(void)
{
	/* Clear */
	MeasurementTimer_TimerHandle.Instance->CNT = 0;

	/* Set 0 */
	MeasurementTimer_ElapsedSeconds = 0;
}



/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	(void)htim;

	/* 1 sec elapsed */
	MeasurementTimer_ElapsedSeconds++;
}



/**
 * \brief	Get value in us
 */
uint32_t MeasurementTimer_GetTime(void)
{
	uint32_t timerValue = MeasurementTimer_TimerHandle.Instance->CNT;

	/* TimerValue is in us */
	return (MeasurementTimer_ElapsedSeconds * 1000000) + timerValue;
}



#endif /* CONFIG_MODULE_MEASUREMENTTIMER_ENABLE */
