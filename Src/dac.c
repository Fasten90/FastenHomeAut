/*
 *		dac.c
 *
 *		Created on:		2016. nov. 21.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2016. nov. 21.
 */


#include "options.h"
#include "include.h"
#include "stm32f4xx_hal.h"
#include "dac.h"

DAC_HandleTypeDef    DacHandle;
static DAC_ChannelConfTypeDef sConfig;


void DAC_Ch1_TriangleConfig(void);


void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{
  GPIO_InitTypeDef          GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO clock ****************************************/
  DACx_CHANNEL_GPIO_CLK_ENABLE();
  /* DAC Periph clock enable */
  DACx_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* DAC Channel1 GPIO pin configuration */
  GPIO_InitStruct.Pin = DACx_CHANNEL1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DACx_CHANNEL1_GPIO_PORT, &GPIO_InitStruct);


}




void DAC_Ch1_TriangleConfig(void)
{

  /*##-1- Configure the DAC peripheral #######################################*/
  DacHandle.Instance = DACx;

  /*##-1- Initialize the DAC peripheral ######################################*/
  if(HAL_DAC_Init(&DacHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- DAC channel2 Configuration #########################################*/
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

  if(HAL_DAC_ConfigChannel(&DacHandle, &sConfig, DACx_CHANNEL1) != HAL_OK)
  {
    /* Channel configuration Error */
    Error_Handler();
  }


  /*##-5- Set DAC channel1 DHR12RD register ################################################*/

  if(HAL_DAC_SetValue(&DacHandle, DACx_CHANNEL1, DAC_ALIGN_8B_R, 0xFF) != HAL_OK)
  {
	/* Setting value Error */
	Error_Handler();
  }



  /*##-4- Enable DAC Channel1 ################################################*/
  if(HAL_DAC_Start(&DacHandle, DACx_CHANNEL1) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }

}

