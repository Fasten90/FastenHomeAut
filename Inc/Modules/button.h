/*
 *		button.h
 *
 *		Created on:		2016
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 8.
 */
 
#ifndef BUTTON_H_
#define BUTTON_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
#if BUTTON_NUM == 1
	PressedButton_User,
#elif BUTTON_NUM > 1
	PressedButton_Up,
	PressedButton_Down,
	PressedButton_Right,
	PressedButton_Left,
#endif

	// Last, do not use
	PressedButton_Count
} ButtonType_t;


typedef enum
{
	ButtonPress_Short,
	ButtonPress_Long,
	ButtonPress_Continuous,
	ButtonPress_ReleasedContinuous
} ButtonPressType_t;


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
extern volatile uint8_t BUTTON_Clicked;


/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void BUTTON_Init(void);
bool BUTTON_GetButtonState(ButtonType_t button);

const char * BUTTON_GetButtonName(ButtonType_t button);
const char * BUTTON_GetPressTypeName(ButtonPressType_t pressType);

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);



#endif /* TEMPLATE_H_ */
