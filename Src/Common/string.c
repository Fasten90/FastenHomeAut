/*
 *		string.c
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		string manipulation functions
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "include.h"
#include <stdarg.h>		// for "..." parameters in uprintf function
#include "String.h"

#ifdef MODULE_STRING_UNITTEST_ENABLE
#include "unittest.h"
#endif

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local variables
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
	return (length + UnsignedDecimalToString (value,&str[length]));
}



/**
 * \brief	Convert unsigned decimal to string
 * \return	created string length
 */
uint8_t UnsignedDecimalToString(uint32_t value, char *str)
{

	uint8_t length = 0;
	bool isStarted = false;

	// Largest num: 1xxxxxx...
	uint32_t decade = 1000000000;

	while (decade > 1)
	{
		if ((value >= decade) || (isStarted == true))
		{
			// Put first digit
			str[length++] = ((value/decade) + '0');
			isStarted = true;
		}

		// Value - first digit
		value %= decade;

		// /10
		decade /= 10;
	}

	// Last digit
	str[length++] = (value+ '0');

	// End character
	str[length] = '\0';

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
	uint8_t i;

	length = UnsignedDecimalLength(value);

	if (length >= fillLength)
	{
		// Not need fill
		// Put number
		length = UnsignedDecimalToString(value, str);
	}
	else
	{
		// Need fill
		for (i=0; i < (fillLength - length); i++)
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
		length = UnsignedDecimalToStringFill((uint32_t)value,str,fillLength,fillCharacter);
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
				// Not need fill
				length = SignedDecimalToString(-value, str);
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
 * \brief	Convet a octet (0-15) to Hexa character ('0' - '9' - 'A' - 'F')
 * \return	character (octet)
 */
char HexToHexChar(uint8_t value)
{
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
 * \brief	Convert a byte to hexadecimal string
 * \return	Length
 */
uint8_t ByteToHexaString(uint8_t byte, char *str)
{
	uint8_t length = 0;
	uint8_t hex;

	// First hex
	hex = (byte >> 4);
	str[length++] = HexToHexChar (hex);

	// Second hex
	hex = (byte & 0x0F);
	str[length++] = HexToHexChar (hex);

	// Put end char
	str[length] = '\0';

	return length;

}



/**
 * \brief
 */
uint8_t DecimalToBinaryString(uint32_t value, char *str, uint8_t maxLength)
{

	uint8_t i;
	uint8_t bitIndex = 31;

	// Search first '1' bit:
	while (!(value & (1 << bitIndex)) && (bitIndex > 0))
	{
		bitIndex--;
	}

	// When reach this, bitIndex found the first '1'bit
	for (i=0; i<maxLength; i++)
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

	// Put end character
	str[i] = '\0';

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
		octet = (uint8_t)(0x0F&(value >> ((length-i-1)*4)));
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
	length += UnsignedDecimalToStringFill(calcValue,&str[length],integerLength,' ');

	// If has fractionLength parameter (=/= 0), print it
	if (fractionLength)
	{

		// Point '.'
		str[length++] = '.';


		// Fraction:
		// float : 4.567
		// fractionLength: 4
		// string: 4.5670

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
 * \return	How many hexa characters are...
 */
uint8_t StringIsHexadecimalString(const char *str)
{
	uint8_t length = 0;
	uint8_t i;

	// Check parameters
	if (str == NULL)
	{
		return 0;
	}
	
	for (i=0; str[i] != '\0'; i++)
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
	uint8_t count = 0;
	uint8_t i;

	for (i=0; str[i]; i++)
	{
		if (IsDecimalChar(str[i]))
		{
			count++;
		}
		else
		{
			// Return 0, if not number
			return 0;
		}
	}

	return count;
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
	return (length + StringIsUnsignedDecimalString (&str[length]));
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
 * \brief	Binary string to uint32_t
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
	for (i=0; str[i]; i++)
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
			// Wrong character here
			return false;
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
bool StringByteToNum(const char *str, uint8_t *byte)
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
	if ( StringIsHexadecimalString(str) == 0)
	{
		// Wrong string or wrong octetlength
		return false;
	}

	// Create hexValue
	for (i=0; str[i] != '\0'; i++)
	{
		// shift <<4 + add next hex
		if (IsHexChar(str[i]))
		{
			if (HexCharToHex(str[i], &calculatedByte))
			{
				// Shift one byte left original value
				calculatedValue <<= 4;
				// Add new value
				calculatedValue += calculatedByte;
			}
			else
			{
				// Failed (hex character)
				return false;
			}
		}
		else
		{
			return false;
		}
		if (i >= 8)
		{
			// Has an "uint32_t"
			break;
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

	if (str == NULL)
	{
		return false;
	}

	for (i=0; str[i] != '\0'; i++)
	{
		if (IsDecimalChar(str[i]))
		{
			decimal = DecimalCharToNum(str[i]);
			// Shift left 1* =  *10
			calculatedValue *= 10;
			// Add new value
			calculatedValue += decimal;
		}
		else
		{
			// Wrong character
			return false;
		}

		if (i>10)
		{
			// To long num
			return false;
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

	if (StringToUnsignedDecimalNum(&str[length],&convertValue))
	{
		if (isNegative)
		{
			// Negative, converted value need * -1
			*value = -1 * convertValue;
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
bool StringToFloat(const char *str, float *Num)
{
	bool isNegative = false;
	uint8_t stringLength;
	uint8_t i;
	uint8_t length;
	uint8_t pointCnt ;
	uint32_t integer;
	float fractionPart;
	char numString[10] = { 0 };

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
	for (i=0; str[length+i] != '.'; i++)
	{
		numString[i] = str[length+i];
		if (i >= stringLength)
		{
			// Error, overflow
			return false;
		}
	}

	numString[i] = '\0';
	pointCnt = length+i;


	// Convert integer
	if (!StringToUnsignedDecimalNum(numString,&integer))
	{
		// Error with convert integer part
		return false;
	}

	// Integer (before .)
	*Num = integer;

	// Convert fraction
	if (!StringToUnsignedDecimalNum(&str[pointCnt+1],&integer))
	{
		// Error with convert fraction part
		return false;
	}

	fractionPart = integer;
	// We converted after point part, ".567", but we need to shift right
	for (i=0; i < (stringLength-pointCnt-1); i++ )
	{
		// >> point
		// Example: 567 == > 56.7
		fractionPart /= 10;
	}
	// Ready fraction

	// Add Integer and Fraction
	*Num += fractionPart;

	if (isNegative)
	{
		*Num = (-1) * (*Num);
	}

	return true;
}



/**
 * \brief	Calculate string length
 * \return	length
 * 			0, if null string
 */
uint8_t StringLength(const char *str)
{
	uint8_t length = 0;

	if (str == NULL)
	{
		return 0;
	}

	while (str[length] !='\0') length++;	// Length = string length
	return length;
}




/**
 * \brief	Compare two string
 * \return	1, if not equal
 * 			0, if equal
 */
uint8_t StrCmp(const char *str1, const char *str2)
{

	while ( *str1 )
	{
		if ( *str1 !=  *str2 )
		{
			return 1;	// not equal
		}
		str1++;
		str2++;
	}

	if (*str1)
	{
		return 1;		// last str2 char has value, not equal
	}
	else
	{
		return 0;		// last str2 char is '\0'
	}

}



/**
 * \brief	Compare string in fix length
 * \return	1, if not equal
 * 			0, if equal
 */
uint8_t StrCmpWithLength(const char * ch1, const char *ch2, uint8_t length)
{
	
	// Check pointers + length
	if ( ch1 == NULL || ch2 == NULL || length == 0 )
	{
		return 1;
	}

	// Compare characters
	uint8_t i;
	for ( i = 0; i < length; i++ )
	{
		if ( *ch1 !=  *ch2 )
		{
			return 1;	// not equal
		}
		ch1++;
		ch2++;		
	}
	
	return 0;		// equal	
}



/**
 * \brief	Copy string to *dest pointer
 * \return	copied string length
 */
uint8_t StrCpy(char *dest, const char *str)
{
	uint8_t i;
	uint8_t length;

	// Check parameter
	if (dest == NULL)
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

	// Return length
	return length;
}



/**
 * \brief	Copy fix length string
 * \return	String length (=parameter)
 */
uint8_t StrCpyFixLength(char *dest, const char *str, uint8_t length)
{

	uint8_t i;

	// Copy characters
	for ( i = 0; i < length; i++ )
	{
		dest[i] = str[i];
	}

	return length;
}



/**
 * \brief	Copy fix length string
 * \return	String length (=parameter)
 */
uint8_t StrCpyFixLengthWithFillCharacter(char *dest, const char *str, uint8_t length, char fillChar)
{

	uint8_t i;

	// Copy characters
	for (i = 0; (i < length) && (str[i]); i++)
	{
		dest[i] = str[i];
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
uint8_t StrCpyMax(char *dest, const char *str, uint8_t maxLength)
{

	uint8_t length = 0;
	length = StringLength(str);

	// Check parameters
	if (dest == NULL || str == NULL || maxLength == 0)
	{
		return 0;
	}

	// Check long
	if (length >= maxLength)
	{
		// Too long, only can copy "length-1" characters + '\0'
		length = maxLength-1;
	}

	// Copy characters
	StrCpyFixLength(dest, str, length);

	// Put end
	dest[length] = '\0';

	return length;
}



/**
 * \brief	Append string to dest's end
 * \length	New string's length (original + copied)
 */
uint8_t StrAppend(char *dest, const char *str)
{
	uint8_t length = 0;

	length = StringLength(dest);

	length += StrCpy(&dest[length],str);

	return length;

}



/**
 * \brief	Find small string in big string
 * 			like strstr()
 * \param	*str	scanned string
 * 			*fincString	which find
 * \return	'findString' position in 'str'
 * 			-1	if not found
 */
int16_t STRING_FindString(const char *str, const char *findString)
{
	uint8_t i;
	uint8_t length = StringLength(str);
	uint8_t findStringLength = StringLength(findString);

	// Search first equal character
	for (i = 0; i < length; i++)
	{
		if (findString[0] == str[i])
		{
			// First character is equal
			if (!StrCmpWithLength(findString, &str[i], findStringLength))
			{
				return i;
			}
		}
	}

	return -1;

}



/**
 * \brief	Separators
 * 			like strtok()
 * 			NOTE: Be careful, pointers to original (source) string
 */
uint8_t STRING_Splitter(char *source, char delimiterChar, char **separated, uint8_t parameterMaxCount)
{
	uint8_t i;
	uint8_t j;
	uint8_t parameters = 0;

	// Check parameters
	if (source == NULL || separated == NULL || parameterMaxCount == 0)
	{
		// Fail parameters
		return 0;
	}

	// Make empty
	separated[0] = NULL;

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
			if (j==0)
			{
				// one length parameter // TODO: Do with more beautiful
				separated[parameters] = &source[i];
			}
			parameters++;
			j = 0;
			if (parameters >= parameterMaxCount)
			{
				// Ok, end
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
				// New string found
				separated[parameters] = &source[i];
			}
			j++;
		}
	}

	return parameters;

}



/**
 * \brief	Instead of sprintf()
 */
uint8_t string_printf(char *str, const char *format, va_list ap)
{
	// Used '%' parameters
	// %d, %u, %x, %X, %w, %h, %b, %c, %s, %f

	// TODO: Use "new" typedefs

	char	*p;			// step on fmt
	char	*sval;		// string
	int		ival;		// int
	unsigned int uival;	// uint
	float	flval;		// float
	char 	cval;		// character

	char *string = str;

	bool paramHasLength;
	uint8_t paramNum1;
	uint8_t paramNum2;
	char fillCharacter;

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
					ival = va_arg(ap, int);	// Decimal = signed int (~int32_t)
					string += SignedDecimalToStringFill(ival, string,
							paramNum2, fillCharacter);
					break;

				case 'u':
					uival = va_arg(ap, int);// Uint = Unsigned int (~uint32_t)
					string += UnsignedDecimalToStringFill(uival, string,
							paramNum2, fillCharacter);
					break;

					// TODO: Create 'x' and 'X' to different
				case 'x':
				case 'X':
					// Hex - parameterized byte num
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

#if 0
					// TODO: Delete w, h, b if not need
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
#endif
#if 0
				case 'b':
					// Hex	// 8 bits	// 2 hex	// 1 byte
					ival = va_arg(ap, int);
					string += DecimalToHexaString(ival, string, 2);
					break;
#endif
				case 'b':
					// Binary print (from uint32_t)
					uival = va_arg(ap,  unsigned int);
					string += DecimalToBinaryString(uival, string, 33);
					break;

				case 'c':
					cval = va_arg(ap, int);					// Char
					*string = cval;							// Copy to string
					string++;
					*string = '\0';
					break;

				case 'f':
					flval = va_arg(ap, double);				// Double / Float
					if (paramHasLength)
					{
						string += FloatToString(flval, string,
								paramNum1, paramNum2);
					}
					else
					{
						string += FloatToString(flval, string, 0, 6);
					}
					break;

				case 's':
					sval = va_arg(ap, char*);
					if (paramHasLength)
					{
						// String copy with length
						uint8_t stringLength = paramNum1*10 + paramNum2;
						string += StrCpyFixLengthWithFillCharacter(string, sval,
								stringLength, ' ');
					}
					else
					{
						// Standard string copy
						string += StrCpy(string, sval);
					}
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

	return (string-str);
}



/**
 * \brief	Function like sprintf(); Print to string
 */
uint8_t usprintf(char *str, const char *format, ...)
{
	uint8_t length = 0;

	va_list ap;									// argument pointer
	va_start(ap, format); 						// ap on arg
	length = string_printf(str,format,ap);		// Separate and process
	va_end(ap);						 			// Cleaning after end

	return length;
}



/**
 * \brief	Function like printf(); Print on debug serial port
 * 			Copy character to buffer and after that, sending.
 */
uint8_t uprintf(const char *format, ...)
{
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	// Working in at:
	char TxBuffer[USART_TXBUFFERSIZE];

	va_list ap;									// argument pointer
	va_start(ap, format); 						// ap on arg
	string_printf(TxBuffer, format,ap);			// Separate and process
	va_end(ap);						 			// Cleaning after end

	return DebugUart_SendMessage(TxBuffer);			// Send on Usart
#else
	return 0;
#endif
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
	int16_t ivalue16;
	uint32_t value32;
	int32_t ivalue32;
	float fvalue;
	char *splitted[10];

	// Start of unittest
	UnitTest_Start("String", __FILE__);


	/// String compare StrCmp()
	// Equal:
	UnitTest_CheckResult(!StrCmp("example", "example"), "StrCmp error", __LINE__);
	// Not equal:
	UnitTest_CheckResult(StrCmp("example", "example1"), "StrCmp error", __LINE__);
	UnitTest_CheckResult(StrCmp("example1", "example2"), "StrCmp error", __LINE__);


	// Float print tests
	// "123.339996" ~ like "%0.6"
	usprintf(buffer,"%f",123.34);
	UnitTest_CheckResult(!StrCmp(buffer, "123.339996"), "Float error", __LINE__);
	// "123"
	usprintf(buffer,"%1.0f",123.34);
	UnitTest_CheckResult(!StrCmp(buffer, "123"), "Float error", __LINE__);
	// "123.3"
	usprintf(buffer, "%1.1f",123.34);
	UnitTest_CheckResult(!StrCmp(buffer, "123.3"), "Float error", __LINE__);
	// "123.33999"
	usprintf(buffer,"%1.5f",123.34);
	UnitTest_CheckResult(!StrCmp(buffer, "123.33999"), "Float error", __LINE__);
	// "  123.3"
	usprintf(buffer, "%5.1f",123.34);
	UnitTest_CheckResult(!StrCmp(buffer, "  123.3"), "Float error", __LINE__);
	// "123.33999"
	usprintf(buffer,"%5.5f",123.34);
	UnitTest_CheckResult(!StrCmp(buffer, "  123.33999"), "Float error", __LINE__);


	// Integer print tests
	// Printed: "123"
	usprintf(buffer, "%0u",123);
	UnitTest_CheckResult(!StrCmp(buffer, "123"), "Integer error", __LINE__);
	// Printed:	"123"
	usprintf(buffer, "%1u",123);
	UnitTest_CheckResult(!StrCmp(buffer, "123"), "Integer error", __LINE__);
	// Printed: " 123"
	usprintf(buffer, "%4u",123);
	UnitTest_CheckResult(!StrCmp(buffer, " 123"), "Integer error", __LINE__);
	// Printed: "      123"
	usprintf(buffer, "%9u",123);
	UnitTest_CheckResult(!StrCmp(buffer, "      123"), "Integer error", __LINE__);
	// Printed: "00123", it is OK
	usprintf(buffer, "%05u",123);
	UnitTest_CheckResult(!StrCmp(buffer, "00123"), "Integer error", __LINE__);

	// Integer print tests (wrong examples):
	usprintf(buffer, "%A5",123);		// Printed: "A5u", because 'A' is not a number
	UnitTest_CheckResult(!StrCmp(buffer, "A5"), "Integer error", __LINE__);
	usprintf(buffer, "%-5u",123);		// Printed: "-5u", because '-' is not a number
	UnitTest_CheckResult(!StrCmp(buffer, "-5u"), "Integer error", __LINE__);

	// Signed Integer print tests:
	// Printed: "-123"
	usprintf(buffer, "%0d",-123);
	UnitTest_CheckResult(!StrCmp(buffer, "-123"), "Integer error", __LINE__);
	// Printed:	"-123"
	usprintf(buffer, "%1d",-123);
	UnitTest_CheckResult(!StrCmp(buffer, "-123"), "Integer error", __LINE__);
	// Printed: "-123"
	usprintf(buffer, "%4d",-123);
	UnitTest_CheckResult(!StrCmp(buffer, "-123"), "Integer error", __LINE__);
	// Printed: "     -123"
	usprintf(buffer, "%9d",-123);
	UnitTest_CheckResult(!StrCmp(buffer, "     -123"), "Integer error", __LINE__);
	// Printed: "-0123", it is OK
	usprintf(buffer, "%05d",-123);
	UnitTest_CheckResult(!StrCmp(buffer, "-00123"), "Integer error", __LINE__);

	// Hexadecimal print tests:
	usprintf(buffer, "0x%x",0xFFFFFFFF);
	UnitTest_CheckResult(!StrCmp(buffer, "0xFFFFFFFF"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%01x",0xFFFFFFFF);
	UnitTest_CheckResult(!StrCmp(buffer, "0xF"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%02x",0xFFFFFFFF);
	UnitTest_CheckResult(!StrCmp(buffer, "0xFF"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%03x",0xFFFFFFFF);
	UnitTest_CheckResult(!StrCmp(buffer, "0xFFF"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%04x",0xFFFFFFFF);
	UnitTest_CheckResult(!StrCmp(buffer, "0xFFFF"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%05x",0xFFFFFFFF);
	UnitTest_CheckResult(!StrCmp(buffer, "0xFFFFF"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%06x",0xFFFFFFFF);
	UnitTest_CheckResult(!StrCmp(buffer, "0xFFFFFF"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%07x",0xFFFFFFFF);
	UnitTest_CheckResult(!StrCmp(buffer, "0xFFFFFFF"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%08x",0xFFFFFFFF);
	UnitTest_CheckResult(!StrCmp(buffer, "0xFFFFFFFF"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%09x",0xFFFFFFFF);
	UnitTest_CheckResult(!StrCmp(buffer, "0x"), "Hexadecimal error", __LINE__);

	usprintf(buffer, "0x%01x",0x12345678);
	UnitTest_CheckResult(!StrCmp(buffer, "0x8"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%02x",0x12345678);
	UnitTest_CheckResult(!StrCmp(buffer, "0x78"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%03x",0x12345678);
	UnitTest_CheckResult(!StrCmp(buffer, "0x678"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%04x",0x12345678);
	UnitTest_CheckResult(!StrCmp(buffer, "0x5678"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%05x",0x12345678);
	UnitTest_CheckResult(!StrCmp(buffer, "0x45678"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%06x",0x12345678);
	UnitTest_CheckResult(!StrCmp(buffer, "0x345678"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%07x",0x12345678);
	UnitTest_CheckResult(!StrCmp(buffer, "0x2345678"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%08x",0x12345678);
	UnitTest_CheckResult(!StrCmp(buffer, "0x12345678"), "Hexadecimal error", __LINE__);
	usprintf(buffer, "0x%09x",0x12345678);
	UnitTest_CheckResult(!StrCmp(buffer, "0x"), "Hexadecimal error", __LINE__);

	// Binaries printf
	usprintf(buffer, "0b%b",0x000000FF);
	UnitTest_CheckResult(!StrCmp(buffer, "0b11111111"), "Binary error", __LINE__);
	usprintf(buffer, "0b%b",0x00000055);
	UnitTest_CheckResult(!StrCmp(buffer, "0b1010101"), "Binary error", __LINE__);

	// String (%s)
	// Standard %s print
	usprintf(buffer, "%s", "text");
	UnitTest_CheckResult(!StrCmp(buffer, "text"), "String error (%s)", __LINE__);
	// max 5 character
	usprintf(buffer, "%5s", "longtext");
	UnitTest_CheckResult(!StrCmp(buffer, "longt"), "String error (%s)", __LINE__);
	// 10 character, need fill with ' '
	usprintf(buffer, "%10s", "longtext");
	UnitTest_CheckResult(!StrCmp(buffer, "longtext  "), "String error (%s)", __LINE__);
	// max 10 character
	usprintf(buffer, "%10s", "toolongtext");
	UnitTest_CheckResult(!StrCmp(buffer, "toolongtex"), "String error (%s)", __LINE__);



	// string -> num converters

	// string -> decimal

	// Byte
	// Good bytes
	result = StringByteToNum("00", &value8);
	UnitTest_CheckResult(result, "StringByteToNum error", __LINE__);
	UnitTest_CheckResult((value8 == 0x00), "StringByteToNum error", __LINE__);
	result = StringByteToNum("15", &value8);
	UnitTest_CheckResult(result, "StringByteToNum error", __LINE__);
	UnitTest_CheckResult((value8 == 0x15), "StringByteToNum error", __LINE__);
	result = StringByteToNum("FF", &value8);
	UnitTest_CheckResult(result, "StringByteToNum error", __LINE__);
	UnitTest_CheckResult((value8 == 0xFF), "StringByteToNum error", __LINE__);
	// Wrong byte
	result = StringByteToNum("FG", &value8);
	UnitTest_CheckResult(!result, "StringByteToNum error", __LINE__);


	// Hexs
	// Good hex
	result = StringHexToNum("12345678", &value32);
	UnitTest_CheckResult(result, "StringHexToNum error", __LINE__);
	UnitTest_CheckResult(value32 == 0x12345678, "StringHexToNum error", __LINE__);
	// Wrong hex
	result = StringHexToNum("123G5678", &value32);
	UnitTest_CheckResult(!result, "StringHexToNum error", __LINE__);


	// Binaries
	result = StringBinaryToNum("010101", &value32);
	UnitTest_CheckResult(result, "StringBinaryToNum error", __LINE__);
	UnitTest_CheckResult(value32 == 0x15, "StringHexToNum error", __LINE__);
	result = StringBinaryToNum("01010101010101010101010101010101", &value32);
	UnitTest_CheckResult(result, "StringBinaryToNum error", __LINE__);
	UnitTest_CheckResult(value32 == 0x55555555, "StringHexToNum error", __LINE__);
	result = StringBinaryToNum("010101010101010101010101010101010", &value32);
	UnitTest_CheckResult(!result, "StringBinaryToNum error", __LINE__);


	// Decimals

	// Good decimals
	result = StringToSignedDecimalNum("-123",&ivalue32);
	UnitTest_CheckResult(result, "StringToSignedDecimalNum error", __LINE__);
	UnitTest_CheckResult(ivalue32 == -123, "StringToSignedDecimalNum error", __LINE__);

	result = StringToUnsignedDecimalNum("123",&value32);
	UnitTest_CheckResult(result, "StringToUnsignedDecimalNum error", __LINE__);
	UnitTest_CheckResult(value32 == 123, "StringToUnsignedDecimalNum error", __LINE__);

	// Wrong decimals
	result = StringToSignedDecimalNum("-123a",&ivalue32);
	UnitTest_CheckResult(!result, "StringToSignedDecimalNum error", __LINE__);

	result = StringToUnsignedDecimalNum("-123",&value32);
	UnitTest_CheckResult(!result, "StringToUnsignedDecimalNum error", __LINE__);

	// Float
	// Good
	result = StringToFloat("-123.3499", &fvalue);
	UnitTest_CheckResult(result, "StringToFloat error", __LINE__);
	UnitTest_CheckResult(((fvalue < -123.3498) && (fvalue > -123.3500)), "StringToFloat error", __LINE__);

	result = StringToFloat("-123a.3999", &fvalue);
	UnitTest_CheckResult(!result, "StringToFloat error", __LINE__);



	/// String function testing

	// STRING_FindString()
	ivalue16 = STRING_FindString("longtexttofinding","text");
	UnitTest_CheckResult(ivalue16 == 4, "FindString error", __LINE__);
	ivalue16 = STRING_FindString("longtexttofinding","wrongtext");
	UnitTest_CheckResult(ivalue16 == -1, "FindString error", __LINE__);


	// STRING_Splitter()
	StrCpy(buffer, "need to separate this text");
	value8 = STRING_Splitter(buffer, ' ', splitted, 10);
	UnitTest_CheckResult(value8 == 5, "STRING_Splitter error", __LINE__);
	UnitTest_CheckResult(!StrCmp(splitted[0],"need"), "STRING_Splitter error", __LINE__);
	UnitTest_CheckResult(!StrCmp(splitted[1],"to"), "STRING_Splitter error", __LINE__);
	UnitTest_CheckResult(!StrCmp(splitted[2],"separate"), "STRING_Splitter error", __LINE__);
	UnitTest_CheckResult(!StrCmp(splitted[3],"this"), "STRING_Splitter error", __LINE__);
	UnitTest_CheckResult(!StrCmp(splitted[4],"text"), "STRING_Splitter error", __LINE__);
	UnitTest_CheckResult(splitted[5] == NULL, "STRING_Splitter error", __LINE__);

	StrCpy(buffer, "text");
	value8 = STRING_Splitter(buffer, ' ', splitted, 10);
	UnitTest_CheckResult(value8 == 1, "STRING_Splitter error", __LINE__);
	UnitTest_CheckResult(!StrCmp(splitted[0],"text"), "STRING_Splitter error", __LINE__);
	UnitTest_CheckResult(splitted[1] == NULL, "STRING_Splitter error", __LINE__);

	StrCpy(buffer, "");
	value8 = STRING_Splitter(buffer, ' ', splitted, 10);
	UnitTest_CheckResult(value8 == 0, "STRING_Splitter error", __LINE__);
	UnitTest_CheckResult(splitted[0] == NULL, "STRING_Splitter error", __LINE__);


	// End of unittest
	UnitTest_End();


	return;
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
