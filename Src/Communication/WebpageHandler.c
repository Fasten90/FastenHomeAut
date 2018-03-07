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
extern size_t WebpageList_IndexHtml_length;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

#if (WEBPAGEHANDLER_MSG_COPY == 1)
static size_t WebpageHandler_SendResponse(char *resp, const char *webpage, const size_t webpageLength);
static size_t WebpageHandler_SendHeader(char *resp, size_t webpageLength);
#endif



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
		WebpageHandler_SendResponse(resp, WebpageList_IndexHtml, WebpageList_IndexHtml_length);
	}
	else if (STRING_FindString(request, "/favicon.ico") != NULL)
	{
		// TODO: Send favico?
		StrCpy(resp, "Favico");
	}
	else
	{
		// TODO: 404 page
		Debug_Print(Debug_WebPage, "Received wrong get request");
		StrCpy(resp, "Empty webpage");
	}
}



#if (WEBPAGEHANDLER_MSG_COPY == 1)
static size_t WebpageHandler_SendResponse(char *resp, const char *webpage, const size_t webpageLength)
{
	size_t length = 0;

	length += WebpageHandler_SendHeader(&resp[length], webpageLength);
	length += StrCpy(&resp[length], webpage);

	Debug_Printf(Debug_WebPage, "Received webpage request, length: %d", length);

	return length;
}



static size_t WebpageHandler_SendHeader(char *resp, size_t webpageLength)
{
	// TODO: Drop not necessary elements
	return usprintf(resp,
			"HTTP/1.1 200 OK\r\n"
			//"Date: Sun, 18 Oct 2009 08:56:53 GMT\r\n"
			//"Server: Apache/2.2.14 (Win32)\r\n"
			//"Last-Modified: Sat, 20 Nov 2004 07:16:26 GMT\r\n"
			//"ETag: \"10000000565a5-2c-3e94b66c2e680\"\r\n"
			"Accept-Ranges: bytes\r\n"
			"Content-Length: %d\r\n"
			"Connection: close\r\n"
			"Content-Type: text/html\r\n"
			"\r\n",
			//"X-Pad: avoid browser bug\r\n"
			webpageLength
	);
}
#else /* #if (WEBPAGEHANDLER_MSG_COPY == 1) */


static void WebpageHandler_SendResponse(char *resp, const char *webpage, const size_t webpageLength)
{

}

#endif


#endif	/* #ifdef CONFIG_MODULE_WEBPAGE_ENABLE */
