/*
 *		String.c
 *		Created on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		String manipulation functions
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include <stdarg.h>		// for "..." parameters in printf function
#include "GenericTypeDefs.h"
#include "MathHelper.h"
#include "MemHandler.h"
#include "StringHelper.h"

#ifdef MODULE_STRING_UNITTEST_ENABLE
	#include "unittest.h"
#endif



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_DEBUG_MODE
#define STRING_ASSERT(_e)			ASSERT(_e)
#else
#define STRING_ASSERT(_e)
#endif

#define STRING_SIZE_MAX				(1024U)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local functions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Convert signed decimal to string
 * \note	Only max INT_MAX / 2 number can be converted
 * \return	created string length
 */
uint8_t SignedDecimalToString(int32_t value, char *str)
{
	uint8_t length = 0;
	
	// Check pointer
	if (str == NULL)
	{
		return 0;
	}

	if (value < 0)	// if negative decimal num
	{
		str[length++] = '-';
		value = (uint32_t) (value * (-1));	// Sign +
	}

	// Return with length
	return (length + UnsignedDecimalToString (value, &str[length]));
}



size_t SignedDecimalToStringSafe(int32_t value, char * str, size_t maxLength)
{
	size_t length = 0;

	// Check pointer
	if (str == NULL)
	{
		return 0;
	}

	if (value < 0)	// if negative decimal num
	{
		if (maxLength > 0)
		{
			str[length] = '-';
			length++;
			value = (uint32_t) (value * (-1));	// Sign +
		}
		else
		{
			// Cannot print
			return 0;
		}
	}

	// Add number
	length += UnsignedDecimalToStringSafe(value, &str[length], (maxLength - length));

	return length;
}



/**
 * \brief	Convert unsigned decimal to string
 * \return	created string length
 */
uint8_t UnsignedDecimalToString(uint32_t value, char *str)
{
	uint8_t length = 0;

	// Check pointer
	if (str == NULL)
	{
		return 0;
	}

	// Largest num: 1xxxxxx...
	uint32_t decade = 1000000000;

	// Find first "largest" decimal digit
	while (decade > value && decade > 1)
		decade /= 10;

	do
	{
		// Put next digit
		str[length] = ((value/decade) + '0');
		length++;

		value %= decade;			// Value - first digit
		decade /= 10;				// /10
	}
	while (decade >= 1);

	str[length] = '\0';				// End character

	return length;
}



/**
 * \brief	uint32_t to string (with length)
 */
size_t UnsignedDecimalToStringSafe(uint32_t value, char *str, size_t maxLength)
{
	size_t length = 0;
	bool isStarted = false;

	if (str == NULL || maxLength == 0)
		return 0;

	// Largest num: 1xxxxxx...
	uint32_t decade = 1000000000;

	/// TODO: Implement without "isStarted"
	while ((decade > 1) && ((length+1) < maxLength))
	{
		if ((value >= decade) || (isStarted == true))
		{
			// Put first digit
			str[length] = ((value/decade) + '0');
			length++;
			isStarted = true;
		}

		value %= decade;			// Value - first digit
		decade /= 10;				// /10
	}

	// TODO: Merge this "last digit" with above code
	if ((length+1) < maxLength)
	{
		str[length] = (value + '0');	// Last digit
		length++;
	}

	str[length] = '\0';				// End character

	return length;
}



/**
 * \brief	Calculate unsigned decimal number "string length"
 * \return	length of number's string
 */
uint8_t UnsignedDecimalLength(uint32_t value)
{
	uint8_t length = 0;

	if (value == 0)
	{
		length = 1;
	}

	while (value > 0)
	{
		// value : 10
		value /= 10;
		length++;
	}

	return length;
}



/**
 * \brief	Unsigned decimal (uint32_t) to String with fill (a character to x length)
 */
uint8_t UnsignedDecimalToStringFill(uint32_t value, char *str, uint8_t fillLength, char fillCharacter)
{
	uint8_t length = 0;

	length = UnsignedDecimalLength(value);

	if (length >= fillLength)
	{
		// Not need fill
		// Put number
		length = UnsignedDecimalToString(value, str);
	}
	else
	{
		uint8_t i;

		// Need fill
		for (i = 0; i < (fillLength - length); i++)
		{
			str[i] = fillCharacter;
		}
		// Put number
		length = i;
		length += UnsignedDecimalToString(value, &str[length]);
	}

	return length;
}



/**
 * \brief	Convert signed decimal (int32_t) to string with fill
 */
uint8_t SignedDecimalToStringFill(int32_t value, char *str, uint8_t fillLength, char fillCharacter)
{
	uint8_t length = 0;
	uint8_t i;

	if (value >= 0)
	{
		// Sign +
		length = UnsignedDecimalToStringFill((uint32_t)value, str, fillLength, fillCharacter);
	}
	else
	{
		// Sign -

		value *= -1;	// Sign swap

		// Check "fill character"
		if (fillCharacter == ' ')
		{
			// ' ', put sign at last
			// "    -123"
			length = UnsignedDecimalLength(value);
			if (length > fillLength)
			{
				length = SignedDecimalToString(-value, str);			// Not need fill
			}
			else
			{
				// Need fill
				// "    -123"
				for (i=0; i<(fillLength-length-1); i++)
				{
					str[i] = ' ';
				}
				length = i;	// sign
				str[length++] = '-';
				length += UnsignedDecimalToString(value,&str[length]);
			}
		}
		else
		{
			// "-0000123"
			length = 0;
			str[length++] = '-';	// Print '-'
			length += UnsignedDecimalToStringFill((uint32_t)value,&str[length],fillLength,fillCharacter);
		}
	}

	return length;
}



/**
 * \brief	Convert a octet (0-15) to Hexa character ('0' - '9' - 'A' - 'F')
 * \return	character (octet)
 */
char HexToHexChar(uint8_t value)
{
	// TODO: Rename hex --> to nibble
	char hexChar;

	if (value <= 9)
	{
		// 0- 9
		hexChar = (value + '0');
	}
	else if ((value >= 10) && (value <= 15))
	{
		// A-F
		hexChar = (value - 10 + 'A');
	}
	else
	{
		hexChar = 'x';
	}

	return hexChar;
}



/**
 * \brief	Convert a byte/octet to hexadecimal string
 * \return	Length
 */
uint8_t ByteToHexaString(uint8_t byte, char *str)
{
	uint8_t length = 0;
	uint8_t hex;

	// First hex
	hex = (byte >> 4);
	str[length++] = HexToHexChar(hex);

	// Second hex
	hex = (byte & 0x0F);
	str[length++] = HexToHexChar(hex);

	// Put end char
	str[length] = '\0';

	return length;
}



/**
 * \brief	Convert decimal number to binary string
 * 			For example: 10 (decimal) --> "1010" (binary)
 */
uint8_t DecimalToBinaryString(uint32_t value, char *str, uint8_t maxLength)
{
	uint8_t i;
	int8_t bitIndex;

	// Search first '1' bit:
	bitIndex = GetLargestBitIndex(value);
	if (bitIndex < 0)
	{
		// Print one '0'
		bitIndex = 0;
	}

	// When reach this, bitIndex found the first '1'bit
	for (i = 0; i < maxLength; i++)
	{
		// It '1' or '0'?
		if (value & (1 << bitIndex))
		{
			str[i] = '1';
		}
		else
		{
			str[i] = '0';
		}
		// If reach end, break
		if (bitIndex == 0)
		{
			i++;
			break;
		}
		bitIndex--;
	}

	str[i] = '\0';		// Put end character

	return i;
}



/**
 * \brief	Convert value to hexadecimal string
 * \return	created string length
 */
uint8_t DecimalToHexaString(uint32_t value, char *str, uint8_t length)
{
	uint8_t i;
	uint8_t octet;

	// Check parameters
	if ((length > 8) || (length == 0))
	{
		return 0;
	}

	for (i = 0; i < length; i++)
	{
		// Convert next byte
		octet = (uint8_t)(0x0F & (value >> ((length-i-1)*4)));
		str[i] = HexToHexChar (octet);
	}

	str[length] = '\0';

	return length;
}



/**
 * \brief	Convert float value to String
 * \return	Length
 */
uint8_t FloatToString(float value, char *str, uint8_t integerLength, uint8_t fractionLength)
{
	uint8_t num;
	uint8_t length = 0;
	uint32_t calcValue;

	// Sign
	if (value < 0)
	{
		// Put '-'
		str[length++] = '-';
		value = (value * (-1));	// make positive
	}

	// Integer: minimum interLength length (if integer part is longer then this num, it printed)
	calcValue = (uint32_t)value;
	length += UnsignedDecimalToStringFill(calcValue, &str[length], integerLength, ' ');

	// If has fractionLength parameter (=/= 0), print it
	if (fractionLength)
	{
		// Point '.'
		str[length++] = '.';

		// Fraction:
		// float : 4.567
		// fractionLength: 4
		// string: 4.5670

		// TODO: Make with multiplex (e.g. 0.567 -> * 1000 = 567 --> UnsignedDecimalToString...

		// 4.567 --> 0.567 --> 5670
		// Only fraction
		value = (value - (uint32_t)value);

		// * 10, and write
		while (fractionLength--)
		{
			// 0.567 --> 5.67
			value *= 10; 			// "shift left" = *10
			// 5.67 --> 5
			num = (uint8_t)value;	// integer value (MSB octet)
			// 5.67 - 5
			value -= num;			// value--
			str[length++] = num + '0';
		}
	}
	// If hasn't fractionLength parameter, '.' and fraction part not printed

	// Put end char
	str[length] = '\0';

	return length;
}



/**
 * \brief	Look the string is hexa or not
 * \str		Null terminated string
 * \return	How many hexa characters are in the string
 */
uint8_t StringIsHexadecimalString(const char *str)
{
	// TODO: uint8_t length to size_t?
	uint8_t length = 0;
	uint8_t i;

	// Check parameters
	if (str == NULL)
	{
		return 0;
	}
	
	for (i = 0; str[i] != '\0'; i++)
	{
		// Number all? 0-9, A-F, a-f
		if (IsHexChar(str[i]))
		{
			length++;
		}
		else
		{
			return 0;
		}
	}

	return length;
}



/**
 * \brief	Check the string, it is unsigned decimal number?
 * \return	> 0 : decimal number length
 * 			0 not number
 */
uint8_t StringIsUnsignedDecimalString(const char *str)
{
	uint8_t length = 0;
	uint8_t i;

	for (i = 0; str[i] != '\0'; i++)
	{
		if (IsDecimalChar(str[i]))
		{
			length++;
		}
		else
		{
			// Return 0, if not number
			return 0;
		}
	}

	return length;
}



/**
 * \brief	Check the string, it is signed decimal number?
 * \return	> 0 : decimal number length
 * 			0 not number
 */
uint8_t StringIsSignedDecimalString(const char *str)
{
	uint8_t length = 0;

	// Sign
	if (str[0] == '-' )
	{
		length++;
	}
	else if (str[0] == '+')
	{
		length++;
	}

	// Check after sign
	length += StringIsUnsignedDecimalString (&str[length]);

	return length;
}



/**
 * \brief	Check, it is hexadecimal number character?
 * \return	true, if hex number
 * 			false, if no hex number
 */
bool IsHexChar(const char c)
{
	bool isOk = false;

	if ((c >= '0') && (c <='9'))
	{
		isOk = true;
	}
	else if ((c >= 'A') && (c <= 'F'))
	{
		isOk = true;
	}
	else if ((c >= 'a') && (c <= 'f'))
	{
		isOk = true;
	}

	return isOk;
}



/**
 * \brief	Check, it is decimal number character?
 * \return	true, if number
 * 			false, if no number
 */
bool IsDecimalChar (const char c)
{
	bool isOk = false;

	if ((c >= '0') && (c <='9'))
	{
		isOk = true;
	}

	return isOk;
}



/**
 * \brief	'Binary string' to 'uint32_t'
 *			Example: "01110" -> 14
 */
bool StringBinaryToNum(const char *str, uint32_t *num)
{
	uint32_t value = 0;
	uint8_t i;

	if (StringLength(str) > 32)
	{
		return false;
	}

	// TODO: Check Prefix ??
	for (i = 0; str[i]; i++)
	{
		if (str[i] == '1')
		{
			value |= 0x01;
		}
		else if (str[i] == '0')
		{
			// Do nothing, it is ok
			// TODO: Optimize: this if (str[i] != '0') { return false; }, and not need else
			value |= 0x00;
		}
		else
		{
			return false;		// Wrong character here
		}

		// Shift left
		if (str[i+1])
		{
			value <<= 1;
		}
	}

	// Finish
	*num = value;

	return true;
}



/**
 * \brief	Convert Hex character to octet (0-9, A-F)
 * \param	c	ASCII character, which you need to convert to value
 * \param	*hexValue	the value
 * \return	result
 * 			true	successful
 * 			false	failed
 */
bool HexCharToHex(const char c, uint8_t *hexValue)
{
	bool isOk = true;

	*hexValue = 0;
	if ((c >= '0') && (c <='9'))
	{
		*hexValue = c - '0';
	}
	else if ((c >= 'A') && (c <= 'F'))
	{
		*hexValue = c - 'A' + 10;
	}
	else if ((c >= 'a') && (c <= 'f'))
	{
		*hexValue = c - 'a' + 10;
	}
	else
	{
		isOk = false;
	}

	return isOk;
}



/**
 * \brief	Convert two hexadecimal character (string) to number (byte)
 * \return	true, if successful
 * 			false, if has error
 */
bool StringHexByteToNum(const char *str, uint8_t *byte)
{
	uint8_t calculatedByte1 = 0;
	uint8_t calculatedByte2 = 0;
	bool result = true;

	result &= HexCharToHex(str[0], &calculatedByte1);
	result &= HexCharToHex(str[1], &calculatedByte2);

	*byte = ((calculatedByte1 & 0x0F)<<4) | (calculatedByte2 & 0x0F);

	return result;
}



/**
 * \brief	Convert Hex string to number(integer)
 * \return	true, if successful
 * 			false, if has error
 */
bool StringHexToNum(const char *str, uint32_t *hexValue)
{
	uint8_t i;
	uint32_t calculatedValue = 0;
	uint8_t calculatedByte = 0;

	// Check length
	if (StringIsHexadecimalString(str) == 0)
	{
		return false;									// Wrong string or wrong octetlength
	}

	// Create hexValue
	for (i = 0; str[i] != '\0'; i++)
	{
		// shift <<4 + add next hex
		if (IsHexChar(str[i]))
		{
			if (HexCharToHex(str[i], &calculatedByte))
			{
				calculatedValue <<= 4;					// Shift one byte left original value
				calculatedValue += calculatedByte;		// Add new value
			}
			else
			{
				return false;							// Failed (hex character)
			}
		}
		else
		{
			return false;
		}
		if (i >= 8)
		{
			break;										// Has an "uint32_t"
		}
	}

	*hexValue = calculatedValue;

	return true;
}



/**
 * \brief Convert decimal character to number (byte)
 * \return	value (number)
 */
uint8_t DecimalCharToNum(char c)
{
	uint8_t value = 0;

	if ((c >= '0') && (c <= '9'))
	{
		value = (c - '0');
	}

	return value;
}



/**
 * \brief	Convert Unsigned decimal string to integer
 * \return	true, if successful
 * 			false, if has error
 */
bool StringToUnsignedDecimalNum(const char *str, uint32_t *value)
{
	uint32_t calculatedValue = 0;
	uint8_t i;
	uint8_t decimal;

	if ((str == NULL) || (value == NULL))
	{
		return false;
	}

	for (i = 0; str[i] != '\0'; i++)
	{
		if (IsDecimalChar(str[i]))
		{
			decimal = DecimalCharToNum(str[i]);
			calculatedValue *= 10;			// Shift left 1* =  *10
			calculatedValue += decimal;		// Add new value
		}
		else
		{
			return false;					// Wrong character
		}

		if (i > 10)
		{
			return false;					// To long num
		}
	}

	*value = calculatedValue;

	return true;
}



/**
 * \brief	Convert signed decimal string to signed integer
 * \return	true, if successful
 * 			false, if has error
 */
bool StringToSignedDecimalNum(const char *str, int32_t *value)
{
	uint32_t convertValue = 0;
	bool isNegative = false;
	uint8_t length = 0;

	if ((str == NULL) || (value == NULL))
		return false;

	if (str[0] == '-')
	{
		// It is negative
		isNegative = true;
		length = 1;
	}
	else if (str[0] == '+')
	{
		// Positive
		isNegative = false ;
		length = 1;
	}
	else
	{
		// Positive
		isNegative = false;
		length = 0;
	}

	if (StringToUnsignedDecimalNum(&str[length], &convertValue))
	{
		if (isNegative)
		{
			// Negative, converted value need * -1
			*value = -1 * (int32_t)convertValue;
		}
		else
		{
			// Positive
			*value = convertValue;
		}
		return true;
	}
	else
	{
		return false;
	}
}



/**
 * \brief	Convert Float string to num(float)
 * \return	true, if successful
 * 			false, if has error
 */
bool StringToFloat(const char *str, float *num)
{
	bool isNegative = false;
	uint8_t stringLength;
	uint8_t i;
	uint8_t length;
	uint8_t pointCnt ;
	uint32_t integer;
	float fractionPart;
	char numString[10] = { 0 };

	if ((str == NULL) || (num == NULL))
		return false;

	stringLength = StringLength(str);

	// Sign
	if (str[0] == '-')
	{	// It is negative
		isNegative = true;
		length = 1;
	}
	else if (str[0] == '+')
	{
		isNegative = false ;
		length = 1;
	}
	else
	{
		isNegative = false;
		length = 0;
	}


	// Integer

	// Find '.'
	for (i = 0; str[length+i] != '.'; i++)
	{
		numString[i] = str[length + i];
		if (i >= stringLength)
		{
			return false;		// Error, overflow
		}
	}

	numString[i] = '\0';
	pointCnt = length+i;


	// Convert integer
	if (!StringToUnsignedDecimalNum(numString, &integer))
	{
		return false;			// Error with convert integer part
	}

	// Integer (before .)
	*num = integer;

	// Convert fraction
	if (!StringToUnsignedDecimalNum(&str[pointCnt + 1], &integer))
	{
		return false;			// Error with convert fraction part
	}

	fractionPart = integer;
	// We converted after point part, ".567", but we need to shift right
	for (i = 0; i < (stringLength - pointCnt - 1); i++ )
	{
		// >> point
		// Example: 567 == > 56.7
		fractionPart /= 10;
	}
	// Ready fraction

	*num += fractionPart;		// Add Integer and Fraction

	if (isNegative)
	{
		*num = (-1) * (*num);
	}

	return true;
}



/**
 * \brief	Convert String to bool value
 */
bool StringToBool(const char * str, bool * val)
{
	if (str == NULL || val == NULL)
		return false;

	bool isBool = false;
	bool boolVal = false;
	uint32_t num;

	// Check it is decimal?
	if (StringToUnsignedDecimalNum(str, &num))
	{
		if (num == 1)
		{
			boolVal = true;
			isBool = true;
		}
		else if (num == 0)
		{
			boolVal = false;
			isBool = true;
		}
		else
		{
			isBool = false;			// Wrong num (not 0, and not 1)
		}
	}
	else
	{
		// Not number

		// Check it is "true" / "false" ?
		if (!StrCmp((const char*)str, "true"))
		{
			boolVal = true;
			isBool = true;
		}
		else if (!StrCmp((const char*)str, "false"))
		{
			boolVal = false;
			isBool = true;
		}
		// Check it is "on" / "off"?
		else if (!StrCmp((const char*)str, "on"))
		{
			boolVal = true;
			isBool = true;
		}
		else if (!StrCmp((const char*)str, "off"))
		{
			boolVal = false;
			isBool = true;
		}
		// Check it is "enable" / "disable"
		else if (!StrCmp((const char*)str, "enable"))
		{
			boolVal = true;
			isBool = true;
		}
		else if (!StrCmp((const char*)str, "disable"))
		{
			boolVal = false;
			isBool = true;
		}
		// Check it is "set" / "reset"
		else if (!StrCmp((const char*)str, "set"))
		{
			boolVal = true;
			isBool = true;
		}
		else if (!StrCmp((const char*)str, "reset"))
		{
			boolVal = false;
			isBool = true;
		}
		else
		{
			// Not good "string"
			isBool = false;;
		}
	}

	*val = boolVal;
	return isBool;
}



/**
 * \brief	Convert char to lowercase
 */
void ToLower(char * c)
{
	if ((*c >= 'A') && (*c <= 'Z'))
	{
		// Need to change to small letter
		// length between Big Letter and small letter
		*c = *c - ('A' - 'a');
	}
}



/**
 * \brief	Convert char to UpperCase
 */
void ToUpper(char * c)
{
	if ((*c >= 'a') && (*c <= 'z'))
	{
		// Need to change to small letter
		// length between Big Letter and small letter
		*c = *c + ('A' - 'a');
	}
}



/**
 * \brief	Calculate string length
 * \return	length
 * 			0, if null string
 */
size_t StringLength(const char *str)
{
	size_t length = 0;

	if (str == NULL)
	{
		return 0;
	}

	// Added max length checking
	while ((length < STRING_SIZE_MAX) && (str[length] != '\0'))
	{
		length++;	// Length = string length
	}

	return length;
}



/**
 * \brief	Compare two string
 * \return	1, if not equal
 * 			0, if equal
 */
uint8_t StrCmp(const char *str1, const char *str2)
{
	size_t i = 0;

	// Check parameters
	if ((str1 == NULL) && (str2 == NULL))
	{
		// Equal, because both of string are NULL
		return 0;
	}
	else if ((str1 == NULL) || (str2 == NULL))
	{
		// One of parameter is NULL... not equal
		return 1;
	}

	// Check characters
	while ((*str1 == *str2) && *str1 && *str2 && (i < STRING_SIZE_MAX))
	{
		str1++;
		str2++;
		i++;
	}

	if (*str1 == *str2)
	{
		return 0;		// last character is equal
	}
	else
	{
		// If string last character is not equal
		return 1;
	}
}



/**
 * \brief	Compare two string (with first string length)
 * \return	1, if not equal
 * 			0, if equal
* 	\note	Be careful! If second string longer than first string and the begin of string are equal, the return value will be "equal" (0)
 */
uint8_t StrCmpFirst(const char *str1, const char *str2)
{
	size_t i = 0;

	if ((str1 == NULL) || (str2 == NULL))
	{
		// One of parameter is NULL... not equal
		return 1;
	}

	while (*str1)
	{
		if ((*str1 != *str2) || (i >= STRING_SIZE_MAX))
		{
			return 1;	// not equal
		}
		str1++;
		str2++;
		i++;
	}

	return 0;			// Good, equal
}



/**
 * \brief	Compare string in fix length
 * \return	1, if not equal
 * 			0, if equal
 */
uint8_t StrCmpWithLength(const char * str1, const char *str2, size_t length)
{
	size_t i;

	// Check pointers + length
	if ((str1 == NULL) || (str2 == NULL) || (length == 0) || (length > STRING_SIZE_MAX))
	{
		return 1;
	}

	// Compare characters
	for (i = 0; i < length; i++)
	{
		if (*str1 != *str2)
		{
			return 1;	// not equal
		}
		str1++;
		str2++;		
	}
	
	return 0;		// equal	
}



/**
 * \brief	Copy string to *dest pointer
 * \return	Copied string length
 */
size_t StrCpy(char *dest, const char *str)
{
	size_t i;
	size_t length;

	// Check parameter
	if ((dest == NULL) || (str == NULL))
	{
		return 0;
	}

	length = StringLength(str);

	// Copy characters
	for (i = 0; i < length; i++)
	{
		dest[i] = str[i];
	}
	dest[length] = '\0';

	return length;					// Return length
}



/**
 * \brief	Copy fix length string
 * \return	String length (=parameter)
 */
size_t StrCpyFixLength(char *dest, const char *str, size_t length)
{
	size_t i;

	if ((dest == NULL) || (str == NULL) || (length == 0))
	{
		return 0;
	}

	// Copy characters
	for (i = 0; i < length; i++)
	{
		dest[i] = str[i];
	}

	// TODO: put '\0'? It is correct?
	dest[i] = '\0';

	return length;
}



/**
 * \brief	Copy fix length string
 * \return	String length (=parameter)
 */
size_t StrCpyFixLengthWithFillCharacter(char *dest, const char *str, size_t length, char fillChar)
{
	size_t i = 0;

	// Check parameters (Not need check str, because str is NULL, we should copy 'fillChar' with 'length' num to 'dest')
	if (dest == NULL)
		return 0;

	// If str is NULL, copy fillChar(s) to *dest
	if (str != NULL)
	{
		// Copy characters
		for (i = 0; (i < length) && (str[i]); i++)
		{
			dest[i] = str[i];
		}
	}

	// Fill with character after string
	if (i < length)
	{
		for (; i < length; i++)
		{
			dest[i] = fillChar;
		}
	}

	return length;
}



/**
 * \brief	Copy ended string with max length
 * \return	String length
 */
size_t StrCpyMax(char *dest, const char *str, size_t maxLength)
{
	size_t length = 0;
	length = StringLength(str);

	// Check parameters
	if (str == NULL || dest == NULL || maxLength == 0 || length == 0)
	{
		return 0;
	}

	// Check long
	if (length >= maxLength)
	{
		length = maxLength - 1;					// Too long, only can copy "length-1" characters + '\0'
	}

	StrCpyFixLength(dest, str, length);			// Copy characters
	dest[length] = '\0';						// Put end

	return length;
}



/**
 * \brief	Copy character x count
 * \return	String length
 */
size_t StrCpyCharacter(char *dest, char c, size_t num)
{
	size_t i;

	// Check parameters
	if (dest == NULL || num == 0)
	{
		return 0;
	}

	// Copy characters
	for (i = 0; i < num; i++)
	{
		dest[i] = c;
	}

	dest[i] = '\0';				// Put end

	return i;
}



/**
 * \brief	Append string to dest's end
 * \length	New string's length (original + copied)
 * \note	dest buffer can be overflowed, because there is no overflow checking!
 */
size_t StrAppend(char *dest, const char *str)
{
	size_t length = 0;

	if (dest == NULL)
	{
		return length;
	}

	length = StringLength(dest);

	if (str != NULL)
	{
		length += StrCpy(&dest[length], str);
	}

	return length;
}



/**
 * \brief	Append string to dest's end
 * \length	New string's length (original + copied)
 */
size_t StrAppendSafe(char *dest, const char *str, size_t destLength)
{
	size_t length = 0;

	// Check parameters (str not need check)
	if (dest == NULL)
	{
		return length;
	}

	length = StringLength(dest);

	if ((str != NULL) && (destLength > length))
	{
		size_t copyLength = StringLength(str);
		if (destLength <= (length + copyLength))
		{
			// How many characters can we copy?
			copyLength = destLength - length - 1;	// Last character should be '\0'
		}
		length += StrCpyFixLength(&dest[length], str, copyLength);
	}

	return length;
}



/**
 * \brief	Trim string (cut space and others at end)
 * \note	Be careful, only call with changeable string!
 */
size_t StrTrim(char *str)
{
	// Check pointer
	if (str == NULL)
	{
		return 0;
	}

	size_t length = StringLength(str) - 1;
	size_t i;

	// Start from end of string
	for (i = length; i > 0; i--)
	{
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\r' || str[i] == '\n')
		{
			str[i] = '\0';		// Replace ' ' to \0
		}
		else
		{
			// 'i' value contain the new length
			break;				// Not space, good character, end
		}
	}

	return i;
}



/**
 * \brief	Convert string to lowercase
 * \str		'\0' terminated string
 * \note	Be careful, only call with changeable string!
 */
void StringLower(char * str)
{
	size_t i;

	if (str == NULL)
	{
		return;
	}

	for (i = 0; str[i] != '\0'; i++)
	{
		ToLower(&str[i]);
	}
}



/**
 * \brief	Convert string to UPPERCASE
 * \str		'\0' terminated string
 * \note	Be careful, only call with changeable string!
 */
void StringUpper(char * str)
{
	size_t i;

	if (str == NULL)
	{
		return;
	}

	for (i = 0; str[i] != '\0'; i++)
	{
		ToUpper(&str[i]);
	}
}



/**
 * \brief	Find character in string
 * \param	*str	scanned string
 * 			*findCharacter	which character should find
 * \return	'findCharacter' position in 'str' (pointer)
 * 			NULL	if not found
 */
const char * STRING_FindCharacter(const char *str, const char findCharacter)
{
	size_t i;
	// TODO: Without StringLength
	size_t length = StringLength(str);

	// Check parameter
	if ((str == NULL) || (length == 0))
	{
		return NULL;
	}

	// Search first equal character
	for (i = 0; i < length; i++)
	{
		if (str[i] == findCharacter)
		{
			// Equal
			return (char *)&str[i];
		}
	}

	return NULL;
}



/**
 * \brief	Find characters in string
 */
const char * STRING_FindCharacters(const char *str, const char *findCharacters)
{
	size_t i;
	uint8_t j;
	const char * findPos = NULL;

	// Check parameter
	if ((str == NULL) || (findCharacters == NULL))
	{
		return NULL;
	}

	// Search in string
	for (i = 0; i < STRING_SIZE_MAX && str[i] != '\0'; i++)
	{
		// Check with finding characters
		for (j = 0; j < 255 && findCharacters[j] != '\0'; j++)
		{
			if (str[i] == findCharacters[j])
			{
				// Equal
				findPos = &str[i];
				break;
			}
		}

		if (findPos != NULL)
			break;
	}

	return findPos;
}



/**
 * \brief	Find small string in big string
 * 			like strstr()
 * \param	*str	scanned string
 * 			*fincString	which find
 * \return	'findString' position in 'str' (pointer)
 * 			NULL	if not found
 */
const char * STRING_FindString(const char *str, const char *findString)
{
	size_t i;
	size_t length = StringLength(str);
	size_t findStringLength = StringLength(findString);
	const char * findPos = NULL;

	// Check parameters
	if ((str == NULL) || (findString == NULL) || (length == 0) || (findStringLength == 0) || findStringLength > length)
	{
		return NULL;
	}

	// Search first equal character
	for (i = 0; i < (length - findStringLength); i++)
	{
		if (findString[0] == str[i])
		{
			// First character is equal
			if (!StrCmpWithLength(findString, &str[i], findStringLength))
			{
				findPos = (char *)&str[i];
				break;
			}
		}
	}

	return findPos;
}



/**
 * \brief	Separators
 * 			like strtok()
 * \note	Be careful, pointers to original (source) string
 * 			source string need to be changeable!
 */
uint8_t STRING_Splitter(char *source, char delimiterChar, char **separated, uint8_t parameterMaxCount)
{
	size_t i;
	size_t j;
	uint8_t parameters = 0;

	// Check parameters
	if (source == NULL || separated == NULL || parameterMaxCount == 0)
	{
		return 0;			// Fail parameters
	}

	separated[0] = NULL;	// Make empty

	// Split
	j = 0;
	for (i = 0; source[i]; i++)
	{
		// There is delimiter?
		if ((source[i] == delimiterChar) || (source[i+1] == '\0'))
		{
			// Found delimiter or end character
			if (source[i] == delimiterChar)
			{
				source[i] = '\0';
			}
			if (j == 0)
			{
				// one length parameter // TODO: Do with more beautiful
				separated[parameters] = &source[i];
			}
			parameters++;
			j = 0;
			if (parameters >= parameterMaxCount)
			{
				// maximal tokens found
				break;
			}
			else
			{
				separated[parameters] = NULL;
			}
		}
		else
		{
			// Not ended, count
			if (j == 0)
			{
				separated[parameters] = &source[i];		// New string found
			}
			j++;
		}
	}

	return parameters;
}



#ifndef STRING_SPRINTF_EXTENDED_ENABLE
/**
 * \brief	Instead of sprintf()
 *			Used '%' parameters
 *			%d, %u, %x, %X, %b, %c, %s, %f
 *			In other settings: %w, %h, %b (hexadecimals)
 *
 * \note	!! Be careful: 'str' can be overflow!!
 */
size_t string_printf(char *str, const char *format, va_list ap)
{
	// TODO: Use "new" typedefs

	// Type variables
	char	*p;			// step on format string
	char	*sval;		// string
	int		ival;		// int
	unsigned int uival;	// uint
	float	flval;		// float
	char 	cval;		// character

	char *string = str;

	// Check parameters
	if (str == NULL || format == NULL)
		return 0;

	for (p = (char *)format; *p; p++)				// p to EOS
	{
		if (*p != '%')								// copy, if not '%'
		{
			*string = *p;							// copy to string
			string++;
		}
		else
		{
			// '%' character
			p++;

			// Process next character (after '%', or etc)
			switch (*p)
			{
				case 'd':
					// signed (int)
					ival = va_arg(ap, int);	// Decimal = signed int (~int32_t)
					string += SignedDecimalToString(ival, string);
					break;

				case 'u':
					// unsigned (int)
					uival = va_arg(ap, int);// Uint = Unsigned int (~uint32_t)
					string += UnsignedDecimalToString(uival, string);
					break;

					// TODO: Create 'x' and 'X' to different
				case 'x':
				case 'X':
					// %x - Hex - parameterized byte num
					uival = va_arg(ap, unsigned int);
					string += DecimalToHexaString(uival, string, 8);
					break;

#if defined(STRING_HEXADECIMAL_FORMATS)
				case 'w':
					// Hex // 32 bits	// 8 hex	// 4 byte
					uival = va_arg(ap, unsigned int);
					string += DecimalToHexaString(uival, string, 8);
					break;

				case 'h':
					// Hex // 16 bits	// 4 hex	// 2 byte
					ival = va_arg(ap, int);
					string += DecimalToHexaString(ival, string, 4);
					break;

				case 'b':
					// Hex	// 8 bits	// 2 hex	// 1 byte
					ival = va_arg(ap, int);
					string += DecimalToHexaString(ival, string, 2);
					break;
#else
				case 'b':
					// Binary print (from uint32_t)
					uival = va_arg(ap,  unsigned int);
					string += DecimalToBinaryString(uival, string, 33);
					break;
#endif
				case 'c':
					// %c - char
					cval = va_arg(ap, int);						// Char
					// Default: copy one character
					*string = cval;							// Copy to string
					string++;
					*string = '\0';
					break;

				case 'f':
					// %f - float
					flval = va_arg(ap, double);					// Double / Float
					string += FloatToString(flval, string, 0, 6);
					break;

				case 's':
					// %s - string
					sval = va_arg(ap, char*);					// String
					// Standard string copy
					string += StrCpy(string, sval);
					break;

				case 'p':
					// %p - pointer - print address in hexadecimal
					uival = va_arg(ap, unsigned int);
					string += DecimalToHexaString(uival, string, 8);
					break;

				default:
					*string = *p;					// Other, for example: '%'
					string++;
					break;
			}
		}	// End of '%'

	}	// End of for loop

	// string's end
	*string = '\0';

	// Return with length
	return (string-str);
}



#else	// #ifdef STRING_SPRINTF_EXTENDED_ENABLE



/**
 * \brief	Instead of sprintf()
 *			Used '%' parameters
 *			%d, %u, %x, %X, %b, %c, %s, %f
 *			In other settings: %w, %h, %b (hexadecimals)
 *			Other processed settings:
 *			%8d --> 8 length decimal number, filled with space
 *			%8u --> 8 length unsigned decimal number, filled with space
 *			%8x --> 8 length hexa string
 *			%b  --> print binary string (E.g. ("%b", 5) --> "101"
 *			%8c --> print character 8 times
 *			%8s --> print string (with max length)
 *			%.2f  --> print float in this format: "123.00"
 *
 * \note	!! Be careful: 'str' can be overflow!!
 */
size_t string_printf(char *str, const char *format, va_list ap)
{
	// TODO: Use "new" typedefs

	// Type variables
	char	*p;			// step on format string
	char	*sval;		// string
	int		ival;		// int
	unsigned int uival;	// uint
	float	flval;		// float
	char 	cval;		// character

	char *string = str;

	// Process variables
	bool paramHasLength;
	uint8_t paramNum1;
	uint8_t paramNum2;
	char fillCharacter;

	// Check parameters
	if (str == NULL || format == NULL)
		return 0;

	for (p = (char *)format; *p; p++)				// p to EOS
	{
		if (*p != '%')								// copy, if not '%'
		{
			*string = *p;							// copy to string
			string++;
		}
		else
		{
			// '%' character
			p++;
			paramNum1 = 0;	// for standard %08x
			paramNum2 = 0;
			paramHasLength = false;
			fillCharacter = ' ';

			// Check %...x (parameter after %, before x, u, f, s)
			// Next character is num?
			if (IsDecimalChar(*p))
			{
				// It is num (1. param)
				// Replace, if has two parameter
				paramNum2 = DecimalCharToNum(*p);
				fillCharacter = *p;
				paramHasLength = true;
				p++;

				if (IsDecimalChar(*p))
				{
					// xy
					// It is num (2. param)
					paramNum1 = paramNum2;
					paramNum2 = DecimalCharToNum(*p);
					p++;
				}
				else if (*p == '.')
				{
					// x.
					p++;
					if (IsDecimalChar(*p))
					{
						// x.y
						paramNum1 = paramNum2;
						paramNum2 = DecimalCharToNum(*p);
						p++;
					}
					else
					{
						// x.?
						// x = paramNum1
						// ?=0 now, for correct float printing
						// Do not step p pointer, because this character can be f, x, etc.
						paramNum1 = paramNum2;
						paramNum2 = 0;
					}
				}
				else
				{
					// x		==>		x = fill character, y = length
					// If only has one parameter
					fillCharacter = ' ';	// Blank character
				}
			}

			// Process next character (after '%', or etc)
			switch (*p)
			{
				case 'd':
					// signed (int)
					ival = va_arg(ap, int);	// Decimal = signed int (~int32_t)
					string += SignedDecimalToStringFill(ival, string,
							paramNum2, fillCharacter);
					break;

				case 'u':
					// unsigned (int)
					uival = va_arg(ap, int);// Uint = Unsigned int (~uint32_t)
					string += UnsignedDecimalToStringFill(uival, string,
							paramNum2, fillCharacter);
					break;

					// TODO: Create 'x' and 'X' to different
				case 'x':
				case 'X':
					// %x - Hex - parameterized byte num
					uival = va_arg(ap, unsigned int);
					if (paramHasLength)
					{
						string += DecimalToHexaString(uival, string, paramNum2);
					}
					else
					{
						string += DecimalToHexaString(uival, string, 8);
					}
					break;
#if defined(STRING_HEXADECIMAL_FORMATS)
				case 'w':
					// Hex // 32 bits	// 8 hex	// 4 byte
					uival = va_arg(ap, unsigned int);
					string += DecimalToHexaString(uival, string, 8);
					break;

				case 'h':
					// Hex // 16 bits	// 4 hex	// 2 byte
					ival = va_arg(ap, int);
					string += DecimalToHexaString(ival, string, 4);
					break;

				case 'b':
					// Hex	// 8 bits	// 2 hex	// 1 byte
					ival = va_arg(ap, int);
					string += DecimalToHexaString(ival, string, 2);
					break;
#else
				case 'b':
					// Binary print (from uint32_t)
					uival = va_arg(ap,  unsigned int);
					string += DecimalToBinaryString(uival, string, 33);
					break;
#endif
				case 'c':
					// %c - char
					cval = va_arg(ap, int);						// Char
					if (paramHasLength)
					{
						// Copy more character
						string += StrCpyCharacter(string, cval, (paramNum1 * 10 + paramNum2));
					}
					else
					{
						// Default: copy one character
						*string = cval;							// Copy to string
						string++;
						*string = '\0';
					}
					break;

				case 'f':
					// %f - float
					flval = va_arg(ap, double);					// Double / Float
					if (paramHasLength)
					{
						string += FloatToString(flval, string, paramNum1, paramNum2);
					}
					else
					{
						string += FloatToString(flval, string, 0, 6);
					}
					break;

				case 's':
					// %s - string
					sval = va_arg(ap, char*);					// String
					if (paramHasLength)
					{
						// String copy with length
						uint8_t stringLength = paramNum1*10 + paramNum2;
						string += StrCpyFixLengthWithFillCharacter(string, sval, stringLength, ' ');
					}
					else
					{
						// Standard string copy
						string += StrCpy(string, sval);
					}
					break;

				case 'p':
					// %p - pointer - print address in hexadecimal
					uival = va_arg(ap, unsigned int);
					string += DecimalToHexaString(uival, string, 8);
					break;

				default:
					*string = *p;					// Other, for example: '%'
					string++;
					break;
			}
		}	// End of '%'

	}	// End of for loop

	// string's end
	*string = '\0';

	// Return with length
	return (string-str);
}
#endif	// #ifdef STRING_SPRINTF_EXTENDED_ENABLE



/**
 * \brief	Instead of snprintf()
 *			Used '%' parameters
 *			%d, %u, %x, %X, %w, %h, %b, %c, %s, %f
 */
size_t string_printf_safe(char *str, size_t maxLen, const char *format, va_list ap)
{
	// TODO: Use "new" typedefs

	// Type variables
	char	*p;			// step on format string
	char	*sval;		// string
	int		ival;		// int
	unsigned int uival;	// uint
	float	flval;		// float
	char 	cval;		// character

	// String variables
	size_t length = 0;
	size_t remainLength = maxLen - 1;

	// Process variables
	bool paramHasLength;
	uint8_t paramNum1;
	uint8_t paramNum2;
	char fillCharacter;

	// Check parameters
	if (str == NULL || format == NULL)
		return 0;

	for (p = (char *)format; *p; p++)				// p to EOS
	{
		if ((*p != '%') && (remainLength > 0))	// copy, if not '%'
		{
			str[length] = *p;						// copy to string
			length++;
			remainLength--;
		}
		else
		{
			// '%' character
			p++;
			paramNum1 = 0;	// for standard %08x
			paramNum2 = 0;
			paramHasLength = false;
			fillCharacter = ' ';

			// Check %...x (parameter after %, before x, u, f, s)
			// Next character is num?
			if (IsDecimalChar(*p))
			{
				// It is num (1. param)
				// Replace, if has two parameter
				paramNum2 = DecimalCharToNum(*p);
				fillCharacter = *p;
				paramHasLength = true;
				p++;

				if (IsDecimalChar(*p))
				{
					// xy
					// It is num (2. param)
					paramNum1 = paramNum2;
					paramNum2 = DecimalCharToNum(*p);
					p++;
				}
				else if (*p == '.')
				{
					// x.
					p++;
					if (IsDecimalChar(*p))
					{
						// x.y
						paramNum1 = paramNum2;
						paramNum2 = DecimalCharToNum(*p);
						p++;
					}
					else
					{
						// x.?
						// x = paramNum1
						// ?=0 now, for correct float printing
						// Do not step p pointer, because this character can be f, x, etc.
						paramNum1 = paramNum2;
						paramNum2 = 0;
					}
				}
				else
				{
					// x		==>		x = fill character, y = length
					// If only has one parameter
					fillCharacter = ' ';	// Blank character
				}
			}

			// Process next character (after '%', or etc)
			switch (*p)
			{
				case 'd':
				{
					// signed (int)
					ival = va_arg(ap, int);	// Decimal = signed int (~int32_t)
					uint8_t decLen = SignedDecimalToStringSafe(ival, &str[length], remainLength+1);
					length += decLen;
					remainLength -= decLen;
					(void)fillCharacter;
					// TODO: with Fill function
					/*
					if (paramNum2 <= remainLength)
					{
						uint8_t decLen = SignedDecimalToStringFill(ival, string,
								paramNum2, fillCharacter);
						length += decLen;
						remainLength -= decLen;
					}
					else
					{
					}
					*/
				}
					break;

				case 'u':
				{
					// unsigned (int)
					uival = va_arg(ap, int);// Uint = Unsigned int (~uint32_t)
					uint8_t decLen = UnsignedDecimalToStringSafe(uival, &str[length], remainLength+1);
					length += decLen;
					remainLength -= decLen;
					(void)fillCharacter;
					// TODO: with Fill function
					/*
					string += UnsignedDecimalToStringFill(uival, string,
							paramNum2, fillCharacter);
					*/
				}
					break;

					// TODO: Create 'x' and 'X' to different
				case 'x':
				case 'X':
					// %x - Hex - parameterized byte num
					uival = va_arg(ap, unsigned int);
					// TODO: Not implemented function (for length safe)
					if (paramHasLength)
					{
						if (paramNum2 > remainLength)
						{
							paramNum2 = remainLength;
						}
					}
					else
					{
						if (remainLength < 8)
						{
							paramNum2 = remainLength;
						}
					}
					uint8_t hexLen = DecimalToHexaString(uival, &str[length], 8);
					length += hexLen;
					remainLength -= hexLen;
					break;
#if defined(STRING_HEXADECIMAL_FORMATS)
				case 'w':
					// Hex // 32 bits	// 8 hex	// 4 byte
					uival = va_arg(ap, unsigned int);
					string += DecimalToHexaString(uival, string, 8);
					break;

				case 'h':
					// Hex // 16 bits	// 4 hex	// 2 byte
					ival = va_arg(ap, int);
					string += DecimalToHexaString(ival, string, 4);
					break;

				case 'b':
					// Hex	// 8 bits	// 2 hex	// 1 byte
					ival = va_arg(ap, int);
					string += DecimalToHexaString(ival, string, 2);
					break;
#else
				case 'b':
					// Binary print (from uint32_t)
					uival = va_arg(ap,  unsigned int);
					// TODO: Not implemented function (for length safe)
					//DEBUG_BREAKPOINT();
					uint8_t binLength = DecimalToBinaryString(uival, &str[length], 33);
					length += binLength;
					remainLength -= binLength;
					break;
#endif
				case 'c':
					// %c - char
					cval = va_arg(ap, int);						// Char
					if (paramHasLength)
					{
						// Copy more character
						uint8_t cNum = paramNum1 * 10 + paramNum2;
						if (cNum > remainLength)
							cNum = remainLength;
						length += StrCpyCharacter(&str[length], cval, cNum);
						remainLength -= cNum;
					}
					else
					{
						// Default: copy one character
						if (remainLength > 0)
						{
							str[length] = cval;							// Copy to string
							length++;
							remainLength--;
							str[length] = '\0';
						}
					}
					break;

				case 'f':
					// %f - float
					flval = va_arg(ap, double);					// Double / Float
					(void)flval;
					// TODO: Not implemented function (for length safe)
					//DEBUG_BREAKPOINT();
					uint8_t floatLength;
					if (paramHasLength)
					{
						floatLength = FloatToString(flval, &str[length], paramNum1, paramNum2);
					}
					else
					{
						floatLength = FloatToString(flval, &str[length], 0, 6);
					}
					length += floatLength;
					remainLength -= floatLength;
					break;

				case 's':
					// %s - string
					sval = va_arg(ap, char*);					// String
					if (paramHasLength)
					{
						// String copy with length
						uint8_t stringLength = paramNum1*10 + paramNum2;
						if (stringLength > remainLength)
							stringLength = remainLength;
						stringLength = StrCpyFixLengthWithFillCharacter(&str[length], sval, stringLength, ' ');
						length += stringLength;
						remainLength -= stringLength;
					}
					else
					{
						// Standard string copy
						uint8_t stringLength = StrCpyMax(&str[length], sval, remainLength);
						length += stringLength;
						remainLength -= stringLength;
					}
					break;

				default:
					if (remainLength > 0)
					{
						str[length] = *p;					// Other, for example: '%'
						length++;
					}
					break;
			}
		}	// End of '%'

	}	// End of for loop

	// string's end
	str[length] = '\0';

	// Return with length
	return length;
}



/**
 * \brief	Function like sprintf(); Print to string
 */
size_t usprintf(char *str, const char *format, ...)
{
	size_t length = 0;

	if ((str == NULL) || (format == NULL))
		return 0;

	STRING_ASSERT(MEM_IN_RAM(str, 0));

	va_list ap;									// argument pointer
	va_start(ap, format); 						// ap on arg
	length = string_printf(str, format, ap);	// Separate and process
	va_end(ap);						 			// Cleaning after end

	return length;
}



/**
 * \brief	Function like snprintf(); Print to string
 */
size_t usnprintf(char * str, size_t maxLen, const char * format, ...)
{
	size_t resultLength = 0;

	if ((str == NULL) || (format == NULL))
		return 0;

	STRING_ASSERT(MEM_IN_RAM(str, maxLen));

	va_list ap;									// argument pointer
	va_start(ap, format); 						// ap on arg
	resultLength = string_printf_safe(str, maxLen, format, ap);	// Separate and process
	va_end(ap);						 			// Cleaning after end

	return resultLength;
}



#ifdef MODULE_STRING_UNITTEST_ENABLE
/**
 * \brief	String module Unit Test
 */
void STRING_UnitTest(void)
{
	char buffer[30];
	uint8_t value8;
	bool result;
	uint32_t value32;
	int32_t ivalue32;
	float fvalue;
	char *splitted[10];
	const char * cpString;


	// Start of unittest
	UnitTest_Start("String", __FILE__);


	/// String compare StrCmp()
	// Equal:
	UNITTEST_ASSERT(!StrCmp("example", "example"), "StrCmp error");
	// Not equal:
	UNITTEST_ASSERT(StrCmp("example", "example1"), "StrCmp error");
	UNITTEST_ASSERT(StrCmp("example1", "example2"), "StrCmp error");
	UNITTEST_ASSERT(StrCmp("example1", "example"), "StrCmp error");


	// StrCmpFirst()
	// Equal:
	UNITTEST_ASSERT(!StrCmpFirst("example", "example"), "StrCmp error");
	UNITTEST_ASSERT(!StrCmpFirst("example", "example1"), "StrCmp error");
	// Not equal:
	UNITTEST_ASSERT(StrCmpFirst("example1", "example2"), "StrCmp error");
	UNITTEST_ASSERT(StrCmpFirst("example1", "example"), "StrCmp error");


	// TODO: Use STRING_SPRINTF_EXTENDED_ENABLE define

	// Float print tests

	// "123.339996" ~ like "%0.6"
	usprintf(buffer,"%f",123.34);
	UNITTEST_ASSERT(!StrCmp(buffer, "123.339996"), "Float error");

	// "123"
	usprintf(buffer,"%1.0f",123.34);
	UNITTEST_ASSERT(!StrCmp(buffer, "123"), "Float error");

	// "123.3"
	usprintf(buffer, "%1.1f",123.34);
	UNITTEST_ASSERT(!StrCmp(buffer, "123.3"), "Float error");

	// "123.33999"
	usprintf(buffer,"%1.5f",123.34);
	UNITTEST_ASSERT(!StrCmp(buffer, "123.33999"), "Float error");

	// "  123.3"
	usprintf(buffer, "%5.1f",123.34);
	UNITTEST_ASSERT(!StrCmp(buffer, "  123.3"), "Float error");

	// "123.33999"
	usprintf(buffer,"%5.5f",123.34);
	UNITTEST_ASSERT(!StrCmp(buffer, "  123.33999"), "Float error");

	// TODO: "%.2f?


	// Integer print tests

	// Printed: "123"
	usprintf(buffer, "%0u",123);
	UNITTEST_ASSERT(!StrCmp(buffer, "123"), "Integer error");

	// Printed:	"123"
	usprintf(buffer, "%1u",123);
	UNITTEST_ASSERT(!StrCmp(buffer, "123"), "Integer error");

	// Printed: " 123"
	usprintf(buffer, "%4u",123);
	UNITTEST_ASSERT(!StrCmp(buffer, " 123"), "Integer error");

	// Printed: "      123"
	usprintf(buffer, "%9u",123);
	UNITTEST_ASSERT(!StrCmp(buffer, "      123"), "Integer error");

	// Printed: "00123", it is OK
	usprintf(buffer, "%05u",123);
	UNITTEST_ASSERT(!StrCmp(buffer, "00123"), "Integer error");

	// Integer print tests (wrong examples):
	usprintf(buffer, "%A5",123);		// Printed: "A5u", because 'A' is not a number
	UNITTEST_ASSERT(!StrCmp(buffer, "A5"), "Integer error");
	usprintf(buffer, "%-5u",123);		// Printed: "-5u", because '-' is not a number
	UNITTEST_ASSERT(!StrCmp(buffer, "-5u"), "Integer error");

	// Signed Integer print tests:

	// Printed: "-123"
	usprintf(buffer, "%0d",-123);
	UNITTEST_ASSERT(!StrCmp(buffer, "-123"), "Integer error");

	// Printed:	"-123"
	usprintf(buffer, "%1d",-123);
	UNITTEST_ASSERT(!StrCmp(buffer, "-123"), "Integer error");

	// Printed: "-123"
	usprintf(buffer, "%4d",-123);
	UNITTEST_ASSERT(!StrCmp(buffer, "-123"), "Integer error");

	// Printed: "     -123"
	usprintf(buffer, "%9d",-123);
	UNITTEST_ASSERT(!StrCmp(buffer, "     -123"), "Integer error");

	// Printed: "-0123", it is OK
	usprintf(buffer, "%05d",-123);
	UNITTEST_ASSERT(!StrCmp(buffer, "-00123"), "Integer error");

	// Hexadecimal print tests:
	usprintf(buffer, "0x%x",0xFFFFFFFF);
	UNITTEST_ASSERT(!StrCmp(buffer, "0xFFFFFFFF"), "Hexadecimal error");
	usprintf(buffer, "0x%01x",0xFFFFFFFF);
	UNITTEST_ASSERT(!StrCmp(buffer, "0xF"), "Hexadecimal error");
	usprintf(buffer, "0x%02x",0xFFFFFFFF);
	UNITTEST_ASSERT(!StrCmp(buffer, "0xFF"), "Hexadecimal error");
	usprintf(buffer, "0x%03x",0xFFFFFFFF);
	UNITTEST_ASSERT(!StrCmp(buffer, "0xFFF"), "Hexadecimal error");
	usprintf(buffer, "0x%04x",0xFFFFFFFF);
	UNITTEST_ASSERT(!StrCmp(buffer, "0xFFFF"), "Hexadecimal error");
	usprintf(buffer, "0x%05x",0xFFFFFFFF);
	UNITTEST_ASSERT(!StrCmp(buffer, "0xFFFFF"), "Hexadecimal error");
	usprintf(buffer, "0x%06x",0xFFFFFFFF);
	UNITTEST_ASSERT(!StrCmp(buffer, "0xFFFFFF"), "Hexadecimal error");
	usprintf(buffer, "0x%07x",0xFFFFFFFF);
	UNITTEST_ASSERT(!StrCmp(buffer, "0xFFFFFFF"), "Hexadecimal error");
	usprintf(buffer, "0x%08x",0xFFFFFFFF);
	UNITTEST_ASSERT(!StrCmp(buffer, "0xFFFFFFFF"), "Hexadecimal error");
	usprintf(buffer, "0x%09x",0xFFFFFFFF);
	UNITTEST_ASSERT(!StrCmp(buffer, "0x"), "Hexadecimal error");

	usprintf(buffer, "0x%01x",0x12345678);
	UNITTEST_ASSERT(!StrCmp(buffer, "0x8"), "Hexadecimal error");
	usprintf(buffer, "0x%02x",0x12345678);
	UNITTEST_ASSERT(!StrCmp(buffer, "0x78"), "Hexadecimal error");
	usprintf(buffer, "0x%03x",0x12345678);
	UNITTEST_ASSERT(!StrCmp(buffer, "0x678"), "Hexadecimal error");
	usprintf(buffer, "0x%04x",0x12345678);
	UNITTEST_ASSERT(!StrCmp(buffer, "0x5678"), "Hexadecimal error");
	usprintf(buffer, "0x%05x",0x12345678);
	UNITTEST_ASSERT(!StrCmp(buffer, "0x45678"), "Hexadecimal error");
	usprintf(buffer, "0x%06x",0x12345678);
	UNITTEST_ASSERT(!StrCmp(buffer, "0x345678"), "Hexadecimal error");
	usprintf(buffer, "0x%07x",0x12345678);
	UNITTEST_ASSERT(!StrCmp(buffer, "0x2345678"), "Hexadecimal error");
	usprintf(buffer, "0x%08x",0x12345678);
	UNITTEST_ASSERT(!StrCmp(buffer, "0x12345678"), "Hexadecimal error");
	usprintf(buffer, "0x%09x",0x12345678);
	UNITTEST_ASSERT(!StrCmp(buffer, "0x"), "Hexadecimal error");

	// Binaries printf
	usprintf(buffer, "0b%b",0x000000FF);
	UNITTEST_ASSERT(!StrCmp(buffer, "0b11111111"), "Binary error");
	usprintf(buffer, "0b%b",0x00000055);
	UNITTEST_ASSERT(!StrCmp(buffer, "0b1010101"), "Binary error");

	// String (%s)

	// Standard %s print
	usprintf(buffer, "%s", "text");
	UNITTEST_ASSERT(!StrCmp(buffer, "text"), "String error (%s)");

	// max 5 character
	usprintf(buffer, "%5s", "longtext");
	UNITTEST_ASSERT(!StrCmp(buffer, "longt"), "String error (%s)");

	// 10 character, need fill with ' '
	usprintf(buffer, "%10s", "longtext");
	UNITTEST_ASSERT(!StrCmp(buffer, "longtext  "), "String error (%s)");

	// max 10 character
	usprintf(buffer, "%10s", "toolongtext");
	UNITTEST_ASSERT(!StrCmp(buffer, "toolongtex"), "String error (%s)");



	// string -> num converters

	// string -> decimal

	// Byte

	// Good bytes
	result = StringHexByteToNum("00", &value8);
	UNITTEST_ASSERT(result, "StringByteToNum error");
	UNITTEST_ASSERT((value8 == 0x00), "StringByteToNum error");
	result = StringHexByteToNum("15", &value8);
	UNITTEST_ASSERT(result, "StringByteToNum error");
	UNITTEST_ASSERT((value8 == 0x15), "StringByteToNum error");
	result = StringHexByteToNum("FF", &value8);
	UNITTEST_ASSERT(result, "StringByteToNum error");
	UNITTEST_ASSERT((value8 == 0xFF), "StringByteToNum error");

	// Wrong byte
	result = StringHexByteToNum("FG", &value8);
	UNITTEST_ASSERT(!result, "StringByteToNum error");


	// Hexs

	// Good hex
	result = StringHexToNum("12345678", &value32);
	UNITTEST_ASSERT(result, "StringHexToNum error");
	UNITTEST_ASSERT(value32 == 0x12345678, "StringHexToNum error");

	// Wrong hex
	result = StringHexToNum("123G5678", &value32);
	UNITTEST_ASSERT(!result, "StringHexToNum error");


	// Binaries
	result = StringBinaryToNum("010101", &value32);
	UNITTEST_ASSERT(result, "StringBinaryToNum error");
	UNITTEST_ASSERT(value32 == 0x15, "StringHexToNum error");
	result = StringBinaryToNum("01010101010101010101010101010101", &value32);
	UNITTEST_ASSERT(result, "StringBinaryToNum error");
	UNITTEST_ASSERT(value32 == 0x55555555, "StringHexToNum error");
	result = StringBinaryToNum("010101010101010101010101010101010", &value32);
	UNITTEST_ASSERT(!result, "StringBinaryToNum error");


	// Decimals

	// Good decimals
	result = StringToSignedDecimalNum("-123",&ivalue32);
	UNITTEST_ASSERT(result, "StringToSignedDecimalNum error");
	UNITTEST_ASSERT(ivalue32 == -123, "StringToSignedDecimalNum error");

	result = StringToUnsignedDecimalNum("123",&value32);
	UNITTEST_ASSERT(result, "StringToUnsignedDecimalNum error");
	UNITTEST_ASSERT(value32 == 123, "StringToUnsignedDecimalNum error");

	// Wrong decimals
	result = StringToSignedDecimalNum("-123a",&ivalue32);
	UNITTEST_ASSERT(!result, "StringToSignedDecimalNum error");

	result = StringToUnsignedDecimalNum("-123",&value32);
	UNITTEST_ASSERT(!result, "StringToUnsignedDecimalNum error");

	// Float

	// Good
	result = StringToFloat("-123.3499", &fvalue);
	UNITTEST_ASSERT(result, "StringToFloat error");
	UNITTEST_ASSERT(((fvalue < -123.3498) && (fvalue > -123.3500)), "StringToFloat error");

	result = StringToFloat("-123a.3999", &fvalue);
	UNITTEST_ASSERT(!result, "StringToFloat error");


	// TODO: StringToBool test


	/// String function testing

	// Test: uint8_t StrCpyCharacter(char *dest, char c, uint8_t num)
	StrCpyCharacter(buffer, 'a', 10);
	uint8_t i;
	for (i = 0; i < 10; i++)
	{
		UNITTEST_ASSERT(buffer[i] == 'a', "StrCpyCharacter error");
	}
	// Check end character
	UNITTEST_ASSERT(buffer[10] == '\0', "StrCpyCharacter error");


	// Test: Test StrTrim()
	// TODO: Test return value!
	StrCpy(buffer, "String with spaces end...    ");
	StrTrim(buffer);
	UNITTEST_ASSERT(!StrCmp("String with spaces end...", buffer), "StrTrim error");

	StrCpy(buffer, "End without space.");
	StrTrim(buffer);
	UNITTEST_ASSERT(!StrCmp("End without space.", buffer), "StrTrim error");


	// STRING_FindCharacter()
	StrCpy(buffer, "longtexttofinding");
	// Valid finding
	cpString = STRING_FindCharacter((const char *)buffer, 't');
	UNITTEST_ASSERT(cpString == buffer+4, "FindCharacter wrong find error");
	cpString = STRING_FindCharacter(buffer, 'l');
	UNITTEST_ASSERT(cpString == buffer, "FindCharacter wrong find error");
	// Invalid finding
	cpString = STRING_FindCharacter(buffer, 'z');
	UNITTEST_ASSERT(cpString == NULL, "FindCharacter not find error error");
	// 0 length string
	cpString = STRING_FindCharacter("", 'z');
	UNITTEST_ASSERT(cpString == NULL, "FindCharacter 0 length error");
	// Null pointer
	cpString = STRING_FindCharacter(NULL, 'z');
	UNITTEST_ASSERT(cpString == NULL, "FindCharacter null pointererror");
	// Unchangeable string
	UNITTEST_ASSERT(!StrCmp(buffer, "longtexttofinding"), "FindCharacter changed original string");


	// STRING_FindCharacters()
	StrCpy(buffer, "longtexttofinding");
	// Valid finding
	cpString = STRING_FindCharacters(buffer, "text");
	UNITTEST_ASSERT(cpString == buffer+4, "FindCharacter wrong find error");
	cpString = STRING_FindCharacters(buffer, "long");
	UNITTEST_ASSERT(cpString == buffer, "FindCharacter wrong find error");
	// Invalid finding
	cpString = STRING_FindCharacters(buffer, "z");
	UNITTEST_ASSERT(cpString == NULL, "FindCharacter not find error error");
	// 0 length string
	cpString = STRING_FindCharacters("", "z");
	UNITTEST_ASSERT(cpString == NULL, "FindCharacter 0 length error");
	// Null pointer
	cpString = STRING_FindCharacters(NULL, "z");
	UNITTEST_ASSERT(cpString == NULL, "FindCharacter null pointererror");
	// Unchangeable string
	UNITTEST_ASSERT(!StrCmp(buffer, "longtexttofinding"), "FindCharacter changed original string");


	// STRING_FindString()
	StrCpy(buffer, "longtexttofinding");
	// Valid finding
	cpString = STRING_FindString(buffer, "text");
	UNITTEST_ASSERT(cpString == buffer+4, "FindString error");
	// There is no
	cpString = STRING_FindString(buffer, "wrongtext");
	UNITTEST_ASSERT(cpString == NULL, "FindString error");
	// Overflow
	cpString = STRING_FindString(buffer, "findingoverflow");
	UNITTEST_ASSERT(cpString == NULL, "FindString overflow error");
	// 0 length string
	cpString = STRING_FindString("", "findingoverflow");
	UNITTEST_ASSERT(cpString == NULL, "FindString 0 length error");
	cpString = STRING_FindString("longtexttofinding", "");
	UNITTEST_ASSERT(cpString == NULL, "FindString 0 length error");
	// Test with NULL pointer
	cpString = STRING_FindString("longtexttofinding", NULL);
	UNITTEST_ASSERT(cpString == NULL, "FindString null pointer error");
	cpString = STRING_FindString(NULL, "findingoverflow");
	UNITTEST_ASSERT(cpString == NULL, "FindString null pointer error");
	// Unchangeable string
	UNITTEST_ASSERT(!StrCmp(buffer, "longtexttofinding"), "FindString changed original string");


	// STRING_Splitter()
	StrCpy(buffer, "need to separate this text");
	value8 = STRING_Splitter(buffer, ' ', splitted, 10);
	UNITTEST_ASSERT(value8 == 5, "STRING_Splitter error");
	UNITTEST_ASSERT(!StrCmp(splitted[0],"need"), "STRING_Splitter error");
	UNITTEST_ASSERT(!StrCmp(splitted[1],"to"), "STRING_Splitter error");
	UNITTEST_ASSERT(!StrCmp(splitted[2],"separate"), "STRING_Splitter error");
	UNITTEST_ASSERT(!StrCmp(splitted[3],"this"), "STRING_Splitter error");
	UNITTEST_ASSERT(!StrCmp(splitted[4],"text"), "STRING_Splitter error");
	UNITTEST_ASSERT(splitted[5] == NULL, "STRING_Splitter error");

	StrCpy(buffer, "text");
	value8 = STRING_Splitter(buffer, ' ', splitted, 10);
	UNITTEST_ASSERT(value8 == 1, "STRING_Splitter error");
	UNITTEST_ASSERT(!StrCmp(splitted[0],"text"), "STRING_Splitter error");
	UNITTEST_ASSERT(splitted[1] == NULL, "STRING_Splitter error");

	StrCpy(buffer, "");
	value8 = STRING_Splitter(buffer, ' ', splitted, 10);
	UNITTEST_ASSERT(value8 == 0, "STRING_Splitter error");
	UNITTEST_ASSERT(splitted[0] == NULL, "STRING_Splitter error");


	// TODO:  Test safe (length) functions

	// usnprintf
	usnprintf(buffer, 30, "%d %u 1234 %c %s", 1, 2, 'a', "str");
	UNITTEST_ASSERT(!StrCmp(buffer, "1 2 1234 a str"), "usnprintf error");
	// TODO: Add other test, if usnprintf improved


	// Test StrAppendSafe
	buffer[0] = '\0';
	StrAppendSafe(buffer, "First string, ", 30);
	StrAppendSafe(buffer, "Second string", 30);
	UNITTEST_ASSERT(!StrCmp(buffer, "First string, Second string"), "StrAppendSafe error");

	// Check overflow
	buffer[0] = '\0';
	buffer[19] = 0xAA;
	buffer[20] = 0xBB;
	StrAppendSafe(buffer, "1234567890", 20);
	StrAppendSafe(buffer, "1234567890", 20);
	UNITTEST_ASSERT(!StrCmp(buffer, "1234567890123456789"), "StrAppendSafe error");
	UNITTEST_ASSERT((buffer[19] == '\0'), "StrAppendSafe overflow error");
	UNITTEST_ASSERT((buffer[20] == (char)0xBB), "StrAppendSafe overflow error");


	// StringLower()
	StrCpy(buffer, "123ABCabc$");
	StringLower(buffer);
	UNITTEST_ASSERT(!StrCmp(buffer, "123abcabc$"), "StringLower error");


	// StringUpper()
	StrCpy(buffer, "123ABCabc$");
	StringUpper(buffer);
	UNITTEST_ASSERT(!StrCmp(buffer, "123ABCABC$"), "StringUpper error");


	// End of unittest
	UnitTest_End();
}
#endif	// #ifdef MODULE_STRING_UNITTEST_ENABLE


// Other printf:
/*
// Link: http://electronics.stackexchange.com/questions/206113/how-do-i-use-the-printf-function-on-stm32/206118
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void vprint(const char *fmt, va_list argp)
{
    char string[200];
    if (0 < vsprintf(string,fmt,argp)) // build string
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)string, strlen(string), 0xffffff); // send message via UART
    }
}

void my_printf(const char *fmt, ...) // custom printf() function
{
    va_list argp;
    va_start(argp, fmt);
    vprint(target, fmt, argp);
    va_end(argp);
}
*/


// Other printf:
/*
// Link: http://www.sparetimelabs.com/tinyprintf/tinyprintf.php
*/