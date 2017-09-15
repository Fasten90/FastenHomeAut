/*
 *		GenericTypeDefs.h
 *
 *		Created on:		2017. febr. 27.
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 27.
 */

#ifndef GENERICTYPEDEFS_H_
#define GENERICTYPEDEFS_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"


// bool typedef
#include <stdbool.h>



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/// Integer typedefs

typedef unsigned char				uint8_t;
typedef signed char					int8_t;
typedef short unsigned int			uint16_t;
typedef short signed int			int16_t;
#ifndef CONFIG_USE_COMPILER_GCC
typedef unsigned int				uint32_t;
typedef signed int					int32_t;
#endif


/// Other typedefs

typedef int8_t						char8_t;
typedef float						float32_t;
typedef bool						bool_t;



#endif /* GENERICTYPEDEFS_H_ */
