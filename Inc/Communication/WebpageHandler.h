/*
 *		WebpageHandler.h
 *		Created on:		2018-03-04
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2018-03-04
 */

#ifndef INC_COMMUNICATION_WEBPAGEHANDLER_H_
#define INC_COMMUNICATION_WEBPAGEHANDLER_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

/*
 * WebpageHandler work:
 * 1 - copy
 * 0 - send with blocked
 */
#define WEBPAGEHANDLER_MSG_COPY			(1)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void WebpageHandler_Init(void);
void WebpageHandler_GetRequrest(const char *request, char *resp);



#endif /* INC_COMMUNICATION_WEBPAGEHANDLER_H_ */
