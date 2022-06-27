/*
 *    CommonIO.h
 *    Created on:   2016-11-28
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Common IO
 *    Target:       STM32Fx
 */

#ifndef COMMONIO_H_
#define COMMONIO_H_


#include "GenericTypeDefs.h"



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
    IO_UNKNOWN,
    IO_INPUT,
    IO_OUTPUT,

    /* Do not use: */
    IO_COUNT
} IO_Type;



typedef enum
{
    OUTPUT_DONTCARE,
    OUTPUT_HIGH,
    OUTPUT_LOW,
    OUTPUT_TOGGLE,
    OUTPUT_STATUS,

    /* Do not use: */
    OUTPUT_COUNT
} Output_Type;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

bool CommonIO_Init(char port, uint8_t pin, IO_Type io);
bool CommonIO_SetOutput(char port, uint8_t pin, Output_Type output);
bool CommonIO_ReadPin(char port, uint8_t pin);



#endif /* COMMONIO_H_ */
