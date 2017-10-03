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

// bool typedef
#include <stdbool.h>
#include "include.h"




/*------------------------------------------------------------------------------
 *  Macros
 *----------------------------------------------------------------------------*/

#define BOOL_MAX	(1)

/*
#define UINT8_MAX	(0xFF)
#define INT8_MAX	(0x7F)
#define UINT16_MAX	(0xFFFF)
#define INT16_MAX	(0x7FFF)
#define UINT32_MAX	(0xFFFFFFFF)
#define INT32_MAX	(0x7FFFFFFF)
*/



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
