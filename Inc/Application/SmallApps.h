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

void App_PeriodicalMessageSending_Set(char * msg);
void App_PeriodicalMessageSending_Run(void);


void App_DisplayInput_Init(void);
void App_DisplayInput_Event(ButtonType_t button, ButtonPressType_t type);
void App_DisplayInput_Update(ScheduleSource_t source);


void App_DisplayCarAnimation_Init(void);
void App_DisplayCarAnimation_Event(ButtonType_t button, ButtonPressType_t type);
void App_DisplayCarAnimation_Update(ScheduleSource_t source);


void App_DisplayLargeClock_Init(void);
void App_DisplayLargeClock_Event(ButtonType_t button, ButtonPressType_t type);
void App_DisplayLargeClock_Update(ScheduleSource_t source);



#endif /* SMALLAPPS_H_ */
