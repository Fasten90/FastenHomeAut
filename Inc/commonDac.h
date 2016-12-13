/*
 *		commonDac.h
 *
 *		Created on:		2016. nov. 21.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2016. dec. 13.
 */

#ifndef COMMONDAC_H_
#define COMMONDAC_H_


#include "include.h"


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

/// DAC Channel number
typedef enum
{
	Channel_Unknown,
	Channel_1,
	Channel_2
} DAC_Channel_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern DAC_HandleTypeDef    DacHandle;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac);
void DAC_Init(void);
bool DAC_SetValue (DAC_Channel_t channel, float voltage);



#endif /* COMMONDAC_H_ */
