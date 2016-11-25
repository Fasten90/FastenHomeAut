/*
 *		string.h
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		string manipulation functions
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef STRING_H_
#define STRING_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"

#include <stdbool.h>	// For bool

#include <stdarg.h>		// for "..." parameters in uprintf function


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/
typedef enum
{
	Print_Unknown,
	Print_DebugUart,
	Print_OtherUart,
	// Do not use:
	Print_Count
} PrintDevice_t;

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

// Decimal --> String converters
uint8_t SignedDecimalToString (int32_t value, char *str);
uint8_t UnsignedDecimalToString (uint32_t value, char *str);
uint8_t UnsignedDecimalLength (uint32_t value);
uint8_t UnsignedDecimalToStringFill (uint32_t value, char *str, uint8_t fillLength, char fillCharacter);
uint8_t SignedDecimalToStringFill (int32_t value, char *str, uint8_t fillLength, char fillCharacter);

// Hexadecimal --> String converters
uint8_t DecimalToHexaString (uint32_t value, char *str, uint8_t length);
uint8_t ByteToHexaString (uint8_t byte, char *str);
char HexToHexChar (uint8_t octet);

// Float --> String converter
uint8_t FloatToString (float value, char *str, uint8_t integerLength, uint8_t fractionLength);

// String checkers
bool IsHexChar (const char c);
bool IsDecimalChar (const char c);

uint8_t StringIsHexadecimalString (const char *str);
uint8_t StringIsUnsignedDecimalString (const char *str);
uint8_t StringIsSignedDecimalString (const char *str);

// HexString --> Integer converters
bool HexCharToHex (const char c, uint8_t *hexValue);
bool StringByteToNum (const char *str, uint8_t *byte);
bool StringHexToNum (const char *str, uint32_t *hexValue);

// Num string --> Integer converters
uint8_t DecimalCharToNum (char c);
bool StringToUnsignedDecimalNum (const char *str, uint32_t *value);
bool StringToSignedDecimalNum (const char *str, int32_t *value);

// String --> Float converter
bool StringToFloat (const char *str, float *Num);


// Standard string functions:
uint8_t StringLength (const char *str );
uint8_t StrCmp (const char * str1, const char *str2);
uint8_t StrCmpWithLength (const char * str1, const char *str2, uint8_t length);
uint8_t StrCpy (char *dest, const char *src );
uint8_t StrCpyFixLength (char *dest, const char *src, uint8_t length );
uint8_t StrCpyFixLengthWithFillCharacter (char *dest, const char *str, uint8_t length, char fillChar);
uint8_t StrCpyMax (char *dest, const char *str, uint8_t maxLength);
uint8_t StrAppend (char *dest, const char *src);

// Find & Separators
int16_t FindString (const char *findString, const char *str);

// Send functions
// !! IMPORTANT !! Send formatted string on USART !!
uint8_t string_printf (char *str, const char *format, va_list ap);
uint8_t usprintf (char *str, const char *format, ...);
uint8_t uprintf (const char *format, ...);
uint8_t duprintf (const PrintDevice_t dev, const char *format, ...);

void STRING_UnitTest (void);


#endif /* STRING_H_ */
