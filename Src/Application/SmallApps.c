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
#include "board.h"
#include "SmallApps.h"
#include "DebugUart.h"
#include "StringHelper.h"
#include "Display.h"
#include "TaskList.h"
#include "CommandHandler.h"
#include "SysTime.h"
#include "IO.h"
#include "AppList.h"

#ifdef CONFIG_HW_DISPLAY_TM1637_ENABLE
    #include "Display_TM1637.h"
#endif

#ifdef CONFIG_FUNCTION_GAME_X2
	#include "Display.h"
	#include "MathHelper.h"
#endif

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
static char PeriodicalMessageSending_Msg[50] = { 0 };
#endif /* CONFIG_FUNCTION_PERIODICAL_SENDING */


#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
static uint8_t DisplayInput_LetterPosition = 0;

static char DisplayInput_ActualRealString[DisplayInput_StringLimit] = { 0 };
static uint8_t DisplayInput_ActualString[DisplayInput_LetterPosition_MaxLimit+1] = { 0 };

static const char DisplayInput_Characters[] = { ' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z' };

static const uint8_t DisplayInput_Characters_size = sizeof(DisplayInput_Characters)/sizeof(DisplayInput_Characters[0]);
#endif /* CONFIG_FUNCTION_DISPLAY_INPUT */


#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
static const uint16_t DisplayCarAnimation_RefreshPeriod_MinLimit = 100;
static const uint16_t DisplayCarAnimation_RefreshPeriod_MaxLimit = 1000;
static uint16_t DisplayCarAnimation_RefreshPeriod_Actual = 300;        /* Do not set const, user can change the refresh period time */
#endif /* CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN */


#if defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK)
static DisplayClock_ChangeState_t App_Clock_SystemTimeConfigState = 0;
#if defined(CONFIG_DISPLAY_CLOCK_SUPPORT_AM_PM)
static bool DisplayClock_AMPM_is12hour = false;
static bool DisplayClock_ShallBeCleaned = false;
#endif /* CONFIG_DISPLAY_CLOCK_SUPPORT_AM_PM */
#endif /* CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK */


#if defined(CONFIG_FUNCTION_TRAFFIC_LIGHT)
static uint32_t TrafficLight_TimeCnt = 0;

static TrafficLight_Mode_t TrafficLight_Mode = TRAFFICLIGHT_MODE_DEFAULT_VALUE;

static const TrafficLight_LampTime_t TrafficLight_LampTime_List[] = {
    {
        .lamp_name = TrafficLight_Lamp_Red,
        .lamp_time_interval = { 10000, 5000, 2000, 500, }    /* Lowest values/timeout --> Fastest */
    },
    {
        .lamp_name = TrafficLight_Lamp_RedYellow,
        .lamp_time_interval = { 2000, 1000, 400, 100, }
    },
    {
        .lamp_name = TrafficLight_Lamp_Green,
        .lamp_time_interval = { 10000, 5000, 2000, 500, }
    },
    {
        .lamp_name = TrafficLight_Lamp_Yellow,
        .lamp_time_interval = { 2000, 1000, 400, 100, }
    }
};

const uint8_t TrafficLight_Automatic_Lamp_Count = NUM_OF(TrafficLight_LampTime_List);

#define TRAFFICLIGHT_AUTOMATIC_LAMP_TIME_DEFAULT_INDEX (0)

static int8_t TrafficLight_Automatic_Lamp_TimeActual_Index = 0;
static int8_t TrafficLight_Automatic_Lamp_StatusActual_Index = 0;

#define TRAFFICLIGHT_AUTOMATIC_LAMP_DEFAULT_INDEX (0)

#define TRAFFICLIGHT_LAMP_DEFAULT_STATUS          (TrafficLight_Lamp_Red)

static TrafficLight_Lamp_t TrafficLight_ActualStatus = TRAFFICLIGHT_LAMP_DEFAULT_STATUS;

const TrafficLight_Lamp_t TrafficLight_Lamp_Down_List[] = { TrafficLight_Lamp_Green, TrafficLight_Lamp_RedYellow, TrafficLight_Lamp_Red };
/*                                                                                 <<<<<                      <<<<<<                   */
const TrafficLight_Lamp_t TrafficLight_Lamp_Up_List[] = { TrafficLight_Lamp_Green, TrafficLight_Lamp_Yellow, TrafficLight_Lamp_Red };
/*                                                                               >>>>>>>                 >>>>>>>                   */
const size_t TrafficLight_Lamp_List_Len = NUM_OF(TrafficLight_Lamp_Up_List);


#endif /* CONFIG_FUNCTION_TRAFFIC_LIGHT */


#if defined(CONFIG_FUNCTION_ELEVATOR)
static volatile int32_t App_Elevator_level = 0;
static volatile bool App_Elevator_in_error_status = false;
#endif /* CONFIG_FUNCTION_ELEVATOR */


#ifdef CONFIG_FUNCTION_GAME_X2
static const char * const App_GameX2_LevelList[] = {
    "2",
	"4",
	"8",
	"16",
	"32",
	"64",
	"128",
	"256",
	"512",
	"1E",
	"2E",
	"4E",
	"8E",
	"16E"
	"32E",
	"64E",
	"128E",
	"256E",
	"513E",
	"1M",
    "2M",
	"4M",
	"8M",
	"16M",
	"32M",
	"64M",
	"128M",
	"257M",
	"514M",
	"1Md",
    "2Md",
	"4Md",
	"8Md",
	"16Md",
	"32Md",
	"64Md",
	"129Md",
	"258Md",
	"516Md",
	"1T",
};

const uint8_t GameX2_LevelList_Size = NUM_OF(App_GameX2_LevelList);
#endif /* CONFIG_FUNCTION_GAME_X2 */



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

extern void Logic_Display_ChangeState(App_Type_t nextState);

#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
static void DisplayInput_StepLetterPosition(int8_t step);
static void DisplayInput_StepLetterNextValue(int8_t step);
#endif

#ifdef CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK
static void DisplayLargeClock_StepConfig(void);
    #if (CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK_ADVANCED == 1)
static void DisplayLargeClock_StepValue(ButtonType_t button);
    #else
static void DisplayLargeClock_StepValue(void);
    #endif
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_FUNCTION_PERIODICAL_SENDING
void App_PeriodicalMessageSending_Set(char * msg)
{
    StrCpyMax(PeriodicalMessageSending_Msg, msg, 50);
}



void App_PeriodicalMessageSending_Run(void)
{
    DebugUart_SendLine(PeriodicalMessageSending_Msg);
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
                DisplayInput_StepLetterPosition((type == ButtonPress_Short || type == ButtonPress_Continuous) ? 1 : 3);
                break;

            case PressedButton_Left:
                /* Left */
                DisplayInput_StepLetterPosition((type == ButtonPress_Short || type == ButtonPress_Continuous) ? -1 : -3);
                break;

            case PressedButton_Up:
                /* Up */
                DisplayInput_StepLetterNextValue((type == ButtonPress_Short || type == ButtonPress_Continuous) ? -1 : -3);
                break;

            case PressedButton_Down:
                /* Down */
                DisplayInput_StepLetterNextValue((type == ButtonPress_Short || type == ButtonPress_Continuous) ? 1 : 3);
                break;

            case PressedButton_Count:
            default:
                /* Error! */
                break;
        }
    }
}



/**
 * @brief       Step active letter selection to next (left-right)
 */
static void DisplayInput_StepLetterPosition(int8_t step)
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
 * @brief       Step Letter value to next (up-down)
 */
static void DisplayInput_StepLetterNextValue(int8_t step)
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
            selectedLetter = DisplayInput_Characters_size - 1;
        }
        else if (step > 0 && (uint8_t)(selectedLetter+step) < sizeof(DisplayInput_Characters)/sizeof(DisplayInput_Characters[0]))
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
        DisplayInput_ActualRealString[DisplayInput_LetterPosition] = DisplayInput_Characters[selectedLetter];
    }

#ifdef CONFIG_MODULE_DISPLAY_ENABLE
    /* Refresh 7segment */
    Display_TM1637_Print(DisplayInput_ActualRealString);
#endif

    /* Refresh display */
    TaskHandler_RequestTaskScheduling(Task_Display);
}



void App_DisplayInput_Update(ScheduleSource_t source)
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
void App_DisplayCarAnimation_Init(void)
{
    Display_LoadCarImage();
}



void App_DisplayCarAnimation_Update(ScheduleSource_t source)
{
    UNUSED_ARGUMENT(source);

    /* Car image */
    /* TODO: Optimize this? */
    Display_ChangeCarImage();
    TaskHandler_SetTaskOnceRun(Task_Display, DisplayCarAnimation_RefreshPeriod_Actual);
}



void App_DisplayCarAnimation_Event(ButtonType_t button, ButtonPressType_t type)
{
    if (type != ButtonPress_ReleasedContinuous)
    {
        switch (button)
        {
            case PressedButton_Right:
                /* Right */
                if (DisplayCarAnimation_RefreshPeriod_Actual <= DisplayCarAnimation_RefreshPeriod_MaxLimit)
                {
                    DisplayCarAnimation_RefreshPeriod_Actual += DisplayCarAnimation_RefreshPeriod_MinLimit;
                    App_DisplayCarAnimation_Update(ScheduleSource_Unknown);
                }
                break;

            case PressedButton_Left:
                /* Left */
                if (DisplayCarAnimation_RefreshPeriod_Actual >= DisplayCarAnimation_RefreshPeriod_MinLimit)
                {
                    DisplayCarAnimation_RefreshPeriod_Actual -= DisplayCarAnimation_RefreshPeriod_MinLimit;
                    App_DisplayCarAnimation_Update(ScheduleSource_Unknown);
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
void App_DisplayLargeClock_Init(void)
{
    App_DisplayLargeClock_Update(ScheduleSource_Unknown);
}



void App_DisplayLargeClock_Event(ButtonType_t button, ButtonPressType_t type)
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
                DisplayLargeClock_StepConfig();
            }
            else if (type == ButtonPress_Short)
            {
                BUTTON_DEBUG_PRINT("Pressed a short time");
                DisplayLargeClock_StepValue();
            }
        }
    #elif defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK) && (BUTTON_NUM > 1)
        /* More button mode */
        UNUSED_ARGUMENT(type);
        /* TODO: Up-Down / Right-Up difference... */
        if ((button == PressedButton_Right) || (button == PressedButton_Left))
        {
            DisplayLargeClock_StepConfig();
        }
        else if ((button == PressedButton_Up) || (button == PressedButton_Down))
        {
            if (App_Clock_SystemTimeConfigState != DisplayClock_HourAndMinute)
            {
                /* Not exit step, step values */
        #if (CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK_ADVANCED == 1)
                DisplayLargeClock_StepValue(button);
        #else
                DisplayLargeClock_StepValue();
        #endif
            }
            else
            {
                /* Exit step */
                /* Go to Main menu */
                Logic_Display_ChangeState(AppType_MainMenu);
            }
        }
    #else
        #error("Display without button")
    #endif
}



void App_DisplayLargeClock_Update(ScheduleSource_t source)
{
    #ifdef CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK
    /* Display refresh by clock */

    /* Display vibrate function: if we are in setting mode, hour or minute will vibrate */
    static bool Display_VibrateStateHide = false;
    static bool colon = true;

    if (source == ScheduleSource_EventTriggered)
    {
        Display_VibrateStateHide = false;
    }

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
                Display_ShowLargeClock(&dateTime.time, true);
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
                Display_ShowLargeClock(&dateTime.time, true);
                Display_VibrateStateHide = true;
            }
            TaskHandler_SetTaskOnceRun(Task_Display, 500);
            break;
#ifdef CONFIG_DISPLAY_CLOCK_SUPPORT_AM_PM
        case DisplayClock_AMPM:
        	Display_Clear();
        	FontFormat_t format = { 0 };
        	format.Format_Center = 1;
        	if (DisplayClock_AMPM_is12hour)
        	{
        		Display_PrintString("12H", 2, Font_12x8, format);
        	}
        	else
        	{
        		Display_PrintString("24H", 2, Font_12x8, format);
        	}
            TaskHandler_SetTaskOnceRun(Task_Display, 500);
			break;
#endif
        case DisplayClock_HourAndMinute:
        case DisplayClock_Count:
        default:
            /* Not in setting, display the hour and minute too */

#ifdef CONFIG_DISPLAY_CLOCK_SUPPORT_AM_PM
			if (DisplayClock_ShallBeCleaned)
			{
				Display_Clear();
				DisplayClock_ShallBeCleaned = false;
			}
        	if (DisplayClock_AMPM_is12hour)
        	{
        		if (dateTime.time.hour > 12)
        		{
        			dateTime.time.hour -= 12;
        		}
        		/* else if (dateTime.time.hour == 0) */
        	}
			Display_ShowLargeClock(&dateTime.time, colon);
            colon = !colon;
            /* TaskHandler_DisableTask(Task_Display); */
            TaskHandler_SetTaskOnceRun(Task_Display, 1000);
            break;
    }
#endif /* CONFIG_DISPLAY_CLOCK_SUPPORT_AM_PM */

#ifdef CONFIG_HW_DISPLAY_TM1637_ENABLE
    Display_TM1637_DisplayTime(&dateTime.time);
#endif
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
 * @brief       SystemTime - step function
 */
static void DisplayLargeClock_StepConfig(void)
{
    /* Change "setting" column */
    TaskHandler_RequestTaskScheduling(Task_Display);

    App_Clock_SystemTimeConfigState++;
    if (App_Clock_SystemTimeConfigState >= DisplayClock_Count)
    {
        App_Clock_SystemTimeConfigState = 0;
    }
}



/**
 * @brief       SystemTime - change (increment) selected value (hour, minute, or none)
 */
#if (CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK_ADVANCED == 1)
static void DisplayLargeClock_StepValue(ButtonType_t button)
#else
static void DisplayLargeClock_StepValue(void)
#endif
{
    switch (App_Clock_SystemTimeConfigState)
    {
        case DisplayClock_HourAndMinute:
            /* Unknown */
            break;

        case DisplayClock_Hour:
        {
            /* Hour */
            DateTime_t dateTime;
            SysTime_GetDateTime(&dateTime);
    #if (CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK_ADVANCED == 1)
            switch (button)
            {
                case PressedButton_Up:
                    DateTime_AddHour(&dateTime);
                    break;
                case PressedButton_Down:
                    DateTime_SubHour(&dateTime);
                    break;
                case PressedButton_Right:
                case PressedButton_Left:
                case PressedButton_Count:
                default:
                    break;
            }
    #else
            DateTime_AddHour(&dateTime);
    #endif
            SysTime_SetTime(&dateTime.time);    /* Only hour changed, date is not */
            TaskHandler_RequestTaskScheduling(Task_Display);
        }
            break;

        case DisplayClock_Minute:
        {
            /* Minute */
            DateTime_t dateTime;
            SysTime_GetDateTime(&dateTime);
    #if (CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK_ADVANCED == 1)
        switch (button)
        {
            case PressedButton_Up:
                DateTime_AddMinute(&dateTime);
                break;
            case PressedButton_Down:
                DateTime_SubMinute(&dateTime);
                break;
            case PressedButton_Right:
            case PressedButton_Left:
            case PressedButton_Count:
            default:
                break;
        }
    #else
        DateTime_AddMinute(&dateTime);
    #endif
            SysTime_SetTime(&dateTime.time);    /* Only hour changed, date is not */
            TaskHandler_RequestTaskScheduling(Task_Display);
        }
            break;

#ifdef CONFIG_DISPLAY_CLOCK_SUPPORT_AM_PM
        case DisplayClock_AMPM:
        	DisplayClock_ShallBeCleaned = true;
        	DisplayClock_AMPM_is12hour = !DisplayClock_AMPM_is12hour;
        	break;
#endif /* CONFIG_DISPLAY_CLOCK_SUPPORT_AM_PM */
        case DisplayClock_Count:
        default:
            /* Error ! */
            App_Clock_SystemTimeConfigState = 0;
            break;
    }
}



/**
 * @brief       Get system time (settings) state
 */
DisplayClock_ChangeState_t Logic_GetSystemTimeState(void)
{
    return App_Clock_SystemTimeConfigState;
}
#endif    /* #ifdef CONFIG_MODULE_DISPLAY_SHOW_CLOCK */




#if defined(CONFIG_FUNCTION_TRAFFIC_LIGHT)

void App_TrafficLight_Init(void)
{
    TrafficLight_TimeCnt = 0;
    TrafficLight_Mode = TRAFFICLIGHT_MODE_DEFAULT_VALUE;
    TrafficLight_Automatic_Lamp_TimeActual_Index = TRAFFICLIGHT_AUTOMATIC_LAMP_TIME_DEFAULT_INDEX;
    TrafficLight_ActualStatus = TRAFFICLIGHT_LAMP_DEFAULT_STATUS;

    /* TODO: merge with the below */
    /* Note: The default settings is for the automatic lamp, that is why we need the timeout from automatic lamp settings */
    uint16_t next_timeout = TrafficLight_LampTime_List[TrafficLight_Automatic_Lamp_StatusActual_Index].lamp_time_interval[TrafficLight_Automatic_Lamp_TimeActual_Index];

    TaskHandler_SetTaskOnceRun(Task_Display, next_timeout);
}



void App_TrafficLight_Event(ButtonType_t button, ButtonPressType_t type)
{
    UNUSED_ARGUMENT(type);

    switch (button)
    {
        case PressedButton_Up:
        case PressedButton_Down:
            {
                /* Lamp or time change */
                if (TrafficLight_Mode == TrafficLight_Mode_Automatic)
                {
                    /* Timeout change */

                    if (button == PressedButton_Up)
                    {
                        TrafficLight_Automatic_Lamp_TimeActual_Index++;
                        if (TrafficLight_Automatic_Lamp_TimeActual_Index >= TrafficLight_Automatic_Lamp_TimeLevel_len)
                        {
                            /* Do not overflow, stay at the max */
                            TrafficLight_Automatic_Lamp_TimeActual_Index = TrafficLight_Automatic_Lamp_TimeLevel_len - 1;
                        }
                    }
                    else
                    {
                        /* if (button == PressedButton_Down) */
                        TrafficLight_Automatic_Lamp_TimeActual_Index--;
                        if (TrafficLight_Automatic_Lamp_TimeActual_Index < TRAFFICLIGHT_AUTOMATIC_LAMP_TIME_DEFAULT_INDEX)
                        {
                            /* Trick: The '--' make a negative value */
                            /* Do not underflow, stay at the min */
                            TrafficLight_Automatic_Lamp_TimeActual_Index = TRAFFICLIGHT_AUTOMATIC_LAMP_TIME_DEFAULT_INDEX;
                        }
                    }
                }
                else if (TrafficLight_Mode == TrafficLight_Mode_Manual)
                {
                    static int8_t TrafficLight_Manual_ActualIndex = 0;

                    /* Lamp change */
                    if (button == PressedButton_Up)
                    {
                        TrafficLight_Manual_ActualIndex++;
                        /* For avoid the warning, the value < 0 cannot check */
                        if (TrafficLight_Manual_ActualIndex >= (int8_t)(TrafficLight_Lamp_List_Len) )
                        {
                            TrafficLight_Manual_ActualIndex = TrafficLight_Lamp_List_Len - 1;
                        }
                        TrafficLight_ActualStatus = TrafficLight_Lamp_Up_List[TrafficLight_Manual_ActualIndex];
                    }
                    else
                    {
                        /* if (button == PressedButton_Down) */
                        TrafficLight_Manual_ActualIndex--;
                        if (TrafficLight_Manual_ActualIndex < 0)
                        {
                            TrafficLight_Manual_ActualIndex = 0;
                        }
                        TrafficLight_ActualStatus = TrafficLight_Lamp_Down_List[TrafficLight_Manual_ActualIndex];
                    }
                }
                else if (TrafficLight_Mode == TrafficLight_Mode_TurnedOff)
                {
                    /* Turned off status, do nothing */
                    TrafficLight_ActualStatus = TrafficLight_Lamp_Count; /* Turn off status */
                }
                else
                {
                    /* TODO: Error situation */
                }

                /* Update the display + lamps */
                App_TrafficLight_Update(ScheduleSource_EventTriggered);
            }
            break;

        case PressedButton_Right:
        case PressedButton_Left:
            {
                /* Mode change */
                if (button == PressedButton_Right)
                {
                    TrafficLight_Mode++;
                    if (TrafficLight_Mode >= TrafficLight_Mode_Count)
                    {
                        TrafficLight_Mode = TRAFFICLIGHT_MODE_DEFAULT_VALUE;
                    }
                }
                else
                {
                    /* button == PressedButton_Left */
                    TrafficLight_Mode--;
                    /* For avoid the warning, the value < 0 cannot check */
                    if (TrafficLight_Mode >= TrafficLight_Mode_Count)
                    {
                        TrafficLight_Mode = TrafficLight_Mode_Count - 1; /* Last value */
                    }
                }

                /* TimeCnt update */
                TrafficLight_TimeCnt = 0;

                /* Calculated new status */
                if (TrafficLight_Mode == TrafficLight_Mode_TurnedOff)
                {
                    TrafficLight_ActualStatus = TrafficLight_Lamp_Count;
                }

                /* Update the display + lamps */
                App_TrafficLight_Update(ScheduleSource_EventTriggered);
            }
            break;

        case  PressedButton_Count:
        default:
            /* Error handling: skip it */
            /* TODO: Report error */
            break;
    }
}



void App_TrafficLight_Update(ScheduleSource_t source)
{
    uint16_t next_timeout = 0;

    bool need_update_display = false;

    if (source == ScheduleSource_EventTriggered)
    {
        /* Button pressed */
        /* In this case, the App_TrafficLight_Event() function has prepared the new statuses */
        /* Draw immediately the actual status */
        need_update_display = true;

        /* Note: possible event: if at a lamp we change the timeout, the timeout will be reset */
    }
    else if ((source == ScheduleSource_RunOnce) || (source == ScheduleSource_PeriodicSchedule))
    {
        need_update_display = true;

        /* Increment the status, due the timeout of actual lamp */
        if (TrafficLight_Mode == TrafficLight_Mode_Automatic)
        {
            TrafficLight_Automatic_Lamp_StatusActual_Index++;
            if (TrafficLight_Automatic_Lamp_StatusActual_Index >= TrafficLight_Automatic_Lamp_Count)
            {
                /* restart it from zero/init */
                TrafficLight_Automatic_Lamp_StatusActual_Index = TRAFFICLIGHT_AUTOMATIC_LAMP_DEFAULT_INDEX;
            }

            TrafficLight_ActualStatus = TrafficLight_LampTime_List[TrafficLight_Automatic_Lamp_StatusActual_Index].lamp_name;

            /* Get new timeout */
            next_timeout = TrafficLight_LampTime_List[TrafficLight_Automatic_Lamp_StatusActual_Index].lamp_time_interval[TrafficLight_Automatic_Lamp_TimeActual_Index];

            /* Task schedule requiring */
            TaskHandler_SetTaskOnceRun(Task_Display, next_timeout);
        }
        /* At manual mode, we dont need the periodic task execution */
    }
    else
    {
        /* TODO: Report */
        need_update_display = true;
        /*TaskHandler_SetTaskOnceRun(Task_Display, next_timeout); */ /* TODO: Check */
    }


    if (need_update_display == true)
    {
        /* Update the lamp */
        switch (TrafficLight_ActualStatus)
        {
            case TrafficLight_Lamp_Red:
                IO_Output_SetStatus(IO_AppTrafficLight_Red, IO_Output_Cmd_SetOn);
                IO_Output_SetStatus(IO_AppTrafficLight_Yellow, IO_Output_Cmd_SetOff);
                IO_Output_SetStatus(IO_AppTrafficLight_Green, IO_Output_Cmd_SetOff);
                break;

            case TrafficLight_Lamp_RedYellow:
                IO_Output_SetStatus(IO_AppTrafficLight_Red, IO_Output_Cmd_SetOn);
                IO_Output_SetStatus(IO_AppTrafficLight_Yellow, IO_Output_Cmd_SetOn);
                IO_Output_SetStatus(IO_AppTrafficLight_Green, IO_Output_Cmd_SetOff);
                break;

            case TrafficLight_Lamp_Green:
                IO_Output_SetStatus(IO_AppTrafficLight_Red, IO_Output_Cmd_SetOff);
                IO_Output_SetStatus(IO_AppTrafficLight_Yellow, IO_Output_Cmd_SetOff);
                IO_Output_SetStatus(IO_AppTrafficLight_Green, IO_Output_Cmd_SetOn);
                break;

            case TrafficLight_Lamp_Yellow:
                IO_Output_SetStatus(IO_AppTrafficLight_Red, IO_Output_Cmd_SetOff);
                IO_Output_SetStatus(IO_AppTrafficLight_Yellow, IO_Output_Cmd_SetOn);
                IO_Output_SetStatus(IO_AppTrafficLight_Green, IO_Output_Cmd_SetOff);
                break;

            case TrafficLight_Lamp_Off: /* Handle in same switch branch */
            case TrafficLight_Lamp_Count:
            default:
                /* Off situation */
                IO_Output_SetStatus(IO_AppTrafficLight_Red, IO_Output_Cmd_SetOff);
                IO_Output_SetStatus(IO_AppTrafficLight_Yellow, IO_Output_Cmd_SetOff);
                IO_Output_SetStatus(IO_AppTrafficLight_Green, IO_Output_Cmd_SetOff);
                break;
        }

        /* Update the display */
        /*
         * +---------+
         * | O       |
         * | X text  |
         * | X       |
         * +---------+
         *
         * Solution: space indent required for the displaying to text
         */

        switch (TrafficLight_Mode)
        {
            case TrafficLight_Mode_Automatic:
                Display_PrintString("  Automatic", 2, Font_12x8, NO_FORMAT);
                break;

            case TrafficLight_Mode_Manual:
                Display_PrintString("  Manual   ", 2, Font_12x8, NO_FORMAT);  /* spaces at end: FastenHomeAut/issues/5 */
                break;

            case TrafficLight_Mode_TurnedOff:
                Display_PrintString("  Off      ", 2, Font_12x8, NO_FORMAT);  /* spaces at end: FastenHomeAut/issues/5 */
                break;

            case TrafficLight_Mode_Count:
            default:
                Display_PrintString("  Error    ", 2, Font_12x8, NO_FORMAT);  /* spaces at end: FastenHomeAut/issues/5 */
                break;
        }

        switch (TrafficLight_ActualStatus)
        {
            case TrafficLight_Lamp_Red:
                Display_PrintFont12x8('O', 0, 1, NO_FORMAT);
                Display_PrintFont12x8(' ', 0, 2, NO_FORMAT);
                Display_PrintFont12x8(' ', 0, 3, NO_FORMAT);
                break;

            case TrafficLight_Lamp_RedYellow:
                Display_PrintFont12x8('O', 0, 1, NO_FORMAT);
                Display_PrintFont12x8('O', 0, 2, NO_FORMAT);
                Display_PrintFont12x8(' ', 0, 3, NO_FORMAT);
                break;

            case TrafficLight_Lamp_Green:
                Display_PrintFont12x8(' ', 0, 1, NO_FORMAT);
                Display_PrintFont12x8(' ', 0, 2, NO_FORMAT);
                Display_PrintFont12x8('O', 0, 3, NO_FORMAT);
                break;

            case TrafficLight_Lamp_Yellow:
                Display_PrintFont12x8(' ', 0, 1, NO_FORMAT);
                Display_PrintFont12x8('O', 0, 2, NO_FORMAT);
                Display_PrintFont12x8(' ', 0, 3, NO_FORMAT);
                break;

            case TrafficLight_Lamp_Off:
            case TrafficLight_Lamp_Count:
            default:
                /* Off situation */
                Display_PrintFont12x8(' ', 0, 1, NO_FORMAT);
                Display_PrintFont12x8(' ', 0, 2, NO_FORMAT);
                Display_PrintFont12x8(' ', 0, 3, NO_FORMAT);
                break;
        }

        Display_Activate();
    }
}


#ifdef CONFIG_FUNCTION_TASK_TRAFFIC_LIGHT
void App_TrafficLight_TaskFunction(ScheduleSource_t source)
{
    TrafficLight_TimeCnt++;

    switch (TrafficLight_TimeCnt % 3)
    {
        case 0:
            IO_Output_SetStatus(IO_AppTrafficLight_Red, IO_Output_Cmd_SetOn);
            IO_Output_SetStatus(IO_AppTrafficLight_Yellow, IO_Output_Cmd_SetOff);
            IO_Output_SetStatus(IO_AppTrafficLight_Green, IO_Output_Cmd_SetOff);
            break;

        case 1:
            IO_Output_SetStatus(IO_AppTrafficLight_Red, IO_Output_Cmd_SetOff);
            IO_Output_SetStatus(IO_AppTrafficLight_Yellow, IO_Output_Cmd_SetOn);
            IO_Output_SetStatus(IO_AppTrafficLight_Green, IO_Output_Cmd_SetOff);
            break;

        case 2:
            IO_Output_SetStatus(IO_AppTrafficLight_Red, IO_Output_Cmd_SetOff);
            IO_Output_SetStatus(IO_AppTrafficLight_Yellow, IO_Output_Cmd_SetOff);
            IO_Output_SetStatus(IO_AppTrafficLight_Green, IO_Output_Cmd_SetOn);
            break;

        default:
            /* Error situation, it never reached */
            IO_Output_SetStatus(IO_AppTrafficLight_Red, IO_Output_Cmd_SetOff);
            IO_Output_SetStatus(IO_AppTrafficLight_Yellow, IO_Output_Cmd_SetOff);
            IO_Output_SetStatus(IO_AppTrafficLight_Green, IO_Output_Cmd_SetOff);
            break;
    }
}
#endif /* CONFIG_FUNCTION_TASK_TRAFFIC_LIGHT */

#endif    /* CONFIG_FUNCTION_TRAFFIC_LIGHT */


#if defined(CONFIG_FUNCTION_ELEVATOR)

void App_DisplayElevator_Init(void)
{
    /* App_Elevator_level = 0; */ /* We would not like to forget the elevator level */
    App_DisplayElevator_Update(ScheduleSource_EventTriggered);

    TaskHandler_SetTaskPeriodicTime(Task_ButtonPressed, 500);  /* 2 level / second */
}

void App_DisplayElevator_Event(ButtonType_t button, ButtonPressType_t type)
{
    if (type != ButtonPress_ReleasedContinuous)
    {
        switch (button)
        {
            case PressedButton_Right:
                /* Right */
                if (type == ButtonPress_Long)
                {
                    Logic_Display_ChangeState(AppType_MainMenu);
                }
                break;

            case PressedButton_Left:
                /* Left */
                if (type == ButtonPress_Short)
                {
                	App_Elevator_in_error_status = ~App_Elevator_in_error_status;
                	App_DisplayElevator_Update(ScheduleSource_EventTriggered);
                }
                break;

            case PressedButton_Up:
                /* Up */
                App_Elevator_level++;
                App_DisplayElevator_Update(ScheduleSource_EventTriggered);
                break;

            case PressedButton_Down:
                /* Down */
                App_Elevator_level--;
                App_DisplayElevator_Update(ScheduleSource_EventTriggered);
                break;

            case PressedButton_Count:
            default:
                /* Error! */
                break;
        }
    }
}


void App_DisplayElevator_Update(ScheduleSource_t source)
{
    UNUSED_ARGUMENT(source);

    char elevator_level_string[6];

	if (!App_Elevator_in_error_status)
	{
		usnprintf(elevator_level_string, 6, "%d  ", App_Elevator_level);
	}
	else
	{
		usnprintf(elevator_level_string, 6, "--    ");
	}

    Display_PrintString(elevator_level_string, 0, Font_32x20, Display_NoFormat);
    Display_Activate();
    TaskHandler_DisableTask(Task_Display);

#ifdef CONFIG_HW_DISPLAY_TM1637_ENABLE
    Display_TM1637_Print(elevator_level_string);  /* Minimum 4 character */
#endif

}


#endif /* CONFIG_FUNCTION_ELEVATOR */



#ifdef CONFIG_FUNCTION_GAME_X2

typedef enum
{
	GameX2_column_min = 0,
	GameX2_column_left = 0, /* Min */
	GameX2_column_mid,
	GameX2_column_right, /* Latest acceptable */
	GameX2_column_count /* Max+1 */
} GameX2_column_t;

#define GameX2_Column_Size (3)
#define GameX2_Row_Size (4)

static const char * GameX2_Matrix[GameX2_Column_Size][GameX2_Row_Size] = { 0 };
/* column index, row index */

static const char * volatile GameX2_ActualNewNumber = NULL;

static const char * volatile GameX2_Record = NULL;
static volatile uint8_t GameX2_NextNumber_MinIndex = 0;

#define GAME_X2_START_RECORD_INDEX ( 5 )
#define GAME_X2_NUMBER_STRING_LENGTH ( 4 )
#define GAME_X2_BOTTOM_LINE_INDEX_FOR_NEXT_NUMBER ( GameX2_Row_Size )
#define GAME_X2_RANDOM_GENERATION_NEXT_NUMBER_MAX_INDEX ( 3 )

#define GAME_X2_STRING_ONE_LINE_MAX_LENGTH ( ( GAME_X2_NUMBER_STRING_LENGTH * GameX2_Column_Size ) + 1)

static void GameX2_CheckAndDoTheAllMerges(void);
static void GameX2_Check_if_new_record_reached(const char * new_val);
static void GameX2_CleanOldNumbers(void);


static bool_t GameX2_CheckAllFields_IsThereFree(void) {
	bool_t result = false; /* It has no free */
	uint8_t row_i;
	uint8_t column_i;
	for (row_i=0; row_i < GameX2_Row_Size; row_i++) {
		for (column_i=0; column_i < GameX2_Column_Size; column_i++) {
			if (GameX2_Matrix[column_i][row_i] == NULL ) {
				result = true;
				break;
			}
		}
	}
	return result;
}

/**
 * @brief   Find the index of a value in the level list.
 * @return  Index if found, -1 if not found.
 */
static int8_t GameX2_GetLevelIndex(const char * value)
{
    uint8_t i;
    for (i = 0; i < GameX2_LevelList_Size; i++)
    {
        if (App_GameX2_LevelList[i] == value)
        {
            return (int8_t)i;
        }
    }
    return -1;
}


static bool_t GameX2_SelectColumn(GameX2_column_t column) {
	bool_t result = true;
	if (column >= GameX2_column_min && column < GameX2_column_count) {
		uint8_t row_i;
		bool_t column_passed = false;
		for (row_i=0; row_i < GameX2_Row_Size; row_i++) {
			if (GameX2_Matrix[column][row_i] == NULL ) {
				GameX2_Matrix[column][row_i] = GameX2_ActualNewNumber;
				column_passed = true;
				break;
			}
			// else : There is a field in that
			if (column_passed == false && row_i == (GameX2_Row_Size - 1)) // Latest line
			{
				if (GameX2_Matrix[column][row_i] == GameX2_ActualNewNumber ) { // Only if they can be merged  Special MERGE !!!
					int8_t lvl_index = GameX2_GetLevelIndex(GameX2_Matrix[column][row_i]);
					GameX2_Matrix[column][row_i] = App_GameX2_LevelList[lvl_index+1];

					// It is very easily possible, if we shall merge anothers also:
					GameX2_CheckAndDoTheAllMerges();
				}
			}
		}
		if (column_passed != true) {
			/* The column was full */
			result = false;
			/* TODO: Check the all columns, because maybe there is no another free column */
		}
	}
	return result;
}

static void GameX2_GenerateNewNumber(void) {
	uint8_t random_index = random() % GAME_X2_RANDOM_GENERATION_NEXT_NUMBER_MAX_INDEX;
	uint8_t id = GameX2_NextNumber_MinIndex + random_index;
	if (id >= GameX2_LevelList_Size)
	{
		id = GameX2_LevelList_Size - 1;
	}
	GameX2_ActualNewNumber = App_GameX2_LevelList[id];
}

static bool_t GameX2_CheckIfMergePossible(void) {
	bool_t result_there_was_change = false; /* There is no change*/
	uint8_t row_i;
	uint8_t column_i;
	for (row_i=0; row_i < GameX2_Row_Size; row_i++) {
		for (column_i=0; column_i < GameX2_Column_Size; column_i++) {
			if (GameX2_Matrix[column_i][row_i] != NULL ) {
				if (row_i < GameX2_Row_Size - 1) {
					if (GameX2_Matrix[column_i][row_i] == GameX2_Matrix[column_i][row_i+1]) // Same
					{
						// Merge !!
						result_there_was_change = true;
						int8_t lvl_index = GameX2_GetLevelIndex(GameX2_Matrix[column_i][row_i]);
						GameX2_Matrix[column_i][row_i] = App_GameX2_LevelList[lvl_index+1]; // "Merge" - next number
						GameX2_Matrix[column_i][row_i+1] = NULL; // Clean the next / below number
						GameX2_Check_if_new_record_reached(GameX2_Matrix[column_i][row_i]);
						break;
					}
				}
			}
		}
	}

	return result_there_was_change;
}

static void GameX2_CheckAndDoTheAllMerges(void) {
	bool_t there_was_change = true; // Tricky point to start once
	const uint8_t max_change = ( GameX2_Row_Size - 1) * GameX2_Column_Size;
	uint8_t i;
	for (i = 0; i < max_change; i++ ) {
		if (there_was_change) {
			there_was_change = GameX2_CheckIfMergePossible();
		}
		else
		{
			//no more available merge
			break;
		}
	}
}

/**
 * @brief   Remove old numbers from the matrix that are smaller than the current
 *          minimum acceptable number (GameX2_NextNumber_MinIndex).
 *          These small numbers can never be merged again, so they would block the board.
 */
static void GameX2_CleanOldNumbers(void)
{
    uint8_t row_i;
    uint8_t column_i;
    for (row_i = 0; row_i < GameX2_Row_Size; row_i++) {
        for (column_i = 0; column_i < GameX2_Column_Size; column_i++) {
            if (GameX2_Matrix[column_i][row_i] != NULL) {
                int8_t lvl_index = GameX2_GetLevelIndex(GameX2_Matrix[column_i][row_i]);
                if (lvl_index < (int8_t)GameX2_NextNumber_MinIndex) {
                    GameX2_Matrix[column_i][row_i] = NULL;
                }
            }
        }
    }
}

static void GameX2_Check_if_new_record_reached(const char * new_val) {
	int8_t actual_val_index = GameX2_GetLevelIndex(new_val);
	int8_t record_index = GameX2_GetLevelIndex(GameX2_Record);
	uint8_t diff;

	if (record_index < actual_val_index)
	{
		GameX2_Record = new_val;
		diff = actual_val_index - record_index;
		// Now, we expect larger newly generated numbers
		GameX2_NextNumber_MinIndex += diff;
		GameX2_CleanOldNumbers();
	}
}

void App_GameX2_Init(void)
{
	// Init game
	ASSERT(GameX2_Column_Size == GameX2_column_right)
	ASSERT(GameX2_Column_Size == GameX2_column_count - 1)

	GameX2_Record = App_GameX2_LevelList[GAME_X2_START_RECORD_INDEX];
	// Put the record to the top center
	GameX2_Matrix[GameX2_column_mid][0] = GameX2_Record;

	// Generate a new number
	GameX2_GenerateNewNumber();

	App_GameX2_Update(ScheduleSource_EventTriggered);

    //TaskHandler_SetTaskPeriodicTime(Task_ButtonPressed, 500);  /* 2 level / second */
}

void App_GameX2_Event(ButtonType_t button, ButtonPressType_t type)
{
	bool_t is_successful = false;

    if (type != ButtonPress_ReleasedContinuous)
    {
        switch (button)
        {
        	// TODO
            case PressedButton_Right:
                /* Right */
                if (type == ButtonPress_Short)
                {
                	is_successful = GameX2_SelectColumn(GameX2_column_right);
                }
                break;

            case PressedButton_Left:
                /* Left */
                if (type == ButtonPress_Short)
                {
                	is_successful = GameX2_SelectColumn(GameX2_column_left);
                }
                break;

            case PressedButton_Up:
                /* Up */
                //App_Elevator_level++;
            	if (type == ButtonPress_Long) {
            		Logic_Display_ChangeState(AppType_MainMenu);
            		is_successful = false;
            	}
                break;

            case PressedButton_Down:
                /* Down */
                if (type == ButtonPress_Short)
                {
                	is_successful = GameX2_SelectColumn(GameX2_column_mid);
                }
                break;

            case PressedButton_Count:
            default:
                /* Error! */
                break;
        }
    }

    if (is_successful == true) {
    	/* So we put successfully the number, let us generated a new number */
    	GameX2_CheckAndDoTheAllMerges();
    	GameX2_GenerateNewNumber();
    }

    App_GameX2_Update(ScheduleSource_EventTriggered);
}


void App_GameX2_Update(ScheduleSource_t source)
{
    UNUSED_ARGUMENT(source);

    char first_line[GAME_X2_STRING_ONE_LINE_MAX_LENGTH * 3];
    bool_t is_there_free_field;

    // Draw screen
    static uint8_t row_i = 0;
    static uint8_t column_i = 0;
	for (row_i=0; row_i < GameX2_Row_Size; row_i++) {
		// one line
		memset(first_line, 0,GAME_X2_STRING_ONE_LINE_MAX_LENGTH);
		for (column_i=0; column_i < GameX2_Column_Size; column_i++) {
		    const char * x2_actual_value = GameX2_Matrix[column_i][row_i];
			uint8_t string_index = column_i * GAME_X2_NUMBER_STRING_LENGTH;
			if (x2_actual_value) // The pointed value (string)
			{
				usnprintf(&first_line[string_index], GAME_X2_NUMBER_STRING_LENGTH + 1, "%s   ", x2_actual_value);
			}
			else
			{
				usnprintf(&first_line[string_index], GAME_X2_NUMBER_STRING_LENGTH + 1, "    ");
			}

		}
		// Display the entire line
		Display_PrintString(first_line, row_i, Font_12x8, Display_NoFormat);
	}

	/* The generated next number into the bottom + center */
	usnprintf(first_line, GAME_X2_NUMBER_STRING_LENGTH * 3, "   %s  ", GameX2_ActualNewNumber);
	//FontFormat_t format;
	//format.Format_Center = 1;
	Display_PrintString(first_line, GAME_X2_BOTTOM_LINE_INDEX_FOR_NEXT_NUMBER, Font_12x8, Display_NoFormat);

#ifdef CONFIG_HW_DISPLAY_TM1637_ENABLE
    //Display_TM1637_Print(first_line);
#endif

    //Display_PrintString(x2_actual_value, 0, Font_32x20, Display_NoFormat);
    Display_Activate();
    TaskHandler_DisableTask(Task_Display); // We dont need periodical refresh

    is_there_free_field = GameX2_CheckAllFields_IsThereFree();
	if (is_there_free_field != true)
	{
		Display_PrintString("Finished", GAME_X2_BOTTOM_LINE_INDEX_FOR_NEXT_NUMBER, Font_12x8, Display_NoFormat);
		Display_Activate();
	}
}

#endif /* CONFIG_FUNCTION_GAME_X2 */

