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

#include "ESP8266.h"
#include "DateTime.h"
#include "Button.h"
#include "TaskList.h"
#include "TaskHandler.h"
#include "String.h"
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
#if defined(CONFIG_FUNCTION_CHANGE_DISPLAY_CLOCK) && (BUTTON_NUM == 1)
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
#elif defined(CONFIG_FUNCTION_CHANGE_DISPLAY_CLOCK) && (BUTTON_NUM > 1)
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

#if defined(CONFIG_FUNCTION_REMOTECONTROLLER)
	static Car_BackForward_t Car_BackForwardState = Car_BackForward_Stop;
	static Car_Turning_t Car_TurningState = Car_Turning_Straight;

	static int8_t Car_BackForward_ActualValue = 0;
	static int8_t Car_Turning_ActualValue = 0;

	static int8_t Car_BackForward_PreviousValue = 0;
	static int8_t Car_Tuning_PreviousValue = 0;

	static const int8_t Car_BackFordward_StopValue = 0;
	static const int8_t Car_Turning_StraightValue = 0;

	static const int8_t Car_BackForward_IncrementValue = 5;
	static const int8_t Car_Turning_IncrementValue = 5;

	static const int8_t Car_BackForward_FordwardStandardValue = 30;
	static const int8_t Car_BackForward_BackStandardValue = -30;

	static const int8_t Car_BackFordward_ForwardMaxValue = 60;
	static const int8_t Car_Turning_MaxValue = 30;
	static const int8_t Car_BackFordward_ForwardMinValue = -60;
	static const int8_t Car_Turning_MinValue = -30;


	// Check buttons
	if (button == PressedButton_Right)
	{
		// Right
		BUTTON_DEBUG_PRINT("Right button pressed");

		if (Car_TurningState == Car_Turning_Right)
		{
			// Larger right
			Car_Turning_ActualValue += Car_Turning_IncrementValue;
			// Check limit
			if (Car_Turning_ActualValue > Car_Turning_MaxValue)
			{
				Car_Turning_ActualValue = Car_Turning_MaxValue;
			}
		}
		else if (Car_TurningState == Car_Turning_Straight)
		{
			// Start right turning
			Car_Turning_ActualValue = Car_Turning_StraightValue + Car_Turning_IncrementValue;
			Car_TurningState = Car_Turning_Right;
		}
		else
		{
			// Straight
			Car_Turning_ActualValue = Car_Turning_StraightValue;
			Car_TurningState = Car_Turning_Straight;
		}
	}
	else if (button == PressedButton_Left)
	{
		// Left
		BUTTON_DEBUG_PRINT("Left button pressed");

		if (Car_TurningState == Car_Turning_Left)
		{
			// Larger left
			Car_Turning_ActualValue -= Car_Turning_IncrementValue;
			// Check limit
			if (Car_Turning_ActualValue < Car_Turning_MinValue)
			{
				Car_Turning_ActualValue = Car_Turning_MinValue;
			}
		}
		else if (Car_TurningState == Car_Turning_Straight)
		{
			// Start left turning
			Car_Turning_ActualValue = Car_Turning_StraightValue - Car_Turning_IncrementValue;
			Car_TurningState = Car_Turning_Left;
		}
		else
		{
			// Straight
			Car_Turning_ActualValue = Car_Turning_StraightValue;
			Car_TurningState = Car_Turning_Straight;
		}
	}
	else if (button == PressedButton_Up)
	{
		// Up
		BUTTON_DEBUG_PRINT("Up button pressed");

		if (Car_BackForwardState == Car_BackForward_Fordward)
		{
			// Larger speed
			Car_BackForward_ActualValue += Car_BackForward_IncrementValue;
			// Check limit
			if (Car_BackForward_ActualValue > Car_BackFordward_ForwardMaxValue)
			{
				Car_BackForward_ActualValue = Car_BackFordward_ForwardMaxValue;
			}
		}
		else if (Car_BackForwardState == Car_BackForward_Stop)
		{
			// Start go
			Car_BackForward_ActualValue = Car_BackForward_FordwardStandardValue;
			Car_BackForwardState = Car_BackForward_Fordward;
		}
		else
		{
			// Stop
			Car_BackForward_ActualValue = Car_BackFordward_StopValue;
			Car_BackForwardState = Car_BackForward_Stop;
		}
	}
	else if (button == PressedButton_Down)
	{
		// Down
		BUTTON_DEBUG_PRINT("Down button pressed");

		if (Car_BackForwardState == Car_BackForward_Back)
		{
			// Larger speed
			Car_BackForward_ActualValue -= Car_BackForward_IncrementValue;
			// Check limit
			if (Car_BackForward_ActualValue < Car_BackFordward_ForwardMinValue)
			{
				Car_BackForward_ActualValue = Car_BackFordward_ForwardMinValue;
			}
		}
		else if (Car_BackForwardState == Car_BackForward_Stop)
		{
			// Start go
			Car_BackForward_ActualValue = Car_BackForward_BackStandardValue;
			Car_BackForwardState = Car_BackForward_Back;
		}
		else
		{
			// Stop
			Car_BackForward_ActualValue = Car_BackFordward_StopValue;
			Car_BackForwardState = Car_BackForward_Stop;
		}
	}


	if ( Car_BackForward_PreviousValue != Car_BackForward_ActualValue)
	{
		// Changed
		DebugUart_Printf("Changed BackFordward value: previous: %d, now: %d\r\n",
				Car_BackForward_PreviousValue, Car_BackForward_ActualValue);
		Car_BackForward_PreviousValue = Car_BackForward_ActualValue;

		// Send message on WiFi
		char msg[30];
		usprintf(msg, "motor dc %d", Car_BackForward_ActualValue);
		ESP8266_RequestSendTcpMessage(msg);
	}

	if (Car_Tuning_PreviousValue != Car_Turning_ActualValue)
	{
		// Changed
		DebugUart_Printf("Changed Turning value: previous: %d, now: %d\r\n",
				Car_Tuning_PreviousValue, Car_Turning_ActualValue);
		Car_Tuning_PreviousValue = Car_Turning_ActualValue;

		// Send message on WiFi
		char msg[30];
		usprintf(msg, "motor servo %d", Car_Turning_ActualValue);
		ESP8266_RequestSendTcpMessage(msg);
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
