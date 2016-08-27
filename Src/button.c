/*******************************************************************************
 * Purpose:
 ******************************************************************************/

/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "button.h"
#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_BUTTON_ENABLE

/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
volatile uint8_t BUTTON_Clicked = 0;

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
void BUTTON_Init ( void )
{

	GPIO_InitTypeDef   GPIO_InitStructure;

	
	// Enable GPIO clocks
	BUTTON_CLK_ENABLES();


	
	
	// Configure x pin as input floating
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	
	GPIO_InitStructure.Pin = BUTTON_UP_GPIO_PIN;
	HAL_GPIO_Init(BUTTON_UP_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = BUTTON_DOWN_GPIO_PIN;
	HAL_GPIO_Init(BUTTON_DOWN_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = BUTTON_LEFT_GPIO_PIN;
	HAL_GPIO_Init(BUTTON_LEFT_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin  = BUTTON_RIGHT_GPIO_PIN;
	HAL_GPIO_Init(BUTTON_RIGHT_GPIO_PORT, &GPIO_InitStructure);
	
	
	
	// Config IT-s
	
	#ifdef CONFIG_USE_PANEL_NODESMALL
	
	// Enable and set EXTI lines 0 to 1 Interrupt to the lowest priority
	HAL_NVIC_SetPriority(EXTI0_1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn); 
	 
	 
	// Enable and set EXTI lines 4 to 15 Interrupt to the lowest priority
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn); 
	
	#endif
	
	
	
	
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	
	// 0, 2, 9, 13. pins
	
	// Enable and set EXTI lines
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn); 
	 
	// Enable and set EXTI lines
	HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn); 
	
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); 
	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 4, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); 

	#endif
	
	
	#ifdef CONFIG_USE_PANEL_CENTERPANEL
	
	// Up		PC7
	// Down		PC8
	// Right	PA0
	// Left		PC9
	
	// Enable and set EXTI lines
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn); 
	 
	// Enable and set EXTI lines
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); 
	

	#endif

}





/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
/*
// move to it.c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
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
	
}
*/


#endif	// #ifdef CONFIG_MODULE_BUTTON_ENABLE


