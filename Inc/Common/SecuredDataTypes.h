/*
 *		SecuredDataTypes.h
 *
 *		Created on:		2017. aug. 24.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. aug. 24.
 */

#ifndef COMMON_SECUREDDATATYPES_H_
#define COMMON_SECUREDDATATYPES_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef struct
{
	bool isEn;
	bool negIsEn;
} SecuredBool_t;

typedef struct
{
	uint32_t data;
	uint32_t negData;
} SecuredUint32_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

bool Get_SecuredBool(SecuredBool_t *data);
void Set_SecuredBool(SecuredBool_t *data, bool value);

uint32_t Get_SecuredUint32(SecuredUint32_t *data);
void Set_SecuredUint32(SecuredUint32_t *data, uint32_t value);



#endif /* COMMON_SECUREDDATATYPES_H_ */
