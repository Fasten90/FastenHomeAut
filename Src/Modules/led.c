/*
 *		LED.c
 *		Creat on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		LED functions
 *		Target:			STM32Fx
 *		Version:		v2
 *		Last modified:	2016-11-23
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "LED.h"
#include "options.h"
#include "board.h"

#include "String.h"
#include "Globals.h"


#ifdef CONFIG_MODULE_LED_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

///< LED names
const char * const LED_NameList[] =
{
	// NOTE: Be careful, when change the order and num, synchronize with LED_NUM_MAX define
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS)
	"green",
	"blue",
	"red",
#elif CONFIG_USE_PANEL_NUCLEOF401RE
	"green",
#else
#warning "Miss CONFIG_USE_PANEL_.. define in LED names"
#endif
};


///< LED types
const char * const LED_TypeNameList[] =
{
	"-",
	"on",
	"off",
	"toggle",
	"status"

	// NOTE: Synchronize with LED_SetType_t
};



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static bool LED_SetRedLed(LED_SetType_t ledSet);
static bool LED_SetBlueLed(LED_SetType_t ledSet);
static bool LED_SetGreenLed(LED_SetType_t ledSet);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	LED GPIO initialization (without TIMER)
 */
void LED_Init(void)
{
	BUILD_BUG_ON((sizeof(LED_TypeNameList)/sizeof(LED_TypeNameList[0])) != LED_Type_Count);
	BUILD_BUG_ON((sizeof(LED_NameList)/sizeof(LED_NameList[0])) != (LED_Count - 1));

	GPIO_InitTypeDef GPIO_InitStructure;

	// GPIO Peripheral clock enable
	LED_PORT_CLK_ENABLES();
	
	// Configure pins
	//GPIO_InitStructure.Alternate = GPIO_AF;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pin = BOARD_LED_GREEN_PIN;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(BOARD_LED_GREEN_PORT, &GPIO_InitStructure);
	
#if LED_NUM_MAX > 1
	GPIO_InitStructure.Pin = BOARD_LED_BLUE_PIN;
	HAL_GPIO_Init(BOARD_LED_BLUE_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = BOARD_LED_RED_PIN;
	HAL_GPIO_Init(BOARD_LED_RED_PORT, &GPIO_InitStructure);
#endif

	LED_RED_OFF();
	LED_BLUE_OFF();
	LED_GREEN_OFF();
}



/**
 * \brief	LED blinking in infinite loop
 * \note	!! It is blocking !!
 */
void LED_Test(void)
{
	while(1)
	{
		// Set LEDs
		LED_GREEN_ON();
		LED_BLUE_ON();
		LED_RED_ON();

		// Delay
		DelayMs(200);

		// Set LEDs
		LED_GREEN_OFF();
		LED_BLUE_OFF();
		LED_RED_OFF();

		// Delay
		DelayMs(200);
	}
}



/**
 * \brief	Set green LED
 */
static bool LED_SetGreenLed(LED_SetType_t ledSet)
{

	switch (ledSet)
	{
		case LED_Set_On:
			LED_GREEN_ON();
			break;

		case LED_Set_Off:
			LED_GREEN_OFF();
			break;

		case LED_Set_Toggle:
			LED_GREEN_TOGGLE();
			break;

		case LED_Get_Status:
		case LED_Set_DontCare:
		case LED_Type_Count:
		default:
			// Do nothing
			break;
	}

	// Return with LED status, so LED_Get_Status state is handled with this
	return LED_GREEN_STATUS();
}



/**
 * \brief	Set blue LED
 */
static bool LED_SetBlueLed(LED_SetType_t ledSet)
{

	switch (ledSet)
	{
		case LED_Set_On:
			LED_BLUE_ON();
			break;

		case LED_Set_Off:
			LED_BLUE_OFF();
			break;

		case LED_Set_Toggle:
			LED_BLUE_TOGGLE();
			break;

		case LED_Get_Status:
		case LED_Set_DontCare:
		case LED_Type_Count:
		default:
			// Do nothing
			break;
	}

	return LED_BLUE_STATUS();
}



/**
 * \brief	Set red LED
 */
static bool LED_SetRedLed(LED_SetType_t ledSet)
{

	switch (ledSet)
	{
		case LED_Set_On:
			LED_RED_ON();
			break;

		case LED_Set_Off:
			LED_RED_OFF();
			break;

		case LED_Set_Toggle:
			LED_RED_TOGGLE();
			break;

		case LED_Get_Status:
		case LED_Set_DontCare:
		case LED_Type_Count:
		default:
			// Do nothing
			break;
	}

	return LED_RED_STATUS();
}



/**
 * \brief	Set LED
 * \param	num		LED number
 * \param	ledSet	Which type (on, off, toggle)
 */
bool LED_SetLed(LED_Pin_t pin, LED_SetType_t ledSet)
{
	bool state = false;

	switch (pin)
	{
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS)
		case LED_Green:
			state = LED_SetGreenLed(ledSet);
			break;

		case LED_Blue:
			state = LED_SetBlueLed(ledSet);
			break;

		case LED_Red:
			state = LED_SetRedLed(ledSet);
			break;
#elif CONFIG_USE_PANEL_NUCLEOF401RE
		case LED_Green:
			state = LED_SetGreenLed(ledSet);
			break;
#endif
		case LED_Unknown:
		case LED_Count:
		default:
			state = false;
			break;
	}

	return state;
}



/**
 * \brief	Get LED status
 * \return	true, if high
 * 			false, if low
 */
bool LED_GetStatus(LED_Pin_t pin)
{
	bool status;

	switch (pin)
	{
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS)
		case LED_Green:
			status = LED_GREEN_STATUS();
			break;

		case LED_Blue:
			status = LED_BLUE_STATUS();
			break;

		case LED_Red:
			status = LED_RED_STATUS();
			break;

#elif CONFIG_USE_PANEL_NUCLEOF401RE
		case LED_Green:
			status = LED_GREEN_STATUS();
			break;
#endif
		case LED_Unknown:
		case LED_Count:
		default:
			status = false;
			break;
	}

	return status;
}



/**
 * \brief	Get LED type from name
 */
LED_Pin_t LED_GetNumFromName(const char *name)
{
	uint8_t i;
	LED_Pin_t ledNum = LED_Unknown;

	// Search LED name in the list
	for (i = 0; i < LED_Count - 1; i++)
	{
		if (!StrCmp(LED_NameList[i], name))
		{
			// LED num = index+1
			ledNum = i + 1;
			break;
		}
	}

	return ledNum;
}



/**
 * \brief	Get type from string
 */
LED_SetType_t LED_GetTypeFromString(const char *typeString)
{
	uint8_t i;
	LED_SetType_t ledType = 0;

	// Search LED type string in the list
	for (i = 0; i < LED_Type_Count; i++)
	{
		if (!StrCmp(LED_TypeNameList[i], typeString))
		{
			ledType = i;
			break;
		}
	}

	return ledType;
}



/**
 * \brief	Get LED status to string
 */
uint8_t LED_GetLedStates(char *str)
{
	uint8_t length = 0;
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS)
	length = usprintf(str, "Led status: %d %d %d",
			LED_GetStatus(1),
			LED_GetStatus(2),
			LED_GetStatus(3));
#elif CONFIG_USE_PANEL_NUCLEOF401RE
	length = usprintf(str, "Led status: %d",
			LED_GREEN_STATUS());
#endif

	return length;
}



#endif	// #ifdef CONFIG_MODULE_LED_ENABLE
