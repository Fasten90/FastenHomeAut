/*
 * MEM.c
 *
 *  Created on: Aug 31, 2016
 *      Author: Vizi Gábor
 */


#include <stdlib.h>	// For size_t
#include <stdint.h>	// For uintx_t
#include "MEM.h"



/**
 * \brief	Memory copy
 * \param[out]	destination	where to copy
 * \param[in]	source		from copy
 * \param[in]	num			How many length to copy (in bytes)?
 */
void * memcpy (void * destination, const void * source, size_t num)
{
	size_t i;
	uint8_t *dest = destination;
	const uint8_t *src = source;

	for (i=0; i < num; i++)
	{
		dest[i] = src[i];
	}

	return NULL;

}



/**
 * \brief		Set memory
 * \param[out]	*ptr	Which memory area need set
 * \param[in]	value	With which value
 * \param[in]	num		How many length to set (in bytes)?
 */
void * memset (void * ptr, int value, size_t num)
{
	size_t i;
	uint8_t *dest = ptr;

	for (i=0; i < num; i++)
	{
		dest[i] = (uint8_t)value;
	}

	return NULL;

}



/**
 * \brief	Memory move
 * \param[out]	destination	where to copy
 * \param[in]	source		from copy
 * \param[in]	num			How many length to move (in bytes)?
 */
void * memmove (void * destination, const void * source, size_t num)
{
	size_t i;
	uint8_t *dest = destination;
	uint8_t *src = (uint8_t *)source;

	for (i=0; i < num; i++)
	{
		dest[i] = src[i];
		src[i] = 0;
	}

	return NULL;
}



/**
 * \brief		Initialize memory area with 0
 * \param[out]	*ptr	Which area
 * \param[in]	num		How many length (in bytes)?
 */
void * meminit (void * ptr, size_t num)
{

	return memset(ptr,0,num);

}


