/*
 *		DateTime.c
 *
 *		Created on:		2017. jan. 10.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. jan. 10.
 */




/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "DateTime.h"

#ifdef MODULE_DATETIME_UNITTEST_ENABLE
#include "unittest.h"
#endif

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/
bool DateTime_CheckValue(uint32_t originalValue, uint32_t minValue, uint32_t maxValue);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Convert DateTime string to DateTime_t
 * 			"2017-01-10 19:55:00  -> DateTime_t
 * 			builded string length is: DATETIME_STRING_MAX_LENGTH
 */
bool DateTime_ConvertStringToDateTime(const char *string, DateTime_t *dateTime)
{
#if DATETIME_OLD_CODE
	DateTimeString_t dateTimeString;
#else
	char dateTimeString[DATETIME_STRING_MAX_LENGTH];
#endif

	DateTime_t convertedDateTime;
	bool isOk = true;
	char *separated[2];

	if (StringLength(string) != (DATETIME_STRING_MAX_LENGTH - 1))
	{
		// Wrong length
		return false;
	}

	StrCpy((char *)&dateTimeString, string);

	// Split at space: "2017-01-18 20:10:00"
	if (STRING_Splitter(dateTimeString, ' ', separated, 2) == 2)
	{
		// Convert date/time strings
		isOk &= DateTime_ConvertDateStringToDate(separated[0], &convertedDateTime.date);
		isOk &= DateTime_ConvertTimeStringToTime(separated[1], &convertedDateTime.time);

		if (isOk)
		{
			// Copy to parameter, if ok
			memcpy(dateTime, &convertedDateTime, sizeof(DateTime_t));
		}
	}
	else
	{
		// Failed split, wrong string
		isOk = false;
	}


#if DATETIME_OLD_CODE

	// Check separators and put 0
	if (dateTimeString.separator_1[0] == '-') dateTimeString.separator_1[0] = '\0';
	else isOk = false;

	if (dateTimeString.separator_2[0] == '-') dateTimeString.separator_2[0] = '\0';
	else isOk = false;

	if (dateTimeString.separator_space[0] == ' ') dateTimeString.separator_space[0] = '\0';
	else isOk = false;

	if (dateTimeString.separator_4[0]== ':') convertDateTime.separator_4[0] = '\0';
	else isOk = false;

	if (dateTimeString.separator_5[0] == ':') dateTimeString.separator_5[0] = '\0';
	else isOk = false;

	// If separator characters are ok
	// TODO: Do no change parameter, while the result is not good?
	if (isOk)
	{
		uint32_t convertValue;
		isOk &= StringToUnsignedDecimalNum(dateTimeString.year, &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 2017, 2050);
		dateTime->date.year = (uint8_t)(convertValue - 2000);
		isOk &= StringToUnsignedDecimalNum(dateTimeString.month, &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 12);
		dateTime->date.month = (uint8_t)convertValue;
		isOk &= StringToUnsignedDecimalNum(dateTimeString.day, &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 31);
		dateTime->date.day = (uint8_t)convertValue;
		isOk &= StringToUnsignedDecimalNum(dateTimeString.hour, &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 24);
		dateTime->time.hour = (uint8_t)convertValue;
		isOk &= StringToUnsignedDecimalNum(dateTimeString.minute, &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 60);
		dateTime->time.minute = (uint8_t)convertValue;
		isOk &= StringToUnsignedDecimalNum(dateTimeString.second, &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 60);
		dateTime->time.second = (uint8_t)convertValue;
	}
#endif

	return isOk;
}



/**
 * \brief	Convert DateString to Date
 */
bool DateTime_ConvertDateStringToDate(char *str, Date_t *date)
{
	char *separated[3] = { 0 };
	bool isOk = true;
	Date_t convertDate;

	// Separate
	if (STRING_Splitter(str, '-', separated, 3) == 3)
	{
		// Successful separate
		uint32_t convertValue;

		isOk &= StringToUnsignedDecimalNum(separated[0], &convertValue);
		isOk &= DateTime_CheckValue(convertValue, DATETIME_DATE_YEAR_MIN_FULL_VALUE, DATETIME_DATE_YEAR_MAX_FULL_VALUE);
		convertDate.year = (uint8_t)(convertValue - 2000);

		isOk &= StringToUnsignedDecimalNum(separated[1], &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 1, 12);
		convertDate.month = (uint8_t)convertValue;

		isOk &= StringToUnsignedDecimalNum(separated[2], &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 1, 31);
		convertDate.day = (uint8_t)convertValue;

		// If ok, copy to parameter
		if (isOk)
		{
			memcpy(date, &convertDate, sizeof(Date_t));
		}
	}
	else
	{
		isOk = false;
	}

	return isOk;
}



/**
 * \brief	Convert DateString to Time
 */
bool DateTime_ConvertTimeStringToTime(char *str, Time_t *time)
{
	char *separated[3] = { 0 };
	bool isOk = true;
	Time_t convertTime;

	// Separate
	if (STRING_Splitter(str, ':', separated, 3) == 3)
	{
		// Successful separate
		uint32_t convertValue;

		isOk &= StringToUnsignedDecimalNum(separated[0], &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 23);
		convertTime.hour = (uint8_t)convertValue;

		isOk &= StringToUnsignedDecimalNum(separated[1], &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 59);
		convertTime.minute = (uint8_t)convertValue;

		isOk &= StringToUnsignedDecimalNum(separated[2], &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 59);
		convertTime.second = (uint8_t)convertValue;

		// If ok, copy to parameter
		if (isOk)
		{
			memcpy(time, &convertTime, sizeof(Time_t));
		}
	}
	else
	{
		isOk = false;
	}

	return isOk;
}



/**
 * \brief	Check value it is ok (is in range?)
 */
bool DateTime_CheckValue(uint32_t originalValue, uint32_t minValue, uint32_t maxValue)
{
	bool isOk = false;

	if (originalValue >= minValue && originalValue <= maxValue)
	{
		isOk = true;
	}

	return isOk;
}



/**
 * \brief	Check DateTime
 * \retval	true	if ok (valid)
 * \retval	false	if invalid
 */
bool DateTime_CheckDateTime(DateTime_t *dateTime)
{
	bool isOk = true;

	// Check values
	isOk &= DateTime_CheckValue(dateTime->date.year, DATETIME_DATE_YEAR_MIN_VALUE, DATETIME_DATE_YEAR_MAX_VALUE);
	isOk &= DateTime_CheckValue(dateTime->date.month, 1, 12);
	isOk &= DateTime_CheckValue(dateTime->date.day, 1, 31);
	isOk &= DateTime_CheckValue(dateTime->time.hour, 0, 23);
	isOk &= DateTime_CheckValue(dateTime->time.minute, 0, 59);
	isOk &= DateTime_CheckValue(dateTime->time.second, 0, 59);

	return isOk;
}


/**
 * \brief	Print DateTime_t to string
 */
uint8_t DateTime_PrintDateTimeToString(char *message, DateTime_t *dateTime)
{

	return usprintf(message, "%04d-%02d-%02d %02d:%02d:%02d",
			dateTime->date.year+2000,
			dateTime->date.month,
			dateTime->date.day,
			dateTime->time.hour,
			dateTime->time.minute,
			dateTime->time.second);
}



/**
 * \brief	Compare DateTimes
 */
DateTimeCompare_t DateTime_CompareDateTime(DateTime_t *dateTime1, DateTime_t *dateTime2)
{
	DateTimeCompare_t compare = DateTimeCompare_Unknown;

	if ((dateTime1 == NULL) || (dateTime2 == NULL))
	{
		return DateTimeCompare_Error_Parameter;
	}

	if (!DateTime_CheckDateTime(dateTime1) && !DateTime_CheckDateTime(dateTime2))
	{
		return DateTimeCompare_Invalid;
	}

	if (dateTime1->date.year < dateTime2->date.year) compare = DateTimeCompare_FirstOldSecondNew;
	else if (dateTime1->date.year > dateTime2->date.year) compare = DateTimeCompare_FirstNewSecondOld;
	else if (dateTime1->date.month < dateTime2->date.month) compare = DateTimeCompare_FirstOldSecondNew;
	else if (dateTime1->date.month > dateTime2->date.month) compare = DateTimeCompare_FirstNewSecondOld;
	else if (dateTime1->date.day < dateTime2->date.day) compare = DateTimeCompare_FirstOldSecondNew;
	else if (dateTime1->date.day > dateTime2->date.day) compare = DateTimeCompare_FirstNewSecondOld;
	else if (dateTime1->time.hour < dateTime2->time.hour) compare = DateTimeCompare_FirstOldSecondNew;
	else if (dateTime1->time.hour > dateTime2->time.hour) compare = DateTimeCompare_FirstNewSecondOld;
	else if (dateTime1->time.minute < dateTime2->time.minute) compare = DateTimeCompare_FirstOldSecondNew;
	else if (dateTime1->time.minute > dateTime2->time.minute) compare = DateTimeCompare_FirstNewSecondOld;
	else if (dateTime1->time.second < dateTime2->time.second) compare = DateTimeCompare_FirstOldSecondNew;
	else if (dateTime1->time.second > dateTime2->time.second) compare = DateTimeCompare_FirstNewSecondOld;
	else compare = DateTimeCompare_Equal;

	return compare;
}



/**
 * \brief	DateTime Unit test
 */
#ifdef MODULE_DATETIME_UNITTEST_ENABLE
/**
 * \brief	String module Unit Test
 */
void DateTime_UnitTest(void)
{
	// Test variables
	DateTimeCompare_t comp;
	bool result;

	// Start of unittest
	UnitTest_Start("DateTime", __FILE__);

	/*				Check DateTime_CompareDateTime function			*/

	// Equal date
	DateTime_t test1 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
	DateTime_t test2 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
	comp = DateTime_CompareDateTime(&test1, &test2);
	UnitTest_CheckResult(comp == DateTimeCompare_Equal, "DateTime_CompareDateTime error", __LINE__);

	// First older
	DateTime_t test3 = { .date.year=17, .date.month=1, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
	DateTime_t test4 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
	comp = DateTime_CompareDateTime(&test3, &test4);
	UnitTest_CheckResult(comp == DateTimeCompare_FirstOldSecondNew, "DateTime_CompareDateTime error", __LINE__);

	// First older
	DateTime_t test5 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
	DateTime_t test6 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=1 };
	comp = DateTime_CompareDateTime(&test5, &test6);
	UnitTest_CheckResult(comp == DateTimeCompare_FirstOldSecondNew, "DateTime_CompareDateTime error", __LINE__);

	// First newer
	DateTime_t test7 = { .date.year=18, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
	DateTime_t test8 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
	comp = DateTime_CompareDateTime(&test7, &test8);
	UnitTest_CheckResult(comp == DateTimeCompare_FirstNewSecondOld, "DateTime_CompareDateTime error", __LINE__);

	// First newer
	DateTime_t test9 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=13, .time.second=0 };
	DateTime_t test10 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
	comp = DateTime_CompareDateTime(&test9, &test10);
	UnitTest_CheckResult(comp == DateTimeCompare_FirstNewSecondOld, "DateTime_CompareDateTime error", __LINE__);


	/*				Check DateTime_CheckDateTime function			*/
	// Check these dateTimes are valid?
	result = true;
	result &= DateTime_CheckDateTime(&test1);
	result &= DateTime_CheckDateTime(&test2);
	result &= DateTime_CheckDateTime(&test3);
	result &= DateTime_CheckDateTime(&test4);
	result &= DateTime_CheckDateTime(&test5);
	result &= DateTime_CheckDateTime(&test6);
	result &= DateTime_CheckDateTime(&test7);
	result &= DateTime_CheckDateTime(&test8);
	result &= DateTime_CheckDateTime(&test9);
	result &= DateTime_CheckDateTime(&test10);
	UnitTest_CheckResult(result == true, "DateTime_CheckDateTime error", __LINE__);

	// Check these dateTimes are invalids?
	DateTime_t test11 = { .date.year=150, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
	DateTime_t test12 = { .date.year=17, .date.month=0, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
	DateTime_t test13 = { .date.year=17, .date.month=2, .date.day=0, .time.hour=12, .time.minute=12, .time.second=0 };
	DateTime_t test14 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=24, .time.minute=12, .time.second=0 };
	DateTime_t test15 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=60, .time.second=0 };
	DateTime_t test16 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=60 };
	result = false;
	result |= DateTime_CheckDateTime(&test11);
	result |= DateTime_CheckDateTime(&test12);
	result |= DateTime_CheckDateTime(&test13);
	result |= DateTime_CheckDateTime(&test14);
	result |= DateTime_CheckDateTime(&test15);
	result |= DateTime_CheckDateTime(&test16);
	UnitTest_CheckResult(result == false, "DateTime_CheckDateTime error", __LINE__);


	/*			DateTime_ConvertStringToDateTime		*/
	DateTime_t test300;
	char strDateTime1[DATETIME_STRING_MAX_LENGTH] = { "2017-02-03 12:12:00" };
	result = DateTime_ConvertStringToDateTime(strDateTime1, &test300);
	UnitTest_CheckResult(result == true, "DateTime_ConvertStringToDateTime error", __LINE__);
	UnitTest_CheckResult(((test300.date.year	== 17)
						&& (test300.date.month	== 2)
						&& (test300.date.day	== 3)
						&& (test300.time.hour	== 12)
						&& (test300.time.minute	== 12)
						&& (test300.time.second	== 0)),
			"DateTime_ConvertStringToDateTime error", __LINE__);


	/*			DateTime_PrintDateTimeToString			*/
	DateTime_t test400 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
	char strDateTime2[DATETIME_STRING_MAX_LENGTH];
	DateTime_PrintDateTimeToString(strDateTime2, &test400);
	result = !StrCmp(strDateTime2, "2017-02-03 12:12:00");
	UnitTest_CheckResult(result == true, "DateTime_ConvertStringToDateTime error", __LINE__);


	// Finish
	UnitTest_End();
}
#endif

