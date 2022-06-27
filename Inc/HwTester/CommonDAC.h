/*
 *    CommonDac.h
 *    Created on:   2016-11-21
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Common DAC
 *    Target:       STM32Fx
 */

#ifndef COMMONDAC_H_
#define COMMONDAC_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"


#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE

/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< DAC Channel number
typedef enum
{
    Channel_Unknown,
    Channel_1,
    Channel_2
} DAC_Channel_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern DAC_HandleTypeDef DacHandle;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac);
void CommonDAC_Init(void);
bool CommonDAC_SetValue (DAC_Channel_t channel, float voltage);


#endif


#endif /* COMMONDAC_H_ */
