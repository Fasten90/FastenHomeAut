/*
 *    CommonPWM.h
 *    Created on:   2017-02-27
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Common PWM
 *    Target:       STM32Fx
 */

#ifndef COMMONPWM_H_
#define COMMONPWM_H_



#include "options.h"

#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define COMMONPWM_PRESCALER_VALUE             (400000U)

#define COMMONPWM_TIMER_PERIOD_VALUE        (665U)


#define COMMONPWM_TIMx                        TIM4

#define COMMONPWM_TIMER_CLK_ENABLES()        __TIM4_CLK_ENABLE()

#define COMMONPWM_TIMER_CHANNEL                TIM_CHANNEL_2

#define COMMONPWM_TIMx_GPIO_AF              GPIO_AF2_TIM4

#define COMMONPWM_TIMER_GPIO_CLK_ENABLES()    __GPIOD_CLK_ENABLE()

#define COMMONPWM_PIN1_GPIO_PORT            GPIOD
#define COMMONPWM_PIN1_GPIO_PIN                GPIO_PIN_13



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern TIM_HandleTypeDef    CommonPWM_Timer_Handle;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void CommonPWM_Init(void);
void CommonPWM_ChangePercent(uint8_t percent);



#endif    // #ifdef CONFIG_MODULE_COMMON_PWM_ENABLE

#endif /* COMMONPWM_H_ */
