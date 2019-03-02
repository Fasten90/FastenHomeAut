/*
 *        SWO.h
 *        Created on:        2016-12-14
 *        Author:            Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        SWO (Serial Wire Output)
 *        Target:            STM32Fx
 *        Version:        v1
 *        Last modified:    2016-12-14
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
