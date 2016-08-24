/*******************************************************************************
 * Purpose:
 ******************************************************************************/

/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "adc.h"
#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/
#ifdef CONFIG_MODULE_ADC_ENABLE

#ifdef CONFIG_USE_PANEL_NODESMALL
#define ADC_MEASURED_VALUE_CNT_VSOURCE	2
#define ADC_MEASURED_VALUE_CNT_TEMPSENSOR	3
#endif	
#ifdef CONFIG_USE_PANEL_NODEMEDIUM
#define ADC_MEASURED_VALUE_CNT_LIGHT	0
#define ADC_MEASURED_VALUE_CNT_VSOURCE	4
#define ADC_MEASURED_VALUE_CNT_TEMPSENSOR	3
#endif



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
// ADC handle declaration
ADC_HandleTypeDef             AdcHandle;

// ADC channel configuration structure declaration
//ADC_ChannelConfTypeDef        sConfig;				// TODO: ha kiszedheto, akkor törölni

// Measured values
volatile uint32_t		ADC_MeasuredValues[ADC_BUFFER_SIZE];

// Calculated values
volatile float			ADC_ConvertedValue_VSource = 0;
volatile uint32_t		ADC_ConvertedValue_SensorLight = 0;
volatile uint32_t		ADC_ConvertedValue_SensorMicrophone = 0;
volatile float			ADC_ConvertedValue_InternalTemperature = 0;
volatile float			ADC_ConvertedValue_InternalVref = 0;


#ifdef CONFIG_USE_PANEL_NODEMEDIUM
volatile uint32_t		ADC_ConvertedValue_SensorProximity = 0;
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




/***************************************************************************//**
 * @brief
 * @details
 * @param[in]
 * @param[out]
 * @return
 ******************************************************************************/
//void ADC_InitGoodnew(void)
void ADC_InitGoodnew(void)
{
	ADC_ChannelConfTypeDef   sConfig;

	/* Configuration of AdcHandle init structure: ADC parameters and regular group */
	AdcHandle.Instance = BOARD_USE_ADC;

	if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
	{
	/* ADC initialization error */
	Error_Handler();
	}

	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV4;
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.ScanConvMode          = ADC_SCAN_ENABLE;               /* Sequencer enabled (ADC conversion on several channels, successively, following settings below) */
	AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
	AdcHandle.Init.LowPowerAutoWait      = ADC_AUTOWAIT_DISABLE;
	AdcHandle.Init.LowPowerAutoPowerOff  = ADC_AUTOPOWEROFF_DISABLE;
	AdcHandle.Init.ChannelsBank          = ADC_CHANNELS_BANK_A;
	AdcHandle.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled to have only 1 rank converted at each conversion trig, and because discontinuous mode is enabled */
	AdcHandle.Init.NbrOfConversion       = 3;                             /* Sequencer of regular group will convert the 3 first ranks: rank1, rank2, rank3 */
	AdcHandle.Init.DiscontinuousConvMode = ENABLE;                        /* Sequencer of regular group will convert the sequence in several sub-divided sequences */
	AdcHandle.Init.NbrOfDiscConversion   = 1;                             /* Sequencer of regular group will convert ranks one by one, at each conversion trig */
	AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Trig of conversion start done manually by software, without external event */
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandle.Init.DMAContinuousRequests = ENABLE;                        /* ADC-DMA continuous requests to match with DMA configured in circular mode */

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
	sConfig.Rank         = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_96CYCLES;

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
	/* Channel Configuration Error */
	Error_Handler();
	}

	/* Configuration of channel on ADCx regular group on sequencer rank 2 */
	/* Replicate previous rank settings, change only channel and rank */
	sConfig.Channel      = ADC_CHANNEL_VREFINT;
	sConfig.Rank         = ADC_REGULAR_RANK_2;

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
	/* Channel Configuration Error */
	Error_Handler();
	}

	/* Configuration of channel on ADCx regular group on sequencer rank 3 */
	/* Replicate previous rank settings, change only channel and rank */
	sConfig.Channel      = ADC_CHANNEL_TEMPSENSOR;
	sConfig.Rank         = ADC_REGULAR_RANK_3;

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
	/* Channel Configuration Error */
	Error_Handler();
	}

  
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



void ADC_Init( void )
{
	// ADC channel configuration structure declaration
	ADC_ChannelConfTypeDef        sConfig;
	
	
	// NodeSmall-nál meg volt híva
	// NodeMediumnál külön meghívja az HAL .. Init() is.
	#ifdef CONFIG_USE_PANEL_NODESMALL
	HAL_ADC_MspInit(&AdcHandle);
	#endif
	
	
	/* ### - 1 - Initialize ADC peripheral #################################### */
	/*
	*  Instance                  = ADC1.
	*  ClockPrescaler            = PCLK divided by 4.
	*  LowPowerAutoWait          = Disabled
	*  LowPowerAutoPowerOff      = Disabled
	*  Resolution                = 12 bit (increased to 16 bit with oversampler)
	*  ScanConvMode              = ADC_SCAN_ENABLE 
	*  DataAlign                 = Right
	*  ContinuousConvMode        = Enabled
	*  DiscontinuousConvMode     = Enabled
	*  ExternalTrigConv          = ADC_SOFTWARE_START
	*  ExternalTrigConvEdge      = None (Software start)
	*  EOCSelection              = End Of Conversion event
	*  DMAContinuousRequests     = ENABLE
	*/

	AdcHandle.Instance = BOARD_USE_ADC;
	
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
	{
		/* ADC initialization error */
		Error_Handler();
	}
	#endif
	
	
	#ifdef CONFIG_USE_PANEL_NODESMALL
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4;	//ADC_CLOCK_SYNC_PCLK_DIV4;
	#endif
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV4;
	#endif
	
	//AdcHandle.Init.LowPowerAutoWait      = DISABLE;					// NodeSmall
	//AdcHandle.Init.LowPowerAutoPowerOff  = DISABLE;					// NodeSmall
	AdcHandle.Init.LowPowerAutoWait      = ADC_AUTOWAIT_DISABLE;		// NodeMedium!
	AdcHandle.Init.LowPowerAutoPowerOff  = ADC_AUTOPOWEROFF_DISABLE;	// NodeMedium!
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;	// ADC_RESOLUTION12B is jó, de nem tudom melyik az általánosabb // ADC_RESOLUTION12b
	AdcHandle.Init.ScanConvMode          = ADC_SCAN_ENABLE;
	/* Sequencer enabled (ADC conversion on several channels, successively, following settings below) */
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandle.Init.EOCSelection          = EOC_SINGLE_CONV;			// End of Conversion
	AdcHandle.Init.DMAContinuousRequests = ENABLE;
	
	#ifdef CONFIG_USE_PANEL_NODESMALL
	AdcHandle.Init.ContinuousConvMode    = ENABLE;
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	
	// Ez nincs az L1-ben:
	AdcHandle.Init.Overrun               = OVR_DATA_OVERWRITTEN;	// OVR_DATA_PRESERVED
	#endif
	
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	AdcHandle.Init.ContinuousConvMode    = DISABLE;	// DISABLE
	AdcHandle.Init.DiscontinuousConvMode = ENABLE;	// ENABLE
	AdcHandle.Init.ChannelsBank			= ADC_CHANNELS_BANK_A;
	AdcHandle.Init.NbrOfConversion		= ADC_BUFFER_SIZE;	// TODO: valószínüleg nem kell, mert többet akarunk	// HA ContinuousConvMode = ENABLE
	AdcHandle.Init.NbrOfDiscConversion	= 1;	// TODO: valószínüleg nem kell, mert többet akarunk
	#endif
	
	//AdcHandle.NbrOfConversionRank      	 = ADC_MEASURE_NUM;		// Ez kell ?? vagy ez változik... ?
	
	// AMI MÁS NEKEM
	//LSAAdcHandle.Init.ContinuousConvMode 	 	= 0;//DISABLE;		// EZ MÁS NEKEM	
	//LSAAdcHandle.Init.ExternalTrigConv 	 		= ADC_EXTERNALTRIGCONV_T1_CC1;
	//LSAAdcHandle.Init.NbrOfConversion 		 	= 4;
	//LSAAdcHandle.Init.EOCSelection 		 		= 0;//DISABLE;


	/* Initialize ADC peripheral according to the passed parameters */
	if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
	{
		Error_Handler();
	}


	/* ### - 2 - Start calibration ############################################ */
	#ifdef CONFIG_USE_PANEL_NODESMALL
	if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)
	{
		Error_Handler();
	}
	#endif

	
	
	
	
	// sConfigs
	
	
	/* ### - 3 - Channel configuration ######################################## */
	/* Select Channel 0 to be converted */
	/*
	sConfig.Channel      = ADC_VSOURCE_CHANNEL;
	sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
	sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;	// 5 volt eredetileg, 41-gyel is ingazodik
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	*/
	


	#ifdef CONFIG_USE_PANEL_NODESMALL
	sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;	// 5 volt eredetileg, 41-gyel is ingazodik
	#endif
	
	
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	//sConfig.Channel      = ADCx_CHANNELa;			// channelek, egyenként kell beállítani
	//sConfig.Rank         = ADC_REGULAR_RANK_1;	// egyenként kell beállítani
	sConfig.SamplingTime = ADC_SAMPLETIME_48CYCLES;
	#endif
	
	
	// ADC_SENSOR_LIGHT
	sConfig.Channel      = ADC_SENSOR_LIGHT_CHANNEL;
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	sConfig.Rank         = ADC_REGULAR_RANK_1;
	#endif
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	
	
	sConfig.Channel      = ADC_SENSOR_MICROPHONE_CHANNEL;
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	sConfig.Rank         = ADC_REGULAR_RANK_2;
	#endif
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	
	
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	sConfig.Channel      = ADC_SENSOR_PROXIMITY_CHANNEL;
	sConfig.Rank         = ADC_REGULAR_RANK_3;
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	#endif
	
	
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	sConfig.Channel      = ADC_TEMPSENSOR_CHANNEL;
	sConfig.Rank         = ADC_REGULAR_RANK_4;

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	#endif
	
	
	// ADC_VSOURCE
	sConfig.Channel      = ADC_VSOURCE_CHANNEL;
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	sConfig.Rank         = ADC_REGULAR_RANK_5;
	#endif

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	
	
	#ifdef CONFIG_USE_PANEL_NODESMALL
	sConfig.Channel      = ADC_TEMPSENSOR_CHANNEL;
	//sConfig.Rank         = ADC_REGULAR_RANK_5;

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	#endif

	

	#ifdef CONFIG_USE_PANEL_NODESMALL
	sConfig.Channel      = ADC_VREFINT_CHANNEL;
	//sConfig.Rank         = 5;
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	#endif
	
	
	
	// ADC starting with DMA
	if (HAL_ADC_Start_DMA(&AdcHandle, (uint32_t *)ADC_MeasuredValues, ADC_BUFFER_SIZE) != HAL_OK)
	{
		Error_Handler();
	}	
	
}




#if 0
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct;
  static DMA_HandleTypeDef  DmaHandle;
  //RCC_OscInitTypeDef        RCC_OscInitStructure;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable clock of GPIO associated to the peripheral channels */
  //__HAL_RCC_GPIOA_CLK_ENABLE();
	ADC_COMMON_CLK_ENABLE();
  
  /* Enable clock of ADCx peripheral */
  __HAL_RCC_ADC1_CLK_ENABLE();

  /* Note: STM32L1xx ADC is using a dedicated asynchronous clock derived        */
  /*       from HSI RC oscillator 16MHz.                                      */
  /*       The clock source has to be enabled at RCC top level using function */
  /*       "HAL_RCC_OscConfig()" (see comments in stm32l1xx_hal_adc.c header)   */

  /* Enable asynchronous clock source of ADCx */
/*  	// put main
HAL_RCC_GetOscConfig(&RCC_OscInitStructure);
  RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStructure.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStructure.HSIState = RCC_HSI_ON;
  HAL_RCC_OscConfig(&RCC_OscInitStructure);
  */
  
  /* Enable clock of DMA associated to the peripheral */
  __HAL_RCC_DMA1_CLK_ENABLE();
  
  /*##-2- Configure peripheral GPIO ##########################################*/
  /* Configure GPIO pin of the selected ADC channel */
  GPIO_InitStruct.Pin = ADC_VSOURCE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ADC_VSOURCE_PORT, &GPIO_InitStruct);
  
  /*##-3- Configure the DMA ##################################################*/
  /* Configure DMA parameters */
  DmaHandle.Instance = DMA1_Channel1;

  DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;   /* Transfer from ADC by half-word to match with ADC configuration: ADC resolution 10 or 12 bits */
  DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;   /* Transfer to memory by half-word to match with buffer variable type: half-word */
  DmaHandle.Init.Mode                = DMA_CIRCULAR;              /* DMA in circular mode to match with ADC configuration: DMA continuous requests */
  DmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;
  
  /* Deinitialize  & Initialize the DMA for new transfer */
  HAL_DMA_DeInit(&DmaHandle);
  HAL_DMA_Init(&DmaHandle);

  /* Associate the initialized DMA handle to the ADC handle */
  __HAL_LINKDMA(hadc, DMA_Handle, DmaHandle);
  
  /*##-4- Configure the NVIC #################################################*/

  /* NVIC configuration for DMA interrupt (transfer completion or error) */
  /* Priority: high-priority */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  

  /* NVIC configuration for ADC interrupt */
  /* Priority: high-priority */
  HAL_NVIC_SetPriority(ADC1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ADC1_IRQn);
}
#endif


#if 1
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	
	GPIO_InitTypeDef          	GPIO_InitStruct;
	static DMA_HandleTypeDef         DmaHandle;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO clock ****************************************/
	//__GPIOA_CLK_ENABLE();
	ADC_COMMON_CLK_ENABLE();
	
	// NodeSmall + NodeMedium:
	
	/* ADC1 Periph clock enable */
	__ADC1_CLK_ENABLE();
	// __ADC_CLK_ENABLE(); // there is not
	
	/* Enable DMA1 clock */
	__DMA1_CLK_ENABLE();

	
	

	/*##- 2- Configure peripheral GPIO #########################################*/
	/* ADC1 Channel8 GPIO pin configuration */
	GPIO_InitStruct.Pin = ADC_VSOURCE_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ADC_VSOURCE_PORT, &GPIO_InitStruct);


	GPIO_InitStruct.Pin = ADC_SENSOR_LIGHT_PIN;
	HAL_GPIO_Init(ADC_SENSOR_LIGHT_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = ADC_SENSOR_MICROPHONE_PIN;
	HAL_GPIO_Init(ADC_SENSOR_MICROPHONE_PORT, &GPIO_InitStruct);
	
	
	#ifndef CONFIG_USE_PANEL_NODESMALL
	GPIO_InitStruct.Pin = ADC_SENSOR_PROXIMITY_PIN;
	HAL_GPIO_Init(ADC_SENSOR_PROXIMITY_PORT, &GPIO_InitStruct);
	#endif


	
	
	// Temperature sensor ADC_IN16
	// Internal voltage reference ADC_IN17

	//ADC_CHANNEL_TEMPSENSOR	
	//ADC_CHANNEL_VREFINT
	//ADC_CHANNEL_VBAT
	

	/*##- 3- Configure DMA #####################################################*/ 

	/*********************** Configure DMA parameters ***************************/
	DmaHandle.Instance                 = DMA1_Channel1;
	DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
	DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
	DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	DmaHandle.Init.Mode                = DMA_CIRCULAR;
	DmaHandle.Init.Priority            = DMA_PRIORITY_MEDIUM;

	/* Deinitialize  & Initialize the DMA for new transfer */
	HAL_DMA_DeInit(&DmaHandle);  
	HAL_DMA_Init(&DmaHandle);

	/* Associate the DMA handle */
	__HAL_LINKDMA(hadc, DMA_Handle, DmaHandle);

	/* NVIC configuration for DMA Input data interrupt */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 12, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	// NVIC configuration for ADC interrupt
	// Priority: high-priority
	HAL_NVIC_SetPriority(ADC1_IRQn, 11, 0);
	HAL_NVIC_EnableIRQ(ADC1_IRQn);
	#endif
}
#endif


#ifdef CONFIG_USE_PANEL_NODEMEDIUM
/**
  * @brief  This function handles ADC interrupt request.
  * @param  None
  * @retval None
  */
void ADC1_IRQHandler(void)
{
  HAL_ADC_IRQHandler(&AdcHandle);
}

/**
* @brief  This function handles DMA interrupt request.
* @param  None
* @retval None
*/
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
}
#endif



#ifdef CONFIG_USE_PANEL_NODESMALL
void ADC1_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
}
#endif


void ADC_Test ( void )
{
	
	float Voltage;
	float Temperature;
	uint32_t dec;
	uint32_t fraction;
	
	//uint32_t readedTempValue;
	//uint32_t lightValue;
	
	
	while (1)
	{
		#ifdef CONFIG_USE_PANEL_NODEMEDIUM
		if (HAL_ADC_Start(&AdcHandle) != HAL_OK)
		{
		  Error_Handler(); 
		}
		
		vTaskDelay(10);
		#endif
		
      
		uprintf("#\r\n");
		for (int i = 0; i < ADC_BUFFER_SIZE; i++)
		{
			uprintf("%d. value: %d\r\n",i,ADC_MeasuredValues[i]);
		}
		
		

		

		// Vsource
		Voltage = ADC_ConvertToVoltage(ADC_MeasuredValues[ADC_MEASURED_VALUE_CNT_VSOURCE]);
		ADC_ConvertedValue_VSource = Voltage;
		
		// Print VSource
		dec = (uint32_t)Voltage;
		fraction = (uint32_t)((Voltage-dec) *100 );	
		uprintf("Source Voltage: %d.%d [V]\r\n",dec, fraction);

		

		// Internal Temperature Sensor
		Temperature = ADC_ConvertInternalTempSensorValue(ADC_MeasuredValues[ADC_MEASURED_VALUE_CNT_TEMPSENSOR]);
		ADC_ConvertedValue_InternalTemperature = Temperature;
			
		// Print Temp
		dec = (uint32_t)Temperature;
		fraction = (uint32_t)((Temperature-dec) *100 );			
		uprintf("Temperature: %d.%d [°C]\r\n",dec, fraction);
			
		/*	
		// Light
		lightValue = ADC_MeasuredValues[0];							// 1. value - light
		uprintf("Light: %d\r\n",lightValue);
		
		// Others
		uprintf("Mic: %d\r\n",ADC_MeasuredValues[1]);				// 2. value - mic
		//uprintf("Temp: %d\r\n",ADC_MeasuredValues[3]);
		#ifdef CONFIG_USE_PANEL_NODESMALL
		uprintf("Vref: %d\r\n",ADC_MeasuredValues[4]);
		#endif
			
			
		// Vdda a Vref-bol
		// VDDA = 3.3 V x VREFINT_CAL / VREFINT_DATA
		
		//...
		*/
		
		#ifdef CONFIG_USE_FREERTOS
		vTaskDelay(1000);
		#else
		HAL_Delay(1000);
		#endif
	}

	
	
	////////////
	// TEST Temperature float value send on USART
	/*
	HAL_Delay(1000);
	// Internal Temperature Sensor
	ADC_ConvertedValue_InternalTemperature = ADC_ConvertInternalTempSensorValue(ADC_MeasuredValues[3]);

	// Print Temp
	//dec = (uint32_t)Temperature;
	//fraction = (uint32_t)((Temperature-dec) *100 );			
	//uprintf("Temperature: %d.%d [V]\r\n",dec, fraction);

	USART_SendFloat(ADC_ConvertedValue_InternalTemperature);
	*/	
	
	
}


/*
\brief		ONLY FOR NODEMEDIUM, FOR EACH MEASURE
*/
void ADC_Run( void )
{
	
	float Voltage;
	float Temperature;
	float Light;
	//uint32_t dec;
	//uint32_t fraction;
	
	if (HAL_ADC_Start(&AdcHandle) != HAL_OK)
	{
	  Error_Handler(); 
	}
	
	vTaskDelay(10);

	
	/*
	// Print all measured values
	uprintf("#\r\n");
	for (int i = 0; i < ADC_BUFFER_SIZE; i++)
	{
		uprintf("%d. value: %d\r\n",i,ADC_MeasuredValues[i]);
	}
	*/
	
	
	// Vsource
	Voltage = ADC_ConvertToSourceVoltage(ADC_MeasuredValues[ADC_MEASURED_VALUE_CNT_VSOURCE]);
	ADC_ConvertedValue_VSource = Voltage;
	
	/*
	// Print VSource
	dec = (uint32_t)Voltage;
	fraction = (uint32_t)((Voltage-dec) *100 );	
	uprintf("Source Voltage: %d.%d [V]\r\n",dec, fraction);
	*/
	

	// Internal Temperature Sensor
	Temperature = ADC_ConvertInternalTempSensorValue(ADC_MeasuredValues[ADC_MEASURED_VALUE_CNT_TEMPSENSOR]);
	ADC_ConvertedValue_InternalTemperature = Temperature;
	
	/*
	// Print Temp
	dec = (uint32_t)Temperature;
	fraction = (uint32_t)((Temperature-dec) *100 );			
	uprintf("Temperature: %d.%d [°C]\r\n",dec, fraction);
	*/
	
	// Light
	Light = ADC_ConvertToVoltage(ADC_MeasuredValues[ADC_MEASURED_VALUE_CNT_LIGHT]);
	ADC_ConvertedValue_SensorLight = Light;
	
}



float ADC_ConvertToSourceVoltage (uint32_t readValue)
{
	float Voltage;
	// /4096 for 12b resolution, *3.3V for reference Voltage, *11 for voltage divide
	Voltage = (float)readValue * 3.3 * 11 / 4096;
	return Voltage;
}



float ADC_ConvertToVoltage (uint32_t readValue)
{
	float Voltage;
	// /4096 for 12b resolution, *3.3V for reference Voltage 
	Voltage = (float)readValue * 3.3 / 4096;
	return Voltage;
}



float ADC_ConvertInternalTempSensorValue (uint32_t readValue)
{
	float Temperature;
	
	if (readValue == 0)
	{
		return 0.0;
	}
	//int32_t part;
	
	#ifdef CONFIG_USE_PANEL_NODESMALL
	#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7C2))
	#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7B8))
	#endif
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FF800FA))
	#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FF800FE))
	#endif
	
	
	
	// Temp = (110C-30C)/ (TS_CAL2 - TS_CAL1) * (TS_DATA - TS_CAL1) + 30C
	#ifdef CONFIG_USE_PANEL_NODESMALL
	Temperature =  (float)((float)((110-30) / (float)(int32_t)(*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR ))
				* ((float)(int32_t)(readValue - *TEMP30_CAL_ADDR)) + 30.0f);
	#endif
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	#define VDDA_APPLI                     ((uint32_t) 3300)    /* Value of analog voltage supply Vdda (unit: mV) */
	#define VDDA_TEMP_CAL                  ((uint32_t) 3000)    /* Vdda value with which temperature sensor has been calibrated in production (+-10 mV). */
	
	// Hardfaultot okoz?
	// TODO: NodeMedium-nál, L1 alatt HardFault lesz...
	/*
	#define COMPUTATION_TEMPERATURE_TEMP30_TEMP110(TS_ADC_DATA)                    \
	  (((( ((int32_t)((TS_ADC_DATA * VDDA_APPLI) / VDDA_TEMP_CAL)                  \
			- (int32_t) *TEMP30_CAL_ADDR)                                          \
		 ) * (int32_t)(110 - 30)                                                   \
		) / (int32_t)(*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR)                        \
	   ) + 30                                                                      \
	)
	*/
	
	#define INTERNAL_TEMPSENSOR_V110       ((int32_t)  627)         /* Internal temperature sensor, parameter V110 (unit: mV). Refer to device datasheet for min/typ/max values. */
	#define RANGE_12BITS                   ((uint32_t) 4095)    /* Max digital value with a full range of 12 bits */
	#define INTERNAL_TEMPSENSOR_AVGSLOPE   ((int32_t) 1610)         /* Internal temperature sensor, parameter Avg_Slope (unit: uV/DegCelsius). Refer to device datasheet for min/typ/max values. */
	
	#define COMPUTATION_TEMPERATURE_STD_PARAMS_AVGSLOPE_V110(TS_ADC_DATA)          \
	  ((( ((int32_t)(((TS_ADC_DATA) * VDDA_APPLI) / RANGE_12BITS)                  \
		   - (int32_t)((INTERNAL_TEMPSENSOR_V110 * VDDA_TEMP_CAL) / VDDA_APPLI)    \
		  ) * 1000                                                                 \
		) / INTERNAL_TEMPSENSOR_AVGSLOPE                                           \
	   ) + 110                                                                     \
	  )


    //Temperature = COMPUTATION_TEMPERATURE_TEMP30_TEMP110(readValue);
	Temperature = COMPUTATION_TEMPERATURE_STD_PARAMS_AVGSLOPE_V110(readValue);
	
	#endif
	
	
	
	/*
	Temperature =  (110-30);
	Temperature = Temperature / (float)((*TEMP110_CAL_ADDR) - (*TEMP30_CAL_ADDR) );	
	Temperature = Temperature * ((float)(int32_t)((uint32_t)readValue - (uint32_t)(*TEMP30_CAL_ADDR)));
	Temperature += 30.0f;
	*/
	
	return Temperature;
}



void ADC_ConvertAllMeasuredValues ( void )
{
		
	// Vsource
	ADC_ConvertedValue_VSource = ADC_ConvertToSourceVoltage(ADC_MeasuredValues[2]);		// 3. value
	
	// Internal Temperature Sensor
	ADC_ConvertedValue_InternalTemperature = ADC_ConvertInternalTempSensorValue(ADC_MeasuredValues[3]);
		
	// Light
	ADC_ConvertedValue_SensorLight = ADC_MeasuredValues[0];							// 1. value - light

	// Microphone
	ADC_ConvertedValue_SensorMicrophone = ADC_MeasuredValues[1];				// 2. value - mic
	
	#ifdef CONFIG_USE_PANEL_NODESMALL
	// Vref
	ADC_ConvertedValue_InternalVref = ADC_MeasuredValues[4];
	#endif
		
			

	
}


/*
uint32_t ADC_ConvertToVoltageMux100 (uint32_t convertedValue)
{
	uint32_t Voltage;
	// /4096  (resolution), *3.3 (Reference Voltage) *11 (voltage divider), *100 (mux)
	Voltage = convertedValue *3630 / 4096;
	
	return Voltage;
}
*/



#endif // #ifdef CONFIG_MODULE_ADC_ENABLE


