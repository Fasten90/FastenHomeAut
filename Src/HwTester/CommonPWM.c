/*
 *    CommonPWM.c
 *    Created on:   2017-02-27
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Common PWM
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "compiler.h"
#include "CommonPWM.h"


#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

TIM_HandleTypeDef    CommonPWM_Timer_Handle;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void CommonPWM_HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief    Timer initialize
 */
void CommonPWM_Init(void)
{

    /* Timer Output Compare Configuration Structure declaration */
    TIM_OC_InitTypeDef sConfig;

    /* Counter Prescaler value */
    uint32_t PrescalerValue = 0;
    uint32_t Period = 0;
    uint32_t Pulse = 0;


    /* Now this HAL_TIM_PWM_MspInit clone not called by HAL */
    CommonPWM_HAL_TIM_PWM_MspInit(NULL);


    /* Compute the prescaler value to have TIM3 counter clock equal to 16000000 Hz */
    /* hPrescalerValue = (uint32_t)(SystemCoreClock / 16000000) - 1; */
    PrescalerValue = (uint32_t)(SystemCoreClock / COMMONPWM_PRESCALER_VALUE) - 1;

    Period = COMMONPWM_TIMER_PERIOD_VALUE;
    Pulse = 0;    /* Because we want 0%...  Old version: Period * percent / 100; */


    /*##-1- Configure the TIM peripheral #######################################*/

    /* PWM1 */

    CommonPWM_Timer_Handle.Instance = COMMONPWM_TIMx;

    CommonPWM_Timer_Handle.Init.Prescaler         = PrescalerValue;
    CommonPWM_Timer_Handle.Init.Period            = Period;
    CommonPWM_Timer_Handle.Init.ClockDivision     = 0;
    CommonPWM_Timer_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    CommonPWM_Timer_Handle.Init.RepetitionCounter = 0;
    if (HAL_TIM_PWM_Init(&CommonPWM_Timer_Handle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }


    /*##-2- Configure the PWM channels #########################################*/
    /* Common configuration for all channels */
    sConfig.OCMode       = TIM_OCMODE_PWM1;
    sConfig.OCPolarity   = TIM_OCNPOLARITY_LOW;        /* TIM_OCPOLARITY_HIGH */
    sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;        /* TIM_OCIDLESTATE_RESET */
    /* Config.OCNPolarity  = TIM_OCNPOLARITY_HIGH;        // TIM_OCNPOLARITY_HIGH */
    /* Config.OCNIdleState = TIM_OCNIDLESTATE_RESET;    // TIM_OCNIDLESTATE_RESET */

    /* Set the pulse value for channel 1 */
    sConfig.Pulse = Pulse;
    if (HAL_TIM_PWM_ConfigChannel(&CommonPWM_Timer_Handle, &sConfig, COMMONPWM_TIMER_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }


    /*##-3- Start PWM signals generation #######################################*/
    /* Start channel 1 */
    if (HAL_TIM_PWM_Start(&CommonPWM_Timer_Handle, COMMONPWM_TIMER_CHANNEL) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

}



/**
 * @brief    HAL driver function: PWM MspInit - Initialize PWM GPIO-s and Timers
 */
static void CommonPWM_HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    (void)htim;


    GPIO_InitTypeDef GPIO_InitStruct;


    /* #-1- Enable peripherals and GPIO Clocks ################################# */
    /* TIMx Peripheral clock enable */
    COMMONPWM_TIMER_CLK_ENABLES();

    /* Enable GPIO Channels Clock */
    COMMONPWM_TIMER_GPIO_CLK_ENABLES();


    /* PWM1 */
    /* Configure GPIO pin */
    GPIO_InitStruct.Pin = COMMONPWM_PIN1_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = COMMONPWM_TIMx_GPIO_AF;
    HAL_GPIO_Init(COMMONPWM_PIN1_GPIO_PORT, &GPIO_InitStruct);

}



/**
 * @brief    Set PWM percent
 */
void CommonPWM_ChangePercent(uint8_t percent)
{
    TIM_HandleTypeDef *TimHandle = &CommonPWM_Timer_Handle;
    uint32_t timerChannel = COMMONPWM_TIMER_CHANNEL;

    /* Timer Output Compare Configuration Structure declaration */
    TIM_OC_InitTypeDef sConfig;


    uint32_t Pulse = COMMONPWM_TIMER_PERIOD_VALUE * percent / 100;    /* % percent calculate */


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

    /* Config ... */
    if (HAL_TIM_PWM_ConfigChannel(TimHandle, &sConfig, timerChannel) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }


    /* Need start... */
    if (HAL_TIM_PWM_Start(TimHandle, timerChannel) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

}

#endif    /* #ifdef CONFIG_MODULE_COMMON_PWM_ENABLE */
