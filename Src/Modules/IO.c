/*
 *		IO.c
 *		Created on:		2016-01-01
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		IO module (Input-Output)
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-08
 */
 


/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "board.h"
#include "include.h"
#include "IO.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_IO_ENABLE


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

#if IO_INPUTS_NUM > 0
InputState_t IO_InputStates[Input_Count];
#endif

#if IO_OUTPUTS_NUM > 0
OutputState_t IO_OutputStates[Output_Count];
#endif


#if IO_INPUTS_NUM > 0
const char * const IO_InputNames[] =
{
#ifdef CONFIG_MODULE_IO_INPUT_MOTION_ENABLE
	"MotionMove",
#endif
#ifdef CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE
	"SoundImpact",
#endif
#ifdef CONFIG_MODULE_IO_BATTERY_CHARGER_ENABLE
	"BatteryCharger",
#endif

	// XXX: Add here new inputs
	// \note: Do not forget Sync with Input_t
};
#endif


#if IO_OUTPUTS_NUM > 0
const char * const IO_OutputNames[] =
{
#ifdef CONFIG_MODULE_IO_OUTPUT_EXAMPLE_ENABLE
	"Example",
#endif

	// XXX: Add here new outputs
	// \note: Do not forget Sync with Output_t
};
#endif


#if IO_INPUTS_NUM > 0
const char * const IO_InputStateNames[] =
{
	"Unknown",
	"Active",
	"Inactive"
};
#endif


#if IO_OUTPUTS_NUM > 0
const char * const IO_OutputStateNames[] =
{
	"Unknown",
	"Active",
	"Inactive",
	"Toggle",
};
#endif



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local functions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Initialize IOs
 */
void IO_Init(void)
{
#if IO_INPUTS_NUM > 0
	// Size check
	BUILD_BUG_ON((sizeof(IO_InputNames)/sizeof(IO_InputNames[0])) != Input_Count);
	//#error "IO_InputNames and Input_t aren't syncronized!"
	BUILD_BUG_ON((sizeof(IO_InputStateNames)/sizeof(IO_InputStateNames[0])) != InputState_Count);
#endif

#if IO_OUTPUTS_NUM > 0
	BUILD_BUG_ON((sizeof(IO_OutputNames)/sizeof(IO_OutputNames[0])) != Output_Count);
	//#error "IO_OutputNames and Output_t aren't syncronized!"
	BUILD_BUG_ON((sizeof(IO_OutputStateNames)/sizeof(IO_OutputStateNames[0])) != OutputState_Count);
#endif


#if (IO_INPUTS_NUM > 0) || (IO_OUTPUTS_NUM)
	GPIO_InitTypeDef GPIO_InitStruct;
#endif

	
#ifdef CONFIG_MODULE_IO_OUTPUT_EXAMPLE_ENABLE
	// OUTPUTS - RELAYS
	
	RELAY_PINS_CLK_ENABLES();
	
	
	GPIO_InitStruct.Pin = RELAY_1_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(RELAY_1_GPIO_PORT, &GPIO_InitStruct);

	RELAY_1_OFF();
	
#endif


#ifdef CONFIG_MODULE_IO_INPUT_MOTION_ENABLE
	
	SENSOR_MOTION_GPIO_PINS_CLK_ENABLE();
	
	// SENSOR_MOTION
	// PIR signaling; HIGH = movement/LOW = no movement
	GPIO_InitStruct.Pin = SENSOR_MOTION_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;	// moving
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(SENSOR_MOTION_GPIO_PORT, &GPIO_InitStruct);
#endif
	

#ifdef CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE

	SENSOR_SOUND_GPIO_PINS_CLK_ENABLE();

	// SENSOR_SOUND_IMPACT
	// PIR signaling; HIGH = movement/LOW = no movement
	GPIO_InitStruct.Pin = SENSOR_SOUND_IMPACT_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;	// sounding
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(SENSOR_SOUND_IMPACT_GPIO_PORT, &GPIO_InitStruct);
	
#endif


#if defined(CONFIG_MODULE_IO_INPUT_MOTION_ENABLE) || defined(CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE)
	// Initialize interrupts
	HAL_NVIC_SetPriority(SENSOR_SOUND_MOTION_INT_EXTI_IRQn,
			SENSOR_SOUND_MOTIOn_INT_PREEMT_PRIORITY,
			SENSOR_SOUND_MOTION_INT_SUB_PRIORITY);
	HAL_NVIC_EnableIRQ(SENSOR_SOUND_MOTION_INT_EXTI_IRQn);
#endif


#ifdef CONFIG_MODULE_IO_BATTERY_CHARGER_ENABLE
	IO_INPUT_BATTERYCHARGER_CLK_ENABLES();

	// BatteryCharger
	// Down: active
	// Up: not active
	GPIO_InitStruct.Pin = IO_INPUT_BATTERYCHARGER_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(IO_INPUT_BATTERYCHARGER_GPIO_PORT, &GPIO_InitStruct);

#endif

}



#if IO_OUTPUTS_NUM > 0
/**
 * \brief	Set output state
 */
void IO_SetOutputState(Output_t outputpin, OutputState_t outstate)
{
	// Save actual state
	if (outputpin < Output_Count && outstate > OutputState_Unknown && outstate < OutputState_Count)
	{
		// Values are okays
		IO_OutputStates[outputpin] = outstate;
		// Set state
		switch (outputpin)
		{
#ifdef CONFIG_MODULE_IO_OUTPUT_EXAMPLE_ENABLE
			case Output_Example:
				switch (outstate)
				{
					case OutputState_Active:
						RELAY_1_ON();
						break;

					case OutputState_Inactive:
						RELAY_1_OFF();
						break;

					case OutputState_Toggle:
						RELAY_1_TOGGLE();
						break;

					case OutputState_Unknown:
					case OutputState_Count:
					default:
						break;
				}
				break;
#endif

			case Output_Count:
			default:
				break;
		}
	}
}
#endif



/**
 * \brief	Set (Save) input state
 */
bool IO_SetInputState(Input_t inputpin, InputState_t inputstate)
{
	bool isOk = false;

	if (inputpin < Input_Count && inputstate > InputState_Unknown && inputstate < InputState_Count)
	{
		// Values are okays
		IO_InputStates[inputpin] = inputstate;

		// TODO: Request task schedule...
		//TaskHandler_RequestTaskScheduling(Task_);

		isOk = true;
	}

	return isOk;
}



/**
 * \brief	Get Input State
 */
InputState_t IO_GetInputState(Input_t inputpin)
{
	InputState_t inputstate = InputState_Unknown;

	if (inputpin < Input_Count)
	{
		// Pin value is okay
		inputstate = IO_InputStates[inputpin];
		if (inputstate == InputState_Unknown || inputstate >= InputState_Count)
		{
			// Wrong input state
			inputstate = InputState_Unknown;
		}
	}
	else
	{
		return InputState_Unknown;
	}


	// TODO: Read or do not read?
	switch (inputpin)
	{
#ifdef CONFIG_MODULE_IO_INPUT_MOTION_ENABLE
		case Input_MotionMove:
			if (HAL_GPIO_ReadPin(SENSOR_MOTION_GPIO_PORT, SENSOR_MOTION_GPIO_PIN) == GPIO_PIN_SET)
				IO_InputStates[inputpin] =
			break;
#endif
#ifdef CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE
	Input_SoundImpact,
#endif
#ifdef CONFIG_MODULE_IO_BATTERY_CHARGER_ENABLE
		case Input_BatteryCharger:
			if (HAL_GPIO_ReadPin(IO_INPUT_BATTERYCHARGER_GPIO_PORT, IO_INPUT_BATTERYCHARGER_GPIO_PIN) == GPIO_PIN_SET)
				IO_InputStates[inputpin] = InputState_Active;
			else
				IO_InputStates[inputpin] = InputState_Inactive;
			break;
#endif

		case Input_Count:
		default:
			break;
	}

	inputstate = IO_InputStates[inputpin];

	return inputstate;
}



#if IO_OUTPUTS_NUM > 0
/**
 * \brief	Get Output State
 */
OutputState_t IO_GetOutputState(Output_t outputpin)
{
	OutputState_t outputstate = OutputState_Unknown;

	if (outputpin < Output_Count)
	{
		// Values are okays
		outputstate = IO_OutputStates[outputpin];
	}

	return outputstate;

	// TODO: Read or do not read?
/*
	switch (inputpin)
	{
#ifdef CONFIG_MODULE_IO_INPUT_MOTION_ENABLE
		case Input_MotionMove:
			if (HAL_GPIO_ReadPin(SENSOR_MOTION_GPIO_PORT, SENSOR_MOTION_GPIO_PIN) == GPIO_PIN_SET)
				IO_InputStates[inputpin] =
			break;
#endif
#ifdef CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE
	Input_SoundImpact,
#endif

	}
*/

}
#endif



/**
 * \brief	Get input name
 */
const char * IO_GetInputName(Input_t inputpin)
{
	const char * str = NULL;

	if (inputpin < Input_Count)
	{
		// Value is ok
		str = IO_InputNames[inputpin];
	}

	return str;
}



#if IO_OUTPUTS_NUM > 0
/**
 * \brief	Get input name
 */
const char * IO_GetOutputName(Output_t outputpin)
{
	const char * str = NULL;

	if (outputpin < Output_Count)
	{
		// Value is ok
		str = IO_OutputNames[outputpin];
	}

	return str;
}
#endif



/**
 * \brief	Get input name
 */
const char * IO_GetInputStateName(InputState_t input)
{
	const char * str = NULL;

	if (input < InputState_Count)
	{
		// Value is ok
		str = IO_InputStateNames[input];
	}

	return str;
}



#if IO_OUTPUTS_NUM > 0
/**
 * \brief	Get input name
 */
const char * IO_GetOutputStateName(OutputState_t output)
{
	const char * str = NULL;

	if (output < OutputState_Count)
	{
		// Value is ok
		str = IO_OutputStateNames[output];
	}

	return str;
}
#endif



#endif // #ifdef CONFIG_MODULE_IO_ENABLE
