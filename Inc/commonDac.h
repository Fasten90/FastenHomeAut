/*
 *		dac.h
 *
 *		Created on:		2016. nov. 21.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2016. nov. 21.
 */

#ifndef COMMONDAC_H_
#define COMMONDAC_H_


#include "include.h"



/// Enums
typedef enum
{
	Channel_Unknown,
	Channel_1,
	Channel_2
} DAC_Channel_t;


/// GLOBAL VARIABLES
extern DAC_HandleTypeDef    DacHandle;


/// FUNCTIONS
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac);
void DAC_Config(void);
bool DAC_SetValue (DAC_Channel_t channel, float voltage);



#endif /* COMMONDAC_H_ */
