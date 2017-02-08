/*
 *		raspberrypi.h
 *
 *		Created on:		2015
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 8.
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
