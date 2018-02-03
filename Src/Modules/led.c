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

#include "options.h"
#include "board.h"
#include "StringHelper.h"
#include "ErrorHandler.h"
#include "Timing.h"
#include "LED.h"
#include "LedList.h"



#ifdef CONFIG_MODULE_LED_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


///< LED command type names
const char * const LED_Cmd_NameList[] =
{
	"-",
	"on",
	"off",
	"toggle",
#if defined(CONFIG_LED_BLINK_ENABLE)
	"blink",
#endif
	"status"

	// NOTE: Synchronize with LED_Cmd_t
};


///< LED List is required in LedList.c
extern LED_Record_t LED_List[];

#if defined(CONFIG_LED_BLINK_ENABLE)
///< LED Actual state is required in LedList.c
extern LED_Cmd_t LED_ActualState[];
#endif



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	LED GPIO initialization (without TIMER)
 */
void LED_Init(void)
{
	// Check list
	BUILD_ASSERT((NUM_OF(LED_Cmd_NameList)) == LED_Cmd_Count);

	// TODO: Check list...

	GPIO_InitTypeDef GPIO_InitStructure;

	// GPIO Peripheral clock enable
	LED_PORT_CLK_ENABLES();

	uint8_t i;
	for (i = 0; i < (LED_Count -1); i++)
	{
		GPIO_TypeDef * port = (GPIO_TypeDef *)LED_List[i].GPIO_Port;
		uint32_t pin = LED_List[i].GPIO_Pin;

		// Configure pins

		// Common settings
		//GPIO_InitStructure.Alternate = GPIO_AF;
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStructure.Pull = GPIO_NOPULL;
		GPIO_InitStructure.Speed = GPIO_SPEED_LOW;

		// Different settings
		GPIO_InitStructure.Pin = pin;
		HAL_GPIO_Init(port, &GPIO_InitStructure);
	}

	// Turn off all LEDs
	for (i = 0; i < (LED_Count -1); i++)
	{
		LED_SetLed(i+1, LED_Cmd_SetOff);
	}
}



/**
 * \brief	LED blinking in infinite loop
 * \note	!! It is blocking !!
 */
void LED_Test(void)
{
	while(1)
	{
		// Turn on all LEDs
		uint8_t i;
		for (i = 0; i < (LED_Count -1); i++)
		{
			LED_SetLed(i+1, LED_Cmd_SetOn);
		}

		// Delay
		DelayMs(200);

		// Turn off all LEDs
		for (i = 0; i < (LED_Count -1); i++)
		{
			LED_SetLed(i+1, LED_Cmd_SetOff);
		}

		// Delay
		DelayMs(200);
	}
}



/**
 * \brief	Set LED
 * \param	num		LED number
 * \param	ledSet	Which type (on, off, toggle)
 */
LED_Status_t LED_SetLed(LED_Name_t ledName, LED_Cmd_t ledCmd)
{
	LED_Status_t status = LED_State_Unknown;

	if ((ledName < LED_Count) && (ledCmd < LED_Cmd_Count) && (ledName != LED_Unknown) && (ledCmd != LED_Cmd_DontCare))
	{
#if defined(CONFIG_LED_BLINK_ENABLE)
		LED_ActualState[ledName - 1] = ledCmd;
#endif

		GPIO_TypeDef * port = (GPIO_TypeDef *)LED_List[ledName-1].GPIO_Port;
		uint32_t pin = LED_List[ledName-1].GPIO_Pin;
		LED_Status_t lowVoltageState = LED_List[ledName-1].lowVoltageState;

		switch (ledCmd)
		{
			case LED_Cmd_SetOn:
	#if defined(CONFIG_LED_BLINK_ENABLE)
			case LED_Cmd_SetBlink:
	#endif
				//LED_COLOR_ON();
				HAL_GPIO_WritePin(port, pin, (lowVoltageState == LED_State_Off) ? (GPIO_PIN_SET) : (GPIO_PIN_RESET));
				break;

			case LED_Cmd_SetOff:
				//LED_COLOR_OFF();
				HAL_GPIO_WritePin(port, pin, (lowVoltageState == LED_State_Off) ? (GPIO_PIN_RESET) : (GPIO_PIN_SET));
				break;

			case LED_Cmd_SetToggle:
				//LED_COLOR_TOGGLE();
				HAL_GPIO_TogglePin(port, pin);
				break;

			case LED_Cmd_GetStatus:
			case LED_Cmd_DontCare:
			case LED_Cmd_Count:
			default:
				// Do nothing
				break;
		}

		// Return with LED status, so LED_Cmd_GetStatus state is handled with this
		//return LED_GREEN_STATUS();
		GPIO_PinState pinState = HAL_GPIO_ReadPin(port, pin);

		status = (lowVoltageState == LED_State_Off)
				? ((pinState == GPIO_PIN_RESET) ? LED_State_Off : LED_State_On)
				: ((pinState == GPIO_PIN_RESET) ? LED_State_On : LED_State_Off);
	}

	return status;
}



/**
 * \brief	Get LED status
 * \return	true, if high
 * 			false, if low
 */
LED_Status_t LED_GetStatus(LED_Name_t ledName)
{
	LED_Status_t status = LED_State_Unknown;

	if ((ledName < LED_Count) && (ledName != LED_Unknown))
	{
		// Get LED datas
		GPIO_TypeDef * port = (GPIO_TypeDef *)LED_List[ledName-1].GPIO_Port;
		uint32_t pin = LED_List[ledName-1].GPIO_Pin;
		LED_Status_t lowVoltageState = LED_List[ledName-1].lowVoltageState;

		// Read pin
		GPIO_PinState pinState = HAL_GPIO_ReadPin(port, pin);

		// Set state
		status = (lowVoltageState == LED_State_Off)
				? ((pinState == GPIO_PIN_RESET) ? LED_State_Off : LED_State_On)
				: ((pinState == GPIO_PIN_RESET) ? LED_State_On : LED_State_Off);
	}

	return status;
}



/**
 * \brief	Get LED type from name
 */
LED_Name_t LED_GetNumFromName(const char *name)
{
	uint8_t i;
	LED_Name_t ledNum = LED_Unknown;

	// Search LED name in the list
	for (i = 0; i < (LED_Count -1) - 1; i++)
	{
		if (!StrCmp(LED_List[i].name, name))
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
LED_Cmd_t LED_GetTypeFromString(const char *typeString)
{
	uint8_t i;
	LED_Cmd_t ledType = 0;

	// Search LED type string in the list
	for (i = 0; i < LED_Cmd_Count; i++)
	{
		if (!StrCmp(LED_Cmd_NameList[i], typeString))
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
	// TODO: It only handle fix count LEDs
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



#if defined(LED_TASK_PWM_STYLE)
/**
 * \brief	LED Task - PWM style
 * \note	Call this function periodically - 2 ms times
 */
void LED_PWMTask(void)
{

	// Blue LED blinking like PWM

	// 50 Hz --> 20ms

	static uint8_t LED_PwmCnt = 0;
	static uint8_t LED_PwmLimit = 0;
	static const uint8_t LED_PwmMaxLimit = 10;
	static bool LED_PwmLimitDir = false;
	static uint8_t LED_100ms = 0;
	static uint8_t LED_2ms = 0;
	static const uint8_t LED_PWM_ChangeDir_100ms_limit = 10;

	LED_2ms++;
	if (LED_2ms >= 100/2)
	{
		// Run every 100. ms

		LED_2ms = 0;
		LED_100ms++;

		// Change PWM percent

		if (LED_PwmLimitDir)
		{
			LED_PwmLimit--;
		}
		else
		{
			LED_PwmLimit++;
		}

		// Change direction
		if (LED_100ms >= LED_PWM_ChangeDir_100ms_limit)
		{
			// Run every 1000. msec = every sec
			LED_100ms = 0;

			// Change dir after 1 sec
			if (LED_PwmLimit == 0)
			{
				LED_PwmLimitDir = false;
			}
			else
			{
				LED_PwmLimitDir = true;
			}
		}
	}

	// PWM limit: 0-10

	// Check, need LED blinking?
	if (LED_PwmCnt < LED_PwmLimit)
	{
		LED_SetLed(LED_Blue, LED_Cmd_SetOn);
		//LED_SetLed(LED_Blue, LED_Cmd_SetToggle);
	}
	else
	{
		LED_SetLed(LED_Blue, LED_Cmd_SetOff);
	}

	// PWM counter
	LED_PwmCnt++;
	if (LED_PwmCnt >= LED_PwmMaxLimit)	// Max limit
	{
		LED_PwmCnt = 0;
	}
}
#endif



#if defined(CONFIG_LED_BLINK_ENABLE)
/**
 * \brief	Handle necessary LED operations (e.g. blink)
 */
void LED_Handler(void)
{
	// Now only have one task: check blink state and turn off the LED if need
	uint8_t i;
	for (i = 0; i < LED_Count; i++)
	{
		if (LED_ActualState[i] == LED_Cmd_SetBlink)
		{
			// Need corrects the LED index
			LED_SetLed((i+1), LED_Cmd_SetOff);
		}
	}
}
#endif /* CONFIG_LED_BLINK_ENABLE */



#endif	// #ifdef CONFIG_MODULE_LED_ENABLE
