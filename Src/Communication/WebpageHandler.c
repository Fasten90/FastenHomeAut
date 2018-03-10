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

extern const WebpageInformation_t WebpageList[];
extern const uint8_t WebpageList_Count;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

#if (WEBPAGEHANDLER_MSG_COPY == 1)
static size_t WebpageHandler_SendResponse(char *resp, const char *webpage, const size_t webpageLength, const char *type);
static size_t WebpageHandler_SendHeader(char *resp, size_t webpageLength, const char *type);
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
	uint8_t i;
	bool isFound = false;

	/* Step through the list */
	for (i = 0; i < WebpageList_Count; i++)
	{
		/* Search file name (from the list) in the GET request */
		if (STRING_FindString(request, WebpageList[i].webpageName) != NULL)
		{
			/* Found file name, e.g.: "index.html" */
			WebpageHandler_SendResponse(resp, WebpageList[i].webpageContain, WebpageList[i].webpageLength, WebpageList[i].webpageType);
			/* Debug print */
			Debug_Printf(Debug_WebPage, "Requested and sent %s webpage/file, length: %d", WebpageList[i].webpageName, WebpageList[i].webpageLength);
			isFound = true;
			break;
		}
	}

	/* If not found... */
	if (!isFound)
	{
		// TODO: 404 page
		Debug_Print(Debug_WebPage, "Received wrong get request");
		StrCpy(resp, "Empty webpage");
	}
}



#if (WEBPAGEHANDLER_MSG_COPY == 1)
static size_t WebpageHandler_SendResponse(char *resp, const char *webpage, const size_t webpageLength, const char *type)
{
	size_t length = 0;

	length += WebpageHandler_SendHeader(&resp[length], webpageLength, type);
	length += StrCpy(&resp[length], webpage);

	return length;
}



static size_t WebpageHandler_SendHeader(char *resp, size_t webpageLength, const char *type)
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
			"Content-Type: %s\r\n" // "Content-Type: text/html\r\n" or "Content-Type: image/x-icon\r\n"
			"\r\n",
			//"X-Pad: avoid browser bug\r\n"
			webpageLength,
			type
	);
}
#else /* #if (WEBPAGEHANDLER_MSG_COPY == 1) */


static void WebpageHandler_SendResponse(char *resp, const char *webpage, const size_t webpageLength)
{
	// TODO: Implement it
	#warning "Not implemented function"
}

#endif


#endif	/* #ifdef CONFIG_MODULE_WEBPAGE_ENABLE */
