/*
 *        ADC.h
 *        Created on:        2016-01-01
 *        Author:            Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        ADC (Analog-to-Digital Converter)
 *        Target:            STM32Fx
 *        Version:        v1
 *        Last modified:    2017-02-08
 */

#ifndef ADC_H_
#define ADC_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"


#ifdef CONFIG_MODULE_ADC_ENABLE

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< ADC "device" list
typedef enum
{
    ADC_Vsource,
    ADC_StandardVoltageExample,

    // Do not use, only for count
    ADC_Count
} ADC_MeasurementData_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern ADC_HandleTypeDef        AdcHandle;
extern volatile uint32_t        ADC_MeasuredValues[ADC_CHANNEL_NUM];
extern volatile float            ADC_ConvertedValues[ADC_CHANNEL_NUM];

extern volatile uint32_t        ADC_RunCnt;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void ADC_Init( void );
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle);

void ADC_ConvertAllMeasuredValues(void);

float ADC_GetValue(ADC_MeasurementData_t meas);

void ADC_PrintAllValues(void);



#endif // #ifdef CONFIG_MODULE_ADC_ENABLE


#endif /* ADC_H_ */
