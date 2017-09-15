/*
 *		RaspberryPI.h
 *		Created on:		2016-01-01
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Raspberry Pi communication
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-08
 */

#ifndef RASPBERRYPI_H_
#define RASPBERRYPI_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "include.h"
#include "HomeAutMessage.h"	// TODO: delete



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

// TODO: Feloldani a HomeAut_FunctionType, HomeAut_DataType hibát
/*void RASPBERRYPI_SendMessage(uint8_t myAddress,
		HomeAut_FunctionType functionType, HomeAut_DataType dataType , float data);
		*/
void RASPBERRYPI_SendMessage(uint8_t myAddress, uint8_t functionType, uint8_t dataType, float data);



#endif /* RASPBERRYPI_H_ */
