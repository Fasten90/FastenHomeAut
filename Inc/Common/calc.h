/*
 *		Calc.h
 *
 *		Created on:		2016.09.26
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017.06.02.
 */
#ifndef CALC_H_
#define CALC_H_


#include "include.h"



/*------------------------------------------------------------------------------
 *	Macros
 *----------------------------------------------------------------------------*/

#define MAX(a, b)			((a > b) ? a : b)
#define MIN(a, b)			((a < b) ? a : b)


// float       pow( float base, float exp );
#define pow(_base, _exp)	power(_base, _exp)



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

uint32_t power(uint32_t a, uint8_t b);
uint32_t power_secured(uint32_t a, uint8_t b);

uint8_t DigitNum(uint32_t num, uint8_t radix);

uint32_t Increment(uint32_t * i);

void Calc_UnitTest(void);



#endif /* CALC_H_ */
