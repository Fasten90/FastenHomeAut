/*
 *		WebpageList.c
 *		Created on:		2018-03-05
 *		Author:			Vizi Gabor
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

///< favico file:
const char WebpageList_favico[] =
{ 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x20, 0x20, 
0x02, 0x00, 0x01, 0x00, 0x01, 0x00, 0x30, 0x01, 
0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00, 
0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 
0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFC, 0x07, 0xFF, 0xFF, 0xF0, 
0x01, 0xFF, 0xFF, 0xC0, 0x00, 0x7F, 0xFF, 0x01, 
0xF0, 0x1F, 0xFC, 0x1F, 0xFE, 0x0F, 0xF8, 0x3F, 
0xFF, 0x87, 0xF0, 0xFF, 0xFF, 0xC3, 0xE1, 0xFF, 
0xE0, 0xE1, 0xC1, 0xFF, 0xF0, 0xF1, 0xC7, 0xFF, 
0xF0, 0xFF, 0x8F, 0xFF, 0xE6, 0xFF, 0x8F, 0xF3, 
0xCF, 0xFF, 0x8F, 0xF1, 0x9F, 0xFF, 0x8F, 0xF8, 
0x3F, 0xFF, 0x8C, 0xFC, 0x7F, 0xFF, 0x8C, 0x7C, 
0x3F, 0xFF, 0x8E, 0x39, 0x1F, 0xFF, 0x8F, 0x13, 
0x8F, 0xFF, 0x87, 0x87, 0xCF, 0xFF, 0x87, 0xC7, 
0xFF, 0xFF, 0x83, 0xE3, 0xFF, 0xF3, 0xC3, 0xF1, 
0xFF, 0xE3, 0xE1, 0xF8, 0xFF, 0xC7, 0xE0, 0xFC, 
0x7F, 0x87, 0xF0, 0x7E, 0x7F, 0x0F, 0xF8, 0x3F, 
0xFC, 0x1F, 0xFC, 0x0F, 0xF0, 0x3F, 0xFE, 0x00, 
0x00, 0x7F, 0xFF, 0x00, 0x01, 0xFF, 0xFF, 0xC0, 
0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
/* End of binary file */


const size_t WebpageList_favico_length = sizeof(WebpageList_favico);


///< index file:
const char WebpageList_index[] =
"<!DOCTYPE html>" \
"<html>" \
"<head>" \
"<title>" \
"FastenHomeAut webpage" \
"</title>" \
"</head>" \
"<body>" \
"<h1 align=\"center\">FastenHomeAut</h2>" \
"<a href=\"http://www.fasten.e5tv.hu\">Fasten webpage</a>" \
"<a href=\"Information.html\">Information page</a>" \
"</body>" \
"</html>" \
"" \
;
/* End of HTML file string */


const size_t WebpageList_index_length = sizeof(WebpageList_index);



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
