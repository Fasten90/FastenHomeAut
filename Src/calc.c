/*
 * calc.c
 *
 *  Created on: Sep 26, 2016
 *      Author: Vizi Gábor
 */

#include "include.h"
#include "calc.h"




/**
 * \brief	Power(a,b) = a^b
 */
uint32_t power(uint32_t a, uint8_t b)
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

