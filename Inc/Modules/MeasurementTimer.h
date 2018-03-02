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




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define MEASUREMENTTIMER_TIMER_PRESCALER		(1000000U)				// 1_000_000 = us timebase
#define MEASUREMENTTIMER_TIMER_PERIOD_VALUE		(1000000U)				// 1_000_000us = 1s



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void MeasurementTimer_Init(void);
void MeasurementTimer_StartMeasurement(void);
uint32_t MeasurementTimer_GetTime(void);


#endif /* INC_MODULES_MEASUREMENTTIMER_H_ */
