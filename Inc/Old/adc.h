/*******************************************************************************
 * Purpose:
 ******************************************************************************/

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


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
extern volatile uint32_t		ADC_MeasuredValues[];

extern volatile float			ADC_ConvertedValue_VSource;
extern volatile uint32_t		ADC_ConvertedValue_SensorLight;
extern volatile uint32_t		ADC_ConvertedValue_SensorMicrophone;
extern volatile float			ADC_ConvertedValue_InternalTemperature;
extern volatile float			ADC_ConvertedValue_InternalVref;

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


void ADC_Test ( void );
void ADC_Run( void );

float ADC_ConvertToSourceVoltage (uint32_t readValue);
float ADC_ConvertToVoltage (uint32_t readValue);
float ADC_ConvertInternalTempSensorValue (uint32_t readValue);
void ADC_ConvertAllMeasuredValues( void );


void DMA1_Channel1_IRQHandler(void);
void ADC1_IRQHandler(void);
void ADC1_DMA_IRQHandler(void);


#endif // #ifdef CONFIG_MODULE_ADC_ENABLE


#endif /* ADC_H_ */
