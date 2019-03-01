/*
 *		MeasurementTimer.h
 *		Created on:		2018-03-02
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2018-03-02
 */

#ifndef INC_MODULES_MEASUREMENTTIMER_H_
#define INC_MODULES_MEASUREMENTTIMER_H_


#include "board.h"


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef MEASUREMENT_RESOLUTION_16BIT
/* 16 bit resolution */
#define MEASUREMENTTIMER_TIMER_PRESCALER		(50000U)				// 20us timebase
#define MEASUREMENTTIMER_TIMER_PERIOD_VALUE		(50000U)

#define MEASUREMENTTIMER_USEC_CORRECTION_MUL	(20)
#else
/* 32 bit resolution */
#define MEASUREMENTTIMER_TIMER_PRESCALER		(1000000U)				// 1_000_000 = us timebase
#define MEASUREMENTTIMER_TIMER_PERIOD_VALUE		(1000000U)				// 1_000_000us = 1s
#define MEASUREMENTTIMER_USEC_CORRECTION_MUL	(1)
#endif


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern TIM_HandleTypeDef MeasurementTimer_TimerHandle;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void MeasurementTimer_Init(void);
void MeasurementTimer_StartMeasurement(void);
uint32_t MeasurementTimer_GetTime(void);

uint32_t MeasurementTimer_UnitTest(void);


#endif /* INC_MODULES_MEASUREMENTTIMER_H_ */
