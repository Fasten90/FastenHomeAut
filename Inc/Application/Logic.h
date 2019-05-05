/*
 *    Logic.h
 *    Created on:   2017-06-23
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Logical functions
 *    Target:       STM32Fx
 */

#ifndef LOGIC_H_
#define LOGIC_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "Button.h"
#include "TaskHandler.h"
#include "SmallApps.h"

#ifdef CONFIG_BUTTON_DEBUG_ENABLE
#include "DebugUart.h"
#include "Debug.h"
#endif



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_BUTTON_DEBUG_ENABLE) && defined(CONFIG_MODULE_DEBUG_ENABLE)
#define BUTTON_DEBUG_ON
#define BUTTON_DEBUG_PRINT(...)        Debug_Printf(Debug_Button, __VA_ARGS__)
#elif defined(CONFIG_BUTTON_DEBUG_ENABLE) && defined(CONFIG_MODULE_DEBUGUART_ENABLE)
#define BUTTON_DEBUG_ON
#define BUTTON_DEBUG_PRINT(...)        uprintf(__VA_ARGS__)
#elif defined(CONFIG_PLATFORM_X86) && !defined(CONFIG_MODULE_BUTTONSIMULATOR_AUTO_ON)
#define BUTTON_DEBUG_ON
#define BUTTON_DEBUG_PRINT(...)        printf(__VA_ARGS__)
#else
#define BUTTON_DEBUG_PRINT(...)
#endif



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< App Function pointers

typedef void (*AppInit_FunctionPointer_t)(void);

typedef void (*AppEvent_FunctionPointer_t)(ButtonType_t button, ButtonPressType_t type);

typedef void (*AppUpdate_FunctionPointer_t)(ScheduleSource_t source);


///< App list struct
typedef struct
{
    char_t * AppName;
    AppInit_FunctionPointer_t initFunction;
    AppEvent_FunctionPointer_t eventFunction;
    AppUpdate_FunctionPointer_t updateFunction;
} App_List_t;



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

bool Logic_Display_GetClockIsNeedRefresh(void);
void Logic_Display_Snake_ChangeToMenu(void);



#endif /* LOGIC_H_ */
