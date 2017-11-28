/*
 *		Logic.h
 *		Created on:		2017-06-23
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Logical functions
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-06-23
 */

#ifndef LOGIC_H_
#define LOGIC_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "Button.h"
#include "TaskHandler.h"

#ifdef CONFIG_BUTTON_DEBUG_ENABLE
#include "DebugUart.h"
#include "Debug.h"
#endif



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_BUTTON_DEBUG_ENABLE) && defined(CONFIG_MODULE_DEBUG_ENABLE)
#define BUTTON_DEBUG_PRINT(...)		Debug_Print(Debug_Button, __VA_ARGS__)
#elif defined(CONFIG_BUTTON_DEBUG_ENABLE) && defined(CONFIG_MODULE_DEBUGUART_ENABLE)
#define BUTTON_DEBUG_PRINT(...)		uprintf(__VA_ARGS__)
#else
#define BUTTON_DEBUG_PRINT(...)
#endif



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_FUNCTION_REMOTECONTROLLER)
typedef enum
{
	Car_DcForward_Stop,
	Car_DcForward_Fordward,
	Car_DcForward_Back
} Car_DcForward_t;


typedef enum
{
	Car_Turning_Straight,
	Car_Turning_Left,
	Car_Turning_Right
} Car_Turning_t;
#endif



#if defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK)
typedef enum
{
	DisplayClock_HourAndMinute,
	DisplayClock_Hour,
	DisplayClock_Minute,

	// Do not use!
	DisplayClock_Count
} DisplayClock_ChangeState_t;
#endif



typedef enum
{
	Menu_Main,
#ifdef CONFIG_FUNCTION_GAME_SNAKE
	Menu_Snake,
#endif
#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
	Menu_Input,
#endif
#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
	Menu_Car,
#endif
#ifdef CONFIG_DISPLAY_CLOCK_LARGE
	Menu_LargeClock,
#endif

	// XXX: Syncronize with Logic_MenuList

	// Count
	Menu_Count
} DisplayMenu_t;


typedef enum
{
	SnakeMenu_NewGame,
	SnakeMenu_Exit,

	SnakeMenu_Count
} DisplaySnakeMenu_t;


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Logic_Display_Init(void);

void Logic_ButtonEventHandler(ButtonType_t button, ButtonPressType_t type);

#if defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK)
DisplayClock_ChangeState_t Logic_GetSystemTimeState(void);
#endif

void Logic_CheckCharger(void);

void Logic_DisplayHandler(ScheduleSource_t source);

void Logic_Display_ChangeState(DisplayMenu_t nextState);
bool Logic_Display_GetClockIsNeedRefresh(void);
void Logic_Display_Snake_ChangeToMenu(void);

void Logic_RemoteController_SendMessage(void);

void Logic_SetPeriodicalMessageSendg(char * msg);
void Logic_PeriodicalSending(void);



#endif /* LOGIC_H_ */
