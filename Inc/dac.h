/*
 *		dac.h
 *
 *		Created on:		2016. nov. 21.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2016. nov. 21.
 */

#ifndef DAC_H_
#define DAC_H_


/// DEFINES

#define DACx                            DAC

/* Definition for DAC clock resources */
#define DACx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define DACx_CLK_ENABLE()               __HAL_RCC_DAC_CLK_ENABLE()

/* Definition for DACx Channel1 Pin */
#define DACx_CHANNEL1_PIN                GPIO_PIN_4
#define DACx_CHANNEL1_GPIO_PORT          GPIOA

#define DACx_CHANNEL1                    DAC_CHANNEL_1


#define DAC_MAX_VOLTAGE					3.3f
#define DAC_DA_MAX_VALUE				4095



/// Enums
typedef enum
{
	Channel_Unknown,
	Channel_1,
	Channel_2
} DAC_Channel_t;


/// GLOBAL VARIABLES
extern DAC_HandleTypeDef    DacHandle;


/// FUNCTIONS
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac);
void DAC_Config(void);
bool DAC_SetValue (DAC_Channel_t channel, float voltage);



#endif /* DAC_H_ */
