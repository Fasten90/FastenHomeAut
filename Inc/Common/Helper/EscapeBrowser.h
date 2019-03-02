/*
 *    EscapeBrowser.h
 *    Created on:   2018-02-26
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef INC_COMMON_HELPER_ESCAPEBROWSER_H_
#define INC_COMMON_HELPER_ESCAPEBROWSER_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "GenericTypeDefs.h"
#include "ColoredMessage.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define ESCAPEBROWSER_LIST_NUM_MAX            (20U)
#define ESCAPEBROWSER_ELEMENT_LENGTH_MAX    (20U)

#define ESCAPEBROWSER_COLOR_INACTIVE_ELEMENT_ENABLE        (0)

#define ESCAPEBROWSER_ELEMENT_INACTIVE_BACKGROUND    ((MsgColors_t)Color_White)
#define ESCAPEBROWSER_ELEMENT_INACTIVE_TEXTCOLOR    ((MsgColors_t)Color_Black)
#define ESCAPEBROWSER_ELEMENT_ACTIVE_BACKGROUND        ((MsgColors_t)Color_Blue)
#define ESCAPEBROWSER_ELEMENT_ACTIVE_TEXTCOLOR        ((MsgColors_t)Color_White)

#define ESCAPEBROWSER_PROCESS_CHAR_BUFFER    (100U)
#define ESCAPEBROWSER_PRINT_CHAR_BUFFER        (100U)


#define ESCAPEBROWSER_ESCAPE_SEQUENCE_MIN_LENGTH    (3U)
#define ESCAPEBROWSER_LOG_MESSAGE_MAX_LENGTH        (30U)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef const char *(*EscapeBrowser_GetLineFunction)(uint8_t);



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void EscapeBrowser_Init(EscapeBrowser_GetLineFunction getFunc);
size_t EscapeBrowser_PrintNewLineList(char *str);
void EscapeBrowser_ProcessReceivedCharaters(void);



#endif /* INC_COMMON_HELPER_ESCAPEBROWSER_H_ */
