/*
 *		led.c
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		LED functions
 *		Target:			STM32Fx
 *		Version:		v2
 *		Last modified:	2016.11.23
 */

#include "options.h"
#include "include.h"
#include "board.h"
#include "LED.h"



#ifdef CONFIG_MODULE_LED_ENABLE


/// LED names
const char * const LED_NameList[] =
{
	// NOTE: Be careful, when change the order and num, syncronize with LED_NUM_MAX define
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS)
	"red",
	"blue",
	"green",
#elif CONFIG_USE_PANEL_NUCLEOF401RE
	"green",
#else
#warning "Miss CONFIG_USE_PANEL_.. define in LED names"
#endif
	NULL
};				///< LED names


/// LED types
const char * const LED_TypeNameList[] =
{
	// NOTE: Syncronize with LED_SetType
	"-",
	"on",
	"off",
	"toggle",
	"status"
};


// Function prototypes

static bool LED_SetRedLed(LED_SetType ledSet);
static bool LED_SetBlueLed(LED_SetType ledSet);
static bool LED_SetGreenLed(LED_SetType ledSet);


// Functions


/**
\brief	LED GPIO initialization (without TIMER)
*/
void LED_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

	// GPIO Periph clock enable
	LED_PORT_CLK_ENABLES();
	
	// Configure pin output pushpull mode
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
\brief	LED blinking in infinite loop
*/
void LED_Test(void)
{

	while(1)
	{
		// Set LEDs
		LED_RED_ON();
		LED_BLUE_ON();
		LED_GREEN_ON();

		// Delay
		DelayMs(200);

		// Set LEDs
		LED_RED_OFF();
		LED_BLUE_OFF();
		LED_GREEN_OFF();

		// Delay
		DelayMs(200);
	}
}



/**
 * \brief	Set red LED
 */
static bool LED_SetRedLed(LED_SetType ledSet)
{

	switch (ledSet)
	{
		case LED_SET_ON:
			LED_RED_ON();
			break;
		case LED_SET_OFF:
			LED_RED_OFF();
			break;
		case LED_SET_TOGGLE:
			LED_RED_TOGGLE();
			break;
		case LED_GET_STATUS:
			break;
		case LED_SET_DONTCARE:
		case LED_TYPE_COUNT:
			// Do not use
			break;
		default:
			break;
	}

	return LED_RED_STATUS();
}



/**
 * \brief	Set blue LED
 */
static bool LED_SetBlueLed(LED_SetType ledSet)
{

	switch (ledSet)
	{
		case LED_SET_ON:
			LED_BLUE_ON();
			break;
		case LED_SET_OFF:
			LED_BLUE_OFF();
			break;
		case LED_SET_TOGGLE:
			LED_BLUE_TOGGLE();
			break;
		case LED_GET_STATUS:
			break;
		case LED_SET_DONTCARE:
		case LED_TYPE_COUNT:
			// Do not use
			break;
		default:
			break;
	}

	return LED_BLUE_STATUS();
}



/**
 * \brief	Set green LED
 */
static bool LED_SetGreenLed(LED_SetType ledSet)
{

	switch (ledSet)
	{
		case LED_SET_ON:
			LED_GREEN_ON();
			break;
		case LED_SET_OFF:
			LED_GREEN_OFF();
			break;
		case LED_SET_TOGGLE:
			LED_GREEN_TOGGLE();
			break;
		case LED_GET_STATUS:
			break;
		case LED_SET_DONTCARE:
		case LED_TYPE_COUNT:
			// Do not use
			break;
		default:
			break;
	}

	return LED_GREEN_STATUS();
}



/**
 * \brief	Set LED
 * \param	num		LED number
 * \param	ledSet	Which type (on, off, toggle)
 */
bool LED_SetLed(uint8_t num, LED_SetType ledSet)
{
	bool state = false;

	switch (num)
	{
#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_HOMEAUTPANELS)
		case 1:
			state = LED_SetRedLed(ledSet);
			break;
		case 2:
			state = LED_SetBlueLed(ledSet);
			break;
		case 3:
			state = LED_SetGreenLed(ledSet);
			break;
#elif CONFIG_USE_PANEL_NUCLEOF401RE
		case 1:
			state = LED_SetGreenLed(ledSet);
			break;
#endif
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
bool LED_GetStatus(uint8_t num)
{
	bool status;

	switch (num)
	{
#ifdef CONFIG_USE_PANEL_STM32F4DISCOVERY
		case 1:
			status = LED_RED_STATUS();
			break;
		case 2:
			status = LED_BLUE_STATUS();
			break;
		case 3:
			status = LED_GREEN_STATUS();
			break;
#elif CONFIG_USE_PANEL_NUCLEOF401RE
		case 1:
			status = LED_GREEN_STATUS();
			break;
#endif
		default:
			status = false;
			break;
	}

	return status;
}



/**
 * \brief	Get LED type from name
 */
uint8_t LED_GetNumFromName(const char *name)
{
	uint8_t i;
	uint8_t ledNum = 0;

	for (i=0; i < LED_NUM_MAX; i++)
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
LED_SetType LED_GetTypeFromString (const char *typeString)
{
	uint8_t i;
	LED_SetType ledType = 0;

	// TODO: Can we optimizing without loop?

	for (i = 0; i < LED_TYPE_COUNT; i++)
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
	uint8_t length;
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
