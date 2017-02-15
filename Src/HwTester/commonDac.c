/*
 *		commonDac.c
 *
 *		Created on:		2016. nov. 21.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2016. dec. 13.
 */


#include "commonDac.h"
#include "options.h"
#include "include.h"
#include "board.h"
#include "stm32f4xx_hal.h"


#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

DAC_HandleTypeDef    DacHandle;
static DAC_ChannelConfTypeDef sConfig;



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * \brief	HAL driver: DAC MSP initialization
 */
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{
	// Suppress warning
	(void)hdac;

	GPIO_InitTypeDef GPIO_InitStruct;

	/*##-1- Enable peripherals and GPIO Clocks */
	/* Enable GPIO clock */
	DACx_CHANNEL_GPIO_CLK_ENABLE();
	/* DAC Periph clock enable */
	DACx_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO */
	/* DAC Channel1 GPIO pin configuration */
	GPIO_InitStruct.Pin = DACx_CHANNEL1_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DACx_CHANNEL1_GPIO_PORT, &GPIO_InitStruct);

	/* DAC Channel1 GPIO pin configuration */
	GPIO_InitStruct.Pin = DACx_CHANNEL2_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DACx_CHANNEL2_GPIO_PORT, &GPIO_InitStruct);

}



/**
 * \brief	Initialize DAC
 */
void DAC_Init(void)
{
	//HAL_DAC_MspInit(&DacHandle);	// TODO: there is no call for this?

	/*##-1- Configure the DAC peripheral */
	DacHandle.Instance = DACx;

	/*##-1- Initialize the DAC peripheral */
	if (HAL_DAC_Init(&DacHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	/*##-2- DAC channel2 Configuration */
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

	if (HAL_DAC_ConfigChannel(&DacHandle, &sConfig, DACx_CHANNEL1) != HAL_OK)
	{
		/* Channel configuration Error */
		Error_Handler();
	}
	if (HAL_DAC_ConfigChannel(&DacHandle, &sConfig, DACx_CHANNEL2) != HAL_OK)
	{
		/* Channel configuration Error */
		Error_Handler();
	}

	/*##-5- Set DAC channel1 DHR12RD register */
	if (HAL_DAC_SetValue(&DacHandle, DACx_CHANNEL1, DAC_ALIGN_12B_R, 0x00) != HAL_OK)
	{
		/* Setting value Error */
		Error_Handler();
	}
	if (HAL_DAC_SetValue(&DacHandle, DACx_CHANNEL2, DAC_ALIGN_12B_R, 0x00) != HAL_OK)
	{
		/* Setting value Error */
		Error_Handler();
	}


	/*##-4- Enable DAC Channel1 */
	if (HAL_DAC_Start(&DacHandle, DACx_CHANNEL1) != HAL_OK)
	{
		/* Start Error */
		Error_Handler();
	}
	if (HAL_DAC_Start(&DacHandle, DACx_CHANNEL2) != HAL_OK)
	{
		/* Start Error */
		Error_Handler();
	}

}



/**
 * \brief	Convert voltage to DA value
 */
uint32_t DAC_VoltageToBinary (float voltage)
{
	return voltage / COMMON_DAC_MAX_VOLTAGE * COMMON_DAC_DA_MAX_VALUE;
}



/**
 * \brief	Set DAC value
 */
bool DAC_SetValue (DAC_Channel_t channel, float voltage)
{

	uint32_t channelDefine = DACx_CHANNEL1;
	uint32_t dacValue = 0;

	// Check channel
	switch (channel)
	{
		case Channel_Unknown:
			channelDefine = DACx_CHANNEL1;
			break;

		case Channel_1:
			channelDefine = DACx_CHANNEL1;
			break;

		case Channel_2:
			channelDefine = DACx_CHANNEL2;
			break;

		default:
			channelDefine = DACx_CHANNEL1;
			break;
	}

	// Check value
	dacValue = DAC_VoltageToBinary(voltage);

	if (HAL_DAC_SetValue(&DacHandle, channelDefine, DAC_ALIGN_12B_R, dacValue) != HAL_OK)
	{
		/* Setting value Error */
		Error_Handler();
	}

	return true;
}

#endif	// CONFIG_MODULE_COMMON_DAC_ENABLE
