/*
 *    IOList.c
 *    Created on:   2018-02-03
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#include "IO.h"
#include "IOList.h"

#ifdef CONFIG_MODULE_IO_ENABLE


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

///< IO List
const IO_Output_Record_t IO_Output_List[] =
{
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)
    {
        .GPIO_Port = BOARD_LED_GREEN_PORT,
        .GPIO_Pin = BOARD_LED_GREEN_PIN,
        .lowVoltageState = IO_Status_On,
        .name = "ledgreen"
    },
#endif
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS)
    {
        .GPIO_Port = BOARD_LED_BLUE_PORT,
        .GPIO_Pin = BOARD_LED_BLUE_PIN,
        .lowVoltageState = IO_Status_On,
        .name = "ledblue"
    },
    {
        .GPIO_Port = BOARD_LED_RED_PORT,
        .GPIO_Pin = BOARD_LED_RED_PIN,
        .lowVoltageState = IO_Status_On,
        .name = "ledred"
    },
#endif


#if defined(CONFIG_FUNCTION_TRAFFIC_LIGHT)
    {
        .GPIO_Port = BOARD_TRAFFICLIGHT_RED_PORT,
        .GPIO_Pin = BOARD_TRAFFICLIGHT_RED_PIN,
        .lowVoltageState = IO_Status_Off,
        .name = "tl_red"
    },

    {
        .GPIO_Port = BOARD_TRAFFICLIGHT_YELLOW_PORT,
        .GPIO_Pin = BOARD_TRAFFICLIGHT_YELLOW_PIN,
        .lowVoltageState = IO_Status_Off,
        .name = "tl_yellow"
    },
    {
        .GPIO_Port = BOARD_TRAFFICLIGHT_GREEN_PORT,
        .GPIO_Pin = BOARD_TRAFFICLIGHT_GREEN_PIN,
        .lowVoltageState = IO_Status_Off,
        .name = "tl_green"
    },
#endif


    /*
     * XXX: Add new IO output here
     * GPIO_TypeDef * GPIO_Port;            ///< GPIO port
     * uint32_t GPIO_Pin;                   ///< GPIO Pin
     * LED_Status_t lowVoltageState;        ///< Low voltage state
     * const char * const name;             ///< Name of LED
     */

    /* NOTE: Be careful, when change the order and num, synchronize with LED_NUM_MAX define */
    /* Note: Not recommend space or other separator character on the name */
};


#if defined(CONFIG_IO_OUTPUT_BLINK_ENABLE)
IO_Output_Cmd_t IO_Output_ActualState[IO_Output_Count] = { 0 };
#endif


#if IO_INPUTS_NUM > 0
const IO_Input_Record_t IO_Input_List[] =
{
#ifdef CONFIG_MODULE_IO_INPUT_MOTION_ENABLE
    {
        .GPIO_Port = BOARD_LED_RED_PORT,
        .GPIO_Pin = BOARD_LED_RED_PIN,
        .lowVoltageState = IO_Status_On,
        .name = "MotionMove"
    },
#endif
#ifdef CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE
    {
        .GPIO_Port = BOARD_LED_RED_PORT,
        .GPIO_Pin = BOARD_LED_RED_PIN,
        .lowVoltageState = IO_Status_On,
        .name = "SoundImpact"
    },
#endif
#ifdef CONFIG_MODULE_IO_BATTERY_CHARGER_ENABLE
    {
        .GPIO_Port = BOARD_LED_RED_PORT,
        .GPIO_Pin = BOARD_LED_RED_PIN,
        .lowVoltageState = IO_Status_On,
        .name = "BatteryCharger"
    },
#endif
};
#endif



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
 * @brief       IOList initialization
 */
void IO_List_Init(void)
{
    /* List IO_Output_List shall be synced with Enum IO_Output_Count */
    BUILD_ASSERT((NUM_OF(IO_Output_List)) == (IO_Output_Count - 1));

    /* Do not be empty */
    BUILD_ASSERT((NUM_OF(IO_Output_List)) != 0);

#if (IO_INPUTS_NUM > 0)
    BUILD_ASSERT((NUM_OF(IO_Input_List)) == (IO_Input_Count - 1));
#endif

    /* Note: RunTime structure checks in the IO.c */
}



#endif /* #ifdef CONFIG_MODULE_IO_ENABLE */
