/*
 *		DateTime.h
 *		Created on:		2017-01-10
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		DateTime handling
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-01-10
 */

#ifndef DATETIME_H_
#define DATETIME_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "MEM.h"
#include "GenericTypeDefs.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

// 2017-01-10 19:56:00\0
#define DATETIME_STRING_MAX_LENGTH				(20)

#define DATETIME_DATE_YEAR_MAX_FULL_VALUE		(2050)
#define DATETIME_DATE_YEAR_MIN_FULL_VALUE		(2017)
#define DATETIME_DATE_YEAR_MAX_VALUE			(DATETIME_DATE_YEAR_MAX_FULL_VALUE - 2000)
#define DATETIME_DATE_YEAR_MIN_VALUE			(DATETIME_DATE_YEAR_MIN_FULL_VALUE - 2000)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< Date structure
typedef struct
{
	uint8_t year;		///< Year - from 2000 (example: 2017 --> value 17)
	uint8_t month;		///< Month:	1-12
	uint8_t day;		///< Day:	1-31
} Date_t;


///< Time structure
typedef struct
{
	uint8_t hour;		///< Hour:	0-23
	uint8_t minute;		///< Minute:0-59
	uint8_t second;		///< Second:0-59
} Time_t;


///< DateTime structure
typedef struct
{
	Date_t date;		///< Date (Year, Month, Day)
	Time_t time;		///< Time (Hour, Minute, Second)
} DateTime_t;


#ifdef DATETIME_OLD_CODE
///< DateTime string structure
typedef struct
{
	char year[4];				// 2017
	char separator_1[1];		// -
	char month[2];				// 01
	char separator_2[1];		// -
	char day[2];				// 10
	char separator_space[1];	// ' '
	char hour[2];				// 20
	char separator_4[1];		// :
	char minute[2];				// 01
	char separator_5[1];		// :
	char second[2];				// 00
	char end[1];				// '\0' or other
} DateTimeString_t;
#endif


///< DateTime compare results (two DateTime compared)
typedef enum
{
	DateTimeCompare_Unknown,				///< Unknown
	DateTimeCompare_FirstOldSecondNew,		///< First DateTime is older than second DateTime
	DateTimeCompare_FirstNewSecondOld,		///< First DateTime is newer then second DateTime
	DateTimeCompare_Equal,					///< The two DateTime are equal
	DateTimeCompare_InvalidFirst,			///< First DateTime is invalid
	DateTimeCompare_InvalidSecond,			///< Second DateTime is invalid
	DateTimeCompare_Invalid,				///< Invalid...
	DateTimeCompare_Error_Parameter			///< Parameter is wrong
} DateTimeCompare_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE
extern DateTime_t DateTime_SystemTime;
#endif



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

bool DateTime_ConvertStringToDateTime(const char *string, DateTime_t *dateTime);

bool DateTime_ConvertDateStringToDate(char *str, Date_t *date);
bool DateTime_ConvertTimeStringToTime(char *str, Time_t *time);

uint8_t DateTime_PrintDateTimeToString(char *message, DateTime_t *dateTime);

bool DateTime_CheckDateTime(DateTime_t *dateTime);
DateTimeCompare_t DateTime_CompareDateTime(DateTime_t *dateTime1, DateTime_t *dateTime2);

int32_t DateTime_CalculateDifferentOf2DateTime(DateTime_t *dateTime1, DateTime_t *dateTime2);
void DateTime_Steps(DateTime_t *dateTime, uint32_t stepSeconds);
void DateTime_Step(DateTime_t *dateTime);

#ifdef NO_OPTIMIZED_DATETIME
void DateTime_GetDateTime(DateTime_t *dateTime);
void DateTime_SetDate(Date_t *date);
void DateTime_SetTime(Time_t *time);
#else

#ifdef CONFIG_MODULE_RTC_ENABLE
	#define DateTime_GetDateTime(dateTime)		RTC_GetDateTime(&dateTime)
	#define DateTime_SetDate(date)				RTC_SetDate(date)
	#define DateTime_SetTime(time)				RTC_SetTime(time)
#else
	#define DateTime_GetDateTime(_dateTime)		memcpy(_dateTime, &DateTime_SystemTime, sizeof(DateTime_t))
	#define DateTime_SetDate(_date)				memcpy(&DateTime_SystemTime.date, _date, sizeof(Date_t))
	#define DateTime_SetTime(_time)				memcpy(&DateTime_SystemTime.time, _time, sizeof(Time_t))
#endif

#endif

#ifdef MODULE_DATETIME_UNITTEST_ENABLE
void DateTime_UnitTest(void);
#endif



#endif /* DATETIME_H_ */
