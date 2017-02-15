/*
 *		LedPWM.h
 *
 *		Created on:		2017. febr. 15.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 15.
 */

#ifndef MODULES_LEDPWM_H_
#define MODULES_LEDPWM_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


#define  LED_TIMER_PERIOD_VALUE       (uint32_t)(666 - 1)  /* Period Value  */


#define LED_BLUE_SET_PERCENT(percent)	LED_PWM_ChangePercent(&TimLedBlueHandle,LED_BLUE_TIMER_CHANNEL,percent);
#define LED_GREEN_SET_PERCENT(percent)	LED_PWM_ChangePercent(&TimLedGreenHandle,LED_GREEN_TIMER_CHANNEL,percent);


/*
#define  PULSE1_VALUE       (uint32_t)(PERIOD_VALUE/2)        // Capture Compare 1 Value
#define  PULSE2_VALUE       (uint32_t)(PERIOD_VALUE*37.5/100) // Capture Compare 2 Value
#define  PULSE3_VALUE       (uint32_t)(PERIOD_VALUE/4)        // Capture Compare 3 Value
#define  PULSE4_VALUE       (uint32_t)(PERIOD_VALUE*12.5/100) // Capture Compare 4 Value
*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


extern TIM_HandleTypeDef    TimLedBlueHandle;
extern TIM_HandleTypeDef    TimLedGreenHandle;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/


void LedPWM_Init(void);
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim);
void LED_PWM_ChangePercent(TIM_HandleTypeDef *TimHandle,
		uint32_t timerChannel, uint8_t percent);



#endif /* MODULES_LEDPWM_H_ */
