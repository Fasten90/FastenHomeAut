/*
 *    Snake.h
 *    Created on:   2017-09-06
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Snake game
 *    Target:       STM32Fx
 */

#ifndef SNAKE_H_
#define SNAKE_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"
#include "Button.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
    Step_Unknown,

    /* "User buttons" */
    Step_Up,
    Step_Down,
    Step_Right,
    Step_Left,

    Step_Gift,

    /* Do not use! Only for checking */
    Step_Count
} SnakeStep_t;

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

void Snake_Init(void);
void Logic_Display_Snake(ScheduleSource_t source);
void Snake_Event(ButtonType_t button, ButtonPressType_t type);

void Snake_Step(SnakeStep_t step);
void Snake_Draw(void);
SnakeStep_t Snake_GetLastStep(void);




#endif /* SNAKE_H_ */
