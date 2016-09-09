/*******************************************************************************
 * Purpose:
 ******************************************************************************/

/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "string.h"
#include "include.h"


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
uint8_t SignedDecimalToString (int32_t value, char *string)
{
	uint8_t length = 0;
	
	// Check pointer
	if (string == NULL)
	{
		return 0;
	}

	if (value < 0)	// if negative decimal num
	{
		string[length++] = '-';
		value = (uint32_t) (value * (-1));// Sign +
	}

	return (length + UnsignedDecimalToString (value,&string[length]));

}



/**
 * \brief	Convert unsigned decimal to string
 * \return	created string length
 */
uint8_t UnsignedDecimalToString (uint32_t value, char *string)
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
			string[length++] = ((value/decade) + '0');
			isStarted = true;
		}

		// Value - first digit
		value %= decade;

		// /10
		decade /= 10;
	}

	// Last digit
	string[length++] = (value+ '0');

	// End character
	string[length] = '\0';

	return length;
}



/**
 * \brief	Convert value to hexadecimalstring
 * \return	created string length
 */
uint8_t DecimalToHexaString (uint32_t value, uint8_t ByteNum, char *string)
{
	uint8_t i;
	uint8_t length = 0;

	// Check parameters
	if ((ByteNum > 4) || (ByteNum == 0))
	{
		return 0;
	}

	for (i = 0; i < ByteNum; i++)
	{
		// Convert next byte
		uint8_t byte = (uint8_t)( value >> ((ByteNum-i-1)*8) );
		length += ByteToHexaString (byte,  string);
	}

	string[length] = '\0';

	return length;

}


/**
 * \brief	Convert a byte to hexa string
 * \return	Length
 */
uint8_t ByteToHexaString (uint8_t byte, char *string)
{
	uint8_t length = 0;
	uint8_t octet;

	// First octet
	octet = (byte >> 4);
	string[length++] = OctetToChar (octet);

	// Second octet
	octet = (byte & 0x0F);
	string[length++] = OctetToChar (octet);

	// Put end char
	string[length] = '\0';

	return length;

}



/**
 * \brief	Convet a octet (0-15) to Hexa character ('0' - '9' - 'A' - 'F')
 * \return	character (octet)
 */
char OctetToChar (uint8_t octet)
{
	char convertedOctet;
	if ((octet >= 0) && (octet <= 9))
	{
		// 0- 9
		convertedOctet = (octet + '0');
	}
	else if ((octet >= 10) && (octet <= 15))
	{
		// A-F
		convertedOctet = (octet - 10 + 'A');
	}
	else
	{
		convertedOctet = 'x';
	}

	return convertedOctet;
}



/**
 * \brief	Convert float value to String
 * \return	Length
 */
uint8_t FloatToString (float value, char *string, uint8_t fractionLength)
{
	uint8_t num;
	uint8_t length = 0;
	uint32_t calcValue;


	// Sign
	if (value < 0)
	{
		// Put '-'
		string[length++] = '-';
		value = (value * (-1));	// make positive
	}


	// Integer
	calcValue = (uint32_t)value;
	length += UnsignedDecimalToString(calcValue,&string[length]);


	// Point '.'
	string[length++] = '.';


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
		string[length++] = num + '0';
	}

	// Put end char
	string[length] = '\0';


	return length;
}



/**
 * \brief	Look the string is hexa or not
 * \return	How many hexa characters are...
 */
uint8_t StringIsHexadecimalString (const char *string)
{
	uint8_t length = 0;
	uint8_t i;

	// Check parameters
	if (string == NULL)
	{
		return 0;
	}
	
	for (i=0; string[i] != '\0'; i++)
	{
		// Number all? 0-9, A-F, a-f
		if (IsHexChar(string[i]))
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
uint8_t StringIsUnsignedDecimalString (const char *string)
{
	uint8_t count = 0;
	uint8_t i;

	for (i=0; string[i]; i++)
	{
		if (IsDecimalChar(string[i]))
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
uint8_t StringIsSignedDecimalString (const char *string)
{
	uint8_t length = 0;

	// Sign
	if (string[0] == '-' )
	{
		length++;
	}
	else if (string[0] == '+')
	{
		length++;
	}

	// Check after sign
	return (length + StringIsUnsignedDecimalString (&string[length]));
}



/**
 * \brief	Check, it is hexadecimal number character?
 * \return	true, if hex number
 * 			false, if no hex number
 */
bool IsHexChar(char c)
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
bool IsDecimalChar(char c)
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
uint8_t HexCharToOctet(char c)
{
	uint8_t octet = 0;
	if ((c >= '0') && (c <='9'))
	{
		octet = c - '0';
	}
	else if ((c >= 'A') && (c <= 'F'))
	{
		octet = c - 'A' + 10;
	}
	else if ((c >= 'a') && (c <= 'f'))
	{
		octet = c - 'a' + 10;
	}

	return octet;
}



/**
 * \brief	Convert two hexadecimal string to number (byte)
 * \return	true, if successul
 * 			false, if has error
 */
bool StringByteToNum(const char *string, uint8_t *byte)
{
	uint8_t calculatedByte = 0;
	calculatedByte |= (HexCharToOctet(string[0])&0x0F) << 4;
	calculatedByte |= (HexCharToOctet(string[1])&0x0F);

	*byte = calculatedByte;
	return true;

}



/**
 * \brief	Convert Hex string to number(integer)
 * \return	true, if successul
 * 			false, if has error
 */
bool StringHexToNum (const char *string, uint32_t *hexValue, uint8_t byteLength)
{
	uint8_t i;
	uint32_t calculatedValue = 0;
	uint8_t calculatedByte = 0;
	uint8_t octetLength = 0;


	octetLength = StringIsHexadecimalString (string);	// octetLength = how many hex character have

	if (octetLength == 0 || (octetLength%2))
	{
		// Wrong string or wrong octetlength
		return false;
	}

	if (byteLength == 0)
	{
		// Calculate byte length
		byteLength = octetLength / 2;
	}
	else if (octetLength != byteLength*2)
	{
		// Wrong byteLength
		return false;
	}

	// Create hexValue
	for (i=0; string[i] != '\0'; i++)
	{
		// 1. = 0 = event --> shift <<4
		// 2. = 1 = odd --> not need shift
		if ( StringByteToNum(&string[i],&calculatedByte) )
		{
			// For example: for 4 byte, first need << 24 (3*8)
			// After that, << 16 (2*8)
			calculatedValue = calculatedByte << ((byteLength-1)*8);
			i++;
			// 2 character converted, need ++
		}
		else
		{
			return false;
		}
	}

	*hexValue = calculatedValue;

	return true;
}



/**
 * \brief Convert deciaml character to number (byte)
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
 * \return	true, if successul
 * 			false, if has error
 */
bool UnsignedDecimalStringToNum (const char *string, uint32_t *value)
{
	uint32_t calculatedValue = 0;
	uint8_t i;
	uint8_t decimal;


	for (i=0; string[i] != '\0'; i++)
	{
		if (IsDecimalChar(string[i]))
		{
			decimal = DecimalCharToNum(string[i]);
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
 * \return	true, if successul
 * 			false, if has error
 */
bool SignedDecimalStringToNum (const char *string, int32_t *value)
{
	uint32_t convertValue = 0;
	bool isNegative = false;
	uint8_t length = 0;

	if (string[0] == '-')
	{
		// It is negative
		isNegative = true;
		length = 1;
	}
	else if(string[0] == '+')
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

	if (UnsignedDecimalStringToNum(&string[length],&convertValue))
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
 * \return	true, if successul
 * 			false, if has error
 */
bool StringToFloat (const char *string, float *Num)
{
	bool isNegative = false;
	uint8_t stringLength;
	uint8_t i;
	uint8_t length;
	uint8_t pointCnt ;
	uint32_t integer;
	float fractionPart;
	char numString[10] = { 0 };

	stringLength = StringLength(string);

	// Sign
	if (string[0] == '-')
	{	// It is negative
		isNegative = true;
		length = 1;
	}
	else if(string[0] == '+')
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
	for (i=0; string[length+i] != '.'; i++)
	{
		numString[i] = string[length+i];
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

	*Num = integer;

	// Convert fraction
	if(!UnsignedDecimalStringToNum(&string[pointCnt],&integer))
	{
		// Error with convert fraction part
		return false;
	}

	fractionPart = integer;
	// We converted after point part, ".567", but we need to shift right
	for (i=0; i < (stringLength-pointCnt); i++ )
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
 * \brief	Power(a,b) = a^b
 */
uint32_t power (uint32_t a, uint8_t b)
{
	uint8_t i;
	uint32_t Num=a;
	if (b==0) return 1;
	for (i=1; i<b; i++)
	{
		Num=Num*a;
	}

	return Num;
}



/**
 * \brief	Calculate string length
 * \return	length
 * 			0, if null string
 */
uint8_t StringLength (const char *string)
{
	uint8_t length = 0;

	if (string == NULL)
	{
		return 0;
	}

	while (string[length] !='\0') length++;	// Length = string length
	return length;
}




/**
 * \brief	Compare two string
 * \return	1, if not equal
 * 			0, if equal
 */
uint8_t StrCmp(const char * ch1, const char *ch2) {

	while ( *ch1 )
	{
		if ( *ch1 !=  *ch2 )
		{
			return 1;	// not equal
		}
		ch1++;
		ch2++;
	}

	if (*ch2)
	{
		return 1;	// last ch2 char has value, not equal
	}
	else
	{
		return 0;		// last ch2 char is '\0'
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
uint8_t StrCpy (char *dest, const char *string)
{
	uint8_t i;
	uint8_t length;

	length = StringLength(string);
	if ( length == 0 )
	{
		// Return 0 length
		return 0;
	}

	// Copy characters
	for ( i = 0; i < length; i++ )
	{
		dest[i] = string[i];
	}
	dest[length] = '\0';

	// Return length
	return length;
}



/**
 * \brief	Copy fix length string
 * \return	String length (=parameter)
 */
uint8_t StrCpyFixLength (char *dest, const char *string, uint8_t length)
{

	uint8_t i;

	// Copy characters
	for ( i = 0; i < length; i++ )
	{
		dest[i] = string[i];
	}

	return length;
}



/**
 * \brief	Copy string with max length
 * \return	String length (=parameter)
 */
uint8_t StrCpyMax (char *dest, const char *string, uint8_t length)
{

	uint8_t i = 0;

	// Copy characters
	while((i < length) && (string[i] != '\0'))
	{
		dest[i] = string[i];
		i++;
	}

	dest[i] = '\0';

	return i;
}



/**
 * \brief	Append string to dest's end
 * \length	New string's length (original + copied)
 */
uint8_t StrAppend (char *dest, const char *string)
{
	uint8_t length = 0;

	length = StringLength(dest);

	length += StrCpy(&dest[length],string);

	return length;

}

