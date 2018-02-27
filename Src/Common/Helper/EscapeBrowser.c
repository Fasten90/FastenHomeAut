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

#ifdef CONFIG_MODULE_ESCAPEBROWSER_ENABLE

#include "GenericTypeDefs.h"
#include "StringHelper.h"
#include "EscapeSequence.h"
#include "CircularBuffer.h"
#include "DebugUart.h"
#include "Terminal.h"
#include "EscapeBrowser.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

static uint8_t EscapeBrowser_ActualSelectedLine = 0;
static uint8_t EscapeBrowser_LineNum = 0;
static EscapeBrowser_GetLineFunction EscapeBrowser_GetLine = NULL;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static size_t EscapeBrowser_PrintBorder(char *str);
static size_t EscapeBrowser_PrintElement(char *str, char *element, bool isSelected);
static size_t EscapeBroser_HandlerString(char *dst, char *str);
static bool EscapeBrowser_IncActualLine(void);
static bool EscapeBrowser_DecActualLine(void);
static size_t EscapeBrowser_RefreshOnlyChangedLines(char *str, bool isUpTheOld);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief  EscapeBrowser initialization
 */
void EscapeBrowser_Init(EscapeBrowser_GetLineFunction getFunc)
{
	EscapeBrowser_GetLine = getFunc;
}



size_t EscapeBrowser_PrintNewLineList(char *str)
{
	uint8_t i;
	size_t length = 0;

	if ((str == NULL) || (EscapeBrowser_GetLine == NULL))
		return 0;

	/* Print top border */
	length += EscapeBrowser_PrintBorder(&str[length]);
	/* Step on the string list */

	for (i = 0; (i < ESCAPEBROWSER_LIST_NUM_MAX) && (EscapeBrowser_GetLine(i) != NULL); i++)
	{
		length += EscapeBrowser_PrintElement(&str[length],
				EscapeBrowser_GetLine(i),
				(i == EscapeBrowser_ActualSelectedLine) ? (true) : (false));

		length += StrCpy(&str[length], "\r\n");
	}

	/* Set line num */
	EscapeBrowser_LineNum = i;

	/* Bottom */
	length += EscapeBrowser_PrintBorder(&str[length]);

	/* Set cursor to first line: line num + bottom line + 1 */
	for (i = 0; i < EscapeBrowser_LineNum + 1 + 1; i++)
	{
		length += StrCpy(&str[length], ESCAPE_CURSORUP);
	}

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



static size_t EscapeBrowser_PrintElement(char *str, char *element, bool isSelected)
{
	size_t length = 0;

	/* Print an element */
	length += StrCpyCharacter(&str[length], '|', 1);

	/* Print the "text" */
#if (ESCAPEBROWSER_COLOR_INACTIVE_ELEMENT_ENABLE == 1)
	/* Inactive is colored too */
	length += ColoredMessage_SendBackgroundAndTextColor(
			&str[length],
			(isSelected) ? (ESCAPEBROWSER_ELEMENT_ACTIVE_BACKGROUND) : (ESCAPEBROWSER_ELEMENT_INACTIVE_BACKGROUND),
			(isSelected) ? (ESCAPEBROWSER_ELEMENT_ACTIVE_TEXTCOLOR) : (ESCAPEBROWSER_ELEMENT_INACTIVE_TEXTCOLOR)
			);
#else
	/* Inactive not colored */
	if (isSelected)
	{
		length += ColoredMessage_SendBackgroundAndTextColor(
				&str[length],
				ESCAPEBROWSER_ELEMENT_ACTIVE_BACKGROUND,
				ESCAPEBROWSER_ELEMENT_ACTIVE_TEXTCOLOR
				);
	}
#endif

	length += StrCpyFixLengthWithFillCharacter(&str[length], element, ESCAPEBROWSER_ELEMENT_LENGTH_MAX, ' ');
#warning "missed 0"
	str[length] = '\0';

#if (ESCAPEBROWSER_COLOR_INACTIVE_ELEMENT_ENABLE == 1)
	/* Inactive is colored too */
	ColoredMessage_SendDefaultFormat(&str[length]);
	length += 4;
#else
	/* Inactive not colored */
	if (isSelected)
	{
		ColoredMessage_SendDefaultFormat(&str[length]);
		length += 4;
	}
#endif

	length += StrCpyCharacter(&str[length], '|', 1);

	return length;
}



static bool EscapeBrowser_IncActualLine(void)
{
	bool isChanged = false;

	if (EscapeBrowser_ActualSelectedLine < (EscapeBrowser_LineNum - 1))
	{
		EscapeBrowser_ActualSelectedLine++;
		isChanged = true;
	}

	return isChanged;
}



static bool EscapeBrowser_DecActualLine(void)
{
	bool isChanged = false;

	if (EscapeBrowser_ActualSelectedLine > 0)
	{
		EscapeBrowser_ActualSelectedLine--;
		isChanged = true;
	}

	return isChanged;
}



static size_t EscapeBrowser_RefreshOnlyChangedLines(char *str, bool isUpTheOld)
{
	size_t length = 0;

	/* Not need jump/change line, we are stay here */

	/* Reprint old cnt */
	ColoredMessage_SendDefaultFormat(&str[length]);
	length += 4;

	length += EscapeBrowser_PrintElement(&str[length],
			EscapeBrowser_GetLine((isUpTheOld) ? (EscapeBrowser_ActualSelectedLine-1) : (EscapeBrowser_ActualSelectedLine+1)),
			false);

	/* Jump cursor (back) */
	length += StrCpy(&str[length],
			(isUpTheOld) ? ESCAPE_CURSORDOWN : ESCAPE_CURSORUP);

	length += StrCpy(&str[length], ESCAPE_CURSORLEFTLOTOF);

	/* Reprint new cnt */
	length += EscapeBrowser_PrintElement(&str[length],
			EscapeBrowser_GetLine(EscapeBrowser_ActualSelectedLine),
			true);

	length += StrCpy(&str[length], ESCAPE_CURSORLEFTLOTOF);

	return length;
}



void EscapeBrowser_ProcessReceivedCharaters(void)
{
	char recvBuf[ESCAPEBROWSER_PROCESS_CHAR_BUFFER];

	/* Received new character? */
	if (CircularBuffer_IsNotEmpty(DebugUart.rx))
	{
		/* Copy received message to buffer */
		CircularBuffer_GetString(DebugUart.rx, recvBuf, ESCAPEBROWSER_PROCESS_CHAR_BUFFER);

		/* Check the received string */
		size_t receivedLength = StringLength(recvBuf);
		if (receivedLength > 0)
		{
			char str[ESCAPEBROWSER_PRINT_CHAR_BUFFER];
			EscapeBroser_HandlerString(str, recvBuf);
			DebugUart_SendMessageBlocked(str);
		}

		/* TODO: Not a beautiful solution... */
		CircularBuffer_DropCharacters(DebugUart.rx, receivedLength);
	}
}



static size_t EscapeBroser_HandlerString(char *dst, char *str)
{
	size_t length = 0;

	/* Has Escape sequence in progress? */
	const char * escapePos = STRING_FindCharacter(str, TERMINAL_KEY_ESCAPESEQUENCE_1);
#warning "Length checking"
	if (escapePos !=  NULL)
	{
		/* Has escape sequence */
		escapePos++;
		if (*escapePos == '[')
		{
			/* We hope, it is cursor: \e[A / B / C / D */
			escapePos++;
			switch (*escapePos)
			{
#warning "Check the line validality (first - last)"
				case 'A':
					if (EscapeBrowser_DecActualLine())
						length += EscapeBrowser_RefreshOnlyChangedLines(dst, false);
					break;

				case 'B':
					if (EscapeBrowser_IncActualLine())
						length += EscapeBrowser_RefreshOnlyChangedLines(dst, true);
					break;

				case 'C':
					DebugUart_SendLine("Pressed C");
					break;

				case 'D':
					DebugUart_SendLine("Pressed D");
					break;

				default:
					break;
			}
		}
	}

	return length;
}



#endif /* #ifdef CONFIG_MODULE_ESCAPEBROWSER_ENABLE */
