/*
 *    WebpageHandler.h
 *    Created on:   2018-03-04
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef WEBPAGEHANDLER_H_
#define WEBPAGEHANDLER_H_




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
#define WEBPAGEHANDLER_MSG_COPY            (1)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef struct
{
    const char * webpageName;
    const char * webpageContain;
    const size_t webpageLength;
    const char * webpageType;
} WebpageInformation_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void WebpageHandler_Init(void);
size_t WebpageHandler_GetRequrest(const char *request, char *resp);



#endif /* WEBPAGEHANDLER_H_ */
