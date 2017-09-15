/*
 *		String.h
 *		Created on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		String manipulation functions
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
 */

#ifndef STRING_H_
#define STRING_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "include.h"

#include <stdbool.h>	// For bool

#include <stdarg.h>		// For "..." parameters in uprintf function



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

// size_t strlen ( const char * str );
#define strlen(str)					StringLength(str)


// int strncmp ( const char * str1, const char * str2, size_t num );
// return: 0 equal, >0 and <0 is not equal
// uint8_t StrCmpWithLength(const char * ch1, const char *ch2, uint8_t length)
// return: 0 equal, 1 not equal
// TODO: Be careful, return values are not equal
#define strncmp(str1, str2, num)	StrCmpWithLength(str1, str2, num)


// char * strncpy ( char * destination, const char * source, size_t num );
// uint8_t StrCpyMax(char *dest, const char *str, uint8_t maxLength)
#define strncpy(dest, src, num)		StrCpyMax(dest, src, num)


#define printf(...)					uprintf(__VA_ARGS__)


// char * strstr ( const char *, const char * )
// TODO: Be careful, return value types not equal (uint vs. char *)
#define strstr(str1, str2)			STRING_FindString(str1, str2)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

// Decimal --> String converters
uint8_t SignedDecimalToString(int32_t value, char *str);
uint8_t UnsignedDecimalToString(uint32_t value, char *str);
uint8_t UnsignedDecimalLength(uint32_t value);
uint8_t UnsignedDecimalToStringFill(uint32_t value, char *str,
		uint8_t fillLength, char fillCharacter);
uint8_t SignedDecimalToStringFill(int32_t value, char *str,
		uint8_t fillLength, char fillCharacter);

// Num -> Binary String converter
uint8_t DecimalToBinaryString(uint32_t value, char *str, uint8_t maxLength);

// Hexadecimal --> String converters
uint8_t DecimalToHexaString(uint32_t value, char *str, uint8_t length);
uint8_t ByteToHexaString(uint8_t byte, char *str);
char HexToHexChar(uint8_t octet);

// Float --> String converter
uint8_t FloatToString(float value, char *str,
		uint8_t integerLength, uint8_t fractionLength);

// String checkers
bool IsHexChar(const char c);
bool IsDecimalChar(const char c);

uint8_t StringIsHexadecimalString(const char *str);
uint8_t StringIsUnsignedDecimalString(const char *str);
uint8_t StringIsSignedDecimalString(const char *str);

// BinaryString --> Integer converter
bool StringBinaryToNum(const char *str, uint32_t *num);

// HexString --> Integer converters
bool HexCharToHex(const char c, uint8_t *hexValue);
bool StringByteToNum(const char *str, uint8_t *byte);
bool StringHexToNum(const char *str, uint32_t *hexValue);

// Num string --> Integer converters
uint8_t DecimalCharToNum(char c);
bool StringToUnsignedDecimalNum(const char *str, uint32_t *value);
bool StringToSignedDecimalNum(const char *str, int32_t *value);

// String --> Float converter
bool StringToFloat(const char *str, float *Num);

// Standard string functions:
uint8_t StringLength(const char *str);
uint8_t StrCmp(const char * str1, const char *str2);
uint8_t StrCmpFirst(const char *str1, const char *str2);
uint8_t StrCmpWithLength(const char * str1, const char *str2, uint8_t length);
uint8_t StrCpy(char *dest, const char *src);
uint8_t StrCpyFixLength(char *dest, const char *src, uint8_t length);
uint8_t StrCpyFixLengthWithFillCharacter(char *dest, const char *str,
		uint8_t length, char fillChar);
uint8_t StrCpyMax(char *dest, const char *str, uint8_t maxLength);
uint8_t StrCpyCharacter(char *dest, char c, uint8_t num);
uint8_t StrAppend(char *dest, const char *src);

// Find & Separators
void StrTrim(char *str);
int16_t STRING_FindString(const char *str, const char *findString);
uint8_t STRING_Splitter(char *source, char delimiterChar,
		char **separated, uint8_t parameterMaxCount);

// Send functions
// !! IMPORTANT !! Send formatted string on USART !!
uint8_t string_printf(char *str, const char *format, va_list ap);
uint8_t usprintf(char *str, const char *format, ...);

// Unit test
void STRING_UnitTest(void);



#endif /* STRING_H_ */
