/*
 *    SWO.h
 *    Created on:   2016-12-14
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     SWO 
 *    Target:       STM32Fx
 */

#ifndef SWO_H_
#define SWO_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "GenericTypeDefs.h"



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

uint8_t SWO_SendMessage(const char *message);
uint8_t SWO_SendChar(uint8_t ch);



#endif /* SWO_H_ */
