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
#include "StringHelper.h"
#include "WebpageHandler.h"
#include "WebpageList.h"

#ifdef CONFIG_MODULE_WEBPAGE_ENABLE



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const char WebpageList_IndexHtml[];



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void WebpageHandler_SendResponse(char *resp, const char *webpage);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief  WebpageHandler initialization
 */
void WebpageHandler_Init(void)
{
	// TODO: Create init function if need
}



void WebpageHandler_GetRequrest(const char *request, char *resp)
{
	(void)request;

	/* TODO: Step on the webpage list */

	/* TODO: These are test codes, refactor and make securable! */
	if (STRING_FindString(request, "index.html") != NULL)
	{
		/* Found index.html */
		WebpageHandler_SendResponse(resp, WebpageList_IndexHtml);
	}

	Debug_Print(Debug_WebPage, "Received get request");
}



static void WebpageHandler_SendResponse(char *resp, const char *webpage)
{
	size_t length = 0;
	// TODO: Do it well
#warning "Drop not necessary elements"
	length += StrCpy(&resp[length],
			"HTTP/1.1 200 OK\r\n"
			"Date: Sun, 18 Oct 2009 08:56:53 GMT\r\n"
			"Server: Apache/2.2.14 (Win32)\r\n"
			//"Last-Modified: Sat, 20 Nov 2004 07:16:26 GMT\r\n"
			//"ETag: \"10000000565a5-2c-3e94b66c2e680\"\r\n"
			"Accept-Ranges: bytes\r\n"
			"Content-Length: 74\r\n"
			"Connection: close\r\n"
			"Content-Type: text/html\r\n"
			"\r\n"
			//"X-Pad: avoid browser bug\r\n"
	);
	length += StrCpy(&resp[length], webpage);

}


#endif	/* #ifdef CONFIG_MODULE_WEBPAGE_ENABLE */
