/*
 *		temperature.h
 *
 *		Created on:		2015
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 8.
 */

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/
#ifdef MODUL_TEMPERATURE_ENABLE


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
extern float GLOBAL_TemperatureValue_LM75;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/
void TEMPERATURE_I2C_Init(void);
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c);
ReturnType TEMPERATURE_GetTemperature (void);


#endif // #ifdef MODUL_TEMPERATURE_ENABLE


#endif /* TEMPERATURE_H_ */
