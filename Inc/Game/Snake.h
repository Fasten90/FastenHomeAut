/*
 *    Snake.h
 *    Created on:   2017-09-06
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Snake game
 *    Target:       STM32Fx
 */

#ifndef GAME_SNAKE_H_
#define GAME_SNAKE_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
    Step_Unknown,

    // "User buttons"
    Step_Up,
    Step_Down,
    Step_Right,
    Step_Left,

    Step_Gift,

    // Do not use! Only for checking
    Step_Count
} SnakeStep_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Snake_Init(void);
void Snake_Step(SnakeStep_t step);
void Snake_Draw(void);
SnakeStep_t Snake_GetLastStep(void);



#endif /* GAME_SNAKE_H_ */
