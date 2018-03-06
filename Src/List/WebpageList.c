/*
 *		WebpageList.c
 *		Created on:		2018-03-05
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2018-03-05
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"
#include "WebpageList.h"


#ifdef CONFIG_MODULE_WEBPAGE_ENABLE


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

const char WebpageList_IndexHtml[] =
"<html>" \
"<head>" \
"<title>" \
"</title>" \
"</head>" \
"<body>" \
"<h1 align=\"center\">Welcome message</h2>" \
"<a href=\"http://www.fasten.e5tv.hu\">Fasten webpage</a>" \
"</body>" \
"</html>";

const size_t WebpageList_IndexHtml_length = sizeof(WebpageList_IndexHtml) - 1;



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
 * \brief  WebpageList initialization
 */
void WebpageList_Init(void)
{
// TODO:
}


#endif	/* #ifdef CONFIG_MODULE_WEBPAGE_ENABLE */
