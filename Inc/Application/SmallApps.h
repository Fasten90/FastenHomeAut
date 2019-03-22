/*
 *    SmallApps.h
 *    Created on:   2019-03-22
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef SMALLAPPS_H_
#define SMALLAPPS_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "Button.h"
#include "TaskHandler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK)
typedef enum
{
    DisplayClock_HourAndMinute,
    DisplayClock_Hour,
    DisplayClock_Minute,

    /* Do not use! */
    DisplayClock_Count
} DisplayClock_ChangeState_t;
#endif



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Logic_SetPeriodicalMessageSendg(char * msg);
void Logic_PeriodicalSending(void);


void Logic_Display_Input(ScheduleSource_t source);
void App_DisplayInput_Init(void);
void App_DisplayInput_Event(ButtonType_t button, ButtonPressType_t type);


void Logic_Display_CarAnimation_Event(ButtonType_t button, ButtonPressType_t type);
void Logic_Display_CarAnimation(ScheduleSource_t source);


void Logic_Display_LargeClock_Init(void);
void App_LargeClock_Event(ButtonType_t button, ButtonPressType_t type);
void Logic_Display_LargeClock_Update(ScheduleSource_t source);



#endif /* SMALLAPPS_H_ */
