/*
 *		WebpageHandler.c
 *		Created on:		2018-03-04
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2018-03-04
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "Debug.h"
#include "DebugList.h"
#include "WebpageHandler.h"

#ifdef CONFIG_MODULE_WEBPAGE_ENABLE



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief  WebpageHandler initialization
 */
void WebpageHandler_Init(void)
{

}



void WebpageHandler_GetRequrest(const char *msg)
{
#warning "Implement it"
(void)msg;

	/* Step on the webpage list */

	Debug_Print(Debug_WebPage, "Received get request");
}



#endif	/* #ifdef CONFIG_MODULE_WEBPAGE_ENABLE */
