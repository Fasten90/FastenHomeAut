/*
 *		IO.c
 *
 *		Created on:		2015.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 8.
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


const char * const IO_InputNames[] =
{
#ifdef CONFIG_MODULE_IO_INPUT_MOTION_ENABLE
	"MotionMove",
#endif
#ifdef CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE
	"SoundImpact",
#endif
};


const char * const IO_OutputNames[] =
{
#ifdef CONFIG_MODULE_IO_OUTPUT_EXAMPLE_ENABLE
	"Example",
#endif
};



const char * const IO_InputStateNames[] =
{
	"Unknown",
	"Active",
	"Inactive"
};


const char * const IO_OutputStateNames[] =
{
	"Unknown",
	"Active",
	"Inactive",
	"Toggle",
};



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

	// Size check
	BUILD_BUG_ON((sizeof(IO_InputNames)/sizeof(IO_InputNames[0])) != Input_Count);
	//#error "IO_InputNames and Input_t aren't syncronized!"

	BUILD_BUG_ON((sizeof(IO_OutputNames)/sizeof(IO_OutputNames[0])) != Output_Count);
	//#error "IO_OutputNames and Output_t aren't syncronized!"

	BUILD_BUG_ON((sizeof(IO_InputStateNames)/sizeof(IO_InputStateNames[0])) != InputState_Count);
	BUILD_BUG_ON((sizeof(IO_OutputStateNames)/sizeof(IO_OutputStateNames[0])) != OutputState_Count);


	GPIO_InitTypeDef GPIO_InitStruct;
	
	
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

}



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



/**
 * \brief	Set (Save) input state
 */
void IO_SetInputState(Input_t inputpin, InputState_t inputstate)
{
	if (inputpin < Input_Count && inputstate > InputState_Unknown && inputstate < InputState_Count)
	{
		// Values are okays
		IO_InputStates[inputpin] = inputstate;

		// TODO: Request task schedule...
		//TaskHandler_RequestTaskScheduling(Task_);
	}
}



/**
 * \brief	Get Input State
 */
InputState_t IO_GetInputState(Input_t inputpin)
{
	InputState_t inputstate = InputState_Unknown;

	if (inputpin < Input_Count && inputstate > InputState_Unknown)
	{
		// Values are okays
		inputstate = IO_InputStates[inputpin];
	}

	return inputstate;

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



#endif // #ifdef CONFIG_MODULE_IO_ENABLE
