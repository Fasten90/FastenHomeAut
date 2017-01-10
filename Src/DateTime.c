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
 */
bool DateTime_ConvertStringToDateTime(const char *string, DateTime_t *dateTime)
{
	DateTimeString_t convertDateTime;
	bool isOk = true;

	if (StringLength(string) != (DATETIME_STRING_MAX_LENGTH - 1))
	{
		// Wrong length
		return false;
	}

	StrCpy((char *)&convertDateTime, string);
	// TODO: scanf

	// Check separators and put 0
	if (convertDateTime.separator_1[0] == '-') convertDateTime.separator_1[0] = '\0';
	else isOk = false;

	if (convertDateTime.separator_2[0] == '-') convertDateTime.separator_2[0] = '\0';
	else isOk = false;

	if (convertDateTime.separator_space[0] == ' ') convertDateTime.separator_space[0] = '\0';
	else isOk = false;

	if (convertDateTime.separator_4[0]== ':') convertDateTime.separator_4[0] = '\0';
	else isOk = false;

	if (convertDateTime.separator_5[0] == ':') convertDateTime.separator_5[0] = '\0';
	else isOk = false;

	// If separator characters are ok
	// TODO: Do no change parameter, while the result is not good?
	if (isOk)
	{
		uint32_t convertValue;
		isOk &= StringToUnsignedDecimalNum(convertDateTime.year, &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 2017, 2050);
		dateTime->year = (uint8_t)(convertValue - 2000);
		isOk &= StringToUnsignedDecimalNum(convertDateTime.month, &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 12);
		dateTime->month = (uint8_t)convertValue;
		isOk &= StringToUnsignedDecimalNum(convertDateTime.day, &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 31);
		dateTime->day = (uint8_t)convertValue;
		isOk &= StringToUnsignedDecimalNum(convertDateTime.hour, &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 24);
		dateTime->hour = (uint8_t)convertValue;
		isOk &= StringToUnsignedDecimalNum(convertDateTime.minute, &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 60);
		dateTime->minute = (uint8_t)convertValue;
		isOk &= StringToUnsignedDecimalNum(convertDateTime.second, &convertValue);
		isOk &= DateTime_CheckValue(convertValue, 0, 60);
		dateTime->second = (uint8_t)convertValue;
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
 * \brief	Print DateTime_t to string
 */
uint8_t DateTime_PrintDateTimeToString(char *message, DateTime_t *dateTime)
{

	return usprintf(message, "%04d-%02d-%02d %02d:%02d:%02d",
			dateTime->year+2000,
			dateTime->month,
			dateTime->day,
			dateTime->hour,
			dateTime->minute,
			dateTime->second);
}

