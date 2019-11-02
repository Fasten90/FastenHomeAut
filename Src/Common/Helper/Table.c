/*
 *    Table.c
 *    Created on:   2017-12-21
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "StringHelper.h"
#include "DebugUart.h"
#include "Table.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void Table_FormatBorder(char * dest, const char * src);
static void Table_FormatHeader(char * dest, const char * src, bool isHeader);
/* TODO: Delete, if not need */
#if 0
static void Table_PrintHeader(char * dest, char * smallHeader, const char * src, bool isHeader, ...);
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief    Print %20c --> 20 times '-'
 */
static void Table_FormatBorder(char * dest, const char * src)
{
    while (*src)
    {
        if (*src == '%')
        {
            /* After '%', check the value */
            src++;
            if (IsDecimalChar(*src))
            {
                uint8_t val1 = DecimalCharToNum(*src);
                /* Check next value */
                src++;
                if (IsDecimalChar(*src))
                {
                    /* 2. char is number */
                    uint8_t val2 = DecimalCharToNum(*src);
                    val1 = val1 * 10 + val2;
                    dest += StrCpyCharacter(dest, '-', val1);
                    src++;
                    if (*src == 'c')
                        src++;
                }
                else if (*src == 'c')
                {
                    /* 2. char is not number, it is 'c' */
                    dest += StrCpyCharacter(dest, '-', val1);
                    src++;
                }
                else
                {
                    /* Error */
                    DEBUG_BREAKPOINT();
                }
            }
            else if (*src == 'c')
            {
                *dest = '-';
                dest++;
                src++;
            }
            else
            {
                /* Not '%', not decimal, not 'c' */
                /* Error */
                DEBUG_BREAKPOINT();
            }
        }
        else if (*src == 'c')
        {
            /* Alone 'c' */
            *dest = '-';
            dest++;
            src++;
        }
        else
        {
            /* Not '%'. E.g. : '-', '+', ... */
            *dest = *src;
            dest++;
            src++;
        }
    }

    *dest = '\0';
}



/**
 * @brief    Convert original data format line to border or header
 *             Original: | %3u | %20s | %9u | %20s | 0x%X | %10s | %20s |
 *             Border: '|' --> '+', ' ' --> '-' , 'usx' --> 'c',  parameters --> '-'
 *            Header: '|' --> '|', 'usx '--> 's', parameters: given, ...
 */
static void Table_FormatHeader(char * dest, const char * src, bool isHeader)
{
    size_t i;
    for (i = 0; src[i] != '\0'; i++)
    {
        if (isHeader)
        {
            /* Header */
            /* E.g. original:    | %3u | %20s | %9u | %20s | 0x%X | %10s | %20s | */
            /* E.g. new:        | %3s | %20s | %9s | %20s | 0x%s | %10s | %20s | */
            if (src[i] == 'u' || src[i] == 's' || src[i] == 'x' || src[i] == 'X' || src[i] == 'd')
            {
                dest[i] = 's';
            }
            /*else if (src[i] == '|' || src[i] == '\r' || src[i] == '\n' || src[i] == ' ')
            {
                dest[i] = src[i];
            }
            else if (
            {
                dest[i] = src[i];
            }*/
            else
            {
                /* Other, e.g.: ' ' (space), '%', '|', decimal (0-9), ... */
                dest[i] = src[i];
            }
        }
        else
        {
            /* Border */
            /* E.g. original:    | %3u | %20s | %9u | %20s | 0x%X | %10s | %20s | */
            /* E.g. new:        +-%3c-+-%20c-+-%9c-+-%20c-+-0x%c-+-%10c-+-%20c-+ */
            if (src[i] == 'u' || src[i] == 's' || src[i] == 'x' || src[i] == 'X' || src[i] == 'd')
            {
                dest[i] = 'c';
            }
            else if (src[i] == '|')
            {
                dest[i] = '+';
            }
            else if (src[i] == ' ')
            {
                dest[i] = '-';
            }
            else
            {
                /* E.g. '%', decimal (0-9), other... */
                dest[i] = src[i];
            }
        }
    }

    dest[i] = '\0';
}



/* TODO: Delete if not need */
#if 0
static void Table_PrintHeader(char * dest, char * smallHeader, const char * src, bool isHeader, ...)
{
    /* 1. conversion */
    Table_FormatHeader(smallHeader, src, isHeader);

    if (isHeader)
    {
        /* Header */
        va_list ap;                                        /* argument pointer */
        va_start(ap, isHeader);                         /* ap on arg */
        string_printf(dest, smallHeader, ap);            /* Separate and process */
        va_end(ap);                                         /* Cleaning after end */
    }
    else
    {
        /* Border */
        Table_FormatBorder(dest, smallHeader);
    }
}
#endif



/**
 * @brief    Print table
 */
void Table_PrintTableWithBorder(const char * fixheader, char * str, uint8_t strMaxLen, char * header, ...)
{
    /* 1. line: border */
    /* tr_PrintHeader(str, header, fixheader, false); */
    Table_FormatHeader(header, fixheader, false);
    Table_FormatBorder(str, header);

    Table_SendLine(str);


    /* 2. line: Header (with columns) */
    Table_FormatHeader(header, fixheader, true);

    va_list ap;                                        /* argument pointer */
    va_start(ap, header);                              /* ap on arg */
    string_printf_safe(str, strMaxLen, header, ap);    /* Separate and process */
    va_end(ap);

    Table_SendLine(str);


    /* 3. line: border */
    /* tr_PrintHeader(str, header, fixheader, false); */
    Table_FormatHeader(header, fixheader, false);
    Table_FormatBorder(str, header);

    Table_SendLine(str);
}


/* TODO: Add UnitTest */
