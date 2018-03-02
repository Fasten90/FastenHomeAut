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
#include "IO.h"
#include "IOList.h"



#ifdef CONFIG_MODULE_IO_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


///< IO output command type names
const char * const IO_Output_Cmd_NameList[] =
{
	"-",
	"on",
	"off",
	"toggle",
#if defined(CONFIG_IO_OUTPUT_BLINK_ENABLE)
	"blink",
#endif
	"status"

	// NOTE: Synchronize with IO_Output_Cmd_t
};


///< IO Output List is required in IOList.c
extern const IO_Output_Record_t IO_Output_List[];
extern const IO_Input_Record_t  IO_Input_List[];

#if defined(CONFIG_IO_OUTPUT_BLINK_ENABLE)
///< IO Actual state is required in IOList.c
extern IO_Output_Cmd_t IO_Output_ActualState[];
#endif


const char * const IO_Input_StateNames[] =
{
	"Unknown",
	"Active",
	"Inactive"
};



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_DEBUG_MODE
static void IO_CheckList(void);
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	IO GPIO initialization (without TIMER)
 */
void IO_Init(void)
{
	// Check list
	BUILD_ASSERT((NUM_OF(IO_Output_Cmd_NameList)) == IO_Output_Cmd_Count);

#ifdef CONFIG_DEBUG_MODE
	// Check list in runtime
	IO_CheckList();
#endif

	GPIO_InitTypeDef GPIO_InitStructure;

	// GPIO Peripheral clock enable
	IO_PORT_CLK_ENABLES();

	uint8_t i;
	for (i = 0; i < (IO_Output_Count -1); i++)
	{
		GPIO_TypeDef * port = (GPIO_TypeDef *)IO_Output_List[i].GPIO_Port;
		uint32_t pin = IO_Output_List[i].GPIO_Pin;

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

	// Turn off all IOs
	for (i = 0; i < (IO_Output_Count -1); i++)
	{
		IO_Output_SetStatus(i+1, IO_Output_Cmd_SetOff);
	}
}



#ifdef CONFIG_DEBUG_MODE
/**
 * \brief	Check IO list
 */
static void IO_CheckList(void)
{
	uint8_t i;

	for (i = 0; i < (IO_Output_Count -1); i++)
	{
		ASSERT(IO_Output_List[i].GPIO_Port != NULL);
		ASSERT((IO_Output_List[i].lowVoltageState == IO_Status_Off) || (IO_Output_List[i].lowVoltageState == IO_Status_On));
		ASSERT(IO_Output_List[i].name != NULL);
	}
}
#endif /* CONFIG_DEBUG_MODE */



/**
 * \brief	IO (e.g. LED) blinking in infinite loop
 * \note	!! It is blocking !!
 */
void IO_Output_Test(void)
{
	while(1)
	{
		// Turn on all IOs
		uint8_t i;
		for (i = 0; i < (IO_Output_Count -1); i++)
		{
			IO_Output_SetStatus(i+1, IO_Output_Cmd_SetOn);
		}

		// Delay
		DelayMs(200);

		// Turn off all IOs
		for (i = 0; i < (IO_Output_Count -1); i++)
		{
			IO_Output_SetStatus(i+1, IO_Output_Cmd_SetOff);
		}

		// Delay
		DelayMs(200);
	}
}



/**
 * \brief	Set IO output state
 * \param	num		IO
 * \param	ledSet	Which type (on, off, toggle)
 */
IO_Status_t IO_Output_SetStatus(IO_Output_Name_t ioName, IO_Output_Cmd_t ioCmd)
{
	IO_Status_t status = IO_Status_Unknown;

	if ((ioName < IO_Output_Count) && (ioCmd < IO_Output_Cmd_Count) && (ioName != IO_Output_Unknown) && (ioCmd != IO_Output_Cmd_DontCare))
	{
#if defined(CONFIG_IO_OUTPUT_BLINK_ENABLE)
		IO_Output_ActualState[ioName - 1] = ioCmd;
#endif

		GPIO_TypeDef * port = (GPIO_TypeDef *)IO_Output_List[ioName-1].GPIO_Port;
		uint32_t pin = IO_Output_List[ioName-1].GPIO_Pin;
		IO_Status_t lowVoltageState = IO_Output_List[ioName-1].lowVoltageState;

		switch (ioCmd)
		{
			case IO_Output_Cmd_SetOn:
	#if defined(CONFIG_IO_OUTPUT_BLINK_ENABLE)
			case IO_Output_Cmd_SetBlink:
	#endif
				//IO_OUT_ON();
				HAL_GPIO_WritePin(port, pin, (lowVoltageState == IO_Status_Off) ? (GPIO_PIN_SET) : (GPIO_PIN_RESET));
				break;

			case IO_Output_Cmd_SetOff:
				//IO_OUT_OFF();
				HAL_GPIO_WritePin(port, pin, (lowVoltageState == IO_Status_Off) ? (GPIO_PIN_RESET) : (GPIO_PIN_SET));
				break;

			case IO_Output_Cmd_SetToggle:
				//IO_OUT_TOGGLE();
				HAL_GPIO_TogglePin(port, pin);
				break;

			case IO_Output_Cmd_GetStatus:
			case IO_Output_Cmd_DontCare:
			case IO_Output_Cmd_Count:
			default:
				// Do nothing
				break;
		}

		// Return with IO status, so IO_Output_Cmd_GetStatus state is handled with this
		//return IO_OUT_STATUS();
		GPIO_PinState pinState = HAL_GPIO_ReadPin(port, pin);

		status = (lowVoltageState == IO_Status_Off)
				? ((pinState == GPIO_PIN_RESET) ? IO_Status_Off : IO_Status_On)
				: ((pinState == GPIO_PIN_RESET) ? IO_Status_On : IO_Status_Off);
	}

	return status;
}



/**
 * \brief	Get IO Output status
 * \return	true, if high
 * 			false, if low
 */
IO_Status_t IO_Output_GetStatus(IO_Output_Name_t ioName)
{
	IO_Status_t status = IO_Status_Unknown;

	if ((ioName < IO_Output_Count) && (ioName != IO_Output_Unknown))
	{
		// Get IO datas
		GPIO_TypeDef * port = (GPIO_TypeDef *)IO_Output_List[ioName-1].GPIO_Port;
		uint32_t pin = IO_Output_List[ioName-1].GPIO_Pin;
		IO_Status_t lowVoltageState = IO_Output_List[ioName-1].lowVoltageState;

		// Read pin
		GPIO_PinState pinState = HAL_GPIO_ReadPin(port, pin);

		// Set state
		status = (lowVoltageState == IO_Status_Off)
				? ((pinState == GPIO_PIN_RESET) ? IO_Status_Off : IO_Status_On)
				: ((pinState == GPIO_PIN_RESET) ? IO_Status_On : IO_Status_Off);
	}

	return status;
}



/**
 * \brief	Get IO Output type from name
 */
IO_Output_Name_t IO_GetOutputNumFromName(const char *name)
{
	uint8_t i;
	IO_Output_Name_t outputNum = IO_Output_Unknown;

	// Search IO Output name in the list
	for (i = 0; i < (IO_Output_Count -1) - 1; i++)
	{
		if (!StrCmp(IO_Output_List[i].name, name))
		{
			// IO Output num = index+1
			outputNum = i + 1;
			break;
		}
	}

	return outputNum;
}



/**
 * \brief	Get type from string
 */
IO_Output_Cmd_t IO_GetOutputTypeFromString(const char *typeString)
{
	uint8_t i;
	IO_Output_Cmd_t outputCmdType = 0;

	// Search IO Output type string in the list
	for (i = 0; i < IO_Output_Cmd_Count; i++)
	{
		if (!StrCmp(IO_Output_Cmd_NameList[i], typeString))
		{
			outputCmdType = i;
			break;
		}
	}

	return outputCmdType;
}



/**
 * \brief	Get IO Output status to string
 */
size_t IO_GetOutputStates(char *str)
{
	uint8_t i;
	size_t length;
	length = usprintf(str, "Output status: ");

	for (i = 0; i < (IO_Output_Count - 1); i++)
	{
		IO_Status_t ioStatus = IO_Output_GetStatus(i+1);
		length += StrAppend(str, (ioStatus == IO_Status_On) ? "on" : "off");
	}

	return length;
}



#if defined(LED_TASK_PWM_STYLE)
/**
 * \brief	LED Task - PWM style
 * \note	Call this function periodically - 2 ms times
 */
void IO_LED_PWMTask(void)
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
		IO_Output_SetStatus(IO_LED_Blue, IO_Output_Cmd_SetOn);
		//IO_Output_SetStatus(IO_LED_Blue, IO_Output_Cmd_SetToggle);
	}
	else
	{
		IO_Output_SetStatus(IO_LED_Blue, IO_Output_Cmd_SetOff);
	}

	// PWM counter
	LED_PwmCnt++;
	if (LED_PwmCnt >= LED_PwmMaxLimit)	// Max limit
	{
		LED_PwmCnt = 0;
	}
}
#endif



#if defined(CONFIG_IO_OUTPUT_BLINK_ENABLE)
/**
 * \brief	Handle necessary IO output operations (e.g. blink)
 */
void IO_Output_Handler(void)
{
	// Now only have one task: check blink state and turn off the LED if need
	uint8_t i;
	for (i = 0; i < IO_Output_Count; i++)
	{
		if (IO_Output_ActualState[i] == IO_Output_Cmd_SetBlink)
		{
			// Need corrects the LED index
			IO_Output_SetStatus((i+1), IO_Output_Cmd_SetOff);
		}
	}
}
#endif /* CONFIG_IO_OUTPUT_BLINK_ENABLE */


/**************************************************
 * 				IO - Input
 **************************************************/


/**
 * \brief	Get Input State
 */
IO_Status_t IO_GetInputState(IO_Input_Name_t inputPin)
{
	IO_Status_t inputState = IO_Status_Unknown;

	if (inputPin >= IO_Input_Count)
	{
		return IO_Status_Unknown;
	}

	GPIO_TypeDef * port = (GPIO_TypeDef *)IO_Input_List[inputPin-1].GPIO_Port;
	uint32_t pin = IO_Input_List[inputPin-1].GPIO_Pin;
	IO_Status_t lowVoltageState = IO_Input_List[inputPin-1].lowVoltageState;

	uint32_t ioReadState = HAL_GPIO_ReadPin(port, pin);

	if (ioReadState == GPIO_PIN_RESET)
	{
		inputState = (lowVoltageState == IO_Status_On) ? IO_Status_On : IO_Status_Off;
	}
	else
	{
		// GPIO_PIN_SET
		inputState = (lowVoltageState == IO_Status_Off) ? IO_Status_On : IO_Status_Off;
	}

	// TODO: Store the read value
	//inputState = IO_InputStates[inputpin];

	return inputState;
}



/**
 * \brief	Get input name
 */
const char * IO_GetInputName(IO_Input_Name_t inputPin)
{
	const char * str = NULL;

	if (inputPin < IO_Input_Count)
	{
		// Value is ok
		str = IO_Input_List[inputPin].name;
	}

	return str;
}

// TODO: SetInputState - from ISR e.g.


#endif	// #ifdef CONFIG_MODULE_IO_ENABLE
