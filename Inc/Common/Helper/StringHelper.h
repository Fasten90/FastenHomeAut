/*
 *    String.h
 *    Created on:   2016-01-01
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     String manipulation functions
 *    Target:       STM32Fx
 */

#ifndef STRINGHELPER_H_
#define STRINGHELPER_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include <stdarg.h>        /* For "..." parameters in uprintf function */
#include "GenericTypeDefs.h"
#include "compiler.h"
#include "MemHandler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


/* Configs */
#ifndef STRING_SPRINTF_EXTENDED_ENABLE
    #define STRING_SPRINTF_EXTENDED_ENABLE
#endif


/* size_t strlen ( const char * str ); */
#define strlen(_str)                      StringLength(_str)


#define strcmp(_str1, _str2)              StrCmp(_str1, _str2)


/* int strncmp ( const char * str1, const char * str2, size_t num ); */
/* return: 0 equal, >0 and <0 is not equal */
/* uint8_t StrCmpWithLength(const char * ch1, const char *ch2, uint8_t length) */
/* return: 0 equal, 1 not equal */
/* TODO: Be careful, return values are not equal */
#define strncmp(_str1, _str2, _num)        StrCmpWithLength(_str1, _str2, _num)


#define strcpy(_dest, _src)                StrCpy(_dest, _src)


/* char * strncpy ( char * destination, const char * source, size_t num ); */
/* uint8_t StrCpyMax(char *dest, const char *str, uint8_t maxLength) */
#define strncpy(_dest, _src, _num)         StrCpyMax(_dest, _src, _num)


#define strcat(_dest_, _src)               StrAppend(_dest, _src)


#define strncat(_dest, _src, _num)         StrAppend(_dest, _src, _num)


/* Location of character in string */
#define strchr(_str, _char)                STRING_FindCharacter(_str, _char)


/* Find characters in string */
/* char * strpbrk ( const char *, const char * ); */
#define strpbrk(_str, _characters)         STRING_FindCharacters(_str, _characters)


/* char * strstr ( const char *, const char * ) */
#define strstr(_str1, _str2)               STRING_FindString(_str1, _str2)


/* TODO: avoid? */
#define sprintf(...)                       usprintf(__VA_ARGS__)


#define snprintf(...)                      usnprintf(__VA_ARGS__)


#ifndef CONFIG_PLATFORM_X86
#define printf(...)                        uprintf(__VA_ARGS__)
#endif



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

/* Decimal --> String converters */
uint8_t SignedDecimalToString(int32_t value, char *str);
size_t SignedDecimalToStringSafe(int32_t value, char * str, size_t maxLength);
uint8_t UnsignedDecimalToString(uint32_t value, char *str);
size_t UnsignedDecimalToStringSafe(uint32_t value, char *str, size_t maxLength);
uint8_t UnsignedDecimalLength(uint32_t value);
uint8_t UnsignedDecimalToStringFill(uint32_t value, char *str, uint8_t fillLength, char fillCharacter);
uint8_t SignedDecimalToStringFill(int32_t value, char *str, uint8_t fillLength, char fillCharacter);

/* Num -> Binary String converter */
uint8_t DecimalToBinaryString(uint32_t value, char *str, uint8_t maxLength);

/* Hexadecimal --> String converters */
uint8_t DecimalToHexaString(uint32_t value, char *str, uint8_t length);
uint8_t DecimalToHexaStringSafe(uint32_t value, char *str, uint8_t length, uint8_t maxLength);
uint8_t ByteToHexaString(uint8_t byte, char *str);
char HexToHexChar(uint8_t octet);

/* Float --> String converter */
uint8_t FloatToString(float value, char *str, uint8_t integerLength, uint8_t fractionLength);

/* String checkers */
bool IsHexChar(const char c);
bool IsDecimalChar(const char c);

uint8_t StringIsHexadecimalString(const char *str);
uint8_t StringIsUnsignedDecimalString(const char *str);
uint8_t StringIsSignedDecimalString(const char *str);

/* BinaryString --> Integer converter */
bool StringBinaryToNum(const char *str, uint32_t *num);

/* HexString --> Integer converters */
bool HexCharToHex(const char c, uint8_t *hexValue);
bool StringByteToNum(const char *str, uint8_t *byte);
bool StringHexToNum(const char *str, uint32_t *hexValue);

/* Num string --> Integer converters */
uint8_t DecimalCharToNum(char c);
bool StringToUnsignedDecimalNum(const char *str, uint32_t *value);
bool StringToSignedDecimalNum(const char *str, int32_t *value);

bool StringToUnsignedDecimalNumWithLength(const char *str, uint32_t *value, uint8_t stringLength);

/* String --> Float converter */
bool StringToFloat(const char *str, float *num);
/* TODO: string to float with length? */

/* String --> Bool converter */
bool StringToBool(const char * str, bool * val);

void ToLower(char * c);
void ToUpper(char * c);

/* Standard string functions: */
size_t StringLength(const char *str);
uint8_t StrCmp(const char * str1, const char *str2);
uint8_t StrCmpFirst(const char *str1, const char *str2);
uint8_t StrCmpWithLength(const char * str1, const char *str2, size_t length);
size_t StrCpy(char *dest, const char *str);
size_t StrCpyFixLength(char *dest, const char *str, size_t length);
size_t StrCpyFixLengthWithFillCharacter(char *dest, const char *str, size_t length, char fillChar);
size_t StrCpyMax(char *dest, const char *str, size_t maxLength);
size_t StrCpyCharacter(char *dest, char c, size_t num);
size_t StrAppend(char *dest, const char *str);
size_t StrAppendSafe(char *dest, const char *str, size_t destLength);
size_t CharAppend(char *dest, const char c);

/* String modify */
size_t StrTrim(char *str);
void StringLower(char * str);
void StringUpper(char * str);

/* Find & Separators */
const char * STRING_FindCharacter(const char *str, const char findCharacter);
const char * STRING_FindCharacters(const char *str, const char *findCharacters);
const char * STRING_FindString(const char *str, const char *findString);
uint8_t STRING_Splitter(char *source, const char *delimiters, char **separated, uint8_t paramLimit);

/* String format functions */
size_t string_printf_safe(char *str, size_t maxLen, const char *format, va_list ap);

size_t usnprintf(char * str, size_t maxLen, const char * format, ...);

/* Unit test */
uint32_t StringHelper_UnitTest(void);



#endif /* STRINGHELPER_H_ */
