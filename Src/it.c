/*
 *		it.c
 *		Created on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Interrupt handler
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-09-15
 */



#include "options.h"
#include "include.h"
#include "board.h"
#ifdef CONFIG_MODULE_BUTTON_ENABLE
#include "Button.h"
#endif
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
#include "TaskHandler.h"
#include "TaskList.h"
#endif
#ifdef CONFIG_MODULE_ADC_ENABLE
#include "ADC.h"
#endif
#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
#include "CommonADC.h"
#endif

// TODO: Delete these, if not need
#ifdef CONFIG_USE_PANEL_HOMEAUTNODESMALL
	#include "stm32f0xx_hal.h"
	#include "stm32f0xx.h"
	#include "stm32f0xx_it.h"
#endif
#ifdef CONFIG_USE_PANEL_HOMEAUTNODEMEDIUM
	#include "stm32l1xx_hal.h"
	#include "stm32l1xx_it.h"
#endif



#ifdef CONFIG_USE_PANEL_HOMEAUTNODESMALL
/*
  EXTI0_1_IRQn
  EXTI2_3_IRQn
  EXTI4_15_IRQn               
*/

void EXTI0_1_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(BUTTON_RIGHT_GPIO_PIN);
}

/**
  * @brief  This function handles external lines 4 to 15 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{

	if (HAL_GPIO_ReadPin(BUTTON_UP_GPIO_PORT, BUTTON_UP_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_UP_GPIO_PIN);
	}
	if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_PORT, BUTTON_DOWN_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_DOWN_GPIO_PIN);
	}
	if ( HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_PORT, BUTTON_LEFT_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_LEFT_GPIO_PIN);
	}

	//HAL_GPIO_EXTI_IRQHandler(TAMPER_BUTTON_PIN);
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
	
	if ( HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_PORT, BUTTON_LEFT_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_LEFT_GPIO_PIN);
	}
}


void EXTI15_10_IRQHandler(void)
{
	
	// 13	- BUTTON_DOWN	
	if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_PORT, BUTTON_DOWN_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_DOWN_GPIO_PIN);
	}
	
	
	// SENSOR_MOTION			GPIO_PIN_12
	// SENSOR_SOUND_IMPACT		GPIO_PIN_14
	if (HAL_GPIO_ReadPin(SENSOR_MOTION_GPIO_PORT, SENSOR_MOTION_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(SENSOR_MOTION_GPIO_PIN);
	}
	if (HAL_GPIO_ReadPin(SENSOR_SOUND_IMPACT_GPIO_PORT, SENSOR_SOUND_IMPACT_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(SENSOR_SOUND_IMPACT_GPIO_PIN);
	}
}	


#endif //#ifdef CONFIG_USE_PANEL_NODEMEDIUM



#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL

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
	
	if (HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_PORT, BUTTON_LEFT_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_LEFT_GPIO_PIN);
	}
	if (HAL_GPIO_ReadPin(BUTTON_UP_GPIO_PORT, BUTTON_UP_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_UP_GPIO_PIN);
	}		
	if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_PORT, BUTTON_DOWN_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_DOWN_GPIO_PIN);
	}
}

#endif //#ifdef CONFIG_USE_PANEL_CENTERPANEL



#ifdef CONFIG_USE_PANEL_STM32F4DISCOVERY
// PA0 - User button
void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(BUTTON_USER_GPIO_PIN);
}
#endif	// #ifdef CONFIG_USE_PANEL_STM32F4DISCOVERY



#ifdef CONFIG_USE_PANEL_NUCLEOF401RE
// PC13 - User button
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(BUTTON_USER_GPIO_PIN);
}
#endif	// #ifdef CONFIG_USE_PANEL_NUCLEOF401RE



#ifdef CONFIG_USE_PANEL_FASTENNODE

/*
BUTTON_UP			GPIOA0
BUTTON_RIGHT		GPIOB8
BUTTON_DOWN			GPIOB9
BUTTON_LEFT			GPIOA15
*/

#ifdef CONFIG_MODULE_BUTTON_ENABLE
void EXTI0_1_IRQHandler(void)
{
	if (HAL_GPIO_ReadPin(BUTTON_UP_GPIO_PORT, BUTTON_UP_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_UP_GPIO_PIN);
	}

	// Clear IT
	__HAL_GPIO_EXTI_CLEAR_IT(BUTTON_UP_GPIO_PIN);

#ifdef CONFIG_DEBUG_MODE
	if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0 | GPIO_PIN_1) != RESET)
		Error_Handler();
#endif
}

void EXTI4_15_IRQHandler(void)
{
	/*
	// Original mode: It sometimes not handle the IRQ and the sw will crash (infinite loop)
	if (HAL_GPIO_ReadPin(BUTTON_RIGHT_GPIO_PORT, BUTTON_RIGHT_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_RIGHT_GPIO_PIN);
	}
	if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_PORT, BUTTON_DOWN_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_DOWN_GPIO_PIN);
	}
	if (HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_PORT, BUTTON_LEFT_GPIO_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_LEFT_GPIO_PIN);
	}*/

	if (__HAL_GPIO_EXTI_GET_IT(BUTTON_RIGHT_GPIO_PIN) != RESET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_RIGHT_GPIO_PIN);
	}
	if (__HAL_GPIO_EXTI_GET_IT(BUTTON_DOWN_GPIO_PIN) != RESET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_DOWN_GPIO_PIN);
	}
	if (__HAL_GPIO_EXTI_GET_IT(BUTTON_LEFT_GPIO_PIN) != RESET)
	{
		HAL_GPIO_EXTI_IRQHandler(BUTTON_LEFT_GPIO_PIN);
	}

	// Clear IT
	if (__HAL_GPIO_EXTI_GET_IT(BUTTON_RIGHT_GPIO_PIN | BUTTON_DOWN_GPIO_PIN | BUTTON_LEFT_GPIO_PIN))
	{
		__HAL_GPIO_EXTI_CLEAR_IT(BUTTON_RIGHT_GPIO_PIN | BUTTON_DOWN_GPIO_PIN | BUTTON_LEFT_GPIO_PIN);
	}
#ifdef CONFIG_DEBUG_MODE
	// Check, there is EXTI interrupt?
	if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6
			| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10
			| GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)
		!= RESET)
		Error_Handler();
#endif
}
#endif	// #ifdef CONFIG_MODULE_BUTTON_ENABLE

#endif	// #ifdef CONFIG_USE_PANEL_FASTENNODE



/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

#ifdef CONFIG_MODULE_BUTTON_ENABLE
	// Button handling

#if defined(CONFIG_USE_PANEL_HOMEAUTPANELS)
	if (GPIO_Pin == BUTTON_UP_GPIO_PIN)
	{
		// Toggle LED
		//LED_RED_TOGGLE();
		BUTTON_Clicked |= (1 << PressedButton_Up);
	}
	if (GPIO_Pin == BUTTON_DOWN_GPIO_PIN)
	{
		// Toggle LED
		//LED_RED_TOGGLE();
		BUTTON_Clicked |= (1 << PressedButton_Down);
	}
	if (GPIO_Pin == BUTTON_RIGHT_GPIO_PIN)
	{
		// Toggle LED
		//LED_RED_TOGGLE();
		BUTTON_Clicked |= (1 << PressedButton_Right);
	}
	if (GPIO_Pin == BUTTON_LEFT_GPIO_PIN)
	{
		// Toggle LED
		//LED_RED_TOGGLE();
		BUTTON_Clicked |= (1 << PressedButton_Left);
	}
	#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
	if (GPIO_Pin & (BUTTON_UP_GPIO_PIN | BUTTON_DOWN_GPIO_PIN | BUTTON_RIGHT_GPIO_PIN | BUTTON_LEFT_GPIO_PIN))
	{
		TaskHandler_RequestTaskScheduling(Task_ButtonPressed);
	}
	#endif
#endif	// #ifdef CONFIG_MODULE_BUTTON_ENABLE

#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)
	if (GPIO_Pin == BUTTON_USER_GPIO_PIN)
	{
		// Toggle LED
		//LED_GREEN_TOGGLE();
		BUTTON_Clicked |= (1 << PressedButton_User);

		#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
		TaskHandler_RequestTaskScheduling(Task_ButtonPressed);
		#endif
	}
#endif
#endif

#if defined(CONFIG_MODULE_IO_ENABLE) && defined(CONFIG_MODULE_IO_INPUT_MOTION_ENABLE)
	if (GPIO_Pin == SENSOR_MOTION_GPIO_PIN)
	{
		// TODO: Check actual state !
		//InputState_t inputState =
		IO_SetInputState(Input_MotionMove, InputState_Active);
	}
#endif
#if defined(CONFIG_MODULE_IO_ENABLE) && defined(CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE)
	if (GPIO_Pin == SENSOR_SOUND_IMPACT_GPIO_PIN)
	{
		// TODO: Check actual state !
		//InputState_t inputState =
		IO_SetInputState(Input_SoundImpact, InputState_Active);
	}
#endif

}	// End of HAL_GPIO_EXTI_Callback()



#if defined(CONFIG_MODULE_COMMON_ADC_ENABLE) || defined(CONFIG_MODULE_ADC_ENABLE)
void ADCx_DMA_IRQHandler(void)
{
	HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
}
#endif


#if defined(CONFIG_USE_PANEL_FASTENNODE) && defined(CONFIG_MODULE_MOTOR_ENABLE)
#if UNUSED
void TIM3_IRQHandler(void)
{
	// Error...
#warning "Delete, not need, never run"
	extern TIM_HandleTypeDef    TimPWMDcMotor_Handle;	// Dc motor
	extern TIM_HandleTypeDef    TimPWMServo_Handle;	// Servo motor

	__HAL_TIM_CLEAR_FLAG(&TimPWMDcMotor_Handle, 0xFFFFFFFF);
	__HAL_TIM_CLEAR_FLAG(&TimPWMServo_Handle, 0xFFFFFFFF);

	LED_RED_ON();
	LED_RED_OFF();
}
#endif
#endif


