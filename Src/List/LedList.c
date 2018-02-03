/*
 *		LedList.c
 *		Created on:		2018-02-03
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2018-02-03
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "LED.h"
#include "LedList.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

///< LED List
const LED_Record_t LED_List[] =
{
		/*// GPIO registrations
		GPIO_TypeDef * GPIO_Port;			///< GPIO port
		uint32_t GPIO_Pin;					///< GPIO Pin
		LED_Status_t lowVoltageState;		///< Low voltage state

		const char * const name;			///< Name of LED*/
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)
	{
		.GPIO_Port = BOARD_LED_GREEN_PORT,
		.GPIO_Pin = BOARD_LED_GREEN_PIN,
		.lowVoltageState = LED_State_On,
		.name = "green"
	},
#endif
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS)
	{
		.GPIO_Port = BOARD_LED_BLUE_PORT,
		.GPIO_Pin = BOARD_LED_BLUE_PIN,
		.lowVoltageState = LED_State_On,
		.name = "blue"
	},
	{
		.GPIO_Port = BOARD_LED_RED_PORT,
		.GPIO_Pin = BOARD_LED_RED_PIN,
		.lowVoltageState = LED_State_On,
		.name = "red"
	},
#endif

	// NOTE: Be careful, when change the order and num, synchronize with LED_NUM_MAX define
};


#if defined(CONFIG_LED_BLINK_ENABLE)
LED_Cmd_t LED_ActualState[LED_Count] = { 0 };
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
 * \brief  LedList initialization
 */
void LedList_Init(void)
{
	BUILD_ASSERT((NUM_OF(LED_List)) == (LED_Count - 1));

	// Do not be empty
	BUILD_ASSERT((NUM_OF(LED_List)) != 0);
}



