/*
 *    RemoteController.h
 *    Created on:   2019-03-22
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef REMOTECONTROLLER_H_
#define REMOTECONTROLLER_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "Button.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



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



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Logic_RemoteController_Button(ButtonType_t button, ButtonPressType_t type);
void Logic_RemoteController_SendMessage(void);



#endif /* REMOTECONTROLLER_H_ */
