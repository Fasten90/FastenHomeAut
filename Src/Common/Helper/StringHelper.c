/*
 *    String.c
 *    Created on:   2016-01-01
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     String manipulation functions
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include <stdarg.h>        /* for "..." parameters in printf function */
#include "GenericTypeDefs.h"
#include "MathHelper.h"
#include "MemHandler.h"
#include "StringHelper.h"

#ifdef CONFIG_MODULE_STRING_UNITTEST_ENABLE
    #include "UnitTest.h"
#endif



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_DEBUG_MODE
    #define STRING_ASSERT(_e)                       ASSERT(_e)
    #define STRING_SNPRINTF_CATCH_ERROR_ENABLED     (1)
    #define STRING_SNPRINTF_CATCH_ERROR()           DEBUG_BREAKPOINT()
#else
    #define STRING_ASSERT(_e)
    #define STRING_SNPRINTF_CATCH_ERROR()           (void)0  /* Empty */
#endif

#define STRING_SIZE_MAX                             (1024U)

#define STRING_INTEGER_MAX_LENGTH                   (10U)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local functions
 *----------------------------------------------------------------------------*/

static uint8_t FloatToStringSafe(float value, char *str, uint8_t integerLength, uint8_t fractionLength, uint8_t maxLen);
static uint8_t UnsignedDecimalToStringFillSafe(uint32_t value, char *str, uint8_t fillLength, char fillCharacter, uint8_t maxLen);



/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/


/**
 * @brief       Convert signed decimal to string
 * @note        Only max INT_MAX / 2 number can be converted
 * @return      created string length
 */
uint8_t SignedDecimalToString(int32_t value, char *str)
{
    uint8_t length = 0;

    /* Check pointer */
    if (str == NULL)
    {
        return 0;
    }

    if (value < 0)    /* if negative decimal num */
    {
        str[length++] = '-';
        value = (uint32_t) (value * (-1));    /* Sign + */
    }

    /* Return with length */
    return (length + UnsignedDecimalToString (value, &str[length]));
}



size_t SignedDecimalToStringSafe(int32_t value, char * str, size_t maxLength)
{
    size_t length = 0;

    /* Check pointer */
    if (str == NULL)
    {
        return 0;
    }

    if (value < 0)    /* if negative decimal num */
    {
        if (maxLength > 0)
        {
            str[length] = '-';
            length++;
            value = (uint32_t) (value * (-1));    /* Sign + */
        }
        else
        {
            /* Cannot print */
            return 0;
        }
    }

    /* Add number */
    length += UnsignedDecimalToStringSafe(value, &str[length], (maxLength - length));

    return length;
}



/**
 * @brief       Convert unsigned decimal to string
 * @return      created string length
 */
uint8_t UnsignedDecimalToString(uint32_t value, char *str)
{
    uint8_t length = 0;

    /* Check pointer */
    if (str == NULL)
    {
        return 0;
    }

    /* Largest num: 1xxxxxx... */
    uint32_t decade = 1000000000;

    /* Find first "largest" decimal digit */
    while (decade > value && decade > 1)
        decade /= 10;

    do
    {
        /* Put next digit */
        str[length] = ((char)(value/decade) + '0');
        length++;

        value %= decade;            /* Value - first digit */
        decade /= 10;                /* /10 */
    }
    while (decade >= 1);

    str[length] = '\0';                /* End character */

    return length;
}



/**
 * @brief       uint32_t to string (with length)
 */
size_t UnsignedDecimalToStringSafe(uint32_t value, char *str, size_t maxLength)
{
    size_t length = 0;
    bool isStarted = false;
    bool overflowed = false;

    if (str == NULL || maxLength == 0)
        return 0;

    /* Largest num: 1xxxxxx... */
    uint32_t decade = 1000000000;

    /* TODO: Implement without "isStarted" */
    while (decade > 1)
    {
        if ((value >= decade) || (isStarted == true))
        {
            /* Put first digit */
            /* Add the ASCII code of '0' character to get the desired value's caracter code */
            /* TODO:
             * Overflow not handled: maxLength < expectedLength
             */
            if ((length+1) >= maxLength)
            {
                /* Possible error situation: Want to print too large number with less string space */
                STRING_SNPRINTF_CATCH_ERROR();
                overflowed = true;
                break;
            }

            str[length] = ((char)(value/decade) + '0');
            length++;
            isStarted = true;
        }

        value %= decade;            /* Value - first digit */
        decade /= 10;                /* /10 */
    }

    /* TODO: Merge this "last digit" with above code */
    if (length < maxLength &&  !overflowed)
    {
        /* Add the ASCII code of '0' character to get the desired value's character code */
        str[length] = ((char)value + '0');    /* Last digit */
        length++;
        /* This branch without 'overflowed' check will be calculate wrong last number E.g. 32 value --> print R character */
    }
    else if (overflowed)
    {
        str[length] = ((char)'?');    /* Last digit */
        length++;
    }


    str[length] = '\0';                /* End character */

    return length;
}



/**
 * @brief       Calculate unsigned decimal number "string length"
 * @return      length of number's string
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
        /* value : 10 */
        value /= 10;
        length++;
    }

    return length;
}



/**
 * @brief       Unsigned decimal (uint32_t) to String with fill (a character to x length)
 *              if fillLength < len(value), the full number will be printed
 */
uint8_t UnsignedDecimalToStringFill(uint32_t value, char *str, uint8_t fillLength, char fillCharacter)
{
    uint8_t length = 0;

    if (str == NULL)
    {
        return 0;
    }

    length = UnsignedDecimalLength(value);

    if (length >= fillLength)
    {
        /* Not need fill */
        /* Put number */
        length = UnsignedDecimalToString(value, str);
    }
    else
    {
        uint8_t i;

        /* Need fill */
        for (i = 0; i < (fillLength - length); i++)
        {
            str[i] = fillCharacter;
        }
        /* Put number */
        length = i;
        length += UnsignedDecimalToString(value, &str[length]);
    }

    return length;
}



/**
 * @brief       Unsigned decimal (uint32_t) to String with fill (a character to x length)
 *              If fillLength < len(value), the full number will be printed
 *              This will make ugly a table printing, but correct value will be printed
 */
uint8_t UnsignedDecimalToStringFillSafe(uint32_t value, char *str, uint8_t fillLength, char fillCharacter, uint8_t maxLen)
{
    uint8_t numLength = 0;
    uint8_t length = 0;

    if ((str == NULL) || (maxLen == 0))
    {
        return 0;
    }

    numLength = UnsignedDecimalLength(value);

    if (fillLength < numLength)
    {
        /* Shall print entire number */
        fillLength = numLength;
    }

    /* Check, could be print? fillLength many characters will be printed */
    if (fillLength > maxLen)
    {
        /* Limiting */
        fillLength = maxLen;
        /* Check length again */
        if (numLength > fillLength)
        {
            numLength = fillLength;
        }
        /* Shall not happen, but handled */
        STRING_SNPRINTF_CATCH_ERROR();
    }

    /* Need fill */
    uint8_t filledLength = fillLength - numLength; /* fillLength - num_length will not underflow */
    for (uint8_t i = 0; i < filledLength; i++)
    {
        str[i] = fillCharacter;
    }

    length += filledLength;

    /* Put number */
    length += (uint8_t)UnsignedDecimalToStringSafe(value, &str[filledLength], numLength);

    return length;
}



/**
 * @brief       Convert signed decimal (int32_t) to string with fill
 */
uint8_t SignedDecimalToStringFill(int32_t value, char *str, uint8_t fillLength, char fillCharacter)
{
    uint8_t length = 0;
    uint8_t i;

    if (str == NULL)
    {
        return 0;
    }

    if (value >= 0)
    {
        /* Sign + */
        length = UnsignedDecimalToStringFill((uint32_t)value, str, fillLength, fillCharacter);
    }
    else
    {
        /* Sign - */

        value *= -1;    /* Sign swap */

        /* Check "fill character" */
        if (fillCharacter == ' ')
        {
            /* ' ', put sign at last */
            /* "    -123" */
            length = UnsignedDecimalLength(value);
            if (length > fillLength)
            {
                length = SignedDecimalToString(-value, str);            /* Not need fill */
            }
            else
            {
                /* Need fill */
                /* "    -123" */
                for (i=0; i<(fillLength-length-1); i++)
                {
                    str[i] = ' ';
                }
                length = i;    /* sign */
                str[length++] = '-';
                length += UnsignedDecimalToString(value,&str[length]);
            }
        }
        else
        {
            /* "-0000123" */
            length = 0;
            str[length++] = '-';    /* Print '-' */
            length += UnsignedDecimalToStringFill((uint32_t)value,&str[length],fillLength,fillCharacter);
        }
    }

    return length;
}



/**
 * @brief       Convert a octet (0-15) to Hexa character ('0' - '9' - 'A' - 'F')
 * @return      character (octet)
 */
char HexToHexChar(uint8_t value)
{
    /* TODO: Rename hex --> to nibble */
    char hexChar;

    if (value <= 9)
    {
        /* 0- 9 */
        hexChar = (value + '0');
    }
    else if ((value >= 10) && (value <= 15))
    {
        /* A-F */
        hexChar = (value - 10 + 'A');
    }
    else
    {
        hexChar = 'x';
    }

    return hexChar;
}



/**
 * @brief       Convert a byte/octet to hexadecimal string
 * @return      Length
 */
uint8_t ByteToHexaString(uint8_t byte, char *str)
{
    uint8_t length = 0;
    uint8_t hex;

    if (str == NULL)
    {
        return 0;
    }

    /* First hex */
    hex = (byte >> 4);
    str[length++] = HexToHexChar(hex);

    /* Second hex */
    hex = (byte & 0x0F);
    str[length++] = HexToHexChar(hex);

    /* Put end char */
    str[length] = '\0';

    return length;
}



/**
 * @brief       Convert decimal number to binary string
 *              For example: 10 (decimal) --> "1010" (binary)
 */
uint8_t DecimalToBinaryString(uint32_t value, char *str, uint8_t maxLength)
{
    uint8_t i; /* This will be the length/size, and because the uint32_t type, string never will be longer the 32+1 */
    int8_t bitIndex;

    if (str == NULL)
    {
        return 0;
    }

    /* Search first '1' bit: */
    bitIndex = GetLargestBitIndex(value);
    if (bitIndex < 0)
    {
        /* Print one '0' */
        bitIndex = 0;
    }

    /* When reach this, bitIndex found the first '1'bit */
    for (i = 0; i < maxLength; i++)
    {
        /* It '1' or '0'? */
        if (value & (1 << bitIndex))
        {
            str[i] = '1';
        }
        else
        {
            str[i] = '0';
        }
        /* If reach end, break */
        if (bitIndex == 0)
        {
            i++;
            break;
        }
        bitIndex--;
    }

    str[i] = '\0';        /* Put end character */

    return i;
}



/**
 * @brief       Convert value to hexadecimal string
 * @return      created string length
 */
uint8_t DecimalToHexaString(uint32_t value, char *str, uint8_t length)
{
    uint8_t i;
    uint8_t octet;

    if (str == NULL)
    {
        return 0;
    }

    /* Check parameters */
    if ((length > 8) || (length == 0))
    {
        return 0;
    }

    for (i = 0; i < length; i++)
    {
        /* Convert next byte */
        octet = (uint8_t)(0x0F & (value >> ((length-i-1)*4)));
        str[i] = HexToHexChar (octet);
    }

    str[length] = '\0';

    return length;
}



/**
 * @brief       Convert value to hexadecimal string
 * @return      Created string length
 */
uint8_t DecimalToHexaStringSafe(uint32_t value, char *str, uint8_t length, uint8_t maxLength)
{
    uint8_t i;
    uint8_t octet;

    if (str == NULL)
    {
        return 0;
    }

    /* Check parameters */
    if ((length > 8) || (length == 0))
    {
        return 0;
    }

    if (length > maxLength)
    {
        /* Safe length mode */
        length = maxLength;
    }

    for (i = 0; i < length; i++)
    {
        /* Convert next byte */
        octet = (uint8_t)(0x0F & (value >> ((length-i-1)*4)));
        str[i] = HexToHexChar (octet);
    }

    str[length] = '\0';

    return length;
}



/**
 * @brief       Convert float value to String
 * @return      Length
 */
uint8_t FloatToString(float value, char *str, uint8_t integerLength, uint8_t fractionLength)
{
    uint8_t num;
    uint8_t length = 0;
    uint32_t calcValue;

    /* Sign */
    if (value < 0)
    {
        /* Put '-' */
        str[length++] = '-';
        value = (value * (-1));    /* make positive */
    }

    /* Integer: minimum interLength length (if integer part is longer then this num, it printed) */
    calcValue = (uint32_t)value;
    length += UnsignedDecimalToStringFill(calcValue, &str[length], integerLength, ' ');

    /* If has fractionLength parameter (=/= 0), print it */
    if (fractionLength)
    {
        /* Point '.' */
        str[length++] = '.';

        /* Fraction: */
        /* float : 4.567 */
        /* fractionLength: 4 */
        /* string: 4.5670 */

        /* TODO: Make with multiplex (e.g. 0.567 -> * 1000 = 567 --> UnsignedDecimalToString... */

        /* 4.567 --> 0.567 --> 5670 */
        /* Only fraction */
        value = (value - (uint32_t)value);

        /* * 10, and write */
        while (fractionLength--)
        {
            /* 0.567 --> 5.67 */
            value *= 10;             /* "shift left" = *10 */
            /* 5.67 --> 5 */
            num = (uint8_t)value;    /* integer value (MSB octet) */
            /* 5.67 - 5 */
            value -= num;            /* value-- */
            str[length++] = num + '0';
        }
    }
    /* If hasn't fractionLength parameter, '.' and fraction part not printed */

    /* Put end char */
    str[length] = '\0';

    return length;
}



/**
 * @brief       Convert float value to String
 * @return      Length
 */
uint8_t FloatToStringSafe(float value, char *str, uint8_t integerLength, uint8_t fractionLength, uint8_t maxLen)
{
    uint8_t num;
    uint8_t length = 0;
    uint32_t calcValue;
    uint8_t remainLength = maxLen;

    if (maxLen == 0)
    {
        return 0;
    }

    /* Sign */
    if (value < 0)
    {
        /* Shall not check the maxLen, because it cannot be 0 */
        /* Put '-' */
        str[length] = '-';
        length++;
        value = (value * (-1));    /* make positive */
        remainLength--;
    }

    /* Integer: minimum interLength length (if integer part is longer then this num, it printed) */
    calcValue = (uint32_t)value;
    /* integerLength == 0 --> handled in the called function */
    length += UnsignedDecimalToStringFillSafe(calcValue, &str[length], integerLength, ' ', remainLength);

    /* If has fractionLength parameter (=/= 0), print it */
    if (fractionLength && (length < maxLen))
    {
        /* Point '.' */
        str[length] = '.';
        length++;
        remainLength--;

        /* Fraction: */
        /* float : 4.567 */
        /* fractionLength: 4 */
        /* string: 4.5670 */

        /* TODO: Make with multiplex (e.g. 0.567 -> * 1000 = 567 --> UnsignedDecimalToString... */

        /* 4.567 --> 0.567 --> 5670 */
        /* Only fraction */
        value = (value - (uint32_t)value);

        /* * 10, and write */
        while (fractionLength && remainLength)
        {
            /* 0.567 --> 5.67 */
            value *= 10;             /* "shift left" = *10 */
            /* 5.67 --> 5 */
            num = (uint8_t)value;    /* integer value (MSB octet) */
            /* 5.67 - 5 */
            value -= num;            /* value-- */
            str[length] = num + '0';
            fractionLength--;
            length++;
            remainLength--;
        }
    }
    /* If hasn't fractionLength parameter, '.' and fraction part not printed */

    /* Put end char */
    str[length] = '\0';

    return length;
}



/**
 * @brief       Look the string is hexa or not
 * @str         Null terminated string
 * @return      How many hexa characters are in the string
 */
uint8_t StringIsHexadecimalString(const char *str)
{
    /* TODO: uint8_t length to size_t? */
    uint8_t length = 0;
    uint8_t i;

    /* Check parameters */
    if (str == NULL)
    {
        return 0;
    }

    for (i = 0; str[i] != '\0'; i++)
    {
        /* Number all? 0-9, A-F, a-f */
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
 * @brief       Check the string, it is unsigned decimal number?
 * @return      > 0 : decimal number length
 *              0 not number
 */
uint8_t StringIsUnsignedDecimalString(const char *str)
{
    uint8_t length = 0;
    uint8_t i;

    if (str == NULL)
    {
        return 0;
    }

    for (i = 0; str[i] != '\0'; i++)
    {
        if (IsDecimalChar(str[i]))
        {
            length++;
        }
        else
        {
            /* Return 0, if not number */
            return 0;
        }
    }

    return length;
}



/**
 * @brief       Check the string, it is signed decimal number?
 * @return      > 0 : decimal number length
 *              0 not number
 */
uint8_t StringIsSignedDecimalString(const char *str)
{
    uint8_t length = 0;

    if (str == NULL)
    {
        return 0;
    }

    /* Sign */
    if (str[0] == '-' )
    {
        length++;
    }
    else if (str[0] == '+')
    {
        length++;
    }

    /* Check after sign */
    length += StringIsUnsignedDecimalString (&str[length]);

    return length;
}



/**
 * @brief       Check, it is hexadecimal number character?
 * @return      true, if hex number
 *              false, if no hex number
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
 * @brief       Check, it is decimal number character?
 * @return      true, if number
 *              false, if no number
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
 * @brief       'Binary string' to 'uint32_t'
 *              Example: "01110" -> 14
 */
bool StringBinaryToNum(const char *str, uint32_t *num)
{
    uint32_t value = 0;
    uint8_t i;

    if ((str == NULL) || (num == NULL))
    {
        return false;
    }

    if (StringLength(str) > 32)
    {
        return false;
    }

    /* TODO: Check Prefix ?? */
    for (i = 0; str[i]; i++)
    {
        if (str[i] == '1')
        {
            value |= 0x01;
        }
        else if (str[i] == '0')
        {
            /* Do nothing, it is ok */
            /* TODO: Optimize: this if (str[i] != '0') { return false; }, and not need else */
            value |= 0x00;
        }
        else
        {
            return false;        /* Wrong character here */
        }

        /* Shift left */
        if (str[i+1])
        {
            value <<= 1;
        }
    }

    /* Finish */
    *num = value;

    return true;
}



/**
 * @brief       Convert Hex character to octet (0-9, A-F)
 * @param       c    ASCII character, which you need to convert to value
 * @param       *hexValue    the value
 * @return      result
 *              true    successful
 *              false    failed
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
 * @brief       Convert two hexadecimal character (string) to number (byte)
 * @return      true, if successful
 *              false, if has error
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
 * @brief       Convert Hex string to number(integer)
 * @return      true, if successful
 *              false, if has error
 */
bool StringHexToNum(const char *str, uint32_t *hexValue)
{
    uint8_t i;
    uint32_t calculatedValue = 0;
    uint8_t calculatedByte = 0;

    /* Check length */
    if (StringIsHexadecimalString(str) == 0)
    {
        return false;                                    /* Wrong string or wrong octetlength */
    }

    /* Create hexValue */
    for (i = 0; str[i] != '\0'; i++)
    {
        /* shift <<4 + add next hex */
        if (IsHexChar(str[i]))
        {
            if (HexCharToHex(str[i], &calculatedByte))
            {
                calculatedValue <<= 4;                    /* Shift one byte left original value */
                calculatedValue += calculatedByte;        /* Add new value */
            }
            else
            {
                return false;                            /* Failed (hex character) */
            }
        }
        else
        {
            return false;
        }
        if (i >= 8)
        {
            break;                                        /* Has an "uint32_t" */
        }
    }

    *hexValue = calculatedValue;

    return true;
}



/**
 * @brief       Convert decimal character to number (byte)
 * @return      value (number)
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
 * @brief       Convert Unsigned decimal string to integer
 * @return      true, if successful
 *              false, if has error
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
            calculatedValue *= 10;            /* Shift left 1* =  *10 */
            calculatedValue += decimal;        /* Add new value */
        }
        else
        {
            return false;                    /* Wrong character */
        }

        if (i > STRING_INTEGER_MAX_LENGTH)
        {
            return false;                    /* To long num */
        }
    }

    *value = calculatedValue;

    return true;
}



/**
 * @brief       Convert Unsigned decimal string to integer
 *              string shall not be null terminated
 * @return      true, if successful
 *              false, if has error
 */
bool StringToUnsignedDecimalNumWithLength(const char * str, uint32_t * value, uint8_t stringLength)
{
    /* TODO: This function was copied from above - optimize! */
    uint32_t calculatedValue = 0;
    uint8_t i;
    uint8_t decimal;

    if ((str == NULL) || (value == NULL) || (stringLength == 0))
    {
        return false;
    }

    for (i = 0; i < stringLength; i++)
    {
        if (IsDecimalChar(str[i]))
        {
            decimal = DecimalCharToNum(str[i]);
            calculatedValue *= 10;            /* Shift left 1* =  *10 */
            calculatedValue += decimal;       /* Add new value */
        }
        else
        {
            return false;                    /* Wrong character */
        }

        if (i > STRING_INTEGER_MAX_LENGTH)
        {
            return false;                    /* To long num */
        }
    }

    *value = calculatedValue;

    return true;
}



/**
 * @brief       Convert signed decimal string to signed integer
 * @return      true, if successful
 *              false, if has error
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
        /* It is negative */
        isNegative = true;
        length = 1;
    }
    else if (str[0] == '+')
    {
        /* Positive */
        isNegative = false ;
        length = 1;
    }
    else
    {
        /* Positive */
        isNegative = false;
        length = 0;
    }

    if (StringToUnsignedDecimalNum(&str[length], &convertValue))
    {
        if (isNegative)
        {
            /* Negative, converted value need * -1 */
            *value = -1 * (int32_t)convertValue;
        }
        else
        {
            /* Positive */
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
 * @brief       Convert Float string to num(float)
 * @return      true, if successful
 *              false, if has error
 */
bool StringToFloat(const char *str, float *num)
{
    bool isNegative = false;
    size_t stringLength;
    uint8_t i;
    uint8_t length;
    uint8_t pointCnt ;
    uint32_t integer;
    float fractionPart;
    char numString[STRING_INTEGER_MAX_LENGTH + 1] = { 0 };

    if ((str == NULL) || (num == NULL))
        return false;

    stringLength = StringLength(str);

    /* Sign */
    if (str[0] == '-')
    {    /* It is negative */
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


    /* Integer */

    /* Find '.' */
    for (i = 0; str[length+i] != '.'; i++)
    {
        numString[i] = str[length + i];
        if (i >= stringLength)
        {
            return false;        /* Error, overflow */
        }
    }

    numString[i] = '\0';
    pointCnt = length+i;


    /* Convert integer */
    if (!StringToUnsignedDecimalNum(numString, &integer))
    {
        return false;            /* Error with convert integer part */
    }

    /* Integer (before .) */
    *num = (float)integer;

    /* Convert fraction */
    if (!StringToUnsignedDecimalNum(&str[pointCnt + 1], &integer))
    {
        return false;            /* Error with convert fraction part */
    }

    fractionPart = (float)integer;
    /* We converted after point part, ".567", but we need to shift right */
    for (i = 0; i < (stringLength - pointCnt - 1); i++ )
    {
        /* >> point */
        /* Example: 567 == > 56.7 */
        fractionPart /= 10;
    }
    /* Ready fraction */

    *num += (float)fractionPart;        /* Add Integer and Fraction */

    if (isNegative)
    {
        *num = (-1) * (*num);
    }

    return true;
}



/**
 * @brief       Convert String to bool value
 */
bool StringToBool(const char * str, bool * val)
{
    if (str == NULL || val == NULL)
        return false;

    bool isBool = false;
    bool boolVal = false;
    uint32_t num;

    /* Check it is decimal? */
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
            isBool = false;            /* Wrong num (not 0, and not 1) */
        }
    }
    else
    {
        /* Not number */

        /* Check it is "true" / "false" ? */
        /* Check it is "on" / "off"? */
        /* Check it is "enable" / "disable" */
        /* Check it is "set" / "reset" */
        if (!StrCmp((const char*)str, "true")
            || !StrCmp((const char*)str, "on")
            || !StrCmp((const char*)str, "enable")
            || !StrCmp((const char*)str, "set"))
        {
            boolVal = true;
            isBool = true;
        }
        else if (!StrCmp((const char*)str, "false")
                || !StrCmp((const char*)str, "off")
                || !StrCmp((const char*)str, "disable")
                || !StrCmp((const char*)str, "reset"))
        {
            boolVal = false;
            isBool = true;
        }
        else
        {
            /* Not good "string" */
            isBool = false;;
        }
    }

    *val = boolVal;
    return isBool;
}



/**
 * @brief       Convert char to lowercase
 */
void ToLower(char * c)
{
    if ((*c >= 'A') && (*c <= 'Z'))
    {
        /* Need to change to small letter */
        /* length between Big Letter and small letter */
        *c = *c - ('A' - 'a');
    }
}



/**
 * @brief       Convert char to UpperCase
 */
void ToUpper(char * c)
{
    if ((*c >= 'a') && (*c <= 'z'))
    {
        /* Need to change to small letter */
        /* length between Big Letter and small letter */
        *c = *c + ('A' - 'a');
    }
}



/**
 * @brief       Calculate string length
 * @return      length
 *              0, if null string
 */
size_t StringLength(const char *str)
{
    size_t length = 0;

    if (str == NULL)
    {
        return 0;
    }

    /* Added max length checking */
    while ((length < STRING_SIZE_MAX) && (str[length] != '\0'))
    {
        length++;    /* Length = string length */
    }

    return length;
}



/**
 * @brief       Compare two string
 * @return      1, if not equal
 *              0, if equal
 */
uint8_t StrCmp(const char *str1, const char *str2)
{
    size_t i = 0;

    /* Check parameters */
    if ((str1 == NULL) && (str2 == NULL))
    {
        /* Equal, because both of string are NULL */
        return 0;
    }
    else if ((str1 == NULL) || (str2 == NULL))
    {
        /* One of parameter is NULL... not equal */
        return 1;
    }

    /* Check characters */
    while ((*str1 == *str2) && *str1 && *str2 && (i < STRING_SIZE_MAX))
    {
        str1++;
        str2++;
        i++;
    }

    if (*str1 == *str2)
    {
        return 0;        /* last character is equal */
    }
    else
    {
        /* If string last character is not equal */
        return 1;
    }
}



/**
 * @brief       Compare two string (with first string length)
 * @return      1, if not equal
 *              0, if equal
 * @note        Be careful! If second string longer than first string and the begin of string are equal, the return value will be "equal" (0)
 */
uint8_t StrCmpFirst(const char *str1, const char *str2)
{
    size_t i = 0;

    if ((str1 == NULL) || (str2 == NULL))
    {
        /* One of parameter is NULL... not equal */
        return 1;
    }

    while (str1[i])
    {
        if ((str1[i] != str2[i]) || (i >= STRING_SIZE_MAX))
        {
            return 1;    /* not equal */
        }
        i++;
    }

    return 0;            /* Good, equal */
}



/**
 * @brief       Compare string in fix length
 * @return      1, if not equal
 *              0, if equal
 */
uint8_t StrCmpWithLength(const char * str1, const char *str2, size_t length)
{
    size_t i;

    /* Check pointers + length */
    if ((str1 == NULL) || (str2 == NULL) || (length == 0) || (length > STRING_SIZE_MAX))
    {
        return 1;
    }

    /* Compare characters */
    for (i = 0; i < length; i++)
    {
        if (*str1 != *str2)
        {
            return 1;    /* not equal */
        }
        str1++;
        str2++;
    }

    return 0;        /* equal     */
}



/**
 * @brief       Copy string to *dest pointer
 * @return      Copied string length
 */
size_t StrCpy(char *dest, const char *str)
{
    size_t i;
    size_t length;

    /* Check parameter */
    if ((dest == NULL) || (str == NULL))
    {
        return 0;
    }

    /* TODO: Optimize: without length calculating (length --> str[i] != '\0') */
    length = StringLength(str);

    /* Copy characters */
    for (i = 0; i < length; i++)
    {
        dest[i] = str[i];
    }
    dest[length] = '\0';

    return length;                    /* Return length */
}



/**
 * @brief       Copy fix length string
 * @return      String length (=parameter)
 */
size_t StrCpyFixLength(char *dest, const char *str, size_t length)
{
    size_t i;

    if ((dest == NULL) || (str == NULL) || (length == 0))
    {
        return 0;
    }

    /* Copy characters */
    for (i = 0; i < length; i++)
    {
        dest[i] = str[i];
    }

    /* TODO: put '\0'? It is correct? */
    dest[i] = '\0';

    return length;
}



/**
 * @brief       Copy fix length string
 * @return      String length (=parameter)
 * @note        EOS put on end of the string!
 */
size_t StrCpyFixLengthWithFillCharacter(char *dest, const char *str, size_t length, char fillChar)
{
    size_t i = 0;

    /* Check parameters (Not need check str, because str is NULL, we should copy 'fillChar' with 'length' num to 'dest') */
    if (dest == NULL)
        return 0;

    /* If str is NULL, copy fillChar(s) to *dest */
    if (str != NULL)
    {
        /* Copy characters */
        for (i = 0; (i < length) && (str[i]); i++)
        {
            dest[i] = str[i];
        }
    }

    /* Fill with character after string */
    if (i < length)
    {
        for (; i < length; i++)
        {
            dest[i] = fillChar;
        }
    }

    /* End of string */
    dest[i] = '\0';

    return length;
}



/**
 * @brief       Copy ended string with max length
 * @return      String length
 */
size_t StrCpyMax(char *dest, const char *str, size_t maxLength)
{
    size_t length = 0;
    length = StringLength(str);

    /* Check parameters */
    if (str == NULL || dest == NULL || maxLength == 0 || length == 0)
    {
        return 0;
    }

    /* TODO: Copy immediately here ? */

    /* Check long */
    if (length >= maxLength)
    {
        length = maxLength - 1;                    /* Too long, only can copy "length-1" characters + '\0' */
    }

    StrCpyFixLength(dest, str, length);            /* Copy characters */
    dest[length] = '\0';                           /* Put end */

    return length;
}



/**
 * @brief       Copy character x count
 * @return      String length
 */
size_t StrCpyCharacter(char *dest, char c, size_t num)
{
    size_t i;

    /* Check parameters */
    if (dest == NULL || num == 0)
    {
        return 0;
    }

    /* Copy characters */
    for (i = 0; i < num; i++)
    {
        dest[i] = c;
    }

    dest[i] = '\0';                /* Put end */

    return i;
}



/**
 * @brief       Append string to dest's end
 *              \length    New string's length (original + copied)
 * @note        dest buffer can be overflowed, because there is no overflow checking!
 */
size_t StrAppend(char *dest, const char *str)
{
    size_t length = 0;

    if (dest == NULL)
    {
        return length;
    }

    /* TODO: Optimize with search EOS here */
    length = StringLength(dest);

    if (str != NULL)
    {
        length += StrCpy(&dest[length], str);
    }

    return length;
}



/**
 * @brief       Append string to dest's end
 *              \length    New string's length (original + copied)
 */
size_t StrAppendSafe(char *dest, const char *str, size_t destLength)
{
    size_t length = 0;

    /* Check parameters (str not need check) */
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
            /* How many characters can we copy? */
            copyLength = destLength - length - 1;    /* Last character should be '\0' */
        }
        length += StrCpyFixLength(&dest[length], str, copyLength);
    }

    return length;
}



/**
 * @brief       Char append
 */
size_t CharAppend(char *dest, const char c)
{
    size_t length = 0;

    if (dest == NULL)
    {
        return length;
    }

    /* TODO: Optimize with search EOS here */
    length = StringLength(dest);

    dest[length] = c;
    length++;
    dest[length] = '\0';

    return length;
}



/**
 * @brief       Trim string (cut space and others at end)
 * @note        Be careful, only call with changeable string!
 */
size_t StrTrim(char *str)
{
    /* Check pointer */
    if (str == NULL)
    {
        return 0;
    }

    size_t length = StringLength(str) - 1;
    size_t i;

    /* Start from end of string */
    for (i = length; i > 0; i--)
    {
        if (str[i] == ' ' || str[i] == '\t' || str[i] == '\r' || str[i] == '\n')
        {
            str[i] = '\0';        /* Replace ' ' to \0 */
        }
        else
        {
            /* 'i' value contain the new length */
            break;                /* Not space, good character, end */
        }
    }

    return i+1; /* Index of last not whitespace char+1 - so, length */
}



/**
 * @brief       Convert string to lowercase
 *              \str        '\0' terminated string
 * @note        Be careful, only call with changeable string!
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
 * @brief       Convert string to UPPERCASE
 *              \str        '\0' terminated string
 * @note        Be careful, only call with changeable string!
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
 * @brief       Find character in string
 * @param       *str    scanned string
 *              *findCharacter    which character should find
 * @return      'findCharacter' position in 'str' (pointer)
 *              NULL    if not found
 */
const char * STRING_FindCharacter(const char *str, const char findCharacter)
{
    size_t i;
    /* TODO: Without StringLength */
    size_t length = StringLength(str);

    /* Check parameter */
    if ((str == NULL) || (length == 0))
    {
        return NULL;
    }

    /* Search first equal character */
    for (i = 0; i < length; i++)
    {
        if (str[i] == findCharacter)
        {
            /* Equal */
            return (char *)&str[i];
        }
    }

    return NULL;
}



/**
 * @brief       Find characters in string
 */
const char * STRING_FindCharacters(const char *str, const char *findCharacters)
{
    size_t i;
    uint8_t j;
    const char * findPos = NULL;

    /* Check parameter */
    if ((str == NULL) || (findCharacters == NULL))
    {
        return NULL;
    }

    /* Search in string */
    for (i = 0; i < STRING_SIZE_MAX && str[i] != '\0'; i++)
    {
        /* Check with finding characters */
        for (j = 0; j < 255 && findCharacters[j] != '\0'; j++)
        {
            if (str[i] == findCharacters[j])
            {
                /* Equal */
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
 * @brief       Find small string in big string
 *              like strstr()
 * @param       *str    scanned string
 *              *fincString    which find
 * @return      'findString' position in 'str' (pointer)
 *              NULL    if not found
 */
const char * STRING_FindString(const char *str, const char *findString)
{
    size_t i;
    size_t length = StringLength(str);
    size_t findStringLength = StringLength(findString);
    const char * pos = NULL;

    /* Check parameters */
    if ((str == NULL) || (findString == NULL) || (length == 0) || (findStringLength == 0) || findStringLength > length)
    {
        return NULL;
    }

    /* Search first equal character */
    for (i = 0; i < (length - findStringLength); i++)
    {
        if (findString[0] == str[i])
        {
            /* First character is equal */
            if (!StrCmpWithLength(findString, &str[i], findStringLength))
            {
                pos = (char *)&str[i];
                break;
            }
        }
    }

    return pos;
}



/**
 * @brief       Separate / split string to small strings by delimiter char
 *              like strtok() (but not match)
 *              E.g.: source: "192.168.0.1", delimiter: '.' --> separated[0] -> "192", separated[1] -> "168", ...
 * @note        Be careful, pointers to original (source) string
 *              source string need to be changeable!
 */
uint8_t STRING_Splitter(char *source, const char *delimiters, char **separated, uint8_t paramLimit)
{
    size_t i;
    size_t j;
    uint8_t parameters = 0;

    /* TODO: Make more beautiful! */

    /* Check parameters */
    if ((source == NULL) || (separated == NULL) || (delimiters == NULL) || (paramLimit == 0))
    {
        return 0;            /* Fail parameters */
    }

    separated[0] = NULL;    /* Make empty */

    /* Split */
    j = 0;
    for (i = 0; source[i]; i++)
    {
        /* There is delimiter? */
        uint8_t k;
        bool isFound = false;
        for (k = 0; delimiters[k] != '\0'; k++)
        {
            if (source[i] == delimiters[k])
            {
                /* Found delimiter */
                source[i] = '\0';
                if (j == 0)
                {
                    /* 0 length parameter */ /* TODO: Do with more beautiful */
                    /* e.g. ',' separator and ',,' message --> middle of ',' will be 0 length message */
                    separated[parameters] = &source[i];
                }
                parameters++;
                j = 0;
                isFound = true;
                break;
            }
            /* else: not delimiter char or next char is not the end char */
        }

        if (source[i+1] == '\0')
        {
            /* finish the last */
            if (j == 0)
            {
                /* 0 length parameter */ /* TODO: Do with more beautiful */
                /* e.g. ',' separator and ',,' message --> middle of ',' will be 0 length message */
                separated[parameters] = &source[i];
            }

            parameters++; /* TODO: Idea, at last, not limiter, but not: separated[parameters] = &source[i]; */
            isFound = true; /* Force put NULL */
        }

        if (isFound)
        {
            if (parameters >= paramLimit)
            {
                /* maximal tokens found */
                break;
            }
            else
            {
                /* Set the last to NULL */
                separated[parameters] = NULL;
            }
        }
        else
        {
            /* Not ended, count */
            if (j == 0)
            {
                separated[parameters] = &source[i];        /* New string found */
            }
            j++;
        }
    }

    return parameters;
}



#if 0
/**
 * @brief       Separate / split string to small strings by delimiter char
 *              like strtok() (but not match)
 *              E.g.: source: "192.168.0.1", delimiter: '.' --> separated[0] -> "192", sepparated[1] -> "168", ...
 * @note        Be careful, pointers to original (source) string
 *              source string need to be changeable!
 */
uint8_t STRING_Splitter(char *source, char delimiterChar, char **separated, uint8_t paramLimit)
{
    size_t i;
    size_t j;
    uint8_t parameters = 0;

    /* Check parameters */
    if (source == NULL || separated == NULL || paramLimit == 0)
    {
        return 0;            /* Fail parameters */
    }

    separated[0] = NULL;    /* Make empty */

    /* Split */
    j = 0;
    for (i = 0; source[i]; i++)
    {
        /* There is delimiter? */
        if ((source[i] == delimiterChar) || (source[i+1] == '\0'))
        {
            /* Found delimiter or end character */
            if (source[i] == delimiterChar)
            {
                source[i] = '\0';
            }
            if (j == 0)
            {
                /* one length parameter // TODO: Do with more beautiful */
                separated[parameters] = &source[i];
            }
            parameters++;
            j = 0;
            if (parameters >= paramLimit)
            {
                /* maximal tokens found */
                break;
            }
            else
            {
                separated[parameters] = NULL;
            }
        }
        else
        {
            /* Not ended, count */
            if (j == 0)
            {
                separated[parameters] = &source[i];        /* New string found */
            }
            j++;
        }
    }

    return parameters;
}
#endif



/**
 * @brief       Instead of snprintf()
 *              Used '%' parameters
 *              %d, %u, %x, %X, %b, %c, %s, %f
 *              In other settings: %w, %h, %b (hexadecimals)
 *              Other processed settings:
 *              %8d --> 8 length decimal number, filled with space
 *              %8u --> 8 length unsigned decimal number, filled with space
 *              %8x --> 8 length hexa string
 *              %b  --> print binary string (E.g. ("%b", 5) --> "101"
 *              %8c --> print character 8 times
 *              %8s --> print string (with max length)
 *              %.2f  --> print float in this format: "123.00"
 *              
 * @note        !! Be careful: 'str' can be overflow!!
 */
size_t string_printf_safe(char *str, size_t maxLen, const char *format, va_list ap)
{
    /* TODO: Use "new" typedefs */

    /* Type variables */
    char     *p;            /* step on format string */
    char     *sval;         /* string */
    int      ival;          /* int */
    unsigned int uival;     /* uint */
    float    flval;         /* float */
    char     cval;          /* character */

    /* Check parameters */
    if ((str == NULL) || (format == NULL) || (maxLen == 0))
        return 0;

    /* String variables */
    size_t length = 0;
    size_t remainLength = maxLen-1;  /* Need space for EOS too */

    /* Process variables */
  #ifdef STRING_SPRINTF_EXTENDED_ENABLE
    bool paramHasLength;
    uint8_t paramNum1;
    uint8_t paramNum2;
    uint8_t paramNumFull;
    char fillCharacter;
  #endif

    for (p = (char *)format; *p; p++)                /* p to EOS */
    {
        if (remainLength == 0)                       /* copy, if not '%' */
        {
            /* There is no enough space, exit */
            break;
        }
        else if (*p != '%')
        {
            /* There is enough empty space, could copy */
            str[length] = *p;                        /* copy to string */
            length++;
            str[length] = '\0';                      /* EOS */
            remainLength--;
        }
        else
        {
            /* '%' character */
            p++;
  #ifdef STRING_SPRINTF_EXTENDED_ENABLE
            paramHasLength = false;
            paramNum1 = 0;    /* for standard %08x */
            paramNum2 = 0;
            fillCharacter = ' ';

            /* Check %...x (parameter after %, before x, u, f, s) */
            /* Next character is num? */
            if (IsDecimalChar(*p))
            {
                /* It is num (1. param) */
                /* Replace, if has two parameter */
                paramNum2 = DecimalCharToNum(*p);
                paramNumFull = paramNum2;
                fillCharacter = *p;
                paramHasLength = true;
                p++;

                if (IsDecimalChar(*p))
                {
                    /* xy - E.g. "%12" */
                    /* It is num (2. param) */
                    paramNum1 = paramNum2;
                    paramNum2 = DecimalCharToNum(*p);
                    p++;

                    paramNumFull = paramNum1 * 10 + paramNum2;
                }
                else if (*p == '.')
                {
                    /* x. */
                    p++;
                    if (IsDecimalChar(*p))
                    {
                        /* x.y */
                        paramNum1 = paramNum2;
                        paramNum2 = DecimalCharToNum(*p);
                        p++;
                    }
                    else
                    {
                        /* x.? */
                        /* x = paramNum1 */
                        /* ?=0 now, for correct float printing */
                        /* Do not step p pointer, because this character can be f, x, etc. */
                        paramNum1 = paramNum2;
                        paramNum2 = 0;
                    }
                }
                else
                {
                    /* x        ==>        x = fill character, y = length */
                    /* If only has one parameter */
                    fillCharacter = ' ';    /* Blank character */
                }
            }
  #endif
            /* else: *p is character) */

            /* Process next character (after '%', or etc) */
            switch (*p)
            {
                case 'd':
                {
                    /* signed (int) */
                    ival = va_arg(ap, int);    /* Decimal = signed int (~int32_t) */
  #ifndef STRING_SPRINTF_EXTENDED_ENABLE
                    size_t decLen = SignedDecimalToStringSafe(ival, &str[length], remainLength);
                    length += decLen;
                    remainLength -= decLen;
  #else /* STRING_SPRINTF_EXTENDED_ENABLE */

                    if (!paramHasLength)
                    {
                        size_t decLen = SignedDecimalToStringSafe(ival, &str[length], remainLength);
                        length += decLen;
                        remainLength -= decLen;
                    }
                    else
                    {
                        /* TODO: Now only handle 1 decimal length (<= 9)
                         * paramNum2 --> without EOS
                         * remainLength --> with EOS
                         */
                        /* TODO: Go to change SignedDecimalToStringFill to remain length contained? */
                        if (paramNum2 > remainLength)
                        {
                            /* More character wanted to print */
                            paramNum2 = remainLength;
                        }
                        /* TODO: NOT length SAFE */
                        size_t decLen = SignedDecimalToStringFill(ival, &str[length],
                                paramNum2, fillCharacter);
                        length += decLen;
                        remainLength -= decLen;
                    }
  #endif /* STRING_SPRINTF_EXTENDED_ENABLE */
                }
                    break;

                case 'u':
                {
                    /* unsigned (int) */
                    uival = va_arg(ap, int);    /* uint = Unsigned int (~uint32_t) */

  #ifndef STRING_SPRINTF_EXTENDED_ENABLE
                    uint8_t decLen = (uint8_t)UnsignedDecimalToStringSafe(uival, &str[length], remainLength);
                    length += decLen;
                    remainLength -= decLen;
  #else /* STRING_SPRINTF_EXTENDED_ENABLE */
                    if (!paramHasLength)
                    {
                        uint8_t decLen = (uint8_t)UnsignedDecimalToStringSafe(uival, &str[length], remainLength);
                        length += decLen;
                        remainLength -= decLen;
                        UNUSED_ARGUMENT(fillCharacter);
                    }
                    else
                    {
                        /* TODO: Now only handle 1 decimal length (<= 9)
                         * paramNum2 --> without EOS
                         * remainLength --> with EOS
                         */
                        if (paramNum2 >= remainLength)
                        {
                            /* More character wanted to print */
                            paramNum2 = remainLength;
                        }

                        size_t decLen = UnsignedDecimalToStringFillSafe(
                                uival, &str[length],
                                paramNum2, fillCharacter, remainLength);
                        length += decLen;
                        remainLength -= decLen;
                    }
  #endif /* STRING_SPRINTF_EXTENDED_ENABLE */
                }
                    break;

                    /* TODO: Create 'x' and 'X' to different
                     * with: STRING_SPRINTF_EXTENDED_ENABLE
                     */
                case 'x':
                case 'X':
                    /* %x - Hex - parameterized byte num */
                    uival = va_arg(ap, unsigned int);
  #ifndef STRING_SPRINTF_EXTENDED_ENABLE
                    /* Length checking is in the called function, so shall not check if (8 > remainLength) */
                    uint8_t hexLen = DecimalToHexaStringSafe(uival, &str[length], 8, remainLength);
                    length += hexLen;
                    remainLength -= hexLen;
  #else /* STRING_SPRINTF_EXTENDED_ENABLE */
                    if (paramHasLength)
                    {
                        if (paramNum2 > remainLength)
                        {
                            paramNum2 = (uint8_t)remainLength; /* clang_sa_ignore[deadcode.DeadStores] */
                        }
                        /* else: paramNum2 is ok */
                    }
                    else
                    {   /* No param */
                        /* Length checking is in the called function, so shall not check if (paramNum2 > remainLength) */
                        paramNum2 = 8; /* Default hexa print */
                    }
                    uint8_t hexLen = DecimalToHexaStringSafe(uival, &str[length], paramNum2, remainLength);
                    length += hexLen;
                    remainLength -= hexLen;
  #endif /* STRING_SPRINTF_EXTENDED_ENABLE */
                    break;

#if defined(STRING_HEXADECIMAL_FORMATS)
                case 'w':
                    /* Hex // 32 bits    // 8 hex    // 4 byte */
                    uival = va_arg(ap, unsigned int);
                    string += DecimalToHexaString(uival, string, 8);
                    break;

                case 'h':
                    /* Hex // 16 bits    // 4 hex    // 2 byte */
                    ival = va_arg(ap, int);
                    string += DecimalToHexaString(ival, string, 4);
                    break;

                case 'b':
                    /* Hex    // 8 bits    // 2 hex    // 1 byte */
                    ival = va_arg(ap, int);
                    string += DecimalToHexaString(ival, string, 2);
                    break;
#else
                case 'b':
                    /* Binary print (from uint32_t) */
                    /* Not handled in %<length>b */
                    uival = va_arg(ap,  unsigned int);
  #ifndef STRING_SPRINTF_EXTENDED_ENABLE
                    uint8_t binLength = DecimalToBinaryString(uival, &str[length], 32);
                    length += binLength;
                    remainLength -= binLength;
  #else /* STRING_SPRINTF_EXTENDED_ENABLE */
                    if (paramHasLength)
                    {
                        if (paramNumFull >= remainLength)
                        {
                            /* More character wanted to print */
                            paramNumFull = remainLength;
                        }
                    }
                    else
                    {
                        paramNumFull = 32;
                    }
                    uint8_t binLength = DecimalToBinaryString(uival, &str[length], paramNumFull);
                    length += binLength;
                    remainLength -= binLength;
  #endif /* STRING_SPRINTF_EXTENDED_ENABLE */
                    break;
#endif /* defined(STRING_HEXADECIMAL_FORMATS) */

                case 'c':
                    /* %c - char */
                    cval = (char)va_arg(ap, int);                        /* Char */
  #ifndef STRING_SPRINTF_EXTENDED_ENABLE
                    /* Default: copy one character */
                    if (remainLength > 0)
                    {
                        str[length] = cval;                            /* Copy to string */
                        length++;
                        remainLength--;
                        str[length] = '\0';
                    }
  #else /* STRING_SPRINTF_EXTENDED_ENABLE */
                    if (paramHasLength)
                    {
                        /* Copy more character */
                        if (paramNumFull > remainLength)
                        {
                            paramNumFull = (uint8_t)remainLength;
                        }
                        uint8_t copiedChar = StrCpyCharacter(&str[length], cval, paramNumFull);
                        length += copiedChar;
                        remainLength -= copiedChar;
                    }
                    else
                    {
                        /* Default: copy one character */
                        if (remainLength > 0)
                        {
                            str[length] = cval;                            /* Copy to string */
                            length++;
                            remainLength--;
                            str[length] = '\0';
                        }
                        /* else: finish */
    #ifdef STRING_SNPRINTF_CATCH_ERROR_ENABLED
                        else
                        {
                            STRING_SNPRINTF_CATCH_ERROR();
                        }
    #endif
                    }
  #endif /* STRING_SPRINTF_EXTENDED_ENABLE */
                    break;

                case 'f':
                    /* %f - float */
                    flval = (float)va_arg(ap, double);                    /* Double / Float */
  #ifndef STRING_SPRINTF_EXTENDED_ENABLE
                    uint8_t floatLength = FloatToStringSafe(flval, &str[length], 0, 6, remainLength);
                    length += floatLength;
                    remainLength -= floatLength;
  #else /* STRING_SPRINTF_EXTENDED_ENABLE */

                    /* Length safe-ing: pre-calculate the lengths */
                    if (!paramHasLength)
                    {
                        /* No param, standard format */
                        paramNum1 = 0;
                        paramNum2 = 6;
                    }
                    /* else: paramNum1, paramNum2 for integer and float part has been set */
                    uint8_t floatLength = FloatToStringSafe(flval, &str[length], paramNum1, paramNum2, remainLength);
                    length += floatLength;
                    remainLength -= floatLength;
 #endif /* STRING_SPRINTF_EXTENDED_ENABLE */
                    break;

                case 's':
                    /* %s - string */
                    sval = va_arg(ap, char*);                    /* String */

  #ifndef STRING_SPRINTF_EXTENDED_ENABLE
                    /* Standard string copy */
                    uint8_t stringLength = (uint8_t)StrCpyMax(&str[length], sval, remainLength);
                    length += stringLength;
                    remainLength -= stringLength;
  #else /* STRING_SPRINTF_EXTENDED_ENABLE */
                    if (paramHasLength)
                    {
                        /* String copy with length */
                        uint8_t stringLength = paramNumFull;
                        if (stringLength > remainLength)
                        {
                            stringLength = (uint8_t)remainLength;
                        }
                        stringLength = (uint8_t)StrCpyFixLengthWithFillCharacter(&str[length], sval, stringLength, ' ');
                        length += stringLength;
                        remainLength -= stringLength;
                    }
                    else
                    {
                        /* Standard string copy */
                        uint8_t stringLength = (uint8_t)StrCpyMax(&str[length], sval, remainLength);
                        length += stringLength;
                        remainLength -= stringLength;
                    }
  #endif /* STRING_SPRINTF_EXTENDED_ENABLE */
                    break;

                default:
                    /* Unknown % after character, e.g. % */
                    if (remainLength > 0)
                    {
                        str[length] = *p;
                        length++;
                        str[length] = '\0'; /* EOS */
                        remainLength--;
                    }
                    else
                    {
                        /* There is no enough space, exit */
                        STRING_SNPRINTF_CATCH_ERROR(); /* In development phase, shall detect */
                        break;
                    }
                    break;
            }
        }    /* End of '%' */

    }    /* End of for loop */

    /* string's end */
    ASSERT(str[length] == '\0');

    /* Return with length */
    return length;
}



/**
 * @brief       Function like snprintf(); Print to string
 */
size_t usnprintf(char * str, size_t maxLen, const char * format, ...)
{
    size_t resultLength = 0;

    if ((str == NULL) || (format == NULL) || (maxLen == 0))
        return 0;

    STRING_ASSERT(MEM_IN_RAM(str, maxLen));

    va_list ap;                                    /* argument pointer */
    va_start(ap, format);                          /* ap on arg */
    resultLength = string_printf_safe(str, maxLen, format, ap);    /* Separate and process */
    va_end(ap);                                    /* Cleaning after end */

    return resultLength;
}



#ifdef CONFIG_MODULE_STRING_UNITTEST_ENABLE
/**
 * @brief       String module Unit Test
 */
uint32_t StringHelper_UnitTest(void)
{
    #define STRING_BUFFER_LENGTH    ((uint8_t)30)
    char buffer[STRING_BUFFER_LENGTH];
    uint8_t value8;
    bool result;
    uint32_t value32;
    int32_t ivalue32;
    float fvalue;
    char *splitted[10];
    const char * cpString;
    size_t length;


    /* Start of unittest */
    UnitTest_Start("StringHelper", __FILE__);


    //* String compare StrCmp() */
    /* Equal: */
    UNITTEST_ASSERT(!StrCmp("example", "example"), "StrCmp error");
    /* Not equal: */
    UNITTEST_ASSERT(StrCmp("example", "example1"), "StrCmp error");
    UNITTEST_ASSERT(StrCmp("example1", "example2"), "StrCmp error");
    UNITTEST_ASSERT(StrCmp("example1", "example"), "StrCmp error");


    /* StrCmpFirst() */
    /* Equal: */
    UNITTEST_ASSERT(!StrCmpFirst("example", "example"), "StrCmpFirst error");
    UNITTEST_ASSERT(!StrCmpFirst("example", "example1"), "StrCmpFirst error");
    /* Not equal: */
    UNITTEST_ASSERT(StrCmpFirst("example1", "example2"), "StrCmpFirst error");
    UNITTEST_ASSERT(StrCmpFirst("example1", "example"), "StrCmpFirst error");


    /* Sprintf tests */

  #ifndef STRING_SPRINTF_EXTENDED_ENABLE
    /* Extend turned off */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%5u",123);
    UNITTEST_ASSERT(!StrCmp(buffer, "5u"), "Extend error"); /* u will not detected as format */

    usnprintf(buffer, STRING_BUFFER_LENGTH, "%5u");
    UNITTEST_ASSERT(!StrCmp(buffer, "5u"), "Extend error"); /* u will not detected as format */

    usnprintf(buffer, STRING_BUFFER_LENGTH, "%5");
    UNITTEST_ASSERT(!StrCmp(buffer, "5"), "Extend error"); /* 5 will not format anything */
  #endif /* STRING_SPRINTF_EXTENDED_ENABLE */

    usnprintf(buffer, STRING_BUFFER_LENGTH, "%%");
    UNITTEST_ASSERT(!StrCmp(buffer, "%"), "Unknown format error"); /* % - format and received unhandled % --> like escaped % */


    /* Float print tests */

    /* "123.339996" ~ like "%0.6" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%f",123.34f);
    UNITTEST_ASSERT(!StrCmp(buffer, "123.339996"), "Float error");

  #ifdef STRING_SPRINTF_EXTENDED_ENABLE
    /* "123" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%1.0f",123.34f);
    UNITTEST_ASSERT(!StrCmp(buffer, "123"), "Float error");

    /* "123.3" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%1.1f",123.34f);
    UNITTEST_ASSERT(!StrCmp(buffer, "123.3"), "Float error");

    /* "123.33999" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%1.5f",123.34f);
    UNITTEST_ASSERT(!StrCmp(buffer, "123.33999"), "Float error");

    /* "  123.3" */
    usnprintf(buffer, STRING_BUFFER_LENGTH,  "%5.1f",123.34f);
    UNITTEST_ASSERT(!StrCmp(buffer, "  123.3"), "Float error");

    /* "123.33999" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%5.5f",123.34f);
    UNITTEST_ASSERT(!StrCmp(buffer, "  123.33999"), "Float error");

    /* "123.33999" */
    /* TODO: Handle "%.f" ? */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%0.5f",123.34f);
    UNITTEST_ASSERT(!StrCmp(buffer, "123.33999"), "Float error");

    /* TODO: Handle "%.f" ? */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%0.1f",123.34f);
    UNITTEST_ASSERT(!StrCmp(buffer, "123.3"), "Float error");
  #endif /* STRING_SPRINTF_EXTENDED_ENABLE */


    /* Integer print tests */

    /* Printed: "123" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%u",123);
    UNITTEST_ASSERT(!StrCmp(buffer, "123"), "Integer error");

    usnprintf(buffer, STRING_BUFFER_LENGTH, "%u",123456789);
    UNITTEST_ASSERT(!StrCmp(buffer, "123456789"), "Integer error");

    usnprintf(buffer, STRING_BUFFER_LENGTH, "%u",1234567890); /* Billion */
    UNITTEST_ASSERT(!StrCmp(buffer, "1234567890"), "Integer error");

  #ifdef STRING_SPRINTF_EXTENDED_ENABLE
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%0u",123);
    UNITTEST_ASSERT(!StrCmp(buffer, "123"), "Integer error");

    /* Printed:    "123" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%1u",123);
    UNITTEST_ASSERT(!StrCmp(buffer, "123"), "Integer error");

    /* Printed: " 123" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%4u",123);
    UNITTEST_ASSERT(!StrCmp(buffer, " 123"), "Integer error");

    /* Printed: "      123" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%9u",123);
    UNITTEST_ASSERT(!StrCmp(buffer, "      123"), "Integer error");

    /* Printed: "00123", it is OK */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%05u", 123);
    UNITTEST_ASSERT(!StrCmp(buffer, "00123"), "Integer error");

    /* Integer print tests (wrong examples): */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%A5",123);        /* Printed: "A5", because 'A' is not a number and not format character*/
    UNITTEST_ASSERT(!StrCmp(buffer, "A5"), "Integer error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%-5u",123);        /* Printed: "-5u", because '-' is not a number */
    UNITTEST_ASSERT(!StrCmp(buffer, "-5u"), "Integer error");
  #endif /* STRING_SPRINTF_EXTENDED_ENABLE */


    /* Signed Integer print tests: */

    /* Printed: "-123" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%d",-123);
    UNITTEST_ASSERT(!StrCmp(buffer, "-123"), "Integer error");

#ifdef STRING_SPRINTF_EXTENDED_ENABLE
    /* Printed: "-123" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%0d",-123);
    UNITTEST_ASSERT(!StrCmp(buffer, "-123"), "Integer error");

    /* Printed:    "-123" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%1d",-123);
    UNITTEST_ASSERT(!StrCmp(buffer, "-123"), "Integer error");

    /* Printed: "-123" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%4d",-123);
    UNITTEST_ASSERT(!StrCmp(buffer, "-123"), "Integer error");

    /* Printed: "     -123" */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%9d",-123);
    UNITTEST_ASSERT(!StrCmp(buffer, "     -123"), "Integer error");

    /* Printed: "-0123", it is OK */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%05d",-123);
    UNITTEST_ASSERT(!StrCmp(buffer, "-00123"), "Integer error");
#endif /* STRING_SPRINTF_EXTENDED_ENABLE */


    /* Hexadecimal print tests: */
    /* TODO: Move to test function with list*/
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%x",0xFFFFFFFF);
    UNITTEST_ASSERT(!StrCmp(buffer, "0xFFFFFFFF"), "Hexadecimal error");
#ifdef STRING_SPRINTF_EXTENDED_ENABLE
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%01x",0xFFFFFFFF);
    UNITTEST_ASSERT(!StrCmp(buffer, "0xF"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%02x",0xFFFFFFFF);
    UNITTEST_ASSERT(!StrCmp(buffer, "0xFF"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%03x",0xFFFFFFFF);
    UNITTEST_ASSERT(!StrCmp(buffer, "0xFFF"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%04x",0xFFFFFFFF);
    UNITTEST_ASSERT(!StrCmp(buffer, "0xFFFF"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%05x",0xFFFFFFFF);
    UNITTEST_ASSERT(!StrCmp(buffer, "0xFFFFF"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%06x",0xFFFFFFFF);
    UNITTEST_ASSERT(!StrCmp(buffer, "0xFFFFFF"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%07x",0xFFFFFFFF);
    UNITTEST_ASSERT(!StrCmp(buffer, "0xFFFFFFF"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%08x",0xFFFFFFFF);
    UNITTEST_ASSERT(!StrCmp(buffer, "0xFFFFFFFF"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%09x",0xFFFFFFFF);
    UNITTEST_ASSERT(!StrCmp(buffer, "0x"), "Hexadecimal error");

    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%01x",0x12345678);
    UNITTEST_ASSERT(!StrCmp(buffer, "0x8"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%02x",0x12345678);
    UNITTEST_ASSERT(!StrCmp(buffer, "0x78"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%03x",0x12345678);
    UNITTEST_ASSERT(!StrCmp(buffer, "0x678"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%04x",0x12345678);
    UNITTEST_ASSERT(!StrCmp(buffer, "0x5678"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%05x",0x12345678);
    UNITTEST_ASSERT(!StrCmp(buffer, "0x45678"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%06x",0x12345678);
    UNITTEST_ASSERT(!StrCmp(buffer, "0x345678"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%07x",0x12345678);
    UNITTEST_ASSERT(!StrCmp(buffer, "0x2345678"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%08x",0x12345678);
    UNITTEST_ASSERT(!StrCmp(buffer, "0x12345678"), "Hexadecimal error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0x%09x",0x12345678);
    UNITTEST_ASSERT(!StrCmp(buffer, "0x"), "Hexadecimal error");
#endif /* STRING_SPRINTF_EXTENDED_ENABLE */


    /* Binaries printf */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0b%b",0x000000FF);
    UNITTEST_ASSERT(!StrCmp(buffer, "0b11111111"), "Binary error");
    usnprintf(buffer, STRING_BUFFER_LENGTH, "0b%b",0x00000055);
    UNITTEST_ASSERT(!StrCmp(buffer, "0b1010101"), "Binary error");

    /* String (%s) */

    /* Standard %s print */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%s", "text");
    UNITTEST_ASSERT(!StrCmp(buffer, "text"), "String error (%s)");

#ifdef STRING_SPRINTF_EXTENDED_ENABLE
    /* max 5 character */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%5s", "longtext");
    UNITTEST_ASSERT(!StrCmp(buffer, "longt"), "String error (%s)");

    /* 10 character, need fill with ' ' */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%10s", "longtext");
    UNITTEST_ASSERT(!StrCmp(buffer, "longtext  "), "String error (%s)");

    /* max 10 character */
    usnprintf(buffer, STRING_BUFFER_LENGTH, "%10s", "toolongtext");
    UNITTEST_ASSERT(!StrCmp(buffer, "toolongtex"), "String error (%s)");
#endif /* STRING_SPRINTF_EXTENDED_ENABLE */


    /* string -> num converters */

    /* string -> decimal */

    /* Byte */

    /* Good bytes */
    result = StringHexByteToNum("00", &value8);
    UNITTEST_ASSERT(result, "StringByteToNum error");
    UNITTEST_ASSERT((value8 == 0x00), "StringByteToNum error");

    result = StringHexByteToNum("15", &value8);
    UNITTEST_ASSERT(result, "StringByteToNum error");
    UNITTEST_ASSERT((value8 == 0x15), "StringByteToNum error");

    result = StringHexByteToNum("FF", &value8);
    UNITTEST_ASSERT(result, "StringByteToNum error");
    UNITTEST_ASSERT((value8 == 0xFF), "StringByteToNum error");

    /* Wrong byte */
    result = StringHexByteToNum("FG", &value8);
    UNITTEST_ASSERT(!result, "StringByteToNum error");


    /* Hexs */

    /* Good hex */
    value32 = 0;
    result = StringHexToNum("12345678", &value32);
    UNITTEST_ASSERT(result, "StringHexToNum error");
    UNITTEST_ASSERT(value32 == 0x12345678, "StringHexToNum error");

    /* Wrong hex */
    value32 = 0;
    /*                          x  <-- Wrong character */
    result = StringHexToNum("123G5678", &value32);
    UNITTEST_ASSERT(!result, "StringHexToNum error");


    /* Binaries */
    value32 = 0;
    result = StringBinaryToNum("010101", &value32);
    UNITTEST_ASSERT(result, "StringBinaryToNum error");
    UNITTEST_ASSERT(value32 == 0x15, "StringHexToNum error");

    value32 = 0;
    result = StringBinaryToNum("01010101010101010101010101010101", &value32);
    UNITTEST_ASSERT(result, "StringBinaryToNum error");
    UNITTEST_ASSERT(value32 == 0x55555555, "StringHexToNum error");

    value32 = 0;
    result = StringBinaryToNum("010101010101010101010101010101010", &value32);
    UNITTEST_ASSERT(!result, "StringBinaryToNum error"); /* 33 bit tried to convert, result shall be "failed" */
    UNITTEST_ASSERT(value32 == 0, "StringHexToNum error"); /* Original value has not changed */


    /* Decimals */

    /* Good decimals */
    ivalue32 = 0;
    result = StringToSignedDecimalNum("-123",&ivalue32);
    UNITTEST_ASSERT(result, "StringToSignedDecimalNum error");
    UNITTEST_ASSERT(ivalue32 == -123, "StringToSignedDecimalNum error");

    ivalue32 = 0;
    result = StringToUnsignedDecimalNum("123",&value32);
    UNITTEST_ASSERT(result, "StringToUnsignedDecimalNum error");
    UNITTEST_ASSERT(value32 == 123, "StringToUnsignedDecimalNum error");

    /* Wrong decimals */
    ivalue32 = 0;
    result = StringToSignedDecimalNum("-123a",&ivalue32);
    UNITTEST_ASSERT(!result, "StringToSignedDecimalNum error");

    ivalue32 = 0;
    result = StringToUnsignedDecimalNum("-123",&value32);
    UNITTEST_ASSERT(!result, "StringToUnsignedDecimalNum error");


    /* bool StringToUnsignedDecimalNumWithLength(const char * str, uint32_t * value, uint8_t stringLength) */
    result = StringToUnsignedDecimalNumWithLength("123", &value32, 3);
    UNITTEST_ASSERT(result, "StringToUnsignedDecimalNumWithLength error");
    UNITTEST_ASSERT(value32 == 123, "StringToUnsignedDecimalNumWithLength error");

    result = StringToUnsignedDecimalNumWithLength("123", &value32, 2); /* Cut end */
    UNITTEST_ASSERT(result, "StringToUnsignedDecimalNumWithLength error");
    UNITTEST_ASSERT(value32 == 12, "StringToUnsignedDecimalNumWithLength error");

    result = StringToUnsignedDecimalNumWithLength("123", &value32, 4); /* extended length, however fix length shall be calculated correctly */
    UNITTEST_ASSERT(!result, "StringToUnsignedDecimalNumWithLength error");

    result = StringToUnsignedDecimalNumWithLength("-123", &value32, 4); /* wrong start (-) at unsigned number */
    UNITTEST_ASSERT(!result, "StringToUnsignedDecimalNumWithLength error");

    result = StringToUnsignedDecimalNumWithLength("a123", &value32, 4); /* wrong start (a) */
    UNITTEST_ASSERT(!result, "StringToUnsignedDecimalNumWithLength error");


    /* Float */

    /* Good Float test */
    fvalue = 0.0f;
    result = StringToFloat("-123.3499", &fvalue); /* CLANG WARNING: Possible to not change fvalue... */
    UNITTEST_ASSERT(result, "StringToFloat error");
    UNITTEST_ASSERT(((fvalue < -123.3498) && (fvalue > -123.3500)), "StringToFloat error");

    /* Wrong Float test */
    fvalue = 0.0f;
    result = StringToFloat("-123a.3999", &fvalue);
    UNITTEST_ASSERT(!result, "StringToFloat error");



    /* StringToBool test */
    typedef struct {
        const char * testString;
        bool expectedResult;
        bool expectedReturnValue;
    } StringBoolTest_t;

    StringBoolTest_t StringBoolTestTable[] =
    {
        /*test result return */
        { "1", true, true },
        { "0", false, true },

        { "true", true, true },
        { "on", true, true },
        { "enable", true, true },
        { "set", true, true },

        { "false", false, true },
        { "off", false, true },
        { "disable", false, true },
        { "reset", false, true },

        { "2", false, false },
        { "-1", false, false },

        { "uglywrongtest", false, false },

        { NULL, false, false },
    };

    uint8_t i;
    for (i = 0; i < NUM_OF(StringBoolTestTable); i++)
    {
        bool testResultValue = false;
        bool testReturnValue;

        testReturnValue = StringToBool(StringBoolTestTable[i].testString, &testResultValue);

        UNITTEST_ASSERT(testReturnValue == StringBoolTestTable[i].expectedReturnValue, "StringToBool return error");
        if (StringBoolTestTable[i].expectedReturnValue)
        {
            /* Expected right convert value, because the return is OK */
            UNITTEST_ASSERT(testResultValue == StringBoolTestTable[i].expectedResult, "StringToBool convert error");
        }
    }


    /* String function testing */

    /* Test: uint8_t StrCpyCharacter(char *dest, char c, uint8_t num) */
    StrCpyCharacter(buffer, 'a', 10);

    for (i = 0; i < 10; i++)
    {
        UNITTEST_ASSERT(buffer[i] == 'a', "StrCpyCharacter error");
    }
    /* Check end character */
    UNITTEST_ASSERT(buffer[10] == '\0', "StrCpyCharacter error");


    /* Test: Test StrTrim() */
    StrCpy(buffer, "String with spaces end...    ");
    length = StrTrim(buffer);
    UNITTEST_ASSERT(length == StringLength("String with spaces end..."), "StrTrim return error");
    UNITTEST_ASSERT(!StrCmp("String with spaces end...", buffer), "StrTrim error");

    StrCpy(buffer, "End without space.");
    length = StrTrim(buffer);
    UNITTEST_ASSERT(length == StringLength("End without space."), "StrTrim return error");
    UNITTEST_ASSERT(!StrCmp("End without space.", buffer), "StrTrim error");


    /* STRING_FindCharacter() */
    StrCpy(buffer, "longtexttofinding");
    /* Valid finding */
    cpString = STRING_FindCharacter((const char *)buffer, 't');
    UNITTEST_ASSERT(cpString == buffer+4, "FindCharacter wrong find error");
    cpString = STRING_FindCharacter(buffer, 'l');
    UNITTEST_ASSERT(cpString == buffer, "FindCharacter wrong find error");
    /* Invalid finding */
    cpString = STRING_FindCharacter(buffer, 'z');
    UNITTEST_ASSERT(cpString == NULL, "FindCharacter not find error error");
    /* 0 length string */
    cpString = STRING_FindCharacter("", 'z');
    UNITTEST_ASSERT(cpString == NULL, "FindCharacter 0 length error");
    /* Null pointer */
    cpString = STRING_FindCharacter(NULL, 'z');
    UNITTEST_ASSERT(cpString == NULL, "FindCharacter null pointererror");
    /* Unchangeable string */
    UNITTEST_ASSERT(!StrCmp(buffer, "longtexttofinding"), "FindCharacter changed original string");


    /* STRING_FindCharacters() */
    StrCpy(buffer, "longtexttofinding");
    /* Valid finding */
    cpString = STRING_FindCharacters(buffer, "text");
    UNITTEST_ASSERT(cpString == buffer+4, "FindCharacter wrong find error");
    cpString = STRING_FindCharacters(buffer, "long");
    UNITTEST_ASSERT(cpString == buffer, "FindCharacter wrong find error");
    /* Invalid finding */
    cpString = STRING_FindCharacters(buffer, "z");
    UNITTEST_ASSERT(cpString == NULL, "FindCharacter not find error error");
    /* 0 length string */
    cpString = STRING_FindCharacters("", "z");
    UNITTEST_ASSERT(cpString == NULL, "FindCharacter 0 length error");
    /* Null pointer */
    cpString = STRING_FindCharacters(NULL, "z");
    UNITTEST_ASSERT(cpString == NULL, "FindCharacter null pointererror");
    /* Unchangeable string */
    UNITTEST_ASSERT(!StrCmp(buffer, "longtexttofinding"), "FindCharacter changed original string");


    /* STRING_FindString() */
    StrCpy(buffer, "longtexttofinding");
    /* Valid finding */
    cpString = STRING_FindString(buffer, "text");
    UNITTEST_ASSERT(cpString == buffer+4, "FindString error");
    /* There is no */
    cpString = STRING_FindString(buffer, "wrongtext");
    UNITTEST_ASSERT(cpString == NULL, "FindString error");
    /* Overflow */
    cpString = STRING_FindString(buffer, "findingoverflow");
    UNITTEST_ASSERT(cpString == NULL, "FindString overflow error");
    /* 0 length string */
    cpString = STRING_FindString("", "findingoverflow");
    UNITTEST_ASSERT(cpString == NULL, "FindString 0 length error");
    cpString = STRING_FindString("longtexttofinding", "");
    UNITTEST_ASSERT(cpString == NULL, "FindString 0 length error");
    /* Test with NULL pointer */
    cpString = STRING_FindString("longtexttofinding", NULL);
    UNITTEST_ASSERT(cpString == NULL, "FindString null pointer error");
    cpString = STRING_FindString(NULL, "findingoverflow");
    UNITTEST_ASSERT(cpString == NULL, "FindString null pointer error");
    /* Unchangeable string */
    UNITTEST_ASSERT(!StrCmp(buffer, "longtexttofinding"), "FindString changed original string");


    /* STRING_Splitter() */
    StrCpy(buffer, "need to separate this text");
    value8 = STRING_Splitter(buffer, " ", splitted, 10);
    UNITTEST_ASSERT(value8 == 5, "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[0],"need"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[1],"to"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[2],"separate"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[3],"this"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[4],"text"), "STRING_Splitter error");
    UNITTEST_ASSERT(splitted[5] == NULL, "STRING_Splitter error");

    StrCpy(buffer, "text");
    value8 = STRING_Splitter(buffer, " ", splitted, 10);
    UNITTEST_ASSERT(value8 == 1, "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[0],"text"), "STRING_Splitter error");
    UNITTEST_ASSERT(splitted[1] == NULL, "STRING_Splitter error");

    StrCpy(buffer, "");
    value8 = STRING_Splitter(buffer, " ", splitted, 10);
    UNITTEST_ASSERT(value8 == 0, "STRING_Splitter error");
    UNITTEST_ASSERT(splitted[0] == NULL, "STRING_Splitter error");


    /* STRING_Splitter - more delimiter */
    StrCpy(buffer, "bla1 bla2_bla3");
    value8 = STRING_Splitter(buffer, " _", splitted, 10);
    UNITTEST_ASSERT(value8 == 3, "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[0],"bla1"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[1],"bla2"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[2],"bla3"), "STRING_Splitter error");
    UNITTEST_ASSERT(splitted[3] == NULL, "STRING_Splitter error");


    /* STRING_Splitter - delimiter "list" string */
    /*                  **    **      */
    StrCpy(buffer, "bla1  bla2__bla3");
    value8 = STRING_Splitter(buffer, " _", splitted, 10);

    /* TODO: Error: "bla1", "", "bla2", "", "bla3" were the result
     * but normally expected: "bla1", "bla2", "bla3"
     *
     * But example gratia, for GPS: 0,0,,,,0 --> it will be better...
     */
  #if 0
    UNITTEST_ASSERT(value8 == 3, "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[0],"bla1"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[1],"bla2"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[2],"bla3"), "STRING_Splitter error");
    UNITTEST_ASSERT(splitted[3] == NULL, "STRING_Splitter error");
  #else
    UNITTEST_ASSERT(value8 == 5, "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[0],"bla1"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[1],""), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[2],"bla2"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[3],""), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[4],"bla3"), "STRING_Splitter error");
    UNITTEST_ASSERT(splitted[5] == NULL, "STRING_Splitter error");
  #endif

    /* Start with delimiter, end with delimiter */

    StrCpy(buffer, "  bla1__");
    value8 = STRING_Splitter(buffer, " _", splitted, 10);
#if 0
    UNITTEST_ASSERT(value8 == 1, "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[0],"bla1"), "STRING_Splitter error");
    UNITTEST_ASSERT(splitted[1] == NULL, "STRING_Splitter error");
#else
    UNITTEST_ASSERT(value8 == 5, "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[0],""), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[1],""), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[2],"bla1"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[3],""), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[4],""), "STRING_Splitter error");
    UNITTEST_ASSERT(splitted[5] == NULL, "STRING_Splitter error");
#endif

    /* Check with overfull */
    StrCpy(buffer, "bla1 bla2 bla3 too much");
    char * expectedDoNotReachString = "mystring";
    splitted[1] = expectedDoNotReachString; /* Expect: do not modify by function*/
    value8 = STRING_Splitter(buffer, " _", splitted, 1);
    UNITTEST_ASSERT(value8 == 1, "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[0],"bla1"), "STRING_Splitter error");
    UNITTEST_ASSERT(splitted[1] == expectedDoNotReachString, "STRING_Splitter error");


    /* Use case test: GPS */
    StrCpy(buffer, "0.1,2,3,,,,0xEF");
    value8 = STRING_Splitter(buffer, ",", splitted, 8);
    UNITTEST_ASSERT(value8 == 7, "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[0],"0.1"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[1],"2"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[2],"3"), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[3],""), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[4],""), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[5],""), "STRING_Splitter error");
    UNITTEST_ASSERT(!StrCmp(splitted[6],"0xEF"), "STRING_Splitter error");
    UNITTEST_ASSERT(splitted[7] == NULL, "STRING_Splitter error");


    /* Test StrAppend */
    /* Test size_t CharAppend(char *dest, const char c) */
    StrCpy(buffer, "start");
    length = CharAppend(buffer,'a');
    UNITTEST_ASSERT(length == StringLength("starta"), "StrAppend error");
    UNITTEST_ASSERT(!StrCmp(buffer,"starta"), "StrAppend error");

    StrCpy(buffer, "");
    length = CharAppend(buffer, 'a');
    UNITTEST_ASSERT(length == StringLength("a"), "StrAppend error");
    UNITTEST_ASSERT(!StrCmp(buffer,"a"), "StrAppend error");


    /* Test safe (length) functions */
    usnprintf(buffer, 30, "%d %u 1234 %c %s", 1, 2, 'a', "str");
    UNITTEST_ASSERT(!StrCmp(buffer, "1 2 1234 a str"), "usnprintf error");

    /* Length test */
    buffer[29] = 'e';
    usnprintf(buffer, 30, "12345678901234567890123456789"); /* OK - last char EOS */
    UNITTEST_ASSERT(!StrCmp(buffer, "12345678901234567890123456789"), "usnprintf error");
    UNITTEST_ASSERT(buffer[29] == '\0', "usnprintf error");

    buffer[29] = 'e';
    usnprintf(buffer, 30, "123456789012345678901234567890"); /* OK - last char EOS */
    UNITTEST_ASSERT(!StrCmp(buffer, "12345678901234567890123456789"), "usnprintf error");
    /* Be careful! Last char shall be EOS because the max length */
    UNITTEST_ASSERT(buffer[29] == '\0', "usnprintf error");

#ifdef STRING_SPRINTF_EXTENDED_ENABLE
    /* usnprintf */
    usnprintf(buffer, 30, "%1d %1u 1234 %1c %3s", 1, 2, 'a', "str");
    UNITTEST_ASSERT(!StrCmp(buffer, "1 2 1234 a str"), "usnprintf error");
#endif


    /* %9dblalba" */
#ifdef STRING_SPRINTF_EXTENDED_ENABLE
    /* %9d --> decimal */
    usnprintf(buffer, 30, "%9dblabla", 123456789);
    UNITTEST_ASSERT(!StrCmp(buffer, "123456789blabla"), "usnprintf error");
#else
    /* %9d has not supported */
    usnprintf(buffer, 30, "%9dblabla", 123456789);
    UNITTEST_ASSERT(!StrCmp(buffer, "9dblabla"), "usnprintf error");
#endif



    /* TODO: Add other test, if usnprintf improved */


    /* Test StrAppendSafe */
    buffer[0] = '\0';
    StrAppendSafe(buffer, "First string, ", 30);
    StrAppendSafe(buffer, "Second string", 30);
    UNITTEST_ASSERT(!StrCmp(buffer, "First string, Second string"), "StrAppendSafe error");

    /* Check overflow */
    buffer[0] = '\0';
    buffer[19] = 0xAA;
    buffer[20] = 0xBB;
    StrAppendSafe(buffer, "1234567890", 20);
    StrAppendSafe(buffer, "1234567890", 20);
    UNITTEST_ASSERT(!StrCmp(buffer, "1234567890123456789"), "StrAppendSafe error");
    UNITTEST_ASSERT((buffer[19] == '\0'), "StrAppendSafe overflow error");
    UNITTEST_ASSERT(((uint8_t)buffer[20] == 0xBB), "StrAppendSafe overflow error");


    /* StringLower() */
    StrCpy(buffer, "123ABCabc$");
    StringLower(buffer);
    UNITTEST_ASSERT(!StrCmp(buffer, "123abcabc$"), "StringLower error");


    /* StringUpper() */
    StrCpy(buffer, "123ABCabc$");
    StringUpper(buffer);
    UNITTEST_ASSERT(!StrCmp(buffer, "123ABCABC$"), "StringUpper error");


    /* UnsignedDecimalToStringFillSafe */
    /* uint8_t UnsignedDecimalToStringFillSafe(uint32_t value, char *str, uint8_t fillLength, char fillCharacter, uint8_t maxLen) */
    length = UnsignedDecimalToStringFillSafe(123, buffer, 0, ' ', 3);
    UNITTEST_ASSERT(length == 3, "UnsignedDecimalToStringFillSafe error");
    UNITTEST_ASSERT(!StrCmp(buffer, "123"), "UnsignedDecimalToStringFillSafe error");

    length = UnsignedDecimalToStringFillSafe(1234, buffer, 0, ' ', 3); /* End of number - (4) has been cut */
    UNITTEST_ASSERT(length == 3, "UnsignedDecimalToStringFillSafe error");
    UNITTEST_ASSERT(!StrCmp(buffer, "12?"), "UnsignedDecimalToStringFillSafe error");

    length = UnsignedDecimalToStringFillSafe(1234, buffer, 0, ' ', 4);
    UNITTEST_ASSERT(length == 4, "UnsignedDecimalToStringFillSafe error");
    UNITTEST_ASSERT(!StrCmp(buffer, "1234"), "UnsignedDecimalToStringFillSafe error");


    length = UnsignedDecimalToStringFillSafe(123, buffer, 0, ' ', 4);
    UNITTEST_ASSERT(length == 3, "UnsignedDecimalToStringFillSafe error");
    UNITTEST_ASSERT(!StrCmp(buffer, "123"), "UnsignedDecimalToStringFillSafe error");

    length = UnsignedDecimalToStringFillSafe(123, buffer, 1, ' ', 4); /* Fillength < required length --> dont care */
    UNITTEST_ASSERT(length == 3, "UnsignedDecimalToStringFillSafe error");
    UNITTEST_ASSERT(!StrCmp(buffer, "123"), "UnsignedDecimalToStringFillSafe error");

    length = UnsignedDecimalToStringFillSafe(123, buffer, 2, ' ', 4); /* However 4 length, 3 number length, but only 2 fill character required --> shall not put */
    UNITTEST_ASSERT(length == 3, "UnsignedDecimalToStringFillSafe error");
    UNITTEST_ASSERT(!StrCmp(buffer, "123"), "UnsignedDecimalToStringFillSafe error");

    length = UnsignedDecimalToStringFillSafe(123, buffer, 3, ' ', 4); /* However 4 length, 3 number length, but only 3 fill character --> shall not put */
    UNITTEST_ASSERT(length == 3, "UnsignedDecimalToStringFillSafe error");
    UNITTEST_ASSERT(!StrCmp(buffer, "123"), "UnsignedDecimalToStringFillSafe error");

    length = UnsignedDecimalToStringFillSafe(123, buffer, 4, ' ', 4); /* However 4 length, 3 number length, and required 4 filllength --> shall put fillchar */
    UNITTEST_ASSERT(length == 4, "UnsignedDecimalToStringFillSafe error");
    UNITTEST_ASSERT(!StrCmp(buffer, " 123"), "UnsignedDecimalToStringFillSafe error");

    length = UnsignedDecimalToStringFillSafe(123, buffer, 5, ' ', 4); /* However 4 length, 3 number length, but required 5 fillength, but could not put: only 1 will be put */
    UNITTEST_ASSERT(length == 4, "UnsignedDecimalToStringFillSafe error");
    UNITTEST_ASSERT(!StrCmp(buffer, " 123"), "UnsignedDecimalToStringFillSafe error");


    /* End of unittest */
    return UnitTest_End();
}
#endif    /* #ifdef CONFIG_MODULE_STRING_UNITTEST_ENABLE */



/* Other printf: */
#if 0
/* Link: http://electronics.stackexchange.com/questions/206113/how-do-i-use-the-printf-function-on-stm32/206118 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void vprint(const char *fmt, va_list argp)
{
    char string[200];
    if (0 < vsprintf(string,fmt,argp)) /* build string */
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)string, strlen(string), 0xffffff); /* send message via UART */
    }
}

void my_printf(const char *fmt, ...) /* custom printf() function */
{
    va_list argp;
    va_start(argp, fmt);
    vprint(target, fmt, argp);
    va_end(argp);
}
#endif


/* Other printf: */
/* Link: http://www.sparetimelabs.com/tinyprintf/tinyprintf.php */
