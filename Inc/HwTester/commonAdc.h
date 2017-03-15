/*
 *		commonAdc.h
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Common ADC: Measure more ADC input
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2016.12.08
 */

#ifndef COMMONADC_H_
#define COMMONADC_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"


#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern ADC_HandleTypeDef		AdcHandle;
extern volatile uint32_t		ADC_MeasuredValues[];
extern volatile float			ADC_ConvertedValues[];


/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void CommonADC_Init(void);

float CommonADC_ConvertToVoltage(uint32_t readValue);
void CommonADC_ConvertAllMeasuredValues(void);
void CommonADC_Test(void);

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle);





#endif // #ifdef CONFIG_MODULE_COMMON_ADC_ENABLE


#endif /* COMMONADC_H_ */
