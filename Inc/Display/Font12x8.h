/*
 *        Font12x8.h
 *
 *        Created on:        2017. márc. 27.
 *        Author:            Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        -
 *        Target:            STM32Fx
 *        Version:        -
 *        Last modified:    2017. márc. 27.
 */

#ifndef MODULES_FONT12X8_H_
#define MODULES_FONT12X8_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define FONT_12X8_NUM            ( 128 )
#define FONT_12X8_STORE_SIZE    ( 12 )

#define FONT_12X8_HEIGHT        ( 12 )
#define FONT_12X8_WIDTH            ( 8 )


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const uint8_t Font12x8[FONT_12X8_NUM][FONT_12X8_STORE_SIZE];



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/



#endif /* MODULES_FONT12X8_H_ */
