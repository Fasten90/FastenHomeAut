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


#define MOTOR_SERVO_MIN_ANGLE		(-90)
#define MOTOR_SERVO_OFFSET_ANGLE	(90)
#define MOTOR_SERVO_MAX_ANGLE		(+90)
#define MOTOR_SERVO_ANGLE_INTERVAL	(MOTOR_SERVO_MAX_ANGLE-MOTOR_SERVO_MIN_ANGLE)
#define MOTOR_SERVO_MIN_TIMER_VALUE	(10)		// ms*10
#define MOTOR_SERVO_MAX_TIMER_VALUE (20)		// ms*10
#define MOTOR_SERVO_TIMER_INTERVAL	(MOTOR_SERVO_MAX_TIMER_VALUE-MOTOR_SERVO_MIN_TIMER_VALUE)
#define MOTOR_SERVO_TIME_FULL		(10000)		// ms*10


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

void Motor_DcMotorTimerInit(uint8_t percent);
void Motor_ServoTimerInit(int8_t angle);


#endif /* HWTESTER_COMMONPWM_H_ */
