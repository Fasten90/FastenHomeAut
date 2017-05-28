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



#endif // #ifdef CONFIG_MODULE_IO_ENABLE
