

#ifdef CONFIG_USE_PANEL_NODESMALL
	#include "stm32f0xx_hal.h"
	#include "stm32f0xx.h"
	#include "stm32f0xx_it.h"
#endif
#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	#include "stm32l1xx_hal.h"
	#include "stm32l1xx_it.h"
#endif
#ifdef CONFIG_USE_PANEL_CENTER

#endif


#include "board.h"



#ifdef CONFIG_USE_PANEL_NODESMALL
/* External variables --------------------------------------------------------*/
 
extern void xPortSysTickHandler(void);

extern UART_HandleTypeDef BluetoothUartHandle;

extern I2C_HandleTypeDef I2cHandle;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
// AT stm32f0xx_it.c
/*
void SysTick_Handler(void)
{
	#ifdef CONFIG_USE_FREERTOS
	xPortSysTickHandler();
	#endif
	HAL_IncTick();
}
*/



//	// because USART1_IRQHandler defined in usart.c
/*
void USARTx_IRQHandler(void)
{
  HAL_UART_IRQHandler(&BluetoothUartHandle);
}


void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&BluetoothUartHandle);
}
//
*/



void EXTI0_1_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(BUTTON_RIGHT_GPIO_PIN);
}


/*
  EXTI0_1_IRQn                = 5      // EXTI Line 0 and 1 Interrupts
  EXTI2_3_IRQn                = 6      //EXTI Line 2 and 3 Interrupts
  EXTI4_15_IRQn               
*/


/**
  * @brief  This function handles external lines 4 to 15 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{

	
	if (HAL_GPIO_ReadPin(BUTTON_UP_GPIO_PORT,BUTTON_UP_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_UP_GPIO_PIN);
	}
	if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_PORT,BUTTON_DOWN_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_DOWN_GPIO_PIN);
	}
	if ( HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_PORT,BUTTON_LEFT_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_LEFT_GPIO_PIN);
	}


	//HAL_GPIO_EXTI_IRQHandler(TAMPER_BUTTON_PIN);
	
}



void I2Cx_DMA_TX_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(I2cHandle.hdmarx);
  HAL_DMA_IRQHandler(I2cHandle.hdmatx);
}


#endif 	// #ifdef CONFIG_USE_PANEL_NODESMALL




#ifdef CONFIG_USE_PANEL_NODEMEDIUM

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(BUTTON_RIGHT_GPIO_PIN);
}


void EXTI2_IRQHandler(void)
{	
	HAL_GPIO_EXTI_IRQHandler(BUTTON_UP_GPIO_PIN);
}


void EXTI9_5_IRQHandler(void)
{
	// 9
	
	if ( HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_PORT,BUTTON_LEFT_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_LEFT_GPIO_PIN);
	}
}


void EXTI15_10_IRQHandler(void)
{
	
	// 13	- BUTTON_DOWN	
	if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_PORT,BUTTON_DOWN_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_DOWN_GPIO_PIN);
	}
	
	
	// SENSOR_MOTION			GPIO_PIN_12
	// SENSOR_SOUND_IMPACT		GPIO_PIN_14
	if (HAL_GPIO_ReadPin(SENSOR_MOTION_GPIO_PORT,SENSOR_MOTION_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(SENSOR_MOTION_GPIO_PIN);
	}
	if (HAL_GPIO_ReadPin(SENSOR_SOUND_IMPACT_GPIO_PORT,SENSOR_SOUND_IMPACT_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(SENSOR_SOUND_IMPACT_GPIO_PIN);
	}
}	


#endif //#ifdef CONFIG_USE_PANEL_NODEMEDIUM





#ifdef CONFIG_USE_PANEL_CENTERPANEL


// Up		PC7
// Down		PC8
// Right	PA0
// Left		PC9


void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(BUTTON_RIGHT_GPIO_PIN);
}


void EXTI9_5_IRQHandler(void)
{
	// 7-8-9
	
	if ( HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_PORT,BUTTON_LEFT_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_LEFT_GPIO_PIN);
	}
	if (HAL_GPIO_ReadPin(BUTTON_UP_GPIO_PORT,BUTTON_UP_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_UP_GPIO_PIN);
	}		
	if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_PORT,BUTTON_DOWN_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_DOWN_GPIO_PIN);
	}
}


#endif //#ifdef CONFIG_USE_PANEL_CENTERPANEL



#ifdef CONFIG_USE_PANEL_DISCOVERY

// PA0 - User button
void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(BUTTON_USER_GPIO_PIN);
}

#endif	// #ifdef CONFIG_USE_PANEL_DISCOVERY



/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

#ifdef CONFIG_MODULE_BUTTON_ENABLE
	// BUTTON MODULE

#if defined(CONFIG_USE_PANEL_NODESMALL) || defined(CONFIG_USE_PANEL_NODEMEDIUM) || defined(CONFIG_USE_PANEL_CENTERPANEL)
	if (GPIO_Pin == BUTTON_UP_GPIO_PIN)
	{
		// Toggle LED
		LED_RED_TOGGLE();
		BUTTON_Clicked |= ( ( 1 << PressedButton_Pressed) | ( 1 << PressedButton_Up ) );
	}
	if (GPIO_Pin == BUTTON_DOWN_GPIO_PIN)
	{
		// Toggle LED
		LED_RED_TOGGLE();
		BUTTON_Clicked |= ( ( 1 << PressedButton_Pressed) | ( 1 << PressedButton_Down ) );
	}
	if (GPIO_Pin == BUTTON_RIGHT_GPIO_PIN)
	{
		// Toggle LED
		LED_RED_TOGGLE();
		BUTTON_Clicked |= ( ( 1 << PressedButton_Pressed) | ( 1 << PressedButton_Right ) );
	}
	if (GPIO_Pin == BUTTON_LEFT_GPIO_PIN)
	{
		// Toggle LED
		LED_RED_TOGGLE();
		BUTTON_Clicked |= ( ( 1 << PressedButton_Pressed) | ( 1 << PressedButton_Left ) );
	}
	
	//BUTTON_Clicked = 1;
#endif	// #ifdef CONFIG_MODULE_BUTTON_ENABLE
#ifdef CONFIG_USE_PANEL_DISCOVERY
	if (GPIO_Pin == BUTTON_USER_GPIO_PIN)
	{
		// Toggle LED
		LED_RED_TOGGLE();
		BUTTON_Clicked |= ( ( 1 << PressedButton_Pressed) | ( 1 << PressedButton_Up ) );
	}
#endif
#endif

	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	if (GPIO_Pin == SENSOR_MOTION_GPIO_PIN)
	{
		GLOBAL_IO_Sensor_Motion_Move = 1;
	}
	if (GPIO_Pin == SENSOR_SOUND_IMPACT_GPIO_PIN)
	{
		GLOBAL_IO_Sensor_Sound_Impact_Sound = 1;
	}
	#endif
	
	
}



