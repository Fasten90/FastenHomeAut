/*
 *		Logic.c
 *		Created on:		2017-06-23
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Logical functions
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-06-23
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
#include "Display.h"
#include "CommandHandler.h"
#include "Communication.h"
#include "Calc.h"

#ifdef CONFIG_FUNCTION_CHARGER
#include "ADC.h"
#endif

#ifdef CONFIG_FUNCTION_GAME_SNAKE
#include "Snake.h"
#endif


#ifdef CONFIG_FUNCTION_REMOTECONTROLLER
	#if defined(CONFIG_MODULE_ESP8266_ENABLE)
	#define SEND_MESSAGE_TO_CAR(_msg)		ESP8266_RequestSendTcpMessage(_msg)
	#elif defined(CONFIG_MODULE_BLUETOOTH_ENABLE)
	#include "Bluetooth_HC05.h"
	#define SEND_MESSAGE_TO_CAR(_msg)		Bluetooth_SendMessage(_msg)
	#else
	#define SEND_MESSAGE_TO_CAR(_msg)		DebugUart_SendMessage(_msg)
	#endif
#endif


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_FUNCTION_CHARGER
bool Logic_BatteryIsCharging = false;
#endif


#ifdef CONFIG_FUNCTION_DISPLAY_MENU
static volatile bool Logic_Display_ChangedState = false;
static volatile DisplayMenu_t Logic_Display_ActualState = Menu_Main;
static volatile DisplayMenu_t Logic_Display_SelectedState = Menu_Main;


static volatile DisplaySnakeMenu_t Logic_Display_SnakeMenu_ActualState = SnakeMenu_NewGame;

bool Logic_Snake_DisplaySnakeMenu = false;
#endif



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK)
static DisplayClock_ChangeState_t Logic_SystemTimeConfigState = 0;
#endif

#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
uint8_t DisplayInput_LetterPosition = 0;

// TODO: Refactor
#define DisplayInput_LetterPosition_MinLimit	0

#define DisplayInput_StringLimit				(DisplayInput_LetterPosition_MaxLimit + 1)

char DisplayInput_ActualRealString[DisplayInput_StringLimit] = { 0 };
static uint8_t DisplayInput_ActualString[DisplayInput_LetterPosition_MaxLimit] = { 0 };

static char const Display_Characters[] = { ' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
	't', 'u', 'v', 'w', 'x', 'y', 'z' };

static uint8_t const Display_Characters_size = sizeof(Display_Characters)/sizeof(Display_Characters[0]);
#endif

#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
static uint16_t Display_CarAnimation_RefreshPeriod_MinLimit = 100;
static uint16_t Display_CarAnimation_RefreshPeriod_MaxLimit = 1000;
static uint16_t Display_CarAnimation_RefreshPeriod_Actual = 300;
#endif



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_DISPLAY_ENABLE
static void Logic_Display_MainMenu(void);
static void Logic_Display_PrintMainMenuList(void);
#endif

#ifdef CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK
static void Logic_SystemTimeStepConfig(void);
static void Logic_SystemTimeStepValue(void);
#endif

#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
static void Logic_StepLetterPosition(int8_t step);
static void Logic_StepLetterNextValue(int8_t step);
#endif

#ifdef CONFIG_FUNCTION_GAME_SNAKE
static void Logic_Display_Snake(void);
static void Logic_Display_PrintSnakeMenuList(void);
#endif

#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
static void Logic_Display_Input(ScheduleSource_t source);
#endif

#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
static void Logic_Display_CarAnimation(void);
#endif

#ifdef CONFIG_FUNCTION_REMOTECONTROLLER
static void Logic_RemoteController_Button(ButtonType_t button, ButtonPressType_t type);
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_FUNCTION_DISPLAY_MENU
void Logic_Display_Init(void)
{
	Display_Clear();

	switch (Logic_Display_ActualState)
	{
		case Menu_Main:
			Logic_Display_PrintMainMenuList();
			break;

		#ifdef CONFIG_FUNCTION_GAME_SNAKE
		case Menu_Snake:
			if (!Logic_Snake_DisplaySnakeMenu)
				Snake_Init();
			else
				Logic_Display_PrintSnakeMenuList();
			break;
		#endif

		#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
		case Menu_Car:
			// Display start screen
			Display_LoadCarImage();
			break;
		#endif

		#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
		case Menu_Input:
			// Fill real string with empty character
			memset(DisplayInput_ActualRealString, ' ', DisplayInput_StringLimit-1);
			// Last char not need fill with ' ', because it is end character

			// Create OK button
			// TODO: Create button?
			Display_PrintFont12x8('O', DisplayInput_LetterPosition_MaxLimit, 2, NO_FORMAT);
			Display_PrintFont12x8('K', DisplayInput_LetterPosition_MaxLimit + 1, 2, NO_FORMAT);
			break;
		#endif

		case Menu_Count:
		default:
			break;
	}

	Display_Activate();
}
#endif



#ifdef CONFIG_MODULE_BUTTON_ENABLE
/**
 * \brief	Button event handler
 * 			Only one button handling (button = i. button
 */
void Logic_ButtonEventHandler(ButtonType_t button, ButtonPressType_t type)
{
#if BUTTON_NUM == 4
	const char * buttonName = BUTTON_GetButtonName(button);
	const char * typeName = BUTTON_GetPressTypeName(type);

	uprintf("Button: %s pressed %s\r\n", buttonName, typeName);

#elif BUTTON_NUM == 1
	(void)button;
	(void)type;
	BUTTON_DEBUG_PRINT("Button pressed");
#endif

#if defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK) && (BUTTON_NUM == 1)
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
#elif defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK) && (BUTTON_NUM > 1)
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

#ifdef CONFIG_FUNCTION_REMOTECONTROLLER
	Logic_RemoteController_Button(button, type);
#endif

#ifdef CONFIG_MODULE_DISPLAY_ENABLE
	if (Logic_Display_ActualState == Menu_Main)
	{
		switch (button)
		{
			case PressedButton_Right:
			case PressedButton_Left:
				Logic_Display_ChangeState(Logic_Display_SelectedState);
				break;

			case PressedButton_Up:
				if (Logic_Display_SelectedState > 0)
					Logic_Display_SelectedState--;
				break;

			case PressedButton_Down:
				if (Logic_Display_SelectedState < Menu_Count-1)
					Logic_Display_SelectedState++;
				break;

			case PressedButton_Count:
			default:
				// Error!
				break;
		}

		TaskHandler_RequestTaskScheduling(Task_Display);
	}
#endif	// #ifdef CONFIG_MODULE_DISPLAY_ENABLE

#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
	// Check buttons
	if (Logic_Display_ActualState == Menu_Input)
	{
		if (type != ButtonPress_ReleasedContinuous)
		{
			switch (button)
			{
				case PressedButton_Right:
					// Right
					Logic_StepLetterPosition((type == ButtonPress_Short || type == ButtonPress_Continuous) ? 1 : 3);
					break;

				case PressedButton_Left:
					// Left
					Logic_StepLetterPosition((type == ButtonPress_Short || type == ButtonPress_Continuous) ? -1 : -3);
					break;

				case PressedButton_Up:
					// Up
					Logic_StepLetterNextValue((type == ButtonPress_Short || type == ButtonPress_Continuous) ? -1 : -3);
					break;

				case PressedButton_Down:
					// Down
					Logic_StepLetterNextValue((type == ButtonPress_Short || type == ButtonPress_Continuous) ? 1 : 3);
					break;

				case PressedButton_Count:
				default:
					// Error!
					break;
			}
		}
	}
#endif

#ifdef CONFIG_FUNCTION_GAME_SNAKE
	if (Logic_Display_ActualState == Menu_Snake)
	{
		if (!Logic_Snake_DisplaySnakeMenu)
		{
			// Check buttons at Snake game
			if (type != ButtonPress_ReleasedContinuous)
			{
				switch (button)
				{
					case PressedButton_Right:
						// Right
						Snake_Step(Step_Right);
						break;

					case PressedButton_Left:
						// Left
						Snake_Step(Step_Left);
						break;

					case PressedButton_Up:
						// Up
						Snake_Step(Step_Up);
						break;

					case PressedButton_Down:
						// Down
						Snake_Step(Step_Down);
						break;

					case PressedButton_Count:
					default:
						// Error!
						break;
				}
			}
		}
		else
		{
			// Check buttons at Snake menu
			switch (button)
			{
				case PressedButton_Right:
				case PressedButton_Left:
					if (Logic_Display_SnakeMenu_ActualState == SnakeMenu_NewGame)
						Logic_Display_ChangeState(Menu_Snake);
					else
						Logic_Display_ChangeState(Menu_Main);
					// Clear SnakeMenu status
					Logic_Snake_DisplaySnakeMenu = false;
					break;

				case PressedButton_Up:
					if (Logic_Display_SnakeMenu_ActualState > 0)
						Logic_Display_SnakeMenu_ActualState--;
					break;

				case PressedButton_Down:
					if (Logic_Display_SnakeMenu_ActualState < SnakeMenu_Count-1)
						Logic_Display_SnakeMenu_ActualState++;
					break;

				case PressedButton_Count:
				default:
					// Error!
					break;
			}

			TaskHandler_RequestTaskScheduling(Task_Display);
		}
	}
#endif
#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
	// Check buttons
	if (Logic_Display_ActualState == Menu_Car)
	{
		if (type != ButtonPress_ReleasedContinuous)
		{
			switch (button)
			{
				case PressedButton_Right:
					// Right
					if (Display_CarAnimation_RefreshPeriod_Actual <= Display_CarAnimation_RefreshPeriod_MaxLimit)
					{
						Display_CarAnimation_RefreshPeriod_Actual += Display_CarAnimation_RefreshPeriod_MinLimit;
						Logic_Display_CarAnimation();
					}
					break;

				case PressedButton_Left:
					// Left
					if (Display_CarAnimation_RefreshPeriod_Actual >= Display_CarAnimation_RefreshPeriod_MinLimit)
					{
						Display_CarAnimation_RefreshPeriod_Actual -= Display_CarAnimation_RefreshPeriod_MinLimit;
						Logic_Display_CarAnimation();
					}
					break;

				case PressedButton_Up:
				case PressedButton_Down:
					// Go to Main menu
					Logic_Display_ChangeState(Menu_Main);
					break;

				case PressedButton_Count:
				default:
					// Error!
					break;
			}
		}
	}
#endif

}
#endif


#if defined(CONFIG_FUNCTION_REMOTECONTROLLER)
static Car_DcForward_t Car_BackForwardState = Car_DcForward_Stop;
static Car_Turning_t Car_TurningState = Car_Turning_Straight;

static int8_t Car_DcForward_ActualValue = 0;
static int8_t Car_Turning_ActualValue = 0;

static int8_t Car_DcForward_PreviousValue = 0;
static int8_t Car_Tuning_PreviousValue = 0;

static const int8_t Car_DcForward_StopValue = 0;
static const int8_t Car_Turning_StraightValue = 0;

static const int8_t Car_DcForward_IncrementValue = 5;
static const int8_t Car_Turning_IncrementValue = 5;

static const int8_t Car_DcForward_FordwardStandardValue = 30;
static const int8_t Car_DcForward_BackStandardValue = -30;

static const int8_t Car_DcForward_ForwardMaxValue = 60;
static const int8_t Car_Turning_MaxValue = 30;
static const int8_t Car_DcForward_ForwardMinValue = -60;
static const int8_t Car_Turning_MinValue = -30;


static void Logic_RemoteController_Button(ButtonType_t button, ButtonPressType_t type)
{
	(void)type;

	// Check buttons
	if (button == PressedButton_Right)
	{
		// Right

		if (Car_TurningState == Car_Turning_Right)
		{
			// Larger right
			Car_Turning_ActualValue -= Car_Turning_IncrementValue;
			// Check limit
			if (Car_Turning_ActualValue < Car_Turning_MinValue)
			{
				Car_Turning_ActualValue = Car_Turning_MinValue;
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
			Car_Turning_ActualValue += Car_Turning_IncrementValue;
			// Check limit
			if (Car_Turning_ActualValue > Car_Turning_MaxValue)
			{
				Car_Turning_ActualValue = Car_Turning_MaxValue;
			}
		}
		else if (Car_TurningState == Car_Turning_Straight)
		{
			// Start left turning
			Car_Turning_ActualValue = Car_Turning_StraightValue + Car_Turning_IncrementValue;
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

		if (Car_BackForwardState == Car_DcForward_Fordward)
		{
			// Larger speed
			Car_DcForward_ActualValue += Car_DcForward_IncrementValue;
			// Check limit
			if (Car_DcForward_ActualValue > Car_DcForward_ForwardMaxValue)
			{
				Car_DcForward_ActualValue = Car_DcForward_ForwardMaxValue;
			}
		}
		else if (Car_BackForwardState == Car_DcForward_Stop)
		{
			// Start go
			Car_DcForward_ActualValue = Car_DcForward_FordwardStandardValue;
			Car_BackForwardState = Car_DcForward_Fordward;
		}
		else
		{
			// Stop
			Car_DcForward_ActualValue = Car_DcForward_StopValue;
			Car_BackForwardState = Car_DcForward_Stop;
		}
	}
	else if (button == PressedButton_Down)
	{
		// Down
		if (Car_BackForwardState == Car_DcForward_Back)
		{
			// Larger speed
			Car_DcForward_ActualValue -= Car_DcForward_IncrementValue;
			// Check limit
			if (Car_DcForward_ActualValue < Car_DcForward_ForwardMinValue)
			{
				Car_DcForward_ActualValue = Car_DcForward_ForwardMinValue;
			}
		}
		else if (Car_BackForwardState == Car_DcForward_Stop)
		{
			// Start go
			Car_DcForward_ActualValue = Car_DcForward_BackStandardValue;
			Car_BackForwardState = Car_DcForward_Back;
		}
		else
		{
			// Stop
			Car_DcForward_ActualValue = Car_DcForward_StopValue;
			Car_BackForwardState = Car_DcForward_Stop;
		}
	}


	bool isChanged = false;
	// Check, need send command?
	if (Car_DcForward_PreviousValue != Car_DcForward_ActualValue)
	{
		// Changed
		DebugUart_Printf("Changed BackFordward value: previous: %d, now: %d\r\n",
				Car_DcForward_PreviousValue, Car_DcForward_ActualValue);
		Car_DcForward_PreviousValue = Car_DcForward_ActualValue;

		isChanged = true;
	}

	if (Car_Tuning_PreviousValue != Car_Turning_ActualValue)
	{
		// Changed
		DebugUart_Printf("Changed Turning value: previous: %d, now: %d\r\n",
				Car_Tuning_PreviousValue, Car_Turning_ActualValue);
		Car_Tuning_PreviousValue = Car_Turning_ActualValue;

		isChanged = true;
	}

	if (isChanged)
		Logic_RemoteController_SendMessage();
}



void Logic_RemoteController_SendMessage(void)
{
	if (Bluetooth_GetSendEnable())
	{
		// Sending immediately

		// Send message
		/*char msg[30];
		usprintf(msg, "motor dc %d\r\n", Car_DcForward_ActualValue);
		SEND_MESSAGE_TO_CAR(msg);

		// Send message on WiFi
		char msg[30];
		usprintf(msg, "motor servo %d\r\n", Car_Turning_ActualValue);
		SEND_MESSAGE_TO_CAR(msg);
		*/
		char msg[40];
		usprintf(msg, "motor %d %d\r\n", Car_DcForward_ActualValue, Car_Turning_ActualValue);
		SEND_MESSAGE_TO_CAR(msg);

		// For periodical message
		TaskHandler_SetTaskPeriodicTime(Task_RemoteController, 300);
	}
	else
	{
		// Check after some ms...
		TaskHandler_SetTaskPeriodicTime(Task_RemoteController, 20);
	}
}
#endif	// CONFIG_FUNCTION_REMOTECONTROLLER



#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
/**
 * \brief	Step active letter selection to next (left-right)
 */
static void Logic_StepLetterPosition(int8_t step)
{
	DisplayInput_LetterPosition += step;
	if (step > 0 && (DisplayInput_LetterPosition > DisplayInput_LetterPosition_MaxLimit))
	{
		// Overflow
		DisplayInput_LetterPosition = DisplayInput_LetterPosition_MinLimit;
	}
	else if (step < 0 && (DisplayInput_LetterPosition > DisplayInput_LetterPosition_MaxLimit))
	{
		// Underflow
		DisplayInput_LetterPosition = DisplayInput_LetterPosition_MaxLimit;
	}

	TaskHandler_RequestTaskScheduling(Task_Display);
}



/**
 * \brief	Step Letter value to next (up-down)
 */
static void Logic_StepLetterNextValue(int8_t step)
{
	uint8_t selectedLetter = DisplayInput_ActualString[DisplayInput_LetterPosition];

	if (DisplayInput_LetterPosition == DisplayInput_LetterPosition_MaxLimit)
	{
		// At "OK" button --> Run command
		// TODO: CommPort --> Display
		char str[DisplayInput_StringLimit];
		StrCpyMax(str, DisplayInput_ActualRealString, DisplayInput_StringLimit);
		StrTrim(str);
		CommandHandler_PrepareFindExecuteCommand(CommProt_DebugUart, str);
		uprintf("Typed command: %s", str);
		// Now, automatically exit to main menu
		Logic_Display_ChangeState(Menu_Main);
	}
	else
	{
		// Not "OK" button
		if (step < 0 && selectedLetter > 0 && (int8_t)selectedLetter-step > 0)
		{
			// Can go "down"
			// += (- value) => -=
			selectedLetter += step;
		}
		else if (step < 0)
		{
			// "Underflow"
			selectedLetter = Display_Characters_size - 1;
		}
		else if (step > 0 && (uint8_t)(selectedLetter+step) < sizeof(Display_Characters)/sizeof(Display_Characters[0]))
		{
			// Can go "up"
			selectedLetter += step;
		}
		else if (step > 0)
		{
			// "Overflow"
			selectedLetter = 0;
		}

		// If change letter
		// Save actual letter
		DisplayInput_ActualString[DisplayInput_LetterPosition] = selectedLetter;

		// Convert to realstring
		DisplayInput_ActualRealString[DisplayInput_LetterPosition] = Display_Characters[selectedLetter];
	}

	// Refresh display
	TaskHandler_RequestTaskScheduling(Task_Display);
}
#endif



#if defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK)
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
	// TODO: Write IP address to Display
#warning "Implement this!"
}
#endif



#ifdef CONFIG_FUNCTION_CHARGER
void Logic_CheckCharger(void)
{
	InputState_t chargeState = IO_GetInputState(Input_BatteryCharger);
	bool isCharging = false;

	// Check actual state
	switch (chargeState)
	{
		case InputState_Active:
			// Battery is charging...
			isCharging = true;
			break;

		case InputState_Inactive:
			// Battery is not charging
			isCharging = false;
			break;

		case InputState_Unknown:
		case InputState_Count:
		default:
			uprintf("Battery charge state error!\r\n");
			//break;
			return;
			// Do not continue to execute, because input state is not correct!
	}


	if (Logic_BatteryIsCharging != isCharging)
	{
		// Charge state is changed!
		Logic_BatteryIsCharging = isCharging;

		// Require Display task run
		TaskHandler_RequestTaskScheduling(Task_Display);

		// Print actual state
		if (isCharging)
			uprintf("Battery is charging...\r\n");
		else
			uprintf("Battery is not charging...\r\n");
	}


}
#endif



#ifdef CONFIG_MODULE_DISPLAY_ENABLE
void Logic_DisplayHandler(ScheduleSource_t source)
{
#ifndef CONFIG_FUNCTION_DISPLAY_INPUT
	UNUSED(source);
#endif

	if (Logic_Display_ChangedState)
	{
		Logic_Display_ChangedState = false;
		Logic_Display_Init();
	}

	switch (Logic_Display_ActualState)
	{
		case Menu_Main:
			Logic_Display_MainMenu();
			break;

#ifdef CONFIG_FUNCTION_GAME_SNAKE
		case Menu_Snake:
			if (!Logic_Snake_DisplaySnakeMenu)
				Logic_Display_Snake();
			else
				Logic_Display_PrintSnakeMenuList();
			break;
#endif
#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
		case Menu_Input:
			Logic_Display_Input(source);
			break;
#endif
#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
		case Menu_Car:
			Logic_Display_CarAnimation();
			break;
#endif
		case Menu_Count:
		default:
			Logic_Display_MainMenu();
			break;
	}
}



static void Logic_Display_MainMenu(void)
{
	// Main menu
	#ifdef CONFIG_FUNCTION_CHARGER
	// Loading image
	static uint8_t loadPercent = 0;

	if (Logic_BatteryIsCharging)
	{
		// Loading (progress bar)
		Display_ChargeLoading(loadPercent);
		loadPercent += 5;

		if (loadPercent >= 100)
		{
			loadPercent = 0;
		}
		// Require periodical schedule
		TaskHandler_SetTaskOnceRun(Task_Display, 1000);
	}
	else
	{
		// Not charging:
		// Display actual voltage of battery
		#ifdef CONFIG_MODULE_ADC_ENABLE
		float voltage = ADC_GetValue(ADC_Vsource);
		uint8_t percent = voltage / VSOURCE_BATTERY_MAX_VOLTAGE * 100;
		if (percent > 100)
			percent = 100;
		Display_ChargeLoading(percent);
		#else
		Display_ChargeLoading(34);
		#endif
	}
	#endif

	#ifdef CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK
	// Display refresh by clock

	// Display vibrate function: if we are in setting mode, hour or minute will vibrate
	static bool Display_VibrateStateHide = false;

	if (source == ScheduleSource_EventTriggered)
		Display_VibrateStateHide = false;


	switch (Logic_GetSystemTimeState())
	{
		case DisplayClock_Hour:
			// Hour setting
			if (Display_VibrateStateHide)
			{
				Display_ShowClockHalf(&DateTime_SystemTime.time, DisplayClock_Minute);
				Display_VibrateStateHide = false;
			}
			else
			{
				Display_ShowClock(&DateTime_SystemTime.time);
				Display_VibrateStateHide = true;
			}
			TaskHandler_SetTaskOnceRun(Task_Display, 500);
			break;

		case DisplayClock_Minute:
			// Minute settings
			if (Display_VibrateStateHide)
			{
				Display_ShowClockHalf(&DateTime_SystemTime.time, DisplayClock_Hour);
				Display_VibrateStateHide = false;
			}
			else
			{
				Display_ShowClock(&DateTime_SystemTime.time);
				Display_VibrateStateHide = true;
			}
			TaskHandler_SetTaskOnceRun(Task_Display, 500);
			break;

		case DisplayClock_HourAndMinute:
		case DisplayClock_Count:
		default:
			// Not in setting
			Display_ShowClock(&DateTime_SystemTime.time);
			TaskHandler_DisableTask(Task_Display);
			break;
	}
	#elif defined(CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK) && defined(CONFIG_DISPLAY_CLOCK_SMALL)
	// Only show clock (small - on menu)
	Display_ShowSmallClock(&DateTime_SystemTime.time);
	TaskHandler_DisableTask(Task_Display);
	#endif

	// Display main menu list
	Logic_Display_PrintMainMenuList();
	Display_Activate();
}



static void Logic_Display_PrintMainMenuList(void)
{
	// Print menu
#if defined(CONFIG_FUNCTION_GAME_SNAKE) || defined(CONFIG_FUNCTION_DISPLAY_INPUT) || defined(CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN)
	uint8_t i = 2;
#endif

	// TODO: Do with smaller text

	FontFormat_t format = { 0 };
	format.Format_Inverse = 1;

#ifdef CONFIG_FUNCTION_GAME_SNAKE
	Display_PrintString("Snake", i, Font_12x8,
			Logic_Display_SelectedState == Menu_Snake ? format : NO_FORMAT);
	i++;
#endif
#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
	Display_PrintString("Input", i, Font_12x8,
			Logic_Display_SelectedState == Menu_Input ? format : NO_FORMAT);
	i++;
#endif
#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
	Display_PrintString("Car animation", i, Font_12x8,
			Logic_Display_SelectedState == Menu_Car ? format : NO_FORMAT);
	i++;
#endif
}



#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
static void Logic_Display_Input(ScheduleSource_t source)
{
	static bool Display_VibrateLetter = false;

	static uint8_t DisplayInput_OldLetterPosition = 0;
	if ((DisplayInput_OldLetterPosition != DisplayInput_LetterPosition)
		&& (DisplayInput_OldLetterPosition == DisplayInput_LetterPosition_MaxLimit))
	{
		// OK
		Display_PrintFont12x8('O', DisplayInput_LetterPosition_MaxLimit, 2, NO_FORMAT);
		Display_PrintFont12x8('K', DisplayInput_LetterPosition_MaxLimit + 1, 2, NO_FORMAT);
	}
	// Save old value
	DisplayInput_OldLetterPosition = DisplayInput_LetterPosition;

	// Button clicked
	if (source == ScheduleSource_EventTriggered)
	{
		// Button press triggering
		Display_VibrateLetter = false;

		// Display "all" string
		Display_PrintString(DisplayInput_ActualRealString, 2, Font_12x8, NO_FORMAT);

		// It is empty char? (=space)
		if (DisplayInput_ActualRealString[DisplayInput_LetterPosition] == ' ')
		{
			// There is a space character, Display a white box
			Display_PrintFont12x8((char)0x01, DisplayInput_LetterPosition, 2, NO_FORMAT);
		}

		Display_Activate();
		TaskHandler_SetTaskOnceRun(Task_Display, 500);
	}
	else
	{
		// Vibration, if need (periodical)
		if (Display_VibrateLetter)
		{
			// Vibrate (not show char)
			if (DisplayInput_LetterPosition == DisplayInput_LetterPosition_MaxLimit)
			{
				// TODO: Button printing
				// OK
				Display_PrintFont12x8('O', DisplayInput_LetterPosition_MaxLimit, 2, NO_FORMAT);
				Display_PrintFont12x8('K', DisplayInput_LetterPosition_MaxLimit + 1, 2, NO_FORMAT);
			}
			else
			{
				// Normal char
				// It is empty char? (=space)
				if (DisplayInput_ActualRealString[DisplayInput_LetterPosition] == ' ')
				{
					// There is a space character, Display a white box
					Display_PrintFont12x8((char)0x01, DisplayInput_LetterPosition, 2, NO_FORMAT);
				}
				else
				{
					// There is a normal character... vibrate with hiding
					Display_PrintFont12x8(' ', DisplayInput_LetterPosition, 2, NO_FORMAT);
				}
			}

			Display_Activate();
			Display_VibrateLetter = false;
			TaskHandler_SetTaskOnceRun(Task_Display, 500);
		}
		else
		{
			// Normal (show char)
			if (DisplayInput_LetterPosition == DisplayInput_LetterPosition_MaxLimit)
			{
				// OK
				FontFormat_t format = { 0 };
				format.Format_Inverse = 1;
				Display_PrintFont12x8('O', DisplayInput_LetterPosition_MaxLimit, 2, format);
				Display_PrintFont12x8('K', DisplayInput_LetterPosition_MaxLimit + 1, 2, format);
			}
			else
			{
				// Normal char
				Display_PrintFont12x8(
					DisplayInput_ActualRealString[DisplayInput_LetterPosition],
					DisplayInput_LetterPosition, 2, NO_FORMAT);
			}

			Display_Activate();
			Display_VibrateLetter = true;
			TaskHandler_SetTaskOnceRun(Task_Display, 500);
		}
	}
}
#endif	// #ifdef CONFIG_FUNCTION_DISPLAY_INPUT



#ifdef CONFIG_FUNCTION_GAME_SNAKE
static void Logic_Display_Snake(void)
{
	static SnakeStep_t SnakeStep = Step_Unknown;

	// Periodical stepping
	SnakeStep = Snake_GetLastStep();

	Snake_Step(SnakeStep);

	TaskHandler_SetTaskOnceRun(Task_Display, 500);
}



static void Logic_Display_PrintSnakeMenuList(void)
{
	FontFormat_t format = { 0 };
	format.Format_Inverse = 1;

	Display_PrintString("New game", 3, Font_12x8,
			(Logic_Display_SnakeMenu_ActualState == SnakeMenu_NewGame) ? format : NO_FORMAT);
	Display_PrintString("Exit", 4, Font_12x8,
			(Logic_Display_SnakeMenu_ActualState == SnakeMenu_Exit) ? format : NO_FORMAT);

	Display_Activate();
}



inline void Logic_Display_Snake_ChangeToMenu(void)
{
	Logic_Snake_DisplaySnakeMenu = true;
}
#endif	// #ifdef CONFIG_FUNCTION_GAME_SNAKE



#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
static void Logic_Display_CarAnimation(void)
{
	// Car image
	// TODO: Optimize this?
	Display_ChangeCarImage();
	TaskHandler_SetTaskOnceRun(Task_Display, Display_CarAnimation_RefreshPeriod_Actual);
}
#endif



void Logic_Display_ChangeState(DisplayMenu_t nextState)
{
	Logic_Display_ChangedState = true;
	Logic_Display_ActualState = nextState;
	TaskHandler_RequestTaskScheduling(Task_Display);
}



bool Logic_Display_GetClockIsNeedRefresh(void)
{
	return (Logic_Display_ActualState == Menu_Main ? true : false);
}
#endif	// #ifdef CONFIG_MODULE_DISPLAY_ENABLE
