/*
 *		Calc.c
 *
 *		Created on:		2016.09.26
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017.06.02.
 */

#include "Calc.h"
#include "include.h"



/**
 * \brief	Power(a,b) = a^b
 */
uint32_t power(uint32_t a, uint8_t b)
{
	uint8_t i;
	uint32_t Num;

	if (b == 0)
		Num = 1;
	else
	{
		Num = a;
		for (i=1; i<b; i++)
		{
			Num=Num*a;
		}
	}

	return Num;
}
