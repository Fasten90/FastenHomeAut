/*
 *		CommonPWM.h
 *
 *		Created on:		2017. febr. 27.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 27.
 */

#ifndef COMMONPWM_H_
#define COMMONPWM_H_



#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE

/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define COMMONPWM_PRESCALER_VALUE 			(400000U)

#define COMMONPWM_TIMER_PERIOD_VALUE		(665U)


#define COMMONPWM_TIMx						TIM4

#define COMMONPWM_TIMER_CLK_ENABLES()		__TIM4_CLK_ENABLE()

#define COMMONPWM_TIMER_CHANNEL				TIM_CHANNEL_2

#define COMMONPWM_TIMx_GPIO_AF          	GPIO_AF2_TIM4

#define COMMONPWM_TIMER_GPIO_CLK_ENABLES()	__GPIOD_CLK_ENABLE()

#define COMMONPWM_PIN1_GPIO_PORT			GPIOD
#define COMMONPWM_PIN1_GPIO_PIN				GPIO_PIN_13



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



#endif	// #ifdef CONFIG_MODULE_COMMON_PWM_ENABLE

#endif /* HWTESTER_COMMONPWM_H_ */
