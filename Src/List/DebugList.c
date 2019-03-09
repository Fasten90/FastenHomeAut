/*
 *    DebugList.c
 *    Created on:   2017-09-08
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Debug 
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "compiler.h"
#include "EscapeSequence.h"
#include "ColoredMessage.h"
#include "Debug.h"
#include "DebugList.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

///< Debug(task) list
DebugRecord_t DebugTasks[] =
{
#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE
    {
        .name = "Event",
        .isEnabled = true,
        .color = Color_Blue,
    },
#endif
#ifdef CONFIG_MODULE_COMMANDHANDLER_ENABLE
    {
        .name = "CommandHandler",
        .isEnabled = true,
        .color = Color_Blue,
    },
#endif
#ifdef CONFIG_FUNCTION_GAME_SNAKE
    {
        .name = "Snake",
        .isEnabled = true,
        .color = Color_Green,
    },
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
    {
        .name = "ESP8266",
        .isEnabled = true,
        .color = Color_Green,
    },
#endif
#ifdef CONFIG_BUTTON_DEBUG_ENABLE
    {
        .name = "Button",
        .isEnabled = true,
        .color = Color_Cyan,
    },
#endif
#ifdef CONFIG_MODULE_WEBPAGE_ENABLE
    {
        .name = "Webpage",
        .isEnabled = true,
        .color = Color_Green,
    },
#endif

    /*
     * XXX: Add here new Debug task struct
     * @note    Do not forget synchronize with Debug_t enum
     */
};



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

 /**
  * @brief    DebugList initialization - Check debug list
  */
void DebugList_Init(void)
{
    /* Run-time error checking: size of the DebugTasks struct must be equal to members is Debug_t enumeration. */
    BUILD_ASSERT(NUM_OF(DebugTasks) == Debug_Count);
}


