/*
 *		SysTime.c
 *		Created on:		2017-12-20
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017-12-20
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE

#include "DateTime.h"
#include "SysTime.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


///< System time (step by timer)
DateTime_t DateTime_SystemTime = { 0 };



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/




/**
 * \brief	Get DateTime
 */
inline __attribute__((always_inline)) void SysTime_GetDateTime(DateTime_t *dateTime)
{
#if defined(CONFIG_MODULE_RTC_ENABLE)
	RTC_GetDateTime(&dateTime);
#elif defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE)
	memcpy(dateTime, &DateTime_SystemTime, sizeof(DateTime_t));
#else
	(void)dateTime;
#endif
}



/**
 * \brief	Set date
 */
inline __attribute__((always_inline)) void SysTime_SetDate(Date_t *date)
{
#if defined(CONFIG_MODULE_RTC_ENABLE)
	RTC_SetDate(&date);
#elif defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE)
	memcpy(&DateTime_SystemTime.date, date, sizeof(Date_t));
#else
	(void)date;
#endif
}



/**
 * \brief	Set time
 */
inline __attribute__((always_inline)) void SysTime_SetTime(Time_t *time)
{
#if defined(CONFIG_MODULE_RTC_ENABLE)
	RTC_SetTime(&time);
#elif defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE)
	memcpy(&DateTime_SystemTime.time, time, sizeof(Time_t));
#else
	(void)time;
#endif
}



/**
 * \brief	Step SysTime (with 1 second)
 */
void SysTime_StepSecond(void)
{
	// Step SystemTime +1 second
	DateTime_Step(&DateTime_SystemTime);
}



#endif	// #ifdef CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE
