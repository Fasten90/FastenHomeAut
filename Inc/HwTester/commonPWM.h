/*
 *		commonPWM.h
 *
 *		Created on:		2017. febr. 11.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 11.
 */

#ifndef HWTESTER_COMMONPWM_H_
#define HWTESTER_COMMONPWM_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define PWM_TIMER_PERIOD_VALUE					(uint32_t)(666 - 1)  /* Period Value  */

#define PWM_MOTOR_SERVO_TIMER_PRESCALER			(10000U)				// ms*10 based timer
#define PWM_MOTOR_SERVO_TIMER_PERIOD_VALUE		(uint32_t)(200 - 1)		// ms*10


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void MOTOR_Init(void);

void CommonPWM_1Init(uint8_t percent);
void CommonPWM_2Init(int8_t angle);


#endif /* HWTESTER_COMMONPWM_H_ */
