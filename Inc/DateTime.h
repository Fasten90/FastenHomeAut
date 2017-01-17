/*
 *		DateTime.h
 *
 *		Created on:		2017. jan. 10.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. jan. 10.
 */

#ifndef DATETIME_H_
#define DATETIME_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

// 2017-01-10 19:56:00\0
#define DATETIME_STRING_MAX_LENGTH			( 20 )


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/// Date structure
typedef struct
{
	uint8_t year;
	uint8_t month;
	uint8_t day;
} Date_t;



/// Time structure
typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} Time_t;


/// DateTime structure
typedef struct
{
	Date_t date;
	Time_t time;
} DateTime_t;



typedef struct
{
	char year[4];				// 2017
	char separator_1[1];			// -
	char month[2];				// 01
	char separator_2[1];			// -
	char day[2];				// 10
	char separator_space[1];	// ' '
	char hour[2];				// 20
	char separator_4[1];		// :
	char minute[2];				// 01
	char separator_5[1];		// :
	char second[2];				// 00
	char end[1];				// '\0' or other
} DateTimeString_t;

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/
bool DateTime_ConvertStringToDateTime(const char *string, DateTime_t *dateTime);

bool DateTime_ConvertDateStringToDate(char *str, Date_t *date);
bool DateTime_ConvertTimeStringToTime(char *str, Time_t *time);

uint8_t DateTime_PrintDateTimeToString(char *message, DateTime_t *dateTime);



#endif /* DATETIME_H_ */
