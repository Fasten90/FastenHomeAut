/*******************************************************************************
 * Purpose:
 ******************************************************************************/

#ifndef STRING_H_
#define STRING_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"

#include <stdbool.h>	// For bool


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
 *  Local function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

// Decimal --> String converters
uint8_t SignedDecimalToString (int32_t value, char *string);
uint8_t UnsignedDecimalToString (uint32_t value, char *string);
uint8_t SignedDecimalToStringFixLength (int32_t value, char *string, uint8_t length);

// Hexadecimal --> String converters
uint8_t DecimalToHexaString (uint32_t value, uint8_t ByteNum, char *string);
uint8_t ByteToHexaString (uint8_t byte, char *string);
char OctetToChar (uint8_t octet);

// Float --> String converter
uint8_t FloatToString (float value, char *string, uint8_t fractionLength);

// String checkers
bool IsHexChar(char c);
bool IsDecimalChar(char c);

uint8_t StringIsHexadecimalString (const char *string);
uint8_t StringIsUnsignedDecimalString (const char *string);
uint8_t StringIsSignedDecimalString (const char *string);

// String --> Integer converters
uint8_t HexCharToOctet(char c);
bool StringByteToNum(const char *string, uint8_t *byte);

bool StringHexToNum (const char *string, uint32_t *hexValue, uint8_t byteLength);
bool UnsignedDecimalStringToNum (const char *string, uint32_t *value);
bool SignedDecimalStringToNum (const char *string, int32_t *value);

// String --> Float converter
bool StringToFloat (const char *string, float *Num);


uint32_t power (uint32_t a, uint8_t b);


// Standard string functions:
uint8_t StringLength (const char *string );
uint8_t StrCmp (const char * ch1, const char *ch2);
uint8_t StrCmpWithLength (const char * ch1, const char *ch2, uint8_t length);
uint8_t StrCpy (char *dest, const char *string );
uint8_t StrCpyFixLength (char *dest, const char *string, uint8_t length );
uint8_t StrAppend (char *dest, const char *string);



#endif /* STRING_H_ */
