/*
 *		adc.c
 *
 *		Created on:		2015
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 8.
 */

/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "board.h"
#include "ADC.h"
#include "DebugUart.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/
#ifdef CONFIG_MODULE_ADC_ENABLE



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

ADC_HandleTypeDef		AdcHandle;
volatile uint32_t		ADC_MeasuredValues[ADC_BUFFER_SIZE];
volatile float			ADC_ConvertedValues[ADC_BUFFER_SIZE];

#ifdef CONFIG_DEBUG_SELFTEST
volatile uint32_t		ADC_RunCnt = 0;
#endif



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local functions
 *----------------------------------------------------------------------------*/

static float ADC_ConvertToVoltage(uint32_t readValue);
static float ADC_ConvertToSourceVoltage(uint32_t readValue);



/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/

/**
 * \brief	Initialize IOs
 */
void ADC_Init(void)
{

	HAL_ADC_MspInit(&AdcHandle);

	ADC_ChannelConfTypeDef   sConfig;

	//HAL_ADC_MspInit(&AdcHandle);	// Called by HAL driver

	/* Configuration of AdcHandle init structure: ADC parameters and regular group */
	AdcHandle.Instance = ADCx;

	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
	AdcHandle.Init.ScanConvMode          = ENABLE;							/* Sequencer enabled (ADC conversion on several channels, successively, following settings below) */
	AdcHandle.Init.ContinuousConvMode    = ENABLE;							/* Continuous mode disabled to have only 1 rank converted at each conversion trig, and because discontinuous mode is enabled */
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;							/* Sequencer of regular group will convert the sequence in several sub-divided sequences */
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	//AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV;		/* Trig of conversion start done manually by software, without external event */
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.DMAContinuousRequests = ENABLE;							/* ADC-DMA continuous requests to match with DMA configured in circular mode */
	AdcHandle.Init.EOCSelection          = DISABLE;


	if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
	{
		/* ADC initialization error */
		Error_Handler();
	}

	/* Configuration of channel on ADCx regular group on sequencer rank 1 */
	/* Note: Considering IT occurring after each ADC conversion (IT by DMA end  */
	/*       of transfer), select sampling time and ADC clock with sufficient   */
	/*       duration to not create an overhead situation in IRQHandler.        */
	/* Note: Set long sampling time due to internal channels (VrefInt,          */
	/*       temperature sensor) constraints.                                   */
	/*       For example, sampling time of temperature sensor must be higher    */
	/*       than 4us. Refer to device datasheet for min/typ/max values.        */
	sConfig.Channel      = ADC_VSOURCE_CHANNEL;
	sConfig.Rank         = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;	// TODO: Make define for sampling times

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		/* Channel Configuration Error */
		Error_Handler();
	}

#if ADC_BUFFER_SIZE >= 2
	/* Configuration of channel on ADCx regular group on sequencer rank 2 */
	/* Replicate previous rank settings, change only channel and rank */
	sConfig.Channel      = ADC_CHANNEL_2;
	sConfig.Rank         = 2;
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		/* Channel Configuration Error */
		Error_Handler();
	}
#endif
#if ADC_BUFFER_SIZE >= 3
	/* Configuration of channel on ADCx regular group on sequencer rank 3 */
	/* Replicate previous rank settings, change only channel and rank */
	sConfig.Channel      = ADC_CHANNEL_3;
	sConfig.Rank         = 3;
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		/* Channel Configuration Error */
		Error_Handler();
	}
#endif

#if ADC_BUFFER_SIZE > 3
#warning "ADC num is not 3, need extend the channel configs"
#endif

	/* Start ADC conversion on regular group with transfer by DMA */
	if (HAL_ADC_Start_DMA(&AdcHandle,
						(uint32_t *)ADC_MeasuredValues,
						ADC_BUFFER_SIZE
					   ) != HAL_OK)
	{
		/* Start Error */
		Error_Handler();
	}
}



/**
 * \brief	ADC Msp Init for HAL driver
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	static DMA_HandleTypeDef DmaHandle;

	/*##-1- Enable peripherals and GPIO Clocks */
	/* Enable GPIO clock */
	ADCx_CHANNEL_GPIO_CLK_ENABLE();
	/* ADC Periph clock enable */
	ADCx_CLK_ENABLE();
	/* Enable DMA clock */
	DMAx_CLK_ENABLE();


	/*##- 2- Configure peripheral GPIO */
	GPIO_InitStruct.Pin = ADC_VSOURCE_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ADC_VSOURCE_GPIO_PORT, &GPIO_InitStruct);

#if ADC_BUFFER_SIZE >= 2
	GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN_2;
	HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
#endif
#if ADC_BUFFER_SIZE >= 3
	GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN_3;
	HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
#endif

#if ADC_BUFFER_SIZE > 3
#warning "ADC num is not 3, need extend the channel configs"
#endif

	/*##- 3- Configure DMA */

	/* Configure DMA parameters */
	DmaHandle.Instance					= ADCx_DMA_CHANNEL;
	DmaHandle.Init.Direction			= DMA_PERIPH_TO_MEMORY;
	DmaHandle.Init.PeriphInc			= DMA_PINC_DISABLE;
	DmaHandle.Init.MemInc				= DMA_MINC_ENABLE;
	DmaHandle.Init.PeriphDataAlignment	= DMA_PDATAALIGN_WORD;
	DmaHandle.Init.MemDataAlignment		= DMA_MDATAALIGN_WORD;
	DmaHandle.Init.Mode					= DMA_CIRCULAR;
	DmaHandle.Init.Priority				= DMA_PRIORITY_LOW;

	/* Deinitialize  & Initialize the DMA for new transfer */
	HAL_DMA_Init(&DmaHandle);

	/* Associate the DMA handle */
	__HAL_LINKDMA(hadc, DMA_Handle, DmaHandle);

	/* NVIC configuration for DMA Input data interrupt */
	HAL_NVIC_SetPriority(ADCx_DMA_IRQn, ADC_DMA_PREEMT_PRIORITY, ADC_DMA_SUB_PRIORITY);
	HAL_NVIC_EnableIRQ(ADCx_DMA_IRQn);

}



/**
 * \brief	ADC callback function for HAL driver
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	// Suppress warning
	(void)hadc;

#ifdef CONFIG_DEBUG_SELFTEST
	ADC_RunCnt++;
#endif
}


void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	(void)hadc;
}


void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	(void)hadc;
	DEBUG_BREAKPOINT();
}



/**
 * \brief	Convert all measurements
 */
void ADC_ConvertAllMeasuredValues(void)
{
	// TODO: We can convert in a loop, but some ADC not work in standard measuring
	uint8_t i;

	for (i = 0; i < ADC_BUFFER_SIZE; i++)
	{
		switch ((ADC_MeasurementData_t)i)
		{
			case ADC_Vsource:
				// Convert source voltage
				ADC_ConvertedValues[i] = ADC_ConvertToSourceVoltage(ADC_MeasuredValues[i]);
				break;

			case ADC_StandardVoltageExample:
				// Convert standard
				ADC_ConvertedValues[i] = ADC_ConvertToVoltage(ADC_MeasuredValues[i]);
				break;

			case ADC_Count:
			default:
				// Error
				break;
		}
	}
}



/**
 * \brief	Convert read value to voltage
 */
static float ADC_ConvertToVoltage(uint32_t readValue)
{
	float voltage;

	voltage = (float)readValue * ADC_VOLTAGE_MAX / ADC_RESOLUTION_MAX;

	return voltage;
}



/**
 * \brief	Convert Source voltage
 */
static float ADC_ConvertToSourceVoltage(uint32_t readValue)
{
	float voltage;

	voltage = (float)readValue * ADC_VOLTAGE_MAX / ADC_RESOLUTION_MAX * ADC_SOURCE_VOLTAGE_MUL;

	return voltage;
}



/**
 * \brief	Get ADC value
 */
float ADC_GetValue(ADC_MeasurementData_t meas)
{
	float value = 0.0f;

	if (meas < ADC_Count)
		value = ADC_ConvertedValues[meas];

	return value;
}



/**
 * \brief	Print all ADC values
 */
void ADC_PrintAllValues(void)
{
	uint8_t i;

	for (i = 0; i < ADC_BUFFER_SIZE; i++)
	{
		uprintf("%d - %f", i, ADC_ConvertedValues[i]);
	}
}



#endif
