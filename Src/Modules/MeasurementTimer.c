/*
 *    MeasurementTimer.c
 *    Created on:   2018-03-02
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     MeasurementTimer for measure events - now only one event can be measured
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"
#include "board.h"
#include "MemHandler.h"
#include "MeasurementTimer.h"

#include "Timing.h"
#include "UnitTest.h"


#ifdef CONFIG_MODULE_MEASUREMENTTIMER_ENABLE


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

TIM_HandleTypeDef MeasurementTimer_TimerHandle = { 0 };
static volatile uint32_t MeasurementTimer_ElapsedSeconds = 0;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * @brief       MeasurementTimer initialization
 */
void MeasurementTimer_Init(void)
{
    /* Counter prescaler value */
    uint32_t PrescalerValue = 0;
    uint32_t Period = 0;

    /* Called by HAL */
    HAL_TIM_PWM_MspInit(&MeasurementTimer_TimerHandle);

    /* Compute the prescaler value */
    /* rescalerValue = (uint32_t)(SystemCoreClock / MEASUREMENTTIMER_TIMER_PRESCALER) - 1; */
    PrescalerValue = (uint32_t)(SystemCoreClock / MEASUREMENTTIMER_TIMER_PRESCALER);
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
 * @brief       HAL driver function: PWM MspInit
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    UNUSED_ARGUMENT(htim);

    /* TIMx Peripheral clock enable */
    MEASUREMENTTIMER_TIMER_CLK_ENABLES();

    /* TODO: Added for Error handling... need it? */
    HAL_NVIC_SetPriority(MEASUREMENTTIMER_TIMx_IRQn, MEASUREMENTTIMER_IT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(MEASUREMENTTIMER_TIMx_IRQn);
}



/**
 * @brief       Set DC motor PWM percent
 */
void MeasurementTimer_StartMeasurement(void)
{
    /* TODO: Hardware dependent zero set */
    /* Clear */
    MeasurementTimer_TimerHandle.Instance->CNT = 0;

    /* Set 0 */
    MeasurementTimer_ElapsedSeconds = 0;
}



/**
 * @brief       Period elapsed callback in non blocking mode
 * @param       htim : TIM handle
 * @retval      None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    UNUSED_ARGUMENT(htim);

    /* 1 sec elapsed */
    MeasurementTimer_ElapsedSeconds++;
}



/**
 * @brief       Get value in us
 */
uint32_t MeasurementTimer_GetTime(void)
{
    /* TODO: Hardware dependent */
    uint32_t timerValue = MeasurementTimer_TimerHandle.Instance->CNT;

    /* TimerValue is in us:
     * Elapsed seconds + actual usec part
     */
    return (MeasurementTimer_ElapsedSeconds * 1000000U) + (timerValue * MEASUREMENTTIMER_USEC_CORRECTION_MUL);
}



#ifdef CONFIG_MODULE_MEASUREMENTTIMER_UNITTEST_ENABLE
/**
 * @brief       MeasurementTimer UnitTest
 */
uint32_t MeasurementTimer_UnitTest(void)
{
    UnitTest_Start("MeasurementTimer", __FILE__);


    /* Test Measurement Timer - Wait ~1 second */
    uint32_t elapsedUsec = 0;
    const uint8_t enabledAccuracyPercent = 1;
    const uint32_t testMsec = 1000;
    const uint32_t testUSec = 1000000U;

    MeasurementTimer_Init();
    MeasurementTimer_StartMeasurement();

    DelayMs(testMsec);

    elapsedUsec = MeasurementTimer_GetTime();

    UNITTEST_ASSERT((elapsedUsec > (testUSec * ((float)(100-enabledAccuracyPercent)/100)))
                     && (elapsedUsec < (testUSec * ((float)(100+enabledAccuracyPercent)/100))), "1 second measurement error");


    /* Finish */
    return UnitTest_End();
}

#endif /* CONFIG_MODULE_MEASUREMENTTIMER_UNITTEST_ENABLE */


#endif /* CONFIG_MODULE_MEASUREMENTTIMER_ENABLE */
