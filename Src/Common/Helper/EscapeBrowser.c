/*
 *		EscapeBrowser.c
 *		Created on:		2018-02-26
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2018-02-26
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "StringHelper.h"
#include "EscapeBrowser.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

static uint8_t EscapeBrowser_ActualSelectedLine = 0;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static size_t EscapeBrowser_PrintBorder(char * str);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief  EscapeBrowser initialization
 */
void EscapeBrowser_Init(void)
{
// TODO: ...
}



size_t EscapeBrowser_PrintNewLineList(char *str, const char **list)
{
	uint8_t i;
	size_t length = 0;

	for (i = 0; (i < ESCAPEBROWSER_LIST_NUM_MAX) && (list[i] != NULL); i++)
	{
		// Print an element
		length += EscapeBrowser_PrintBorder(&str[length]);

		length += StrCpyCharacter(&str[length], '|', 1);

		// Print the "text"
		#warning "Correct this! +define and normal situation: send inactive line with simple, and selected line with format (like this)"
		length += ColoredMessage_SendBackgroundAndTextColor(
				&str[length],
				(i == EscapeBrowser_ActualSelectedLine) ? (ESCAPEBROWSER_ELEMENT_ACTIVE_BACKGROUND) : (ESCAPEBROWSER_ELEMENT_INACTIVE_BACKGROUND),
				(i == EscapeBrowser_ActualSelectedLine) ? (ESCAPEBROWSER_ELEMENT_ACTIVE_TEXTCOLOR) : (ESCAPEBROWSER_ELEMENT_INACTIVE_TEXTCOLOR)
				);

		length += StrCpyFixLengthWithFillCharacter(&str[length], list[i], ESCAPEBROWSER_ELEMENT_LENGTH_MAX, ' ');

		ColoredMessage_SendDefaultFormat(&str[length]);
		length += 4;

		length += StrCpyCharacter(&str[length], '|', 1);

		length += StrCpy(&str[length], "\r\n");
	}

	// Bottom
	length += EscapeBrowser_PrintBorder(&str[length]);

	return length;
}



static size_t EscapeBrowser_PrintBorder(char * str)
{
	size_t length = 0;

	length += StrCpyCharacter(&str[length], '+', 1);
	length += StrCpyCharacter(&str[length], '-', ESCAPEBROWSER_ELEMENT_LENGTH_MAX);
	length += StrCpyCharacter(&str[length], '+', 1);
	length += StrCpy(&str[length], "\r\n");

	return length;
}



void EscapeBrowser_IncActualLine(void)
{
	EscapeBrowser_ActualSelectedLine++;
}



void EscapeBrowser_DecActualLine(void)
{
	if (EscapeBrowser_ActualSelectedLine > 0)
		EscapeBrowser_ActualSelectedLine--;
}



void EscapeBrowser_RefreshOnlyChangedLines(uint8_t oldLineCnt, uint8_t newLineCnt)
{
	// Jump cursor
	// Reprint old cnt

	// Jump cursor
	// Reprint new cnt
}

