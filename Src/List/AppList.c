/*
 *    AppList.c
 *    Created on:   2019-03-22
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "AppList.h"


#ifdef CONFIG_FUNCTION_DISPLAY

/* Note: Shall be excluded if there is no used function, because  AppList will be empty */

#include "Logic.h"
#include "Display.h"

#ifdef CONFIG_FUNCTION_GAME_SNAKE
#include "Snake.h"
#endif

#include "SmallApps.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

const App_List_t AppList[] = {
    #ifdef CONFIG_FUNCTION_GAME_SNAKE
    {
        .AppName = "Snake",
        .initFunction = Snake_Init,
        .eventFunction = Snake_Event,
        .updateFunction = Snake_Update
    },
    #endif
    #ifdef CONFIG_FUNCTION_DISPLAY_INPUT
    {
        .AppName = "Input",
        .initFunction = App_DisplayInput_Init,
        .eventFunction = App_DisplayInput_Event,
        .updateFunction = App_DisplayInput_Update
    },
    #endif
    #ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
    {
        .AppName = "CarAnimation",
        .initFunction = App_DisplayCarAnimation_Init,
        .eventFunction = App_DisplayCarAnimation_Event,
        .updateFunction = App_DisplayCarAnimation_Update
    },
    #endif
    #ifdef CONFIG_DISPLAY_CLOCK_LARGE
    {
        .AppName = "Clock",
        .initFunction = App_DisplayLargeClock_Init,
        .eventFunction = App_DisplayLargeClock_Event,
        .updateFunction = App_DisplayLargeClock_Update
    },
    #endif
    #ifdef CONFIG_DISPLAY_ESP8266_ENABLE
    {
        /* TODO: */
        .AppName = "ESP8266",
        .initFunction = NULL,
        .eventFunction = NULL,
        .updateFunction = NULL
    },
    #endif
};

const uint8_t AppList_Num = NUM_OF(AppList);



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
 * @brief  AppList initialization
 */
void AppList_Init(void)
{
    /* Check AppList size */
    BUILD_ASSERT(NUM_OF(AppList) == (AppType_Count - 1));
}


#endif /* CONFIG_FUNCTION_DISPLAY */
