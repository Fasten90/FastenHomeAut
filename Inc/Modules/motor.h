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

// DC Motor
// DC Motor PWM control: 0-100% PWM
#define PWM_TIMER_DCMOTOR_PERIOD_VALUE					(uint32_t)(666 - 1)  /* Period Value  */



// Servo motor
// Set Servo motor PWM from 10ms to 20ms, and 15ms (middle) is center angle
#define PWM_MOTOR_SERVO_TIMER_PRESCALER			(100000U)				// ms*10 based timer
#define PWM_MOTOR_SERVO_TIMER_PERIOD_VALUE		(uint32_t)(2000 - 1)	// ms*100

#define MOTOR_SERVO_MIN_ANGLE		(-90)
#define MOTOR_SERVO_OFFSET_ANGLE	(90)
#define MOTOR_SERVO_MAX_ANGLE		(+90)
#define MOTOR_SERVO_ANGLE_INTERVAL	(MOTOR_SERVO_MAX_ANGLE-MOTOR_SERVO_MIN_ANGLE)

#define MOTOR_SERVO_MIN_TIMER_VALUE	(100)		// ms*100
#define MOTOR_SERVO_MAX_TIMER_VALUE (200)		// ms*100
#define MOTOR_SERVO_TIMER_INTERVAL	(MOTOR_SERVO_MAX_TIMER_VALUE-MOTOR_SERVO_MIN_TIMER_VALUE)
#define MOTOR_SERVO_TIME_FULL		(100000)	// ms*100

#define MOTOR_SERVO_MECHANICAL_MAX_ANGLE	(30)
#define MOTOR_SERVO_MECHANICAL_MIN_ANGLE	(-30)


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
	MotorDir_Unknown,
	MotorDir_Forward,
	MotorDir_Backward,
	MotorDir_Stop
} MotorDir_t;


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Motor_Init(void);

void Motor_DcMotorTimerInit(uint8_t percent);
void Motor_ServoTimerInit(int8_t angle);
void Motor_ServoChangeAngle(int8_t angle);

void Motor_DcMotorSeDirection(MotorDir_t dir);
void Motor_DcMotorChangePercent(uint8_t percent);



#endif /* HWTESTER_COMMONPWM_H_ */
