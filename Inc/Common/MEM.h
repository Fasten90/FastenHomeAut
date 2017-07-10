/*
 * MEM.h
 *
 *  Created on: Aug 31, 2016
 *      Author: Vizi G�bor
 */

#ifndef MEM_H_
#define MEM_H_



/*------------------------------------------------------------------------------
 *	Includes
 *----------------------------------------------------------------------------*/

#include "GenericTypeDefs.h"


#define	CONFIG_MEM_CHECK_POINTERS		1

#define CONFIG_MEM_STACK_GUARD_LENGTH	(1000U)
#define CONFIG_MEM_STACK_GUARD_VALUE	(0xEF)


/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void * memcpy(void * destination, const void * source, size_t num);

void * memset(void * ptr, int value, size_t num);

void * memmove(void * destination, const void * source, size_t num);

void * meminit(void * ptr, size_t num);

int memcmp(const void * ptr1, const void * ptr2, size_t num);

void mem_StackFillWithGuardValues(void);
void mem_CheckStackGuardValues(void);

void MEM_UnitTest(void);



#endif /* MEM_H_ */
