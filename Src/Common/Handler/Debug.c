/*
 *    Debug.c
 *    Created on:   2017-08-17
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Debug print
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_MODULE_DEBUG_ENABLE

#include <stdarg.h>/* for "va_list" */
#include "ColoredMessage.h"
#include "StringHelper.h"
#include "DebugUart.h"
#include "DebugList.h"
#include "Debug.h"
#include "Table.h"



/*------------------------------------------------------------------------------
 *  Macros
 *----------------------------------------------------------------------------*/

#define DEBUG_GUARD_VALUE       ((char)0xFF)



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern DebugRecord_t DebugTasks[];



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/* TODO: Modify Debug function for other protocol (like telnet) */


/**
 * @brief       Print fix string
 */
void Debug_Print(Debug_t debugTask, const char *msg)
{
    ASSERT(MEM_IN_FLASH_OR_RAM(msg, 0));
    if ((debugTask >= Debug_Count) || (msg == NULL))
        return;

    if (DebugTasks[debugTask].isEnabled)
    {
#ifdef CONFIG_MODULE_COLOREDMESSAGE_ENABLE
        /* Text color */
        char colorMsg[ESCAPE_FORMAT_STANDARD_STRING_MAX_LENGTH * 2] = { 0 };
        ColoredMessage_SendTextColor(colorMsg, DebugTasks[debugTask].color);
    #ifdef CONFIG_DEBUG_BACKGROUND_ENABLE
        if (DebugTasks[debugTask].background)    /* TODO: Not good idea. Now, we cannot use "black = 0" color in background */
        {
            ColoredMessage_SendBackgroundColor(colorMsg, DebugTasks[debugTask].background);
        }
    #endif
        DebugUart_SendMessage(colorMsg);
#endif

        /* TaskName */
        uprintf("%s: ", DebugTasks[debugTask].name);

        /* Send debug message: */
        DebugUart_SendLine(msg);

#ifdef CONFIG_MODULE_COLOREDMESSAGE_ENABLE
        /* Set default color */
        colorMsg[0] = '\0';    /* Clear colorMsg */
        ColoredMessage_SendDefaultFormat(colorMsg);
        DebugUart_SendMessage(colorMsg);
#endif
    }
}



/**
 * @brief       Debug print
 *              The parameterized debug task is has color and task name, which printed out
 */
void Debug_Printf(Debug_t debugTask, const char *format, ...)
{
    /* Check DebugTasks list size */
    ASSERT(MEM_IN_FLASH_OR_RAM(format, 0));

    if ((debugTask >= Debug_Count) || (format == NULL))
        return;

    if (DebugTasks[debugTask].isEnabled)
    {
#ifdef CONFIG_MODULE_COLOREDMESSAGE_ENABLE
        /* Text color */
        char colorMsg[ESCAPE_FORMAT_STANDARD_STRING_MAX_LENGTH * 2] = { 0 };
        ColoredMessage_SendTextColor(colorMsg, DebugTasks[debugTask].color);
    #ifdef CONFIG_DEBUG_BACKGROUND_ENABLE
        if (DebugTasks[debugTask].background)    /* TODO: Not good idea. Now, we cannot use "black = 0" color in background */
        {
            ColoredMessage_SendBackgroundColor(colorMsg, DebugTasks[debugTask].background);
        }
    #endif
        DebugUart_SendMessage(colorMsg);
#endif

        /* TaskName */
        uprintf("%s: ", DebugTasks[debugTask].name);

        /* Send debug message: */
        /* Working in at: */
        char txBuffer[DEBUGUART_TX_BUFFER_SIZE] = { 0 };

#ifdef CONFIG_DEBUG_MODE
        txBuffer[DEBUGUART_TX_BUFFER_SIZE-1] = DEBUG_GUARD_VALUE;
#endif

        va_list ap;                                     /* argument pointer */
        va_start(ap, format);                           /* ap on arg */
        size_t sentChars = string_printf_safe(txBuffer, DEBUGUART_TX_BUFFER_SIZE-1, format, ap);        /* Separate and process */
        va_end(ap);                                     /* Cleaning after end */

        if (sentChars >= (DEBUGUART_TX_BUFFER_SIZE - 5))
        {
            StrCpy(&txBuffer[DEBUGUART_TX_BUFFER_SIZE-1-6], "[...]");
        }

#ifdef CONFIG_DEBUG_MODE
        if (txBuffer[DEBUGUART_TX_BUFFER_SIZE-1] != DEBUG_GUARD_VALUE)
        {
            /* TODO: Report */
            DEBUG_BREAKPOINT();
        }
#endif

        DebugUart_SendLine(txBuffer);                /* Send on Usart */

#ifdef CONFIG_MODULE_COLOREDMESSAGE_ENABLE
        /* Set default color */
        colorMsg[0] = '\0';    /* Clear colorMsg */
        ColoredMessage_SendDefaultFormat(colorMsg);
        DebugUart_SendMessage(colorMsg);
#endif
    }
}



/**
 * @brief       Enable-Disable debug print
 */
bool Debug_SetEnable(Debug_t task, bool enable)
{
    if (task >= Debug_Count)
        return false;

    DebugTasks[task].isEnabled = enable;

    return true;
}



/**
 * @brief       Enable-Disable debug task with name
 */
bool Debug_SetDebugTaskWithName(char *name, bool enable)
{
    uint8_t i;
    bool result = false;

    for (i = 0; i < Debug_Count; i++)
    {
        if (!StrCmp(name, DebugTasks[i].name))
        {
            /* Found */
            DebugTasks[i].isEnabled = enable;
            result = true;

            break;
        }
    }

    return result;
}



/**
 * @brief       Print DebugList Table Header
 */
static void Debug_PrintDebugListTableHeader(const char * fixheader, char * str, uint8_t strMaxLen, char * header)
{
    Table_PrintTableWithBorder(fixheader, str, strMaxLen, header, "DebugName", "en");
}



/**
 * @brief       Print Debug list
 */
void Debug_PrintDebugList(void)
{
    uint8_t i;
    static const char fixheader[] = "| %20s | %3s |";
    #define DEBUG_STR_LENGTH    ((uint8_t)(2 + 20 + 3 + 3 + 2 + 1))
    char str[DEBUG_STR_LENGTH];
    /* sizeof(fixheader) -- string size */
    char header[sizeof(fixheader)];

    Debug_PrintDebugListTableHeader(fixheader, str, DEBUG_STR_LENGTH, header);

    for (i = 0; i < Debug_Count; i++)
    {
        usnprintf(str, DEBUG_STR_LENGTH, fixheader,
                DebugTasks[i].name,
                ((DebugTasks[i].isEnabled) ? ("x") : (" ")));

        Table_SendLine(str);
    }

    Debug_PrintDebugListTableHeader(fixheader, str, DEBUG_STR_LENGTH, header);
}



#else

/* Debug module is not used */
/* Suppressed "ISO C forbids an empty translation unit [-Wpedantic]" */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic pop



#endif    /* #ifdef CONFIG_MODULE_DEBUG_ENABLE */
