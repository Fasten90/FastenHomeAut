/*
 *    DateTime.c
 *    Created on:   2017-01-10
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     DateTime handling
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "StringHelper.h"
#include "DateTime.h"

#ifdef CONFIG_MODULE_DATETIME_UNITTEST_ENABLE
    #include "UnitTest.h"
#endif

#ifndef STRING_SPRINTF_EXTENDED_ENABLE
    #define DATETIME_PARSE_IS_NOT_STRICT
#endif


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

///< Days of months
static const uint8_t days_of_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static bool DateTime_CheckValue(uint32_t originalValue, uint32_t minValue, uint32_t maxValue);
static uint8_t DateTime_GetDaysOfMonth(uint8_t year, uint8_t month);
static uint32_t DateTime_CalculateDateTimeSecond(DateTime_t *dateTime);
static bool DateTime_CheckItIsLeapYear(uint8_t year);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief       Convert DateTime string to DateTime_t
 *              "2017-01-10 19:55:00  -> DateTime_t
 *              builded string length is: DATETIME_STRING_MAX_LENGTH
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

    if ((string == NULL) || (dateTime == NULL))
    {
        return false;
    }

  #ifdef DATETIME_PARSE_IS_NOT_STRICT
    if (StringLength(string) > (DATETIME_STRING_MAX_LENGTH - 1))
  #else
    /* Strict */
    if (StringLength(string) != (DATETIME_STRING_MAX_LENGTH - 1))
  #endif
    {
        /* Wrong length */
        return false;
    }

    StrCpy((char *)&dateTimeString, string);

    /* Split at space: "2017-01-18 20:10:00" */
    if (STRING_Splitter(dateTimeString, " ", separated, 2) == 2)
    {
        /* Convert date/time strings */
        isOk &= DateTime_ConvertDateStringToDate(separated[0], &convertedDateTime.date);
        isOk &= DateTime_ConvertTimeStringToTime(separated[1], &convertedDateTime.time);

        if (isOk)
        {
            /* Copy to parameter, if ok */
            memcpy(dateTime, &convertedDateTime, sizeof(DateTime_t));
        }
    }
    else
    {
        /* Failed split, wrong string */
        isOk = false;
    }


#if DATETIME_OLD_CODE

    /* Check separators and put 0 */
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

    /* If separator characters are ok */
    /* TODO: Do no change parameter, while the result is not good? */
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
 * @brief       Convert DateString to Date
 * @note        Str must be modifiable!
 */
bool DateTime_ConvertDateStringToDate(char *str, Date_t *date)
{
    char *separated[3] = { 0 };
    bool isOk = true;
    Date_t convertDate;

    if ((str == NULL) || (date == NULL))
    {
        return false;
    }

    /* Separate */
    if (STRING_Splitter(str, "-", separated, 3) == 3)
    {
        /* Successful separate */
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

        /* If ok, copy to parameter */
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
 * @brief       Convert DateString to Time
 * @param[in]    str        String like "12:00:00"
 * @param[out]    time    Converted time
 */
bool DateTime_ConvertTimeStringToTime(char *str, Time_t *time)
{
    char *separated[3] = { 0 };
    bool isOk = true;
    Time_t convertTime;

    /* Separate */
    if (STRING_Splitter(str, ":", separated, 3) == 3)
    {
        /* Successful separate */
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

        /* If ok, copy to parameter */
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
 * @brief       Check value it is OK? (Is in range?)
 */
static bool DateTime_CheckValue(uint32_t originalValue, uint32_t minValue, uint32_t maxValue)
{
    bool isOk = false;

    if ((originalValue >= minValue) && (originalValue <= maxValue))
    {
        isOk = true;
    }

    return isOk;
}



/**
 * @brief       Check DateTime
 * @retval      true    if OK (valid)
 * @retval      false    if invalid
 */
bool DateTime_CheckDateTime(DateTime_t *dateTime)
{
    bool isOk = true;

    /* Check values */
    isOk &= DateTime_CheckValue(dateTime->date.year, DATETIME_DATE_YEAR_MIN_VALUE, DATETIME_DATE_YEAR_MAX_VALUE);
    isOk &= DateTime_CheckValue(dateTime->date.month, 1, 12);
    isOk &= DateTime_CheckValue(dateTime->date.day, 1, 31);
    isOk &= DateTime_CheckValue(dateTime->time.hour, 0, 23);
    isOk &= DateTime_CheckValue(dateTime->time.minute, 0, 59);
    isOk &= DateTime_CheckValue(dateTime->time.second, 0, 59);

    return isOk;
}



/**
 * @brief       Print DateTime_t to string
 *              like: "YYYY-MM-DD HH:mm:ss"
 *              19 character + 1 EOS
 *              !! Be careful - could not pre-check the buffer length !!
 *              !! Please provide 20 length buffer !!
 */
size_t DateTime_PrintDateTimeToString(char *message, DateTime_t *dateTime)
{

    if ((message == NULL) || (dateTime == NULL))
    {
        return 0;
    }

#ifdef STRING_SPRINTF_EXTENDED_ENABLE
    return usnprintf(message, 20,
            "%04d-%02d-%02d %02d:%02d:%02d",
            dateTime->date.year+2000,
            dateTime->date.month,
            dateTime->date.day,
            dateTime->time.hour,
            dateTime->time.minute,
            dateTime->time.second);
#else
    return usnprintf(message, 20,
            "%d-%d-%d %d:%d:%d",
            dateTime->date.year+2000,
            dateTime->date.month,
            dateTime->date.day,
            dateTime->time.hour,
            dateTime->time.minute,
            dateTime->time.second);
#endif
}



/**
 * @brief       Compare DateTimes
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
 * @brief       Different of two DateTime
 * @retval      seconds
 *              if retval > 0, 1. DateTime is larger (later)
 *              if retval < 0, 1. DateTime is smaller (former)
 *              if retval == 0, wrong or no different
 */
int32_t DateTime_CalculateDifferentOf2DateTime(DateTime_t *dateTime1, DateTime_t *dateTime2)
{
    /* Calculate seconds of DateTime */
    bool isOk = true;
    uint32_t seconds1;
    uint32_t seconds2;
    int32_t different;

    isOk &= DateTime_CheckDateTime(dateTime1);
    isOk &= DateTime_CheckDateTime(dateTime2);

    if (isOk)
    {
        /* OK */
        seconds1 = DateTime_CalculateDateTimeSecond(dateTime1);
        seconds2 = DateTime_CalculateDateTimeSecond(dateTime2);
        different = seconds1 - seconds2;
    }
    else
    {
        /* Wrong */
        different = 0;
    }

    return different;
}



/**
 * @brief       Calculate DateTime second
 *              Result it is elapsed seconds from 2000.01.01, 00:00:00
 *              If DateTime is smaller than 2000..., the result is 0
 * @return      Elapsed time in seconds
 */
static uint32_t DateTime_CalculateDateTimeSecond(DateTime_t *dateTime)
{
    uint32_t second = 0;
    uint8_t i;

    /* Check DateTime, it is valid? */
    if (DateTime_CheckDateTime(dateTime))
    {
        /* If valid... */
        /* Calculate "full" years */
        for (i = 0; i < dateTime->date.year; i++)
        {
            /* One year second = days of year * hours of day * minutes of hour * seconds of minute */
            second += 365 * 24 * 60 * 60;
            /* Check, this year is leap year? */
            if (DateTime_CheckItIsLeapYear(i))
            {
                /* Add one day */
                second += 24 * 60 * 60;
            }
        }

        /* Add part year's second */
        /* Month */
        for (i = 1; i < dateTime->date.month; i++)
        {
            second += DateTime_GetDaysOfMonth(dateTime->date.year, i) * 24 * 60 * 60;
        }

        second += (dateTime->date.day -1) * 24 * 60 * 60;   /* Days */
        second += dateTime->time.hour * 60 * 60;            /* Hours */
        second += dateTime->time.minute * 60;               /* Minutes */
        second += dateTime->time.second;                    /* Seconds */
    }
    else
    {
        second = 0;                                         /* Wrong */
    }

    return second;
}



/**
 * @brief       Check parameter year is Leap year?
 * @param       year    year after 2000 (e.g.: 2017 --> 17)
 * @retval      true    If leap year
 * @retval      false    If not leap year
 */
static bool DateTime_CheckItIsLeapYear(uint8_t year)
{
    bool isLeapYear = false;

    if ((year % 400) == 0)
    {
        isLeapYear = true;
    }
    else if ((year % 100) == 0)
    {
        isLeapYear = false;
    }
    else if ((year % 4) == 0)
    {
        isLeapYear = true;
    }
    else
    {
        isLeapYear = false;
    }

    return isLeapYear;
}



/**
 * @brief        Get days of Month
 * @retval        Parameter month's day number
 */
static uint8_t DateTime_GetDaysOfMonth(uint8_t year, uint8_t month)
{
    uint8_t days;
    bool valid;

    /* Check parameters */
    if ((month == 0) || (month > 12))
    {
        valid = false;
    }
    else if (year > 99)
    {
        valid = false;
    }
    else
    {
        valid = true;
    }

    if (valid)
    {
        /* If parameters are good, get days */
        /* f ((month == 2) && ((year & 0x03) == 0)) */
        if ((month == 2) && (!DateTime_CheckItIsLeapYear(year)))
        {
            /* February and leap year */
            days = 29;
        }
        else
        {
            days = days_of_month[month - 1];
        }
    }
    else
    {
        days = 0;
    }

    return days;
}



/**
 * @brief       Step DateTime with more seconds
 */
void DateTime_StepMoreSecond(DateTime_t *dateTime, uint32_t stepSeconds)
{
    uint32_t i;

    if (stepSeconds < DATETIME_STEPSECOND_MAX_LIMIT)
    {
        for (i = 0; i < stepSeconds; i++)
        {
            DateTime_StepOneSecond(dateTime);
        }
    }
    /* TODO: Else: do not steps, because too much seconds received */
}



/**
 * @brief        DateTime Second++
 */
void DateTime_StepOneSecond(DateTime_t *dateTime)
{
    /* TODO: Check pointer */

    ++dateTime->time.second;

    if (dateTime->time.second > 59)
    {
        dateTime->time.second = 0;
        ++dateTime->time.minute;

        if (dateTime->time.minute > 59)
        {
            dateTime->time.minute = 0;
            ++dateTime->time.hour;

            if (dateTime->time.hour > 23)
            {
                dateTime->time.hour = 0;
                ++dateTime->date.day;

                uint8_t days = DateTime_GetDaysOfMonth(dateTime->date.year, dateTime->date.month);

                if (dateTime->date.day > days)
                {
                    dateTime->date.day  = 1;
                    ++dateTime->date.month;

                    if (dateTime->date.month > 12)
                    {
                        dateTime->date.month = 1;
                        ++dateTime->date.year;

                        if (dateTime->date.year > 99)
                        {
                            dateTime->date.year = 0;
                        }
                    }
                }
            }
        }
    }
}



/**
 * @brief       Add one minute to actual DateTime
 * @note        If minute is "59" the next value will "0" (and the hour is not stepped!)
 *              Only can be use for "settings" not for periodically stepping
 */
void DateTime_AddMinute(DateTime_t *dateTime)
{
    if (dateTime == NULL)
        return;

    if (dateTime->time.minute >= 59)
    {
        dateTime->time.minute = 0;
    }
    else
    {
        dateTime->time.minute++;
    }
}


/* TODO: Idea, if more minute / hour added: dateTime->time.minute = (dateTime->time.minute + 1) % 60; */



/**
 * @brief       Add one hour to actual DateTime
 * @note        If hour is "23" the next value will "0" (and the day is not stepped!)
 */
void DateTime_AddHour(DateTime_t *dateTime)
{
    if (dateTime == NULL)
        return;

    if (dateTime->time.hour >= 23)
    {
        dateTime->time.hour = 0;
    }
    else
    {
        dateTime->time.hour++;
    }

    /* ateTime->time.hour = (dateTime->time.hour + 1) % 24; */
}



/**
 * @brief       DateTime Unit test
 */
#ifdef CONFIG_MODULE_DATETIME_UNITTEST_ENABLE
/**
 * @brief       String module Unit Test
 */
uint32_t DateTime_UnitTest(void)
{
    /* Test variables */
    DateTimeCompare_t comp;
    bool result;

    /* Start of unittest */
    UnitTest_Start("DateTime", __FILE__);

    /*                Check DateTime_CompareDateTime function            */

    /* Equal date */
    DateTime_t test1 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
    DateTime_t test2 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
    comp = DateTime_CompareDateTime(&test1, &test2);
    UNITTEST_ASSERT(comp == DateTimeCompare_Equal, "DateTime_CompareDateTime error");

    /* First older */
    DateTime_t test3 = { .date.year=17, .date.month=1, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
    DateTime_t test4 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
    comp = DateTime_CompareDateTime(&test3, &test4);
    UNITTEST_ASSERT(comp == DateTimeCompare_FirstOldSecondNew, "DateTime_CompareDateTime error");

    /* First older */
    DateTime_t test5 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
    DateTime_t test6 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=1 };
    comp = DateTime_CompareDateTime(&test5, &test6);
    UNITTEST_ASSERT(comp == DateTimeCompare_FirstOldSecondNew, "DateTime_CompareDateTime error");

    /* First newer */
    DateTime_t test7 = { .date.year=18, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
    DateTime_t test8 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
    comp = DateTime_CompareDateTime(&test7, &test8);
    UNITTEST_ASSERT(comp == DateTimeCompare_FirstNewSecondOld, "DateTime_CompareDateTime error");

    /* First newer */
    DateTime_t test9 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=13, .time.second=0 };
    DateTime_t test10 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
    comp = DateTime_CompareDateTime(&test9, &test10);
    UNITTEST_ASSERT(comp == DateTimeCompare_FirstNewSecondOld, "DateTime_CompareDateTime error");


    /*                Check DateTime_CheckDateTime function            */
    /* Check these dateTimes are valid? */
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
    UNITTEST_ASSERT(result == true, "DateTime_CheckDateTime error");

    /* Check these dateTimes are invalids? */
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
    UNITTEST_ASSERT(result == false, "DateTime_CheckDateTime error");


    /*            DateTime_ConvertStringToDateTime        */
    DateTime_t test300 = { 0 };
    char strDateTime1[DATETIME_STRING_MAX_LENGTH] = { "2017-02-03 12:12:00" };
    result = DateTime_ConvertStringToDateTime(strDateTime1, &test300);
    UNITTEST_ASSERT(result == true, "DateTime_ConvertStringToDateTime error");
    UNITTEST_ASSERT(((test300.date.year          == 17)
                        && (test300.date.month   == 2)
                        && (test300.date.day     == 3)
                        && (test300.time.hour    == 12)
                        && (test300.time.minute  == 12)
                        && (test300.time.second  == 0)),
            "DateTime_ConvertStringToDateTime error");


    /*            DateTime_PrintDateTimeToString            */
    DateTime_t test400 = { .date.year=17, .date.month=2, .date.day=3, .time.hour=12, .time.minute=12, .time.second=0 };
    char strDateTime2[DATETIME_STRING_MAX_LENGTH];
    DateTime_PrintDateTimeToString(strDateTime2, &test400);
  #ifdef STRING_SPRINTF_EXTENDED_ENABLE
    result = !StrCmp(strDateTime2, "2017-02-03 12:12:00");
  #else
    result = !StrCmp(strDateTime2, "2017-2-3 12:12:0");
  #endif
    UNITTEST_ASSERT(result == true, "DateTime_ConvertStringToDateTime error");


    /*            DateTime_CalculateDifferentOf2DateTime        */

    /* 1 second */
    DateTime_t test500 = { { 17, 05, 06 }, { 20, 42, 00} };
    DateTime_t test501 = { { 17, 05, 06 }, { 20, 42, 01} };
    int32_t test500_diff = DateTime_CalculateDifferentOf2DateTime(&test500, &test501);
    UNITTEST_ASSERT(test500_diff == -1, "DateTime_CalculateDifferentOf2DateTime error");

    /* 1 day */
    DateTime_t test502 = { { 17, 05, 06 }, { 20, 42, 00} };
    DateTime_t test503 = { { 17, 05, 07 }, { 20, 42, 00} };
    int32_t test502_diff = DateTime_CalculateDifferentOf2DateTime(&test502, &test503);
    UNITTEST_ASSERT(test502_diff == -(24 * 60 * 60), "DateTime_CalculateDifferentOf2DateTime error");

    /* 1 month */
    DateTime_t test504 = { { 17, 06, 06 }, { 20, 42, 00} };
    DateTime_t test505 = { { 17, 05, 06 }, { 20, 42, 00} };
    int32_t test504_diff = DateTime_CalculateDifferentOf2DateTime(&test504, &test505);
    UNITTEST_ASSERT(test504_diff == (31 * 24 * 60 * 60), "DateTime_CalculateDifferentOf2DateTime error");

    /* 1 year */
    DateTime_t test506 = { { 18, 05, 07 }, { 20, 42, 00} };
    DateTime_t test507 = { { 17, 05, 07 }, { 20, 42, 00} };
    int32_t test506_diff = DateTime_CalculateDifferentOf2DateTime(&test506, &test507);
    UNITTEST_ASSERT(test506_diff == (365 * 24 * 60 * 60), "DateTime_CalculateDifferentOf2DateTime error");


    /*            DateTime_StepMoreSecond            */
    /* Step 1 second */
    DateTime_t test601 = { { 17, 05, 07 }, { 20, 42, 00} };
    DateTime_t test602 = { { 17, 05, 07 }, { 20, 42, 01} };
    DateTime_StepMoreSecond(&test601, 1);
    UNITTEST_ASSERT(!memcmp(&test601, &test602, sizeof(DateTime_t)), "DateTime_Steps error");

    /* Step from 2017.12.31 23:59:59 -> to 2018.01.01 00:00:00 */
    DateTime_t test605 = { { 17, 12, 31 }, { 23, 59, 59 } };
    DateTime_t test606 = { { 18, 1, 1 }, { 0, 0, 0 } };
    DateTime_StepMoreSecond(&test605, 1);
    UNITTEST_ASSERT(!memcmp(&test605, &test606, sizeof(DateTime_t)), "DateTime_Steps error");

    /* Step 62 second */
    DateTime_t test603 = { { 17, 05, 07 }, { 20, 42, 59} };
    DateTime_t test604 = { { 17, 05, 07 }, { 20, 44, 01} };
    DateTime_StepMoreSecond(&test603, 62);
    UNITTEST_ASSERT(!memcmp(&test603, &test604, sizeof(DateTime_t)), "DateTime_Steps error");

    /* Step 60 seconds */
    DateTime_t test700tst = { { 18, 1, 1 }, { 0, 0, 0 } };
    DateTime_t test700res = { { 18, 1, 1 }, { 0, 1, 0 } };
    DateTime_StepMoreSecond(&test700tst, 60);
    UNITTEST_ASSERT(!memcmp(&test700tst, &test700res, sizeof(DateTime_t)), "DateTime_Steps error");

    /* Step 600 seconds */
    DateTime_t test701tst = { { 18, 1, 1 }, { 0, 0, 0 } };
    DateTime_t test701res = { { 18, 1, 1 }, { 0, 10, 0 } };
    DateTime_StepMoreSecond(&test701tst, 600);
    UNITTEST_ASSERT(!memcmp(&test701tst, &test701res, sizeof(DateTime_t)), "DateTime_Steps error");

    /* Step too much: DATETIME_STEPSECOND_MAX_LIMIT */
    DateTime_t test702tst = { { 18, 1, 1 }, { 0, 0, 0 } };
    DateTime_t test702res = { { 18, 1, 1 }, { 0, 0, 0 } };
    DateTime_StepMoreSecond(&test702tst, DATETIME_STEPSECOND_MAX_LIMIT);
    /* Expected result: Do not modify */
    UNITTEST_ASSERT(!memcmp(&test702tst, &test702res, sizeof(DateTime_t)), "DateTime_Steps error");


    /*            Test DateTime_AddMinute()            */

    /* Add one minute in simple situation */
    DateTime_t test800tst = { { 18, 1, 1 }, { 0, 0, 0 } };
    DateTime_t test800res = { { 18, 1, 1 }, { 0, 1, 0 } };
    DateTime_AddMinute(&test800tst);
    UNITTEST_ASSERT(!memcmp(&test800tst, &test800res, sizeof(DateTime_t)), "DateTime_AddMinute error");

    /* Add one minute at year's end */
    DateTime_t test801tst = { { 18, 12, 31 }, { 23, 59, 30 } };
    DateTime_t test801res = { { 18, 12, 31 }, { 23,  0, 30 } };
    DateTime_AddMinute(&test801tst);
    UNITTEST_ASSERT(!memcmp(&test801tst, &test801res, sizeof(DateTime_t)), "DateTime_AddMinute error");

    DateTime_t *test802tst = NULL;
    DateTime_AddMinute(test802tst);
    /* Cannot test the result */


    /*            Test DateTime_AddHour()            */

    /* Add one hour in simple situation */
    DateTime_t test900tst = { { 18, 1, 1 }, { 0, 0, 0 } };
    DateTime_t test900res = { { 18, 1, 1 }, { 1, 0, 0 } };
    DateTime_AddHour(&test900tst);
    UNITTEST_ASSERT(!memcmp(&test900tst, &test900res, sizeof(DateTime_t)), "DateTime_AddHour error");

    /* Add one hour at year's end */
    DateTime_t test901tst = { { 18, 12, 31 }, { 23, 30, 0 } };
    DateTime_t test901res = { { 18, 12, 31 }, {  0, 30, 0 } };
    DateTime_AddHour(&test901tst);
    UNITTEST_ASSERT(!memcmp(&test901tst, &test901res, sizeof(DateTime_t)), "DateTime_AddHour error");

    DateTime_t *test902tst = NULL;
    DateTime_AddHour(test902tst);
    /* Cannot test the result */


    /* TODO: Add more DateTime UnitTests */


    /* Finish */
    return UnitTest_End();
}
#endif


