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



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_BUTTON_DEBUG_ENABLE
#define BUTTON_DEBUG_PRINT(msg)		DebugUart_SendLine(msg)
#else
#define BUTTON_DEBUG_PRINT(msg)
#endif


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

void Logic_ButtonEventHandler(ButtonType_t button, ButtonPressType_t type);



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/



#endif /* LOGIC_H_ */
