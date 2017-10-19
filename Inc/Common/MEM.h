/*
 *		MEM.h
 *		Created on:		2016-08-31
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		MEM functions
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-07-15
 */

#ifndef MEM_H_
#define MEM_H_



/*------------------------------------------------------------------------------
 *	Includes
 *----------------------------------------------------------------------------*/

#include "GenericTypeDefs.h"


#define	CONFIG_MEM_CHECK_POINTERS		1
#define CONFIG_MEM_CHECK_POINTER_RANGE	1

#define CONFIG_MEM_STACK_GUARD_LENGTH	(1000U)
#define CONFIG_MEM_STACK_GUARD_VALUE	(0xEF)



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void * memcpy(void * destination, const void * source, size_t size);
void * memset(void * ptr, int value, size_t size);
void * memmove(void * destination, const void * source, size_t size);
void * meminit(void * ptr, size_t num);
int memcmp(const void * ptr1, const void * ptr2, size_t size);

void mem_StackFillWithGuardValues(void);
void mem_CheckStackGuardValues(void);

bool mem_CheckPointer(void * pnt, size_t size);

void mem_UnitTest(void);



#endif /* MEM_H_ */
