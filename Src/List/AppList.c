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
        .updateFunction = Logic_Display_Snake
    },
    #endif
    #ifdef CONFIG_FUNCTION_DISPLAY_INPUT
    {
        .AppName = "Input",
        .initFunction = App_DisplayInput_Init,
        .eventFunction = App_DisplayInput_Event,
        .updateFunction = Logic_Display_Input
    },
    #endif
    #ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
    {
        .AppName = "CarAnimation",
        .initFunction = Display_LoadCarImage,
        .eventFunction = Logic_Display_CarAnimation_Event,
        .updateFunction = Logic_Display_CarAnimation
    },
    #endif
    #ifdef CONFIG_DISPLAY_CLOCK_LARGE
    {
        .AppName = "Clock",
        .initFunction = Logic_Display_LargeClock_Init,
        .eventFunction = App_LargeClock_Event,
        .updateFunction = Logic_Display_LargeClock_Update
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


