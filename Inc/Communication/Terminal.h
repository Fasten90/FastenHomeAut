/*
 *		Terminal.h
 *		Created on:		2017-02-02
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Terminal
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-02
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define TERMINAL_MAX_COMMAND_LENGTH					(255U)


#define TERMINAL_HISTORY_MAX_COUNT					(5)


// Optimized version:
#define TERMINAL_SEND_WELCOME()						\
		CommandHandler_SendMessage(					\
		"\r\n"										\
		"Welcome in CommandHandler!\r\n"			\
		"(c) Copyright, Vizi Gabor\r\n"				\
		"\r\n"										\
		"Try \"help\" command for help!\r\n"		\
		"Version: "VERSION_DEFINE"\r\n" 			\
		"Compile date: "DATE_VERSION", "TIME_VERSION"\r\n" \
		"Used panel: "BOARD_NAME"\r\n")


#ifdef CONFIG_TERMINAL_PROMT_ENABLE
	#define TERMINAL_SEND_PROMT_NEW_LINE()	CommandHandler_SendMessage("\r\n# ")
	#define TERMINAL_SEND_PROMT()			CommandHandler_SendMessage("# ")
#else
	#define TERMINAL_SEND_PROMT_NEW_LINE()	CommandHandler_SendMessage("\r\n")
	#define TERMINAL_SEND_PROMT()
#endif


#define TERMINAL_SEND_NEW_LINE()			CommandHandler_SendMessage("\r\n")


///	Defines for characters

#if defined(CONFIG_TERMINAL_USE_ZOC) || defined(CONFIG_TERMINAL_USE_PUTTY)
	#define TERMINAL_KEY_DELETE				(0x08)
	#define TERMINAL_KEY_BACKSPACE			(0x7F)
#else
	#define TERMINAL_KEY_DELETE				(0x7F)
	#define TERMINAL_KEY_BACKSPACE			(0x08)
#endif


#define TERMINAL_KEY_ENTER					('\r')
#define TERMINAL_KEY_SPACE					(' ')
#define TERMINAL_KEY_ESCAPE					('\x1B')
#define TERMINAL_KEY_BELL					('\a')


#define TERMINAL_ESCAPESEQUENCE_1			('\x1B')
#define TERMINAL_ESCAPESEQUENCE_2			('[')


//	BUTTONS


// 8 = BS = Backspace
#define TERMINAL_SEND_KEY_BACKSPACE()		CommandHandler_SendChar(8)

// 127 = DEL = Delete
#define TERMINAL_SEND_KEY_DEL()		 		CommandHandler_SendChar(127)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern volatile char Terminal_CommandActual[TERMINAL_MAX_COMMAND_LENGTH];
extern const bool DebugUart_CommandReceiveEnable;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Terminal_Init(void);
void Terminal_SendWelcome(void);
void Terminal_SendCls(void);

void Terminal_CheckCommand(void);



#endif /* TERMINAL_H_ */
