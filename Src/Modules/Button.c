/*
 *        Button.c
 *        Created on:        2016-01-01
 *        Author:            Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        Button handler module
 *        Target:            STM32Fx
 *        Version:        v1
 *        Last modified:    2017-02-08
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "compiler.h"
#include "Button.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_BUTTON_ENABLE



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

volatile uint8_t BUTTON_Clicked = 0;



/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/


/**
 * \brief    Initialize buttons
 */
void BUTTON_Init(void)
{
    GPIO_InitTypeDef   GPIO_InitStructure;

    // Enable GPIO clocks
    BUTTON_CLK_ENABLES();

    // Configure x pin as input floating
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    
#if defined(CONFIG_USE_PANEL_HOMEAUTPANELS)

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
    // User    button
    GPIO_InitStructure.Pin  = BUTTON_USER_GPIO_PIN;
    HAL_GPIO_Init(BUTTON_USER_GPIO_PORT, &GPIO_InitStructure);
#endif
    
    // Config IT-s
    
#ifdef CONFIG_USE_PANEL_HOMEAUTNODESMALL
    // Enable and set EXTI lines 0 to 1 Interrupt to the lowest priority
    HAL_NVIC_SetPriority(EXTI0_1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn); 
     
     
    // Enable and set EXTI lines 4 to 15 Interrupt to the lowest priority
    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn); 
#endif


#ifdef CONFIG_USE_PANEL_FASTENNODE
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
    // Up        PC7
    // Down        PC8
    // Right    PA0
    // Left        PC9
    
    // Enable and set EXTI lines
    HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn); 
     
    // Enable and set EXTI lines
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); 
#endif


#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)
    // User    button

    // Enable and set EXTI lines
    HAL_NVIC_SetPriority(BUTTON_USER_EXTI_IRQn,
            BUTTON_USER_INTERRUPT_PREEMT_PRIORITY,
            BUTTON_USER_INTERRUPT_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(BUTTON_USER_EXTI_IRQn);
#endif
}



/**
 * \brief    Get Button state
 *             Only one button can check
 * \retval    true    if pressed/pressing
 *             false    if not or wrong button
 */
bool BUTTON_GetButtonState(ButtonType_t button)
{
    bool state = false;

    switch (button)
    {
#if BUTTON_NUM == 1
        case PressedButton_User:
            state = (HAL_GPIO_ReadPin(BUTTON_USER_GPIO_PORT, BUTTON_USER_GPIO_PIN) == GPIO_PIN_SET) ? true : false;
            break;

#elif BUTTON_NUM > 1
            // Set (high) state = pressed state = true
        case PressedButton_Up:
            state = (HAL_GPIO_ReadPin(BUTTON_UP_GPIO_PORT, BUTTON_UP_GPIO_PIN) == GPIO_PIN_SET) ? true : false;
            break;

        case PressedButton_Down:
            state = (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_PORT, BUTTON_DOWN_GPIO_PIN) == GPIO_PIN_SET) ? true : false;
            break;

        case PressedButton_Right:
            state = (HAL_GPIO_ReadPin(BUTTON_RIGHT_GPIO_PORT, BUTTON_RIGHT_GPIO_PIN) == GPIO_PIN_SET) ? true : false;
            break;

        case PressedButton_Left:
            state = (HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_PORT, BUTTON_LEFT_GPIO_PIN) == GPIO_PIN_SET) ? true : false;
            break;
#endif

        case PressedButton_Count:
        default:
            state = false;
            break;
    }

    return state;
}



/**
 * \brief    Get button name
 * \brief    button    button type
 * \retval    Button name (string)
 */
const char * BUTTON_GetButtonName(ButtonType_t button)
{
    const char * buttonName = NULL;

#if BUTTON_NUM == 4
    switch (button)
    {
        case PressedButton_Right:
            buttonName = "right";
            break;

        case PressedButton_Left:
            buttonName = "left";
            break;

        case PressedButton_Up:
            buttonName = "up";
            break;

        case PressedButton_Down:
            buttonName = "down";
            break;

        case PressedButton_Count:
        default:
            buttonName = "";
            break;
    }
#elif BUTTON_NUM == 1
    if (button == PressedButton_User)
        buttonName = "user";
#endif

    return buttonName;
}



/**
 * \brief    Get button press name
 * \param    pressType    button press type
 * \retval    press type name (string)
 */
const char * BUTTON_GetPressTypeName(ButtonPressType_t pressType)
{
    const char * name = NULL;

    switch (pressType)
    {
        case ButtonPress_Short:
            name = "short";
            break;

        case ButtonPress_Long:
            name = "long";
            break;

        case ButtonPress_Continuous:
            name = "continuous";
            break;

        case ButtonPress_ReleasedContinuous:
            name = "released";
            break;

        default:
            name = "";
            break;
    }

    return name;
}



/*
 * \note    EXTI GPIO callbacks function moved to it.c, because not every GPIO are button pin
 */



#endif    // #ifdef CONFIG_MODULE_BUTTON_ENABLE
