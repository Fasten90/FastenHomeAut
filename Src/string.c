/*******************************************************************************
 * Purpose:
 ******************************************************************************/

/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "include.h"

// For printfs
#include <stdarg.h>		// for "..." parameters in uprintf function

#include "string.h"

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
uint8_t SignedDecimalToString (int32_t value, char *str)
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
uint8_t UnsignedDecimalToString (uint32_t value, char *str)
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
	bool isStarted = false;

	// Largest num: 1xxxxxx...
	uint32_t decade = 1000000000;

	while (decade > 1)
	{
		if ((value >= decade) || (isStarted == true))
		{
			length++;
			isStarted = true;
		}

		// Value - first digit
		value %= decade;

		// /10
		decade /= 10;
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
		length = i-1;
		length += UnsignedDecimalToString(value, &str[length]);
	}

	return length;
}



/**
 * \brief	Convert signed decimal (int32_t) to string with fill
 */
uint8_t SignedDecimalToStringFill (int32_t value, char *str, uint8_t fillLength, char fillCharacter)
{
	// TODO: A kelleténél bonyolultabb lett.

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
		if(fillCharacter == ' ')
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
					*str++ = ' ';
				}
				*str++ = '-';
				length++;	// sign
				length += UnsignedDecimalToString(value,str);
			}
		}
		else
		{
			// "-0000123"
			*str++ = '-';	// Print '-'
			length++;
			length += UnsignedDecimalToStringFill((uint32_t)value,str,fillLength,fillCharacter);
		}
	}

	return length;
}



/**
 * \brief	Convet a octet (0-15) to Hexa character ('0' - '9' - 'A' - 'F')
 * \return	character (octet)
 */
char HexToHexChar (uint8_t value)
{
	char hexChar;
	if ((value >= 0) && (value <= 9))
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
uint8_t ByteToHexaString (uint8_t byte, char *str)
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
 * \brief	Convert value to hexadecimal string
 * \return	created string length
 */
uint8_t DecimalToHexaString (uint32_t value, char *str, uint8_t length)
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
uint8_t FloatToString (float value, char *str, uint8_t integerLength, uint8_t fractionLength)
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


	// Integer
	calcValue = (uint32_t)value;
	length += UnsignedDecimalToStringFill(calcValue,&str[length],integerLength,' ');


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

	// Put end char
	str[length] = '\0';


	return length;
}



/**
 * \brief	Look the string is hexa or not
 * \return	How many hexa characters are...
 */
uint8_t StringIsHexadecimalString (const char *str)
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
uint8_t StringIsUnsignedDecimalString (const char *str)
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
uint8_t StringIsSignedDecimalString (const char *str)
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
 * \brief	Convert Hex character to octet (0-9, A-F)
 * \return	Value (number)
 */
uint8_t HexCharToHex (const char c)
{
	uint8_t hexValue = 0;
	if ((c >= '0') && (c <='9'))
	{
		hexValue = c - '0';
	}
	else if ((c >= 'A') && (c <= 'F'))
	{
		hexValue = c - 'A' + 10;
	}
	else if ((c >= 'a') && (c <= 'f'))
	{
		hexValue = c - 'a' + 10;
	}

	return hexValue;
}



/**
 * \brief	Convert two hexadecimal string to number (byte)
 * \return	true, if successful
 * 			false, if has error
 */
bool StringByteToNum(const char *str, uint8_t *byte)
{
	uint8_t calculatedByte = 0;
	calculatedByte |= (HexCharToHex(str[0])&0x0F) << 4;
	calculatedByte |= (HexCharToHex(str[1])&0x0F);

	*byte = calculatedByte;
	return true;

}



/**
 * \brief	Convert Hex string to number(integer)
 * \return	true, if successful
 * 			false, if has error
 */
bool StringHexToNum (const char *str, uint32_t *hexValue)
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
			calculatedByte = HexCharToHex(str[i]);
			// Shift one byte left original value
			calculatedValue <<= 4;
			// Add new value
			calculatedValue += calculatedByte;
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
uint8_t DecimalCharToNum (char c)
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
bool UnsignedDecimalStringToNum (const char *str, uint32_t *value)
{
	uint32_t calculatedValue = 0;
	uint8_t i;
	uint8_t decimal;


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
bool SignedDecimalStringToNum (const char *str, int32_t *value)
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
	else if(str[0] == '+')
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

	if (UnsignedDecimalStringToNum(&str[length],&convertValue))
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
bool StringToFloat (const char *str, float *Num)
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
	else if(str[0] == '+')
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
		if(i >= stringLength)
		{
			// Error, overflow
			return false;
		}
	}

	numString[i] = '\0';
	pointCnt = length+i;


	// Convert integer
	if(!UnsignedDecimalStringToNum(numString,&integer))
	{
		// Error with convert integer part
		return false;
	}

	// Integer (before .)
	*Num = integer;

	// Convert fraction
	if(!UnsignedDecimalStringToNum(&str[pointCnt+1],&integer))
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

	if(isNegative)
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
uint8_t StringLength (const char *str)
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
uint8_t StrCmp(const char *str1, const char *str2) {

	while ( *str1 )
	{
		if ( *str1 !=  *str2 )
		{
			return 1;	// not equal
		}
		str1++;
		str2++;
	}

	if (*str2)
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
uint8_t StrCmpWithLength(const char * ch1, const char *ch2, uint8_t length) {
	
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
uint8_t StrCpy (char *dest, const char *str)
{
	uint8_t i;
	uint8_t length;

	length = StringLength(str);
	if ( length == 0 )
	{
		// Return 0 length
		return 0;
	}

	// Copy characters
	for ( i = 0; i < length; i++ )
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
uint8_t StrCpyFixLength (char *dest, const char *str, uint8_t length)
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
 * \brief	Copy string with max length
 * \return	String length (=parameter)
 */
uint8_t StrCpyMax (char *dest, const char *str, uint8_t length)
{

	uint8_t i = 0;

	// Copy characters
	while((i < length) && (str[i] != '\0'))
	{
		dest[i] = str[i];
		i++;
	}

	dest[i] = '\0';

	return i;
}



/**
 * \brief	Append string to dest's end
 * \length	New string's length (original + copied)
 */
uint8_t StrAppend (char *dest, const char *str)
{
	uint8_t length = 0;

	length = StringLength(dest);

	length += StrCpy(&dest[length],str);

	return length;

}



/**
 * \brief	Find small string in big string
 * \return	'findString' position in 'str'
 */
int16_t FindString (const char *findString, const char *str)
{
	uint8_t i;
	uint8_t length = StringLength(str);

	// Search first equal character
	for (i=0; i<length; i++)
	{
		if (findString[0] == str[i])
		{
			// First character is equal
			if(!StrCmp(findString, &str[i]))
			{
				return i;
			}
		}
	}

	return -1;

}



/**
 * \brief	Instead of sprintf()
 */
uint8_t string_printf (char *str, const char *format, va_list ap)
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

	uint8_t paramDescCnt = 0;
	uint8_t paramNum1 = 0;
	uint8_t paramNum2 = 0;
	char fillCharacter = ' ';

	for (p = (char *)format; *p; p++)						// p to EOS
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
			paramNum2 = 8;
			fillCharacter = ' ';

			// Check %...x (parameter after %, before x, u, f)
			// Next character is num?
			if(IsDecimalChar(*p))
			{
				// It is num (1. param)
				paramNum1 = DecimalCharToNum(*p);
				fillCharacter = *p;
				paramDescCnt++;
				p++;

				if (IsDecimalChar(*p))
				{
					// xy
					// It is num (2. param)
					paramNum2 = DecimalCharToNum(*p);
					paramDescCnt++;
					p++;
				}
				else if (*p == '.')
				{
					// x.
					p++;
					if (IsDecimalChar(*p))
					{
						// x.x
						paramNum2 = DecimalCharToNum(*p);
						paramDescCnt++;
						p++;
					}
					else
					{
						// x.y
						// x = paramNum1
						// y=2 now, for correct float printing
						paramNum2 = 2;
					}
				}
				else
				{
					// x		==>		x = fill character, y = length
					// If only has one parameter
					paramNum2 = paramNum1;	// Length
					fillCharacter = ' ';	// Blank character
				}
			}

			// Process next character (after '%', or etc)
			switch (*p)
			{
				case 'd': ival = va_arg(ap, int);						// Decimal = signed int (~int32_t)
						  string += SignedDecimalToStringFill(ival,string, paramNum2, fillCharacter);
						  break;

				case 'u': uival = va_arg(ap, int);						// Uint = Unsigned int (~uint32_t)
						  string += UnsignedDecimalToStringFill(uival,string, paramNum2, fillCharacter);
						  break;

				// TODO: Create 'x' and 'X' to different
				case 'x':
				case 'X': uival = va_arg(ap, unsigned int);				// Hex // 32 bits	// 8 hex	// 4 byte
						  string += DecimalToHexaString(uival, string, paramNum2);// Copy to string
						  break;

				// TODO: Delete w, h, b if not need
				case 'w': uival = va_arg(ap, unsigned int);					// Hex // 32 bits	// 8 hex	// 4 byte
						  string += DecimalToHexaString(uival, string, 8);	// Copy to string
						  break;

				case 'h': ival = va_arg(ap, int);							// Hex // 16 bits	// 4 hex	// 2 byte
						  string += DecimalToHexaString(ival, string, 4);	// Copy to string
						  break;

				case 'b': ival = va_arg(ap, int);							// Hex	// 8 bits	// 2 hex	// 1 byte
						  string += DecimalToHexaString(ival, string, 2);	// Copy to string
						  break;

				case 'c': cval = va_arg(ap, int);						// Char
						  *string = cval;								// Copy to string
						  string++;
						  *string = '\0';
						  break;

				case 'f': flval = va_arg(ap, double);					// Double / Float
						  string += FloatToString(flval,string,paramNum1,paramNum2);
						  break;

				case 's': for(sval = va_arg(ap,char*); *sval; sval++)	// String
						  {
							*string = *sval;							// Copy to string
							string++;
						  }
						  break;

				default:
						*string = *p;									// Other, for example: '%'
						string++;
					 	 break;
		  }
		}	// End of '%'

	}	// End of for loop

	*string = '\0';												// string's end

	return (string-str);

}



/**
 * \brief	Function like sprintf(); Print to string
 */
uint8_t usprintf (char *str, const char *format, ...)
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
uint8_t uprintf (const char *format, ...)
{
	// Working in at:
	char TxBuffer[TXBUFFERSIZE];

	va_list ap;									// argument pointer
	va_start(ap, format); 						// ap on arg
	string_printf(TxBuffer,format,ap);				// Separate and process
	va_end(ap);						 			// Cleaning after end

	return USART_SendMessage(TxBuffer);			// Send on Usart
}



#ifdef MODULE_STRING_UNIT_TEST_ENABLED
/**
 * \brief	String module Unit Test
 */
void STRING_UnitTest (void)
{

	/*
	// Lekezelve
	%1.5f	// Max 9.9f, pl.		1.5f == > 1.12345
	%4u		// unsigned és folytathatja ha hosszabb lenne, kiegészíti blank karakterrel
	%02u	// Feltölti 0-val, ha 2 jegyűnél kisebb a szám, ellenkező esetben kiírja teljesen
			// Csak 0-9 számmal lehet feltölteni
	%4d
	%04d

	// Hexadecimális csak így működik (Mindenképpen 0-val tölt fel):
	%08x		00001234
	%04x		1234
	%02x		34

	// Nem működik:
	%-10d
	 */

	uprintf("Float print tests:\r\n");
	uprintf("%1.5f\r\n",123.34);	// Printed: "123.33999"
	uprintf("%5.5f\r\n",123.34);	// Printed: "  123.33999"
	uprintf("%5.1f\r\n",123.34);	// Printed: "  123.3"
	uprintf("%1.1f\r\n",123.34);	// Printed: "123.3"

	uprintf("Integer print tests:\r\n");
	uprintf("%0u\r\n",123);			// Printed: "123"
	uprintf("%1u\r\n",123);			// Printed:	"123"
	uprintf("%4u\r\n",123);			// Printed: " 123"
	uprintf("%9u\r\n",123);			// Printed: "      123"
	uprintf("%05u\r\n",123);		// Printed: "00123", it is OK

	uprintf("Integer print tests (wrong examples):\r\n");
	uprintf("Wrong example: %A5u\r\n",123);		// Printed: "A5u", because 'A' is not a number
	uprintf("Wrong example: %-5u\r\n",123);		// Printed: "-5u", because '-' is not a number

	uprintf("Signed Integer print tests:\r\n");
	uprintf("%0d\r\n",-123);			// Printed: "123"
	uprintf("%1d\r\n",-123);			// Printed:	"123"
	uprintf("%4d\r\n",-123);			// Printed: " 123"
	uprintf("%9d\r\n",-123);			// Printed: "      123"
	uprintf("%05d\r\n",-123);			// Printed: "00123", it is OK

	uprintf("Hexadecimal print tests:\r\n");
	uprintf("0x%01x\r\n",0xFFFFFFFF);
	uprintf("0x%02x\r\n",0xFFFFFFFF);
	uprintf("0x%03x\r\n",0xFFFFFFFF);
	uprintf("0x%04x\r\n",0xFFFFFFFF);
	uprintf("0x%05x\r\n",0xFFFFFFFF);
	uprintf("0x%06x\r\n",0xFFFFFFFF);
	uprintf("0x%07x\r\n",0xFFFFFFFF);
	uprintf("0x%08x\r\n",0xFFFFFFFF);
	uprintf("0x%09x\r\n",0xFFFFFFFF);
	// FLOAT TEST
	/*
	char String[20];
	float AnNum;

	AnNum = StringToFloat ( "3.14" );
	FloatToString ( AnNum , String, 2 );
	uprintf("%s\r\n",String);


	AnNum = StringToFloat ( "3.1234567" );
	FloatToString ( AnNum , String,4 );
	uprintf("%s\r\n",String);


	AnNum = StringToFloat ( "3.0" );
	FloatToString ( AnNum , String,6 );
	uprintf("%s\r\n",String);

	AnNum = StringToFloat ( "3.2" );
	FloatToString ( AnNum , String,4 );
	uprintf("%s\r\n",String);

	AnNum = StringToFloat ( "0.24" );
	FloatToString ( AnNum , String,4 );
	uprintf("%s\r\n",String);


	uprintf("%f\r\n",1.2);
	uprintf("%f\r\n",3.1234567);
	uprintf("%f\r\n",0.24);
	uprintf("%f\r\n",-0.24);
	*/

	return;
}
#endif	// #ifdef MODULE_STRING_UNIT_TEST_ENABLED


// Other printf:
/*
// Link: http://electronics.stackexchange.com/questions/206113/how-do-i-use-the-printf-function-on-stm32/206118
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void vprint(const char *fmt, va_list argp)
{
    char string[200];
    if(0 < vsprintf(string,fmt,argp)) // build string
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
