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



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

uint32_t power(uint32_t a, uint8_t b);
uint32_t power_secured(uint32_t a, uint8_t b);

void Calc_UnitTest(void);


#endif /* CALC_H_ */
