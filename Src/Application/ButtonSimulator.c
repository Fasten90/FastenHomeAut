/*
 *    ButtonSimulator.c
 *    Created on:   2019-03-12
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "ButtonSimulator.h"
#include "Terminal.h"
#include "Button.h"
#include "Logic.h"
#include "DebugUart.h"


#ifdef CONFIG_MODULE_BUTTONSIMULATOR_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

bool ButtonSimulator_IsEnabled =  false;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void ButtonSimulator_ExecuteKey(ButtonSimulator_Key_t key);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief  ButtonSimulator set
 */
void ButtonSimulator_Set(bool newValue)
{
    ButtonSimulator_IsEnabled = newValue;
}



bool_t ButtonSimulator_ProcessChar(char_t * str)
{
    ButtonSimulator_Key_t key = Key_None;

#ifdef CONFIG_PLATFORM_PC_WINDOWS
    /* TODO: Update the comments */
    if (str[0] == -32)        /* ESC */
    {
        /* 'A' - Up cursor */
        if (str[1] == 72)
        {
            key = Key_Up;
        }
        /* 'B' Down cursor */
        else if (str[1] == 80)
        {
            key = Key_Down;
        }
        /* 'C' - Right cursor */
        else if (str[1] == 77)
        {
            key = Key_Right;
        }
        /* 'D' Left cursor */
        else if (str[1] == 75)
        {
            key = Key_Left;
        }
        else
        {
            key = Key_None;
        }
    }
#else
    if (str[0] == TERMINAL_KEY_ESCAPESEQUENCE_1)        /* ESC */
    {
        if (str[1] == TERMINAL_KEY_ESCAPESEQUENCE_2)    /* '[', escape sequence 2. letter */
        {
            /* This is an escape sequence */
            /* 'A' Up cursor = previous History command */
            if (str[2] == 'A')
            {
                key = Key_Up;
            }
            /* 'B' Down cursor        // next History command */
            else if (str[2] == 'B')
            {
                key = Key_Down;
            }
            /* 'C' - Right cursor - Step right */
            else if (str[2] == 'C')
            {
                key = Key_Right;
            }
            /* 'D' Left cursor - Step left */
            else if (str[2] == 'D')
            {
                key = Key_Left;
            }
        }
        else    /* This is not escape sequence */
        {
            key = Key_None;
        }
    }
#endif
    else
    {
        key = Key_None;
    }

    if (key != Key_None)
    {
        ButtonSimulator_ExecuteKey(key);
    }
    else
    {
        /* TODO: Print */
        DebugUart_SendLine("Not arrow key received!");
    }

    /**
     * Return true, if it was arrow key
     */
    return (key != Key_None);
}



static void ButtonSimulator_ExecuteKey(ButtonSimulator_Key_t key)
{
    /**
     * TODO: Add button press time
     */
    switch(key)
    {
        case Key_Up:
            Logic_ButtonEventHandler(PressedButton_Up, ButtonPress_Short);
            break;

        case Key_Down:
            Logic_ButtonEventHandler(PressedButton_Down, ButtonPress_Short);
            break;

        case Key_Right:
            Logic_ButtonEventHandler(PressedButton_Right, ButtonPress_Short);
            break;

        case Key_Left:
            Logic_ButtonEventHandler(PressedButton_Left, ButtonPress_Short);
            break;

        case Key_None:
        default:
            break;
    }
}


#endif /* CONFIG_MODULE_BUTTONSIMULATOR_ENABLE */
