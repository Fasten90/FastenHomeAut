/*
 *		Font32x20.h
 *
 *		Created on:		2017. máj. 12.
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. máj. 12.
 */

#ifndef MODULES_FONT32X20_H_
#define MODULES_FONT32X20_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "compiler.h"


// This font contain only 11 "font character": 0-9 (numbers) + ':' character for hour displaying
#define FONT_32X20_NUM				( 11 )
#define FONT_32X20_STORE_SIZE		( 20 )

#define FONT_32X20_HEIGHT			( 32 )
#define FONT_32X20_WIDTH			( 20 )


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const uint32_t Font32x20[FONT_32X20_NUM][FONT_32X20_STORE_SIZE];


/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/



#endif /* MODULES_FONT32X20_H_ */
