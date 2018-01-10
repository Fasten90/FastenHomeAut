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
#include "board.h"



/*------------------------------------------------------------------------------
 *	Macros
 *----------------------------------------------------------------------------*/

#define	CONFIG_MEM_CHECK_POINTERS		1
#define CONFIG_MEM_CHECK_POINTER_RANGE	1

#define CONFIG_MEM_STACK_GUARD_LENGTH	(1000U)
#define CONFIG_MEM_STACK_GUARD_VALUE	(0xEF)


#if CONFIG_MEM_CHECK_POINTER_RANGE == 1
	#define MEM_ASSERT(_con)							if (!_con) MEM_ERROR_HANDLER();
	#define MEM_IN_FLASH(_pnt, _size)					((uint32_t)_pnt >= MEM_FLASH_START && ((uint32_t)_pnt + _size) < MEM_FLASH_END)
	#define MEM_IN_RAM(_pnt, _size)						((uint32_t)_pnt >= MEM_RAM_START && ((uint32_t)_pnt + _size) < MEM_RAM_END)
	#define MEM_HAS_NOT_OVERLAP(_pnt1, _pnt2, _size)	(((uint32_t)_pnt1 < (uint32_t)_pnt2) ? (((uint32_t)_pnt1 + _size) <= (uint32_t)_pnt2) : (((uint32_t)_pnt2 + _size) <= (uint32_t)_pnt1))

	#define MEM_IN_FLASH_OR_RAM(_pnt, _size)			(MEM_IN_FLASH(_pnt, _size) || MEM_IN_RAM(_pnt, _size))

	#define MEM_ERROR_HANDLER()							Error_Handler()
#else
	#define MEM_ASSERT(_con)
	#define MEM_IN_RAM(_pnt, _size)						(true)
#endif



///< Make address to aligned address (upward rounding to word aligned)
// TODO: It is architecture dependent
#define MEM_MAKE_ALIGNED_ADDRESS(_address)				(((uint32_t)_address % 4) ? ((uint32_t)_address + (4 - ((uint32_t)_address % 4))) : ((uint32_t)_address))



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

void MEM_UnitTest(void);



#endif /* MEM_H_ */
