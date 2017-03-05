/*
 *		CircularBuffer.c
 *
 *		Created on:		2017. márc. 4.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. márc. 4.
 */




/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "CircularBuffer.h"


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief
 */
void CircularBuffer_Init(void)
{
	// TODO: Clear, if not need
}



/**
 * \brief	Get characters from ReadCnt to WriteCnt
 */
uint8_t CircularBuffer_GetCharacters(char *receiveBuffer, char *str, uint16_t bufferSize, uint16_t writeCnt, uint16_t readCnt, bool putEnd)
{
	uint16_t i;

	if (readCnt < writeCnt)
	{
		// No overflow
		for (i = 0; i <= writeCnt-readCnt; i++)
		{
			str[i] = receiveBuffer[readCnt+i];
		}
		// Put end
		if (putEnd)
		{
			str[i] =  '\0';
		}
	}
	else if (readCnt > writeCnt)
	{
		// Buffer to end
		for (i = 0; i <= bufferSize-readCnt; i++)
		{
			str[i] = receiveBuffer[readCnt+i];
		}

		uint16_t oldCnt = i;
		// Begin of buffer
		for (i = 0; i <= writeCnt; i++)
		{
			str[oldCnt+i] = receiveBuffer[i];
		}
		// Put end
		i += oldCnt;
		if (putEnd)
		{
			str[i] =  '\0';
		}
	}
	// else : readCnt == writeCnt

	return i;
}



/**
 * \brief	Clear buffer from readCnt to writeCnt
 */
void CircularBuffer_Clear(char *receiveBuffer, uint16_t bufferSize, uint16_t readCnt, uint16_t writeCnt)
{
	uint16_t i;

	if (readCnt < writeCnt)
	{
		// No overflow
		for (i = 0; i <= writeCnt-readCnt; i++)
		{
			receiveBuffer[readCnt+i] = '\0';
		}
	}
	else if (readCnt > writeCnt)
	{
		// Buffer to end
		for (i = 0; i < bufferSize-readCnt; i++)
		{
			receiveBuffer[readCnt+i] = '\0';
		}

		// Begin of buffer
		for (i = 0; i <= writeCnt; i++)
		{
			receiveBuffer[i] = '\0';
		}
	}
}



// TODO: Unit test...




