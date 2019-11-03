/*
 *    RemoteController.c
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
#include "RemoteController.h"
#include "Button.h"
#include "TaskList.h"
#include "DebugUart.h"
#include "StringHelper.h"


#if defined(CONFIG_MODULE_ESP8266_ENABLE)
#define SEND_MESSAGE_TO_CAR(_msg)        ESP8266_RequestSendTcpMessage(_msg)
#elif defined(CONFIG_MODULE_BLUETOOTH_ENABLE)
#include "Bluetooth_HC05.h"
#define SEND_MESSAGE_TO_CAR(_msg)        Bluetooth_SendMessage(_msg)
#else
#define SEND_MESSAGE_TO_CAR(_msg)        DebugUart_SendMessage(_msg)
#endif



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



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



void Logic_RemoteController_Button(ButtonType_t button, ButtonPressType_t type)
{
    UNUSED_ARGUMENT(type);

    /* Check buttons */
    if (button == PressedButton_Right)
    {
        /* Right */

        if (Car_TurningState == Car_Turning_Right)
        {
            /* Larger right */
            Car_Turning_ActualValue -= Car_Turning_IncrementValue;
            /* Check limit */
            if (Car_Turning_ActualValue < Car_Turning_MinValue)
            {
                Car_Turning_ActualValue = Car_Turning_MinValue;
            }
        }
        else if (Car_TurningState == Car_Turning_Straight)
        {
            /* Start right turning */
            Car_Turning_ActualValue = Car_Turning_StraightValue + Car_Turning_IncrementValue;
            Car_TurningState = Car_Turning_Right;
        }
        else
        {
            /* Straight */
            Car_Turning_ActualValue = Car_Turning_StraightValue;
            Car_TurningState = Car_Turning_Straight;
        }
    }
    else if (button == PressedButton_Left)
    {
        /* Left */

        if (Car_TurningState == Car_Turning_Left)
        {
            /* Larger left */
            Car_Turning_ActualValue += Car_Turning_IncrementValue;
            /* Check limit */
            if (Car_Turning_ActualValue > Car_Turning_MaxValue)
            {
                Car_Turning_ActualValue = Car_Turning_MaxValue;
            }
        }
        else if (Car_TurningState == Car_Turning_Straight)
        {
            /* Start left turning */
            Car_Turning_ActualValue = Car_Turning_StraightValue + Car_Turning_IncrementValue;
            Car_TurningState = Car_Turning_Left;
        }
        else
        {
            /* Straight */
            Car_Turning_ActualValue = Car_Turning_StraightValue;
            Car_TurningState = Car_Turning_Straight;
        }
    }
    else if (button == PressedButton_Up)
    {
        /* Up */

        if (Car_BackForwardState == Car_DcForward_Fordward)
        {
            /* Larger speed */
            Car_DcForward_ActualValue += Car_DcForward_IncrementValue;
            /* Check limit */
            if (Car_DcForward_ActualValue > Car_DcForward_ForwardMaxValue)
            {
                Car_DcForward_ActualValue = Car_DcForward_ForwardMaxValue;
            }
        }
        else if (Car_BackForwardState == Car_DcForward_Stop)
        {
            /* Start go */
            Car_DcForward_ActualValue = Car_DcForward_FordwardStandardValue;
            Car_BackForwardState = Car_DcForward_Fordward;
        }
        else
        {
            /* Stop */
            Car_DcForward_ActualValue = Car_DcForward_StopValue;
            Car_BackForwardState = Car_DcForward_Stop;
        }
    }
    else if (button == PressedButton_Down)
    {
        /* Down */
        if (Car_BackForwardState == Car_DcForward_Back)
        {
            /* Larger speed */
            Car_DcForward_ActualValue -= Car_DcForward_IncrementValue;
            /* Check limit */
            if (Car_DcForward_ActualValue < Car_DcForward_ForwardMinValue)
            {
                Car_DcForward_ActualValue = Car_DcForward_ForwardMinValue;
            }
        }
        else if (Car_BackForwardState == Car_DcForward_Stop)
        {
            /* Start go */
            Car_DcForward_ActualValue = Car_DcForward_BackStandardValue;
            Car_BackForwardState = Car_DcForward_Back;
        }
        else
        {
            /* Stop */
            Car_DcForward_ActualValue = Car_DcForward_StopValue;
            Car_BackForwardState = Car_DcForward_Stop;
        }
    }


    bool isChanged = false;
    /* Check, need send command? */
    if (Car_DcForward_PreviousValue != Car_DcForward_ActualValue)
    {
        /* Changed */
        DebugUart_Printf("Changed BackFordward value: previous: %d, now: %d\r\n",
                Car_DcForward_PreviousValue, Car_DcForward_ActualValue);
        Car_DcForward_PreviousValue = Car_DcForward_ActualValue;

        isChanged = true;
    }

    if (Car_Tuning_PreviousValue != Car_Turning_ActualValue)
    {
        /* Changed */
        DebugUart_Printf("Changed Turning value: previous: %d, now: %d\r\n",
                Car_Tuning_PreviousValue, Car_Turning_ActualValue);
        Car_Tuning_PreviousValue = Car_Turning_ActualValue;

        isChanged = true;
    }

    if (isChanged)
        Logic_RemoteController_SendMessage();
    else
        DebugUart_SendLine("Remote car state not changed");
}



void Logic_RemoteController_SendMessage(void)
{
    /* Send message */
#if 0
    const uint8_t msgLength = 30;
    char msg[msgLength];
    usnprintf(msg, msgLength, "motor dc %d\r\n", Car_DcForward_ActualValue);
    SEND_MESSAGE_TO_CAR(msg);

    /* Send message on WiFi */
    const uint8_t msgLength = 30;
    char msg[msgLength];
    usnprintf(msg, msgLength, "motor servo %d\r\n", Car_Turning_ActualValue);
    SEND_MESSAGE_TO_CAR(msg);
#endif
    const uint8_t msgLength = 40;
    char msg[msgLength];
    usnprintf(msg, msgLength, "motor %d %d\r\n", Car_DcForward_ActualValue, Car_Turning_ActualValue);
    SEND_MESSAGE_TO_CAR(msg);

    /* For periodical message */
    TaskHandler_SetTaskPeriodicTime(Task_RemoteController, 300);
}
#endif    /* CONFIG_FUNCTION_REMOTECONTROLLER */

