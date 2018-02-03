/*
 *		CommonAdc.c
 *		Created on:		2016-11-21
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Common ADC (Analog-to-Digital Converter)
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2016-12-13
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "compiler.h"
#include "CommandHandler.h"
#include "ErrorHandler.h"
#include "DebugUart.h"
#include "CommonADC.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

///< ADC handle declaration
ADC_HandleTypeDef		AdcHandle;
static DMA_HandleTypeDef  DmaHandle;


///< Measured values
volatile uint32_t		ADC_MeasuredValues[ADC_CHANNEL_NUM] = { 0 };

///< Calculated values
volatile float			ADC_ConvertedValues[ADC_CHANNEL_NUM] = { 0 };

static uint8_t			ADC_PrintNum = 0;

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



/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/

extern void Error_Handler(void);



/*------------------------------------------------------------------------------
 * Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Initialize ADC with DMA
 */
void CommonADC_Init(void)
{
	ADC_ChannelConfTypeDef   sConfig;

	//HAL_ADC_MspInit(&AdcHandle);	// Called by HAL driver

	/* Configuration of AdcHandle init structure: ADC parameters and regular group */
	AdcHandle.Instance = ADCx;

	AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV8;
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
	AdcHandle.Init.ScanConvMode          = ENABLE;							/* Sequencer enabled (ADC conversion on several channels, successively, following settings below) */
	AdcHandle.Init.ContinuousConvMode    = ENABLE;							/* Continuous mode disabled to have only 1 rank converted at each conversion trig, and because discontinuous mode is enabled */
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;							/* Sequencer of regular group will convert the sequence in several sub-divided sequences */
	AdcHandle.Init.NbrOfDiscConversion   = 0;								/* Sequencer of regular group will convert ranks one by one, at each conversion trig */
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;		/* Trig of conversion start done manually by software, without external event */
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.NbrOfConversion       = ADC_CHANNEL_NUM;					/* Sequencer of regular group will convert the 3 first ranks: rank1, rank2, rank3 */
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
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	sConfig.Offset		 = 0;

	sConfig.Channel      = ADCx_CHANNEL_1;
	sConfig.Rank         = 1;

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		/* Channel Configuration Error */
		Error_Handler();
	}

#if ADC_CHANNEL_NUM >= 2
	/* Configuration of channel on ADCx regular group on sequencer rank 2 */
	/* Replicate previous rank settings, change only channel and rank */
	sConfig.Channel      = ADCx_CHANNEL_2;
	sConfig.Rank         = 2;

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		/* Channel Configuration Error */
		Error_Handler();
	}
#endif
#if ADC_CHANNEL_NUM >= 3
	/* Configuration of channel on ADCx regular group on sequencer rank 3 */
	/* Replicate previous rank settings, change only channel and rank */
	sConfig.Channel      = ADCx_CHANNEL_3;
	sConfig.Rank         = 3;

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		/* Channel Configuration Error */
		Error_Handler();
	}
#endif

#if ADC_CHANNEL_NUM > 3
#warning "ADC num is not 3, need extend the channel configs"
#endif

	/* Start ADC conversion on regular group with transfer by DMA */
	if (HAL_ADC_Start_DMA(&AdcHandle,
						(uint32_t *)ADC_MeasuredValues,
						ADC_CHANNEL_NUM
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
	GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);

#if ADC_CHANNEL_NUM >= 2
	GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN_2;
	HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
#endif
#if ADC_CHANNEL_NUM >= 3
	GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN_3;
	HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
#endif
	
#if ADC_CHANNEL_NUM > 3
#warning "ADC num is not 3, need extend the channel configs"
#endif

	/*##- 3- Configure DMA */

	/* Configure DMA parameters */
	DmaHandle.Instance					= ADCx_DMA_STREAM;
	DmaHandle.Init.Channel				= ADCx_DMA_CHANNEL;
	DmaHandle.Init.Direction			= DMA_PERIPH_TO_MEMORY;
	DmaHandle.Init.PeriphInc			= DMA_PINC_DISABLE;
	DmaHandle.Init.MemInc				= DMA_MINC_ENABLE;
	DmaHandle.Init.PeriphDataAlignment	= DMA_PDATAALIGN_WORD;
	DmaHandle.Init.MemDataAlignment		= DMA_MDATAALIGN_WORD;
	DmaHandle.Init.Mode					= DMA_CIRCULAR;
	DmaHandle.Init.Priority				= DMA_PRIORITY_LOW;
	DmaHandle.Init.FIFOMode				= DMA_FIFOMODE_DISABLE;
	DmaHandle.Init.FIFOThreshold		= DMA_FIFO_THRESHOLD_HALFFULL;
	DmaHandle.Init.MemBurst				= DMA_MBURST_SINGLE;
	DmaHandle.Init.PeriphBurst			= DMA_PBURST_SINGLE;

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
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
	// Suppress warning
	(void)AdcHandle;

#ifdef CONFIG_DEBUG_SELFTEST
	ADC_RunCnt++;
#endif
}



/**
 * \brief	Infinite loop: print ADC values
 * \note	!! Be careful, it is blocking mode !!
 */
void CommonADC_Test(void)
{
	
	while (1)
	{
		uint8_t i;
		
		DelayMs(10);

		uprintf("\r\n");
		for (i = 0; i < ADC_CHANNEL_NUM; i++)
		{
			uprintf("%d. value: %d\r\n", i, ADC_MeasuredValues[i]);
		}
		
		DelayMs(1000);
	}
}



/**
 * \brief	Convert read value to voltage
 */
float CommonADC_ConvertToVoltage(uint32_t readValue)
{
	float voltage;

	voltage = (float)readValue * COMMON_ADC_VOLTAGE_MAX / COMMON_ADC_RESOLUTION_MAX;

	return voltage;
}



/**
 * \brief	Convert all read values
 */
void CommonADC_ConvertAllMeasuredValues(void)
{
	uint8_t i;
	
	for (i = 0; i < ADC_CHANNEL_NUM; i++)
	{
		// Convert all values
		ADC_ConvertedValues[i] = CommonADC_ConvertToVoltage(ADC_MeasuredValues[i]);
	}
}



/**
 * \brief	Set "Print num"
 */
void CommonADC_SetPrintNum(uint8_t adcChannel)
{
	if (adcChannel < ADC_CHANNEL_NUM)
		ADC_PrintNum = adcChannel;
	else
		ADC_PrintNum = 0;
}



/**
 * \brief	Print x. adcChannel value (converted!)
 */
void CommonADC_PrintAdcValue(uint8_t adcChannel)
{
	if (adcChannel < ADC_CHANNEL_NUM)
	{
		// Print
		CmdH_Printf("ADC: %d. value: %2.2f\r\n", adcChannel, ADC_ConvertedValues[adcChannel]);
	}
	else
	{
		CmdH_Printf("ERROR! Wrong ADC channel: %d!", adcChannel);
	}
}



/**
 * \brief	Print saved channel's ADC
 */
void CommonADC_PrintAdc(void)
{
	CommonADC_PrintAdcValue(ADC_PrintNum);
}



#endif // #ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
