/*
 *		Logic.h
 *
 *		Created on:		2017. máj. 23.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. máj. 23.
 */

#ifndef LOGIC_H_
#define LOGIC_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "Button.h"

#ifdef CONFIG_BUTTON_DEBUG_ENABLE
#include "DebugUart.h"
#endif


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_BUTTON_DEBUG_ENABLE) && defined(CONFIG_MODULE_DEBUGUSART_ENABLE)
#define BUTTON_DEBUG_PRINT(msg)		DebugUart_SendLine(msg)
#else
#define BUTTON_DEBUG_PRINT(msg)
#endif


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_FUNCTION_REMOTECONTROLLER)
typedef enum
{
	Car_BackForward_Stop,
	Car_BackForward_Fordward,
	Car_BackForward_Back
} Car_BackForward_t;


typedef enum
{
	Car_Turning_Straight,
	Car_Turning_Left,
	Car_Turning_Right
} Car_Turning_t;


#endif



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

void Logic_ButtonEventHandler(ButtonType_t button, ButtonPressType_t type);



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/



#endif /* LOGIC_H_ */
