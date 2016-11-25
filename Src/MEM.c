/*
 * MEM.c
 *
 *  Created on: Aug 31, 2016
 *      Author: Vizi Gábor
 */


#include <stdlib.h>	// For size_t
#include <stdint.h>	// For uintx_t
#include "MEM.h"


void * memcpy ( void * destination, const void * source, size_t num )
{
	uint16_t i;
	uint32_t * dest = destination;
	const uint32_t * src = source;

	for ( i=0; i < num; i++)
	{
		dest[i]=src[i];
	}

	return NULL;

}


void * memset ( void * ptr, int value, size_t num )
{
	uint16_t i;
	uint32_t * dest = ptr;

	for ( i=0; i < num; i++)
	{
		dest[i]=value;
	}

	return NULL;

}



void * memmove ( void * destination, const void * source, size_t num )
{
	uint16_t i;
	uint32_t * dest = destination;
	uint32_t * src = (uint32_t *)source;

	for ( i=0; i < num; i++)
	{
		dest[i]=src[i];
		src[i]=0;
	}

	return NULL;

}



// TODO: Vagy Makróval megcsinálni?
void * meminit ( void * ptr, size_t num )
{

	return memset(ptr,0,num);

}


