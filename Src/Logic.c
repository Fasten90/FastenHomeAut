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
#include "IO.h"
#include "Logic.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_FUNCTION_CHANGE_DISPLAY_CLOCK)
static DisplayClock_ChangeState_t Logic_SystemTimeConfigState = 0;
#endif

#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
uint8_t DisplayInput_LetterPosition = 0;

// TODO: Refactor
#define DisplayInput_LetterPosition_MaxLimit	10
#define DisplayInput_LetterPosition_MinLimit	0

char DisplayInput_ActualRealString[DisplayInput_LetterPosition_MaxLimit+1] = { ' ' };
static uint8_t DisplayInput_ActualString[DisplayInput_LetterPosition_MaxLimit] = { 0 };

static char const Display_Characters[] = { ' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
	't', 'u', 'v', 'w', 'x', 'y', 'z' };

static uint8_t const Display_Characters_size = sizeof(Display_Characters)/sizeof(Display_Characters[0]);
#endif



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_FUNCTION_CHANGE_DISPLAY_CLOCK
static void Logic_SystemTimeStepConfig(void);
static void Logic_SystemTimeStepValue(void);
#endif

#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
static void Logic_StepLetterPosition(int8_t step);
static void Logic_StepLetterNextValue(int8_t step);
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

	switch (button)
	{
		case PressedButton_Right:
			BUTTON_DEBUG_PRINT("Right button pressed");
			break;

		case PressedButton_Left:
			BUTTON_DEBUG_PRINT("Left button pressed");
			break;

		case PressedButton_Up:
			BUTTON_DEBUG_PRINT("Up button pressed");
			break;

		case PressedButton_Down:
			BUTTON_DEBUG_PRINT("Down button pressed");
			break;

		case PressedButton_Count:
		default:
			break;
	}

#if defined(CONFIG_FUNCTION_CHANGE_DISPLAY_CLOCK) && (BUTTON_NUM == 1)
	// One button mode
	(void)type;
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
	(void)type;
	// TODO: Up-Down / Right-Up difference...
	if ((button == PressedButton_Right) || (button == PressedButton_Left))
	{
		Logic_SystemTimeStepConfig();
	}
	else if ((button == PressedButton_Up) || (button == PressedButton_Down))
	{
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


	// Check, need send command?
	if (Car_BackForward_PreviousValue != Car_BackForward_ActualValue)
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

#endif	// CONFIG_FUNCTION_REMOTECONTROLLER

#ifdef CONFIG_FUNCTION_DISPLAY_INPUT


	// Check buttons
	switch (button)
	{
		case PressedButton_Right:
			// Right
			Logic_StepLetterPosition((type == ButtonPress_Short) ? 1 : 5);
			break;

		case PressedButton_Left:
			// Left
			Logic_StepLetterPosition((type == ButtonPress_Short) ? -1 : -5);
			break;

		case PressedButton_Up:
			// Up
			Logic_StepLetterNextValue((type == ButtonPress_Short) ? -1 : -5);
			break;

		case PressedButton_Down:
			// Down
			Logic_StepLetterNextValue((type == ButtonPress_Short) ? 1 : 5);
			break;

		case PressedButton_Count:
		default:
			// Error!
			break;
	}

#endif
}


#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
/**
 * \brief	Step active letter selection to next (left-right)
 */
static void Logic_StepLetterPosition(int8_t step)
{
	DisplayInput_LetterPosition += step;
	if (DisplayInput_LetterPosition > DisplayInput_LetterPosition_MaxLimit)
	{
		DisplayInput_LetterPosition = DisplayInput_LetterPosition_MinLimit;
	}
	// We dont need handle "value < min", because it will step to last letter

	TaskHandler_RequestTaskScheduling(Task_Display);
}



/**
 * \brief	Step Letter value to next (up-down)
 */
static void Logic_StepLetterNextValue(int8_t step)
{
	uint8_t selectedLetter = DisplayInput_ActualString[DisplayInput_LetterPosition];

	if (step < 0 && selectedLetter > 0 && (int8_t)selectedLetter-step > 0)
	{
		// Can go "down"
		selectedLetter--;
	}
	else if (step < 0)
	{
		// "Underflow"
		selectedLetter = Display_Characters_size - 1;
	}
	else if (step > 0 && (uint8_t)(selectedLetter+step) < sizeof(Display_Characters)/sizeof(Display_Characters[0]))
	{
		// Can go "up"
		selectedLetter++;
	}
	else if (step > 0)
	{
		// "Overflow"
		selectedLetter = 0;
	}

	// Save actual letter
	DisplayInput_ActualString[DisplayInput_LetterPosition] = selectedLetter;

	// Convert to realstring
	DisplayInput_ActualRealString[DisplayInput_LetterPosition] = Display_Characters[selectedLetter];

	// Refresh display
	TaskHandler_RequestTaskScheduling(Task_Display);
}
#endif



#if defined(CONFIG_FUNCTION_CHANGE_DISPLAY_CLOCK)
/**
 * \brief	SystemTime - step function
 */
static void Logic_SystemTimeStepConfig(void)
{
	// Change "setting" column
	TaskHandler_RequestTaskScheduling(Task_Display);

	Logic_SystemTimeConfigState++;
	if (Logic_SystemTimeConfigState >= DisplayClock_Count)
		Logic_SystemTimeConfigState = 0;
}



/**
 * \brief	SystemTime - increment selected value (hour, minute, or none)
 */
static void Logic_SystemTimeStepValue(void)
{
	switch (Logic_SystemTimeConfigState)
	{
		case DisplayClock_HourAndMinute:
			// Unknown
			break;

		case DisplayClock_Hour:
			// Hour
			DateTime_Steps(&DateTime_SystemTime, 60*60);
			TaskHandler_RequestTaskScheduling(Task_Display);
			break;

		case DisplayClock_Minute:
			// Minute
			DateTime_Steps(&DateTime_SystemTime, 60);
			TaskHandler_RequestTaskScheduling(Task_Display);
			break;

		case DisplayClock_Count:
		default:
			// Error !
			Logic_SystemTimeConfigState = 0;
			break;
	}
}



/**
 * \brief	Get system time (settings) state
 */
DisplayClock_ChangeState_t Logic_GetSystemTimeState(void)
{
	return Logic_SystemTimeConfigState;
}
#endif	// #ifdef CONFIG_MODULE_DISPLAY_SHOW_CLOCK



#ifdef CONFIG_FUNCTION_ESP8266_WRITE_IP_TO_DISPLAY
void Logic_WriteIpToDisplay(void)
{
	// TODO:
#warning "Implement this!"
}
#endif



#ifdef CONFIG_FUNCTION_CHARGER
void Logic_CheckCharger(void)
{
	if (IO_GetInputState(Input_BatteryCharger) == InputState_Active)
	{
		// Battery is charging...
		uprintf("Battery is charging...\r\n");
	}
	else
	{
		uprintf("Battery is not charging...\r\n");
	}
}
#endif
