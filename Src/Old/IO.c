/*
 *		IO.c
 *
 *		Created on:		2015.
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
#include "IO.h"
#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_IO_ENABLE
/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
uint8_t GLOBAL_IO_Sensor_Motion_Move = 0;
uint8_t GLOBAL_IO_Sensor_Sound_Impact_Sound = 0;

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

/***************************************************************************//**
 * @brief
 * @details
 * @param[in]
 * @param[out]
 * @return
 ******************************************************************************/
void IO_Init ( void )
{
	
	GPIO_InitTypeDef          	GPIO_InitStruct;
	
	
	// OUTPUTS - RELAYS
	
	RELAY_PINS_CLK_ENABLES();
	
	
	GPIO_InitStruct.Pin = RELAY_1_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(RELAY_1_GPIO_PORT, &GPIO_InitStruct);

	
	RELAY_1_OFF();
	
	
	/*
	GPIO_InitStruct.Pin = RELAY_2_GPIO_PIN;
	HAL_GPIO_Init(RELAY_2_GPIO_PORT, &GPIO_InitStruct);
	*/
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	
	SENSOR_PINS_CLK_ENABLES();
	
	// SENSOR_MOTION
	// PIR signaling; HIGH = movement/LOW = no movement
	GPIO_InitStruct.Pin = SENSOR_MOTION_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;	// moving
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(SENSOR_MOTION_GPIO_PORT, &GPIO_InitStruct);
	
	
	// SENSOR_SOUND_IMPACT
	// PIR signaling; HIGH = movement/LOW = no movement
	GPIO_InitStruct.Pin = SENSOR_SOUND_IMPACT_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;	// sounding
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(SENSOR_SOUND_IMPACT_GPIO_PORT, &GPIO_InitStruct);
	
	// NodeMedium:
	// SENSOR_MOTION			GPIO_PIN_12
	// SENSOR_SOUND_IMPACT		GPIO_PIN_14
	
	#endif
	
	
	
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM	
	// 12, 14. pins
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 4, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); 
	#endif



	
	
}


#endif // #ifdef CONFIG_MODULE_IO_ENABLE
