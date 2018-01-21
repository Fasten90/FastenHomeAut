/*
 *		RTC.h
 *		Created on:		2017-01-17
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		RTC (Real Time Clock)
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-01-17
 */

#ifndef RTC_H_
#define RTC_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"


#ifdef CONFIG_MODULE_RTC_ENABLE

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

// Defines related to Clock configuration
#define RTC_ASYNCH_PREDIV	0x7F	// LSE as RTC clock
#define RTC_SYNCH_PREDIV	0x00FF	// LSE as RTC clock



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
void RTC_SetDate(Date_t *date);
void RTC_SetTime(Time_t *time);

void RTC_GetDateTime(DateTime_t *dateTime);
void RTC_GetDate(Date_t *date);
void RTC_GetTime(Time_t *time);

void RTC_CalendarShow(char *showdate, char *showtime);



#endif // #ifdef CONFIG_MODULE_RTC_ENABLE


#endif /* RTC_H_ */
