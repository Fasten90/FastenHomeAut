/*
 *		CommonAdc.h
 *		Created on:		2016-11-21
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Common ADC (Analog-to-Digital Converter)
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2016-12-13
 */

#ifndef COMMONADC_H_
#define COMMONADC_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"


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
extern volatile uint32_t		ADC_MeasuredValues[ADC_CHANNEL_NUM];
extern volatile float			ADC_ConvertedValues[ADC_CHANNEL_NUM];



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
void CommonADC_SetPrintNum(uint8_t adcChannel);
void CommonADC_PrintAdcValue(uint8_t adcChannel);
void CommonADC_PrintAdc(void);

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle);



#endif // #ifdef CONFIG_MODULE_COMMON_ADC_ENABLE


#endif /* COMMONADC_H_ */
