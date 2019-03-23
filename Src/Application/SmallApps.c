/*
 *    SmallApps.c
 *    Created on:   2019-03-22
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "SmallApps.h"
#include "DebugUart.h"
#include "StringHelper.h"
#include "Display.h"
#include "TaskList.h"
#include "CommandHandler.h"
#include "SysTime.h"
#include "AppList.h"


#define DisplayInput_LetterPosition_MaxLimit        (11)

#define DisplayInput_LetterPosition_MinLimit        (0)

#define DisplayInput_StringLimit                    (DisplayInput_LetterPosition_MaxLimit + 1)



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_FUNCTION_PERIODICAL_SENDING
static char PeriodicalSending_Message[50] = { 0 };
#endif /* CONFIG_FUNCTION_PERIODICAL_SENDING */


#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
static uint8_t DisplayInput_LetterPosition = 0;

static char DisplayInput_ActualRealString[DisplayInput_StringLimit] = { 0 };
static uint8_t DisplayInput_ActualString[DisplayInput_LetterPosition_MaxLimit+1] = { 0 };

static const char const Display_Characters[] = { ' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z' };

static const uint8_t Display_Characters_size = sizeof(Display_Characters)/sizeof(Display_Characters[0]);
#endif /* CONFIG_FUNCTION_DISPLAY_INPUT */


#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
static const uint16_t Display_CarAnimation_RefreshPeriod_MinLimit = 100;
static const uint16_t Display_CarAnimation_RefreshPeriod_MaxLimit = 1000;
static uint16_t Display_CarAnimation_RefreshPeriod_Actual = 300;        /* Do not set const, user can change the refresh period time */
#endif /* CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN */


#if defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK)
static DisplayClock_ChangeState_t Logic_SystemTimeConfigState = 0;
#endif /* CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK */



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

extern void Logic_Display_ChangeState(App_Type_t nextState);

#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
static void Logic_StepLetterPosition(int8_t step);
static void Logic_StepLetterNextValue(int8_t step);
#endif

#ifdef CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK
static void Logic_SystemTimeStepConfig(void);
static void Logic_SystemTimeStepValue(void);
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_FUNCTION_PERIODICAL_SENDING
void Logic_SetPeriodicalMessageSendg(char * msg)
{
    StrCpyMax(PeriodicalSending_Message, msg, 50);
}



void Logic_PeriodicalSending(void)
{
    DebugUart_SendLine(PeriodicalSending_Message);
}

#endif /* CONFIG_FUNCTION_PERIODICAL_SENDING */



#ifdef CONFIG_FUNCTION_DISPLAY_INPUT

void App_DisplayInput_Init(void)
{
    /* Fill real string with empty character */
    memset(DisplayInput_ActualRealString, ' ', DisplayInput_StringLimit-1);
    /* Last char not need fill with ' ', because it is end character */

    /* Create OK button */
    /* TODO: Create button? */
    Display_PrintFont12x8('O', DisplayInput_LetterPosition_MaxLimit, 2, NO_FORMAT);
    Display_PrintFont12x8('K', DisplayInput_LetterPosition_MaxLimit + 1, 2, NO_FORMAT);
}



void App_DisplayInput_Event(ButtonType_t button, ButtonPressType_t type)
{
    if (type != ButtonPress_ReleasedContinuous)
    {
        switch (button)
        {
            case PressedButton_Right:
                /* Right */
                Logic_StepLetterPosition((type == ButtonPress_Short || type == ButtonPress_Continuous) ? 1 : 3);
                break;

            case PressedButton_Left:
                /* Left */
                Logic_StepLetterPosition((type == ButtonPress_Short || type == ButtonPress_Continuous) ? -1 : -3);
                break;

            case PressedButton_Up:
                /* Up */
                Logic_StepLetterNextValue((type == ButtonPress_Short || type == ButtonPress_Continuous) ? -1 : -3);
                break;

            case PressedButton_Down:
                /* Down */
                Logic_StepLetterNextValue((type == ButtonPress_Short || type == ButtonPress_Continuous) ? 1 : 3);
                break;

            case PressedButton_Count:
            default:
                /* Error! */
                break;
        }
    }
}



/**
 * @brief    Step active letter selection to next (left-right)
 */
static void Logic_StepLetterPosition(int8_t step)
{
    DisplayInput_LetterPosition += step;
    if (step > 0 && (DisplayInput_LetterPosition > DisplayInput_LetterPosition_MaxLimit))
    {
        /* Overflow */
        DisplayInput_LetterPosition = DisplayInput_LetterPosition_MinLimit;
    }
    else if (step < 0 && (DisplayInput_LetterPosition > DisplayInput_LetterPosition_MaxLimit))
    {
        /* Underflow */
        DisplayInput_LetterPosition = DisplayInput_LetterPosition_MaxLimit;
    }

    TaskHandler_RequestTaskScheduling(Task_Display);
}



/**
 * @brief    Step Letter value to next (up-down)
 */
static void Logic_StepLetterNextValue(int8_t step)
{
    uint8_t selectedLetter = DisplayInput_ActualString[DisplayInput_LetterPosition];

    if (DisplayInput_LetterPosition == DisplayInput_LetterPosition_MaxLimit)
    {
        /* At "OK" button --> Run command */
        /* TODO: CommPort --> Display */
        char str[DisplayInput_StringLimit];
        StrCpyMax(str, DisplayInput_ActualRealString, DisplayInput_StringLimit);
        StrTrim(str);
        /* TODO: Check 0 length command? */
        char respBuffer[50];
        CmdH_ExecuteCommand(str, respBuffer, 50);
        uprintf("Display input command: \"%s\"\r\n", str);
        /* Now, automatically exit to main menu */
        Logic_Display_ChangeState(AppType_MainMenu);
    }
    else
    {
        /* Not "OK" button */
        if (step < 0 && selectedLetter > 0 && (int8_t)selectedLetter-step > 0)
        {
            /* Can go "down" */
            /* += (- value) => -= */
            selectedLetter += step;
        }
        else if (step < 0)
        {
            /* "Underflow" */
            selectedLetter = Display_Characters_size - 1;
        }
        else if (step > 0 && (uint8_t)(selectedLetter+step) < sizeof(Display_Characters)/sizeof(Display_Characters[0]))
        {
            /* Can go "up" */
            selectedLetter += step;
        }
        else if (step > 0)
        {
            /* "Overflow" */
            selectedLetter = 0;
        }

        /* If change letter */
        /* Save actual letter */
        DisplayInput_ActualString[DisplayInput_LetterPosition] = selectedLetter;

        /* Convert to realstring */
        DisplayInput_ActualRealString[DisplayInput_LetterPosition] = Display_Characters[selectedLetter];
    }

    /* Refresh display */
    TaskHandler_RequestTaskScheduling(Task_Display);
}



void Logic_Display_Input(ScheduleSource_t source)
{
    static bool Display_VibrateLetter = false;

    static uint8_t DisplayInput_OldLetterPosition = 0;
    if ((DisplayInput_OldLetterPosition != DisplayInput_LetterPosition)
        && (DisplayInput_OldLetterPosition == DisplayInput_LetterPosition_MaxLimit))
    {
        /* TODO: Button printing - string printing */
        /* OK */
        Display_PrintFont12x8('O', DisplayInput_LetterPosition_MaxLimit, 2, NO_FORMAT);
        Display_PrintFont12x8('K', DisplayInput_LetterPosition_MaxLimit + 1, 2, NO_FORMAT);
    }
    /* Save old value */
    DisplayInput_OldLetterPosition = DisplayInput_LetterPosition;

    /* Button clicked */
    if (source == ScheduleSource_EventTriggered)
    {
        /* Button press triggering */
        Display_VibrateLetter = false;

        /* Display "all" string */
        Display_PrintString(DisplayInput_ActualRealString, 2, Font_12x8, NO_FORMAT);

        /* It is empty char? (=space) */
        if (DisplayInput_ActualRealString[DisplayInput_LetterPosition] == ' ')
        {
            /* There is a space character, Display a white box */
            Display_PrintFont12x8((char)0x01, DisplayInput_LetterPosition, 2, NO_FORMAT);
        }

        Display_Activate();
        TaskHandler_SetTaskOnceRun(Task_Display, 500);
    }
    else
    {
        /* Vibration, if need (periodical) */
        if (Display_VibrateLetter)
        {
            /* Vibrate (not show char) */
            if (DisplayInput_LetterPosition == DisplayInput_LetterPosition_MaxLimit)
            {
                /* TODO: Button printing - string printing */
                /* OK */
                Display_PrintFont12x8('O', DisplayInput_LetterPosition_MaxLimit, 2, NO_FORMAT);
                Display_PrintFont12x8('K', DisplayInput_LetterPosition_MaxLimit + 1, 2, NO_FORMAT);
            }
            else
            {
                /* Normal char */
                /* It is empty char? (=space) */
                if (DisplayInput_ActualRealString[DisplayInput_LetterPosition] == ' ')
                {
                    /* There is a space character, Display a white box */
                    Display_PrintFont12x8((char)0x01, DisplayInput_LetterPosition, 2, NO_FORMAT);
                }
                else
                {
                    /* There is a normal character... vibrate with hiding */
                    Display_PrintFont12x8(' ', DisplayInput_LetterPosition, 2, NO_FORMAT);
                }
            }

            Display_Activate();
            Display_VibrateLetter = false;
            TaskHandler_SetTaskOnceRun(Task_Display, 500);
        }
        else
        {
            /* Normal (show char) */
            if (DisplayInput_LetterPosition == DisplayInput_LetterPosition_MaxLimit)
            {
                /* TODO: Button printing */
                /* OK */
                FontFormat_t format = { 0 };
                format.Format_Inverse = 1;
                Display_PrintFont12x8('O', DisplayInput_LetterPosition_MaxLimit, 2, format);
                Display_PrintFont12x8('K', DisplayInput_LetterPosition_MaxLimit + 1, 2, format);
            }
            else
            {
                /* Normal char */
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

#endif /* CONFIG_FUNCTION_DISPLAY_INPUT */



#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
void Logic_Display_CarAnimation(ScheduleSource_t source)
{
    UNUSED_ARGUMENT(source);

    /* Car image */
    /* TODO: Optimize this? */
    Display_ChangeCarImage();
    TaskHandler_SetTaskOnceRun(Task_Display, Display_CarAnimation_RefreshPeriod_Actual);
}



void Logic_Display_CarAnimation_Event(ButtonType_t button, ButtonPressType_t type)
{
    if (type != ButtonPress_ReleasedContinuous)
    {
        switch (button)
        {
            case PressedButton_Right:
                /* Right */
                if (Display_CarAnimation_RefreshPeriod_Actual <= Display_CarAnimation_RefreshPeriod_MaxLimit)
                {
                    Display_CarAnimation_RefreshPeriod_Actual += Display_CarAnimation_RefreshPeriod_MinLimit;
                    Logic_Display_CarAnimation(ScheduleSource_Unknown);
                }
                break;

            case PressedButton_Left:
                /* Left */
                if (Display_CarAnimation_RefreshPeriod_Actual >= Display_CarAnimation_RefreshPeriod_MinLimit)
                {
                    Display_CarAnimation_RefreshPeriod_Actual -= Display_CarAnimation_RefreshPeriod_MinLimit;
                    Logic_Display_CarAnimation(ScheduleSource_Unknown);
                }
                break;

            case PressedButton_Up:
            case PressedButton_Down:
                /* Go to Main menu */
                Logic_Display_ChangeState(AppType_MainMenu);
                break;

            case PressedButton_Count:
            default:
                /* Error! */
                break;
        }
    }
}
#endif /* CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN */



#ifdef CONFIG_DISPLAY_CLOCK_LARGE
void Logic_Display_LargeClock_Init(void)
{
    Logic_Display_LargeClock_Update(ScheduleSource_Unknown);
}



void App_LargeClock_Event(ButtonType_t button, ButtonPressType_t type)
{
    #if !defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK)

        /* Go to Main menu */
        Logic_Display_ChangeState(AppType_MainMenu);
        /* TODO: Handle button change functions? */
    #elif defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK) && (BUTTON_NUM == 1)
        /* One button mode */
        UNUSED_ARGUMENT(type);

        if (button == PressedButton_User)
        {
            /* TODO: Add "exit" */
            if (type == ButtonPress_Long)
            {
                BUTTON_DEBUG_PRINT("Pressed a long time");
                Logic_SystemTimeStepConfig();
            }
            else if (type == ButtonPress_Short)
            {
                BUTTON_DEBUG_PRINT("Pressed a short time");
                Logic_SystemTimeStepValue();
            }
        }
    #elif defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK) && (BUTTON_NUM > 1)
        /* More button mode */
        UNUSED_ARGUMENT(type);
        /* TODO: Up-Down / Right-Up difference... */
        if ((button == PressedButton_Right) || (button == PressedButton_Left))
        {
            Logic_SystemTimeStepConfig();
        }
        else if ((button == PressedButton_Up) || (button == PressedButton_Down))
        {
            if (Logic_SystemTimeConfigState != DisplayClock_HourAndMinute)
            {
                /* Not exit step, step values */
                Logic_SystemTimeStepValue();
            }
            else
            {
                /* Exit step */
                /* Go to Main menu */
                Logic_Display_ChangeState(AppType_MainMenu);
            }
        }
    #endif
}



void Logic_Display_LargeClock_Update(ScheduleSource_t source)
{
    #ifdef CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK
    /* Display refresh by clock */

    /* Display vibrate function: if we are in setting mode, hour or minute will vibrate */
    static bool Display_VibrateStateHide = false;

    if (source == ScheduleSource_EventTriggered)
        Display_VibrateStateHide = false;

    /* Get actual DateTime */
    DateTime_t dateTime;
    SysTime_GetDateTime(&dateTime);

    switch (Logic_GetSystemTimeState())
    {
        case DisplayClock_Hour:
            /* Hour setting */
            if (Display_VibrateStateHide)
            {
                Display_ShowLargeClockHalf(&dateTime.time, DisplayClock_Minute);
                Display_VibrateStateHide = false;
            }
            else
            {
                Display_ShowLargeClock(&dateTime.time);
                Display_VibrateStateHide = true;
            }
            TaskHandler_SetTaskOnceRun(Task_Display, 500);
            break;

        case DisplayClock_Minute:
            /* Minute settings */
            if (Display_VibrateStateHide)
            {
                Display_ShowLargeClockHalf(&dateTime.time, DisplayClock_Hour);
                Display_VibrateStateHide = false;
            }
            else
            {
                Display_ShowLargeClock(&dateTime.time);
                Display_VibrateStateHide = true;
            }
            TaskHandler_SetTaskOnceRun(Task_Display, 500);
            break;

        case DisplayClock_HourAndMinute:
        case DisplayClock_Count:
        default:
            /* Not in setting, display the hour and minute too */
            Display_ShowLargeClock(&dateTime.time);
            TaskHandler_DisableTask(Task_Display);
            break;
    }
    #else
    /* Only display a simple large clock (there is no vibration, not changeable) */

    DateTime_t dateTime;
    SysTime_GetDateTime(&dateTime);
    Display_ShowLargeClock(&dateTime.time);

    #endif

    Display_Activate();
}
#endif /* CONFIG_DISPLAY_CLOCK_LARGE */



#if defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK)
/**
 * @brief    SystemTime - step function
 */
static void Logic_SystemTimeStepConfig(void)
{
    /* Change "setting" column */
    TaskHandler_RequestTaskScheduling(Task_Display);

    Logic_SystemTimeConfigState++;
    if (Logic_SystemTimeConfigState >= DisplayClock_Count)
        Logic_SystemTimeConfigState = 0;
}



/**
 * @brief    SystemTime - change (increment) selected value (hour, minute, or none)
 */
static void Logic_SystemTimeStepValue(void)
{
    switch (Logic_SystemTimeConfigState)
    {
        case DisplayClock_HourAndMinute:
            /* Unknown */
            break;

        case DisplayClock_Hour:
        {
            /* Hour */
            DateTime_t dateTime;
            SysTime_GetDateTime(&dateTime);
            DateTime_AddHour(&dateTime);
            SysTime_SetTime(&dateTime.time);    /* Only hour changed, date is not */
            TaskHandler_RequestTaskScheduling(Task_Display);
        }
            break;

        case DisplayClock_Minute:
        {
            /* Minute */
            DateTime_t dateTime;
            SysTime_GetDateTime(&dateTime);
            DateTime_AddMinute(&dateTime);
            SysTime_SetTime(&dateTime.time);    /* Only hour changed, date is not */
            TaskHandler_RequestTaskScheduling(Task_Display);
        }
            break;

        case DisplayClock_Count:
        default:
            /* Error ! */
            Logic_SystemTimeConfigState = 0;
            break;
    }
}



/**
 * @brief    Get system time (settings) state
 */
DisplayClock_ChangeState_t Logic_GetSystemTimeState(void)
{
    return Logic_SystemTimeConfigState;
}
#endif    /* #ifdef CONFIG_MODULE_DISPLAY_SHOW_CLOCK */


