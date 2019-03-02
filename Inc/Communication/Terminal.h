/*
 *        Terminal.h
 *        Created on:        2017-02-02
 *        Author:            Vizi G�bor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        Terminal
 *        Target:            STM32Fx
 *        Version:        v1
 *        Last modified:    2017-02-02
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"
#include "version.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define TERMINAL_MAX_COMMAND_LENGTH                    (255U)


#define TERMINAL_HISTORY_MAX_COUNT                    (5)


// Optimized version:
#define TERMINAL_SEND_WELCOME()                        \
        Terminal_SendMessage(                        \
        "\r\n"                                        \
        "Welcome in CommandHandler!\r\n"            \
        "(c) Copyright, Vizi Gabor\r\n"                \
        "\r\n"                                        \
        "Try \"help\" command for help!\r\n"        \
        "Version: " VERSION_STRING "\r\n"             \
        "Compile date: " DATE_VERSION ", " TIME_VERSION "\r\n" \
        "Used panel: " BOARD_NAME "\r\n")


#ifdef CONFIG_TERMINAL_PROMT_ENABLE
    #define TERMINAL_SEND_PROMT_NEW_LINE()    Terminal_SendMessage("\r\n# ")
    #define TERMINAL_SEND_PROMT()            Terminal_SendMessage("# ")
#else
    #define TERMINAL_SEND_PROMT_NEW_LINE()    Terminal_SendMessage("\r\n")
    #define TERMINAL_SEND_PROMT()
#endif


#define TERMINAL_SEND_NEW_LINE()            Terminal_SendMessage("\r\n")


///    Defines for characters

#if defined(CONFIG_TERMINAL_USE_ZOC) || defined(CONFIG_TERMINAL_USE_PUTTY)
    #define TERMINAL_KEY_DELETE                ('\x08')
    #define TERMINAL_KEY_BACKSPACE            ('\x7F')
#else
    #define TERMINAL_KEY_DELETE                ('\x7F')
    #define TERMINAL_KEY_BACKSPACE            ('\x08')
#endif

#define TERMINAL_KEY_BELL                    ('\a')

#define TERMINAL_KEY_ESCAPESEQUENCE_1        ('\x1B')
#define TERMINAL_KEY_ESCAPESEQUENCE_2        ('[')


// 8 = BS = Backspace
#define TERMINAL_SEND_KEY_BACKSPACE()        Terminal_SendMessage("\x08")

// 127 = DEL = Delete
#define TERMINAL_SEND_KEY_DEL()                 Terminal_SendMessage("\x7F'")


// -\_("))_/-
#define TERMINAL_STRING_DONTKNOW            ("-\_(\"))_/-")



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Terminal_Init(void);
void Terminal_SendWelcome(void);
void Terminal_SendCls(void);

void Terminal_CheckCommand(void);

void Terminal_SendMessage(const char * message);
void Terminal_SendLine(const char * message);

void Terminal_TestLoading(void);
void Terminal_SendLoadingPercent(uint8_t percent);
void Terminal_SendLoadingPercent2(uint8_t percent);



#endif /* TERMINAL_H_ */
