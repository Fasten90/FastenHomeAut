/*
 *    Logic.c
 *    Created on:   2017-06-23
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Logical functions
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "board.h"
#include "ESP8266.h"
#include "DateTime.h"
#include "Button.h"
#include "TaskList.h"
#include "TaskHandler.h"
#include "StringHelper.h"
#include "Logic.h"
#include "Display.h"
#include "CommandHandler.h"
#include "Communication.h"
#include "MathHelper.h"
#include "EventHandler.h"
#include "EventList.h"
#include "AppList.h"

#ifdef CONFIG_FUNCTION_CHARGER
#include "ADC.h"
#endif

#ifdef CONFIG_FUNCTION_GAME_SNAKE
#include "Snake.h"
#endif

#if defined(CONFIG_DISPLAY_CLOCK_SMALL) || defined(CONFIG_DISPLAY_CLOCK_LARGE)
#include "SysTime.h"
#endif


#ifdef CONFIG_FUNCTION_REMOTECONTROLLER
#include "RemoteController.h"
#endif



/*------------------------------------------------------------------------------
 *  Macros
 *----------------------------------------------------------------------------*/


#define DisplayMenu_ShowMenuLimit                   (3)

#if defined(CONFIG_HW_DISPLAY_ENABLE)
    #define DISPLAY_GENERATE_EVENT(__event_type, __data)    EventHandler_GenerateEvent(__event_type, __data, Task_Display)
#else
    #define DISPLAY_GENERATE_EVENT(__event_type, __data)
#endif



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const App_List_t AppList[];

extern const uint8_t AppList_Num;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_FUNCTION_CHARGER
static bool Logic_BatteryIsCharging = false;
#endif


#ifdef CONFIG_FUNCTION_DISPLAY_MENU
static volatile bool Logic_Display_ChangedState = false;

static volatile App_Type_t Logic_Display_ActualState = AppType_MainMenu;
static volatile App_Type_t Logic_Display_SelectedState = AppType_MainMenu;

static const uint8_t Logic_Display_MenuListLineOffset = 2;
#endif /* CONFIG_FUNCTION_DISPLAY_MENU */



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

///< This function used from another Apps, but because the App_Type_t, cannot externed at .h file
void Logic_Display_ChangeState(App_Type_t nextState);

#ifdef CONFIG_FUNCTION_DISPLAY_MENU
static void Logic_Display_MainMenu();
static void Logic_Display_PrintMainMenuList(void);
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_FUNCTION_DISPLAY_MENU
/**
 * @brief    "New submenu" initialization
 * @note    Do not call often, only at menu changing
 */
void Logic_Display_Init(void)
{
    Display_Clear();

    if (Logic_Display_ActualState == AppType_MainMenu)
    {
    #ifdef CONFIG_DISPLAY_CLOCK_SMALL
        DateTime_t dateTime;
        SysTime_GetDateTime(&dateTime);
        Display_ShowSmallClock(&dateTime.time);
    #endif
        Logic_Display_PrintMainMenuList();
    }
    else if (Logic_Display_ActualState < AppType_Count)
    {
        AppList[Logic_Display_ActualState-1].initFunction();
    }
    else
    {
        /* TODO: Error situation */
        Logic_Display_MainMenu();
    }

    Display_Activate();
}
#endif



#if defined(CONFIG_MODULE_BUTTON_ENABLE) || defined(CONFIG_MODULE_BUTTONSIMULATOR_ENABLE)
/**
 * @brief    Button event handler
 *             Only one button handling (button = i. button
 */
void Logic_ButtonEventHandler(ButtonType_t button, ButtonPressType_t type)
{
#if defined(BUTTON_DEBUG_ON)
  #if BUTTON_NUM == 4
    const char * buttonName = BUTTON_GetButtonName(button);
    const char * typeName = BUTTON_GetPressTypeName(type);

    BUTTON_DEBUG_PRINT("%s pressed %s", buttonName, typeName);

  #elif BUTTON_NUM == 1
    UNUSED_ARGUMENT(button);
    UNUSED_ARGUMENT(type);
    BUTTON_DEBUG_PRINT("Pressed");
  #else
    UNUSED_ARGUMENT(button);
    UNUSED_ARGUMENT(type);
    #warning "BUTTON_NUM is not defined or has unimplemented value!"
  #endif
#else
    UNUSED_ARGUMENT(button);
    UNUSED_ARGUMENT(type);
#endif


#ifdef CONFIG_FUNCTION_REMOTECONTROLLER
    /* Call button --> remote control handle-ing */
    Logic_RemoteController_Button(button, type);
#endif

    #ifdef CONFIG_FUNCTION_DISPLAY_MENU
    if (Logic_Display_ActualState == AppType_MainMenu)
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
                if (Logic_Display_SelectedState < AppType_Count-1)
                    Logic_Display_SelectedState++;
                break;

            case PressedButton_Count:
            default:
                /* Error! */
                break;
        }

        TaskHandler_RequestTaskScheduling(Task_Display);
        return;
    }
    else if (Logic_Display_ActualState < AppType_Count)
    {
        AppList[Logic_Display_ActualState-1].eventFunction(button, type);
    }
    else
    {
        /* TODO: Error situation */
    }
    #endif    /* #ifdef CONFIG_FUNCTION_DISPLAY_MENU */

}
#endif /* CONFIG_MODULE_BUTTON_ENABLE || CONFIG_MODULE_BUTTONSIMULATOR_ENABLE */




#ifdef CONFIG_FUNCTION_CHARGER
void Logic_CheckCharger(void)
{
    InputState_t chargeState = IO_GetInputState(Input_BatteryCharger);
    bool isCharging = false;

    /* Check actual state */
    switch (chargeState)
    {
        case InputState_Active:
            /* Battery is charging... */
            isCharging = true;
            break;

        case InputState_Inactive:
            /* Battery is not charging */
            isCharging = false;
            break;

        case InputState_Unknown:
        case InputState_Count:
        default:
            uprintf("Battery charge state error!\r\n");
            /* reak; */
            return;
            /* Do not continue to execute, because input state is not correct! */
    }


    if (Logic_BatteryIsCharging != isCharging)
    {
        /* Charge state is changed! */
        Logic_BatteryIsCharging = isCharging;

        /* Require Display task run */
        TaskHandler_RequestTaskScheduling(Task_Display);

        /* Print actual state */
        if (isCharging)
            uprintf("Battery is charging...\r\n");
        else
            uprintf("Battery is not charging...\r\n");
    }


}
#endif



#ifdef CONFIG_FUNCTION_DISPLAY_MENU
void Logic_DisplayHandler(ScheduleSource_t source)
{
    if (Logic_Display_ChangedState)
    {
        Logic_Display_ChangedState = false;
        Logic_Display_Init();
    }

    if (Logic_Display_ActualState == AppType_MainMenu)
    {
        Logic_Display_MainMenu();
    }
    else if (Logic_Display_ActualState < AppType_Count)
    {
        AppList[Logic_Display_ActualState-1].updateFunction(source);
    }
    else
    {
        /* TODO: Error situation */
    }
}



static void Logic_Display_MainMenu(void)
{
    /*  +---------------------+
     *  | HH:mm         [---] |
     *  |                     |
     */


    /* Check Menu list size */
    /* TODO: Delete */
    /* BUILD_ASSERT(NUM_OF(AppL) == (Menu_Count - 1)); */

    /* Main menu */
    #ifdef CONFIG_FUNCTION_CHARGER
    /* Loading image for Display battery charge state */
    static uint8_t loadPercent = 0;

    if (Logic_BatteryIsCharging)
    {
        /* Loading (progress bar) */
        Display_ChargeLoading(loadPercent);
        loadPercent += 5;

        if (loadPercent >= 100)
        {
            loadPercent = 0;
        }
        /* Require periodical schedule */
        TaskHandler_SetTaskOnceRun(Task_Display, 1000);
    }
    else
    {
        /* Not charging: */
        /* Display actual voltage of battery */
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

    #if defined(CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK) && defined(CONFIG_DISPLAY_CLOCK_SMALL)
    /* Display small clock on main menu */
    DISPLAY_GENERATE_EVENT(Event_Display_SpiEvent, 0);

    /* Only show clock (small - on menu) */
    if (Logic_Display_ActualState == AppType_MainMenu)
    {
        /* TODO: Optimize... This function run around 14ms from 8MHz clock */
        DateTime_t dateTime;
        SysTime_GetDateTime(&dateTime);
        Display_ShowSmallClock(&dateTime.time);

        /* Disable Display task, because when clock will update, it require Display task scheduling */
        TaskHandler_DisableTask(Task_Display);
    }
    #endif

    DISPLAY_GENERATE_EVENT(Event_Display_SpiEvent, 1);

    static App_Type_t oldSelectedMenu = AppType_MainMenu;
    if (Logic_Display_SelectedState != oldSelectedMenu)
    {
        oldSelectedMenu = Logic_Display_SelectedState;
        /* Display main menu list */
        /* TODO: Optimize... Run at ~40ms */
        Logic_Display_PrintMainMenuList();
    }

    DISPLAY_GENERATE_EVENT(Event_Display_SpiEvent, 2);

    Display_Activate();
}



static void Logic_Display_PrintMainMenuList(void)
{
    FontFormat_t selectedFormat = { 0 };
    selectedFormat.Format_Inverse = 1;

    /* TODO: RunTime warning! This function run at 40ms! Optimize! */

    /* Print menu */
    /* TODO: Do with smaller text */

    uint8_t i;

#ifdef CONFIG_FUNCTION_DISPLAY_MENU_SCROLLING
    /* Scrolled menu (only for > DisplayMenu_ShowMenuLimit) */

    uint8_t startLine = 0;
    if (Logic_Display_SelectedState > 2)
    {
        if ((uint8_t)(Logic_Display_SelectedState + DisplayMenu_ShowMenuLimit - 2) >= AppList_Num)
        {
            /* Overflowed */
            startLine = AppList_Num - DisplayMenu_ShowMenuLimit;
        }
        else
        {
            startLine = Logic_Display_SelectedState - 1;
        }
    }

    for (i = 0; i < DisplayMenu_ShowMenuLimit; i++)
    {
        /* Clear */
        /* TODO: Not a beautiful solution */
        Display_PrintString("             ",
                Logic_Display_MenuListLineOffset + i,            /* <x.> line */
                Font_12x8,
                NO_FORMAT);

        /* Print menu name */
        Display_PrintString(
                AppList[startLine + i].AppName,                /* Menu "name" string */
                Logic_Display_MenuListLineOffset + i,            /* <x.> line */
                Font_12x8,                /* Font */
                Logic_Display_SelectedState == startLine+i+1 ? selectedFormat : NO_FORMAT);    /* i + 1, because enum started with "Main" */
    }
#else
    /* Only max DisplayMenu_ShowMenuLimit menu item can displayed */
    BUILD_ASSERT(NUM_OF(Logic_MenuList) <= DisplayMenu_ShowMenuLimit);

    for (i = 0; i < NUM_OF(Logic_MenuList); i++)
    {
        Display_PrintString(
                Logic_MenuList[i],        /* Menu "name" string */
                i + Logic_Display_MenuListLineOffset,            /* <x.> line */
                Font_12x8,                /* Font */
                Logic_Display_SelectedState == i+1 ? selectedFormat : NO_FORMAT);    /* i + 1, because enum started with "Main" */
    }
#endif
}



void Logic_Display_ChangeState(App_Type_t nextState)
{
    Logic_Display_ChangedState = true;
    Logic_Display_ActualState = nextState;
    TaskHandler_RequestTaskScheduling(Task_Display);
}



bool Logic_Display_GetClockIsNeedRefresh(void)
{
    return (Logic_Display_ActualState == AppType_MainMenu ? true : false);
}


#endif    /* #ifdef CONFIG_FUNCTION_DISPLAY_MENU */



#ifdef CONFIG_DISPLAY_ESP8266_ENABLE

void Logic_DisplayESP8266(ScheduleSource_t source)
{

    UNUSED_ARGUMENT(source);

    const char * const esp8266_status = ESP8266_GetStatusName();

    char statusMsg[20];
    usnprintf(statusMsg, 20, "Status: %s", esp8266_status);

    Display_PrintString(
                    statusMsg,              /* msg */
                    0,                      /* line */
                    Font_12x8,              /* Font */
                    NO_FORMAT);             /* Format */

    /* TODO:
     * - button icons
     * - Wifi hostname / client
     * - Connection type (TCP server or client)
     *
     */


    /* Require periodical schedule */
    TaskHandler_SetTaskOnceRun(Task_Display, 1000);

    Display_Activate();
}

#endif /* CONFIG_DISPLAY_ESP8266_ENABLE */


