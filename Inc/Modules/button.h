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
typedef enum {
	PressedButton_Pressed,
	PressedButton_Up,
	PressedButton_Down,
	PressedButton_Right,
	PressedButton_Left
	
} BUTTON_PressedButtonType;



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
void BUTTON_Init ( void );
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);



#endif /* TEMPLATE_H_ */
