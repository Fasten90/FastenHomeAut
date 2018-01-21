/*
 *		SecuredDataTypes.h
 *		Created on:		2017-08-24
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Secure data types and handlers
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-08-24
 */

#ifndef COMMON_SECUREDDATATYPES_H_
#define COMMON_SECUREDDATATYPES_H_



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

void SecuredDataTypes_UnitTest(void);



#endif /* COMMON_SECUREDDATATYPES_H_ */
