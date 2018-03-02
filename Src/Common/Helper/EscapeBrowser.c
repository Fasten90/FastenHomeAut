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
static size_t EscapeBrowser_PrintElement(char *str, const char *element, bool isSelected);
static size_t EscapeBroser_HandlerString(char *dst, const char *str);
static bool EscapeBrowser_IncActualLine(void);
static bool EscapeBrowser_DecActualLine(void);
static size_t EscapeBrowser_RefreshOnlyChangedLines(char *str, bool isUpTheOld);
static size_t EscapeBrowser_PrintLogMessage(char *str, const char *msg);



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

	/* Clear screen */
	length += StrCpy(&str[length], ESCAPE_ERASE_CLS);
	length += StrCpy(&str[length], ESCAPE_CURSOR_TOPLEFT);

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



static size_t EscapeBrowser_PrintElement(char *str, const char *element, bool isSelected)
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

#if (ESCAPEBROWSER_COLOR_INACTIVE_ELEMENT_ENABLE == 1)
	/* Inactive is colored too */
	length += ColoredMessage_SendDefaultFormat(&str[length]);
#else
	/* Inactive not colored */
	if (isSelected)
	{
		length += ColoredMessage_SendDefaultFormat(&str[length]);
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
	length += ColoredMessage_SendDefaultFormat(&str[length]);

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



static size_t EscapeBrowser_PrintLogMessage(char *str, const char *msg)
{
	size_t length = 0;
	uint8_t i;

	/* Jump down */
	for (i = 0; i < EscapeBrowser_LineNum - EscapeBrowser_ActualSelectedLine + 1; i++)
	{
		length += StrCpy(&str[length], ESCAPE_CURSORDOWN);
	}

	/* Clear log line */
	length += StrCpy(&str[length], ESCAPE_DELETELINE);

	/* Print */
	length += StrCpy(&str[length], msg);

	/* Jump up */
	for (i = 0; i < EscapeBrowser_LineNum - EscapeBrowser_ActualSelectedLine + 1; i++)
	{
		length += StrCpy(&str[length], ESCAPE_CURSORUP);
	}

	/* Jump to left */
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
		size_t processedChar = 0;
		if (receivedLength > 0)
		{
			char str[ESCAPEBROWSER_PRINT_CHAR_BUFFER] = { 0 };
			processedChar = EscapeBroser_HandlerString(str, recvBuf);
			DebugUart_SendMessageBlocked(str);
		}

		CircularBuffer_DropCharacters(DebugUart.rx, processedChar);
	}
}



static size_t EscapeBroser_HandlerString(char *dst, const char *str)
{
	size_t processedChar = 0;

	/* Has Escape sequence in progress? */
	const char * escapePos = STRING_FindCharacter(str, TERMINAL_KEY_ESCAPESEQUENCE_1);

	if (escapePos !=  NULL)
	{
		/* Has escape sequence */

		/* Drop characters until escape sequence */
		processedChar += (escapePos - str);

		const size_t strLength = StringLength(escapePos);

		if (strLength >= ESCAPEBROWSER_ESCAPE_SEQUENCE_MIN_LENGTH)
		{
			escapePos++;
			if (*escapePos == '[')
			{
				/* We hope, it is cursor: \e[A / B / C / D */
				escapePos++;
				switch (*escapePos)
				{
					case 'A':
						if (EscapeBrowser_DecActualLine())
							EscapeBrowser_RefreshOnlyChangedLines(dst, false);
						break;

					case 'B':
						if (EscapeBrowser_IncActualLine())
							EscapeBrowser_RefreshOnlyChangedLines(dst, true);
						break;

					case 'C':
						EscapeBrowser_PrintLogMessage(dst, "Right");
						break;

					case 'D':
						EscapeBrowser_PrintLogMessage(dst, "Left");
						break;

					default:
						break;
				}

				processedChar += 3; /* "\e[x" */
			}
			else
			{
				/* Wrong char after escape */
				processedChar += 2; /* "\r[" */
			}
		}
		else
		{
			/* Too short escape string... Wait continue */
			/* Do not add more */
		}
	}
	else
	{
		/* There is no escape */
		/* TODO: Handle normal message ? */
		char logMsg[ESCAPEBROWSER_LOG_MESSAGE_MAX_LENGTH];

		switch (*str)
		{
			case '\r':
			case '\n':
				/* Received enter */
				usnprintf(logMsg, ESCAPEBROWSER_LOG_MESSAGE_MAX_LENGTH,
						"Executed command: %20s",
						EscapeBrowser_GetLine(EscapeBrowser_ActualSelectedLine));
				/* TODO: Execute the command */
				break;

			default:
				usnprintf(logMsg, ESCAPEBROWSER_LOG_MESSAGE_MAX_LENGTH,
						"Pressed character: \"%c\"",
						*str);
				break;
		}

		EscapeBrowser_PrintLogMessage(dst, logMsg);
		/* TODO: Only one character processed! */
		processedChar += 1;
	}

	return processedChar;
}



#endif /* #ifdef CONFIG_MODULE_ESCAPEBROWSER_ENABLE */
