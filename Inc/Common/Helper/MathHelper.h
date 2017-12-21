/*
 *		Calc.h
 *		Created on:		2016-09-26
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Calculate / Math functions
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-06-02
 */

#ifndef CALC_H_
#define CALC_H_


#include "include.h"



/*------------------------------------------------------------------------------
 *	Macros
 *----------------------------------------------------------------------------*/

#define MAX(a, b)			(((a) > (b)) ? (a) : (b))
#define MIN(a, b)			(((a) < (b)) ? (a) : (b))
#define ABS(x)				(((x) < 0) ? -(x) : (x))	/* UNSAFE */
#define ABSDIFF(a, b)		((a) > (b) ? (a) - (b) : (b) - (a))


#ifndef SET_BIT
#define SET_BIT(val, bitIndex) val |= (1 << bitIndex)
#endif
#ifndef CLEAR_BIT
#define CLEAR_BIT(val, bitIndex) val &= ~(1 << bitIndex)
#endif
#define TOGGLE_BIT(val, bitIndex) val ^= (1 << bitIndex)
#define BIT_IS_SET(val, bitIndex) (val & (1 << bitIndex))


// float       pow( float base, float exp );
#define pow(_base, _exp)	power(_base, _exp)


#define rand()				(random())


#ifndef RAND_MAX
#define RAND_MAX			(0x7FFF)
#endif



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

// TODO: Rename these functions?

uint32_t power(uint32_t a, uint8_t b);
uint32_t power_secured(uint32_t a, uint8_t b);

uint8_t DigitNum(uint32_t num, uint8_t radix);

uint32_t Increment(uint32_t * i);
uint32_t Decrement(uint32_t * i);

int8_t GetLargestBitIndex(uint32_t value);
int8_t GetSmallestBitIndex(uint32_t value);
uint8_t popcount(uint32_t value);
void ClearBit(uint32_t * value, uint8_t index);
void SetBit(uint32_t * value, uint8_t index);
uint8_t GetBit(uint32_t value, uint8_t index);

uint16_t random(void);

void Calc_UnitTest(void);



#endif /* CALC_H_ */