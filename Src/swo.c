/*
 *		swo.c
 *
 *		Created on:		2016. dec. 14.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2016. dec. 14.
 */


/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "swo.h"


#ifdef CONFIG_SWO_ENABLE

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000


/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Send string on SWO
 */
uint8_t SWO_SendMessage(const char *message)
{
	uint8_t length = StringLength(message);

	while(*message)
	{
		// Send next char
		SWO_SendChar(*message);
		message++;
	}

	return length;
}



/**
 * \brief	Send character on SWO
 */
uint8_t SWO_SendChar(uint8_t ch)
{

	if (DEMCR & TRCENA)
	{
		while (ITM_Port32(0) == 0);
		ITM_Port8(0) = ch;
	}

	return 1;
}

#endif	// CONFIG_SWO_ENABLE


