/*
 *    ButtonSimulator.h
 *    Created on:   2019-03-12
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef BUTTONSIMULATOR_H_
#define BUTTONSIMULATOR_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum {
    Key_None,
    Key_Up,
    Key_Down,
    Key_Right,
    Key_Left
} ButtonSimulator_Key_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern bool ButtonSimulator_IsEnabled;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void ButtonSimulator_Set(bool newValue);
bool_t ButtonSimulator_ProcessChar(char_t * str);



#endif /* BUTTONSIMULATOR_H_ */
