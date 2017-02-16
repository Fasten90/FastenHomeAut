/*
 *		button.c
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
#include "include.h"
#include "Button.h"


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
	
#if defined(CONFIG_USE_PANEL_NODESMALL) \
	|| defined(CONFIG_USE_PANEL_NODEMEDIUM) \
	|| defined(CONFIG_USE_PANEL_HOMEAUTCENTERPANEL)

	GPIO_InitStructure.Pin = BUTTON_UP_GPIO_PIN;
	HAL_GPIO_Init(BUTTON_UP_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = BUTTON_DOWN_GPIO_PIN;
	HAL_GPIO_Init(BUTTON_DOWN_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = BUTTON_LEFT_GPIO_PIN;
	HAL_GPIO_Init(BUTTON_LEFT_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin  = BUTTON_RIGHT_GPIO_PIN;
	HAL_GPIO_Init(BUTTON_RIGHT_GPIO_PORT, &GPIO_InitStructure);
#endif
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)
	GPIO_InitStructure.Pin  = BUTTON_USER_GPIO_PIN;
	HAL_GPIO_Init(BUTTON_USER_GPIO_PORT, &GPIO_InitStructure);
#endif
	
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
	
	
#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL
	
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


#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)

	// User	button

	// Enable and set EXTI lines
	HAL_NVIC_SetPriority(BUTTON_USER_EXTI_IRQn,
			BUTTON_USER_INTERRUPT_PREEMT_PRIORITY,
			BUTTON_USER_INTERRUPT_SUB_PRIORITY);
	HAL_NVIC_EnableIRQ(BUTTON_USER_EXTI_IRQn);
#endif

}


/*
 * \note	EXTI GPIO callbacks function moved to it.c, because not every GPIO are button pin
 */

#endif	// #ifdef CONFIG_MODULE_BUTTON_ENABLE


