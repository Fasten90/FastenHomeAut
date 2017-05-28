/*
 * MEM.h
 *
 *  Created on: Aug 31, 2016
 *      Author: Vizi G�bor
 */

#ifndef MEM_H_
#define MEM_H_



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

//void * memcpy ( void * destination, const void * source, size_t num );
void * memcpy(void * destination, const void * source, size_t num);

//void * memset ( void * ptr, int value, size_t num );
void * memset(void * ptr, int value, size_t num);

void * memmove(void * destination, const void * source, size_t num);

void * meminit(void * ptr, size_t num);

int memcmp(const void * ptr1, const void * ptr2, size_t num);

void mem_StackFillWithGuardValues(void);


#endif /* MEM_H_ */
