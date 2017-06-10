/*
 *		Logic.c
 *
 *		Created on:		2017. máj. 23.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. máj. 23.
 */




/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "DateTime.h"
#include "Button.h"
#include "TaskList.h"
#include "TaskHandler.h"
#include "Logic.h"


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_MODULE_DISPLAY_SHOW_CLOCK) && defined(CONFIG_MODULE_BUTTON_ENABLE)
static uint8_t Logic_SystemTimeConfigState = 0;
#endif



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_MODULE_DISPLAY_SHOW_CLOCK) && defined(CONFIG_MODULE_BUTTON_ENABLE)
static void Logic_SystemTimeStepConfig(void);
static void Logic_SystemTimeStepValue(void);
#endif


/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * \brief	Button event handler
 * 			Only one button handling (button = i. button
 */
void Logic_ButtonEventHandler(ButtonType_t button, ButtonPressType_t type)
{
#if defined(CONFIG_MODULE_DISPLAY_SHOW_CLOCK) && (BUTTON_NUM == 1)
	// One button mode
	if (button == PressedButton_User)
	{
		if (type == ButtonPress_Long)
		{
			BUTTON_DEBUG_PRINT("Button pressed a long time");
			Logic_SystemTimeStepConfig();
		}
		else if (type == ButtonPress_Short)
		{
			BUTTON_DEBUG_PRINT("Button pressed a short time");
			Logic_SystemTimeStepValue();
		}
	}
#elif defined(CONFIG_MODULE_DISPLAY_SHOW_CLOCK) && (BUTTON_NUM > 1)
	// More button mode
	// TODO: Up-Down / Right-Up difference...
	if ((button == PressedButton_Right) || (button == PressedButton_Left))
	{
		BUTTON_DEBUG_PRINT("Left-Right button pressed");
		Logic_SystemTimeStepConfig();
	}
	else if ((button == PressedButton_Up) || (button == PressedButton_Down))
	{
		BUTTON_DEBUG_PRINT("Up-Down button pressed");
		Logic_SystemTimeStepValue();
	}
#endif
}



#if defined(CONFIG_MODULE_DISPLAY_SHOW_CLOCK) && defined(CONFIG_MODULE_BUTTON_ENABLE)
/**
 * \brief	SystemTime - step function
 */
static void Logic_SystemTimeStepConfig(void)
{
	if (Logic_SystemTimeConfigState < 2)
		Logic_SystemTimeConfigState++;
	else
		Logic_SystemTimeConfigState = 0;
}



/**
 * \brief	SystemTime - increment selected value (hour, minute, or none)
 */
void Logic_SystemTimeStepValue(void)
{
	switch (Logic_SystemTimeConfigState)
	{
		case 0:
			// Unknown
			break;

		case 1:
			// Hour
			DateTime_Steps(&DateTime_SystemTime, 60*60);
			TaskHandler_RequestTaskScheduling(Task_SystemTime);
			break;

		case 2:
			// Minute
			DateTime_Steps(&DateTime_SystemTime, 60);
			TaskHandler_RequestTaskScheduling(Task_SystemTime);
			break;

		default:
			break;
	}
}
#endif	// #ifdef CONFIG_MODULE_DISPLAY_SHOW_CLOCK
