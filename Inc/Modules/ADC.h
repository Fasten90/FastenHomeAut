/*
 *		adc.h
 *
 *		Created on:		2015
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 8.
 */

#ifndef ADC_H_
#define COMMONADC_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"


#ifdef CONFIG_MODULE_ADC_ENABLE

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

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

extern ADC_HandleTypeDef		AdcHandle;
extern volatile uint32_t		ADC_MeasuredValues[];
extern volatile float			ADC_ConvertedValues[ADC_BUFFER_SIZE];



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
float ADC_ConvertToVoltage(uint32_t readValue);
float ADC_ConvertToSourceVoltage(uint32_t readValue);


// TODO: Delete these
/*
void ADC_Test ( void );
void ADC_Run( void );

float ADC_ConvertToVoltage (uint32_t readValue);
float ADC_ConvertInternalTempSensorValue (uint32_t readValue);

void DMA1_Channel1_IRQHandler(void);
void ADC1_IRQHandler(void);
void ADC1_DMA_IRQHandler(void);
*/


#endif // #ifdef CONFIG_MODULE_ADC_ENABLE


#endif /* ADC_H_ */
