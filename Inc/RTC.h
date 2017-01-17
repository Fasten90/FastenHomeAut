/*
 *		RTC.h
 *
 *		Created on:		2017. jan. 17.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. jan. 17.
 */

#ifndef RTC_H_
#define RTC_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/
void RTC_Init(void);
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);

void RTC_CalendarConfig(DateTime_t *dateTime);
void RTC_DateConfig(Date_t *date);
void RTC_TimeConfig(Time_t *time);

void RTC_CalendarShow(char *showdate, char *showtime);


#endif /* RTC_H_ */
