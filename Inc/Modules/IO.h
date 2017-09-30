/*
 *		IO.h
 *		Created on:		2016-01-01
 *		Author:			Vizi G�bor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		IO module (Input-Output)
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-08
 */

#ifndef IO_H_
#define IO_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "board.h"
#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
#ifdef CONFIG_MODULE_IO_INPUT_MOTION_ENABLE
	Input_MotionMove,
#endif
#ifdef CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE
	Input_SoundImpact,
#endif
#ifdef CONFIG_MODULE_IO_BATTERY_CHARGER_ENABLE
	Input_BatteryCharger,
#endif

	// XXX: Add here new inputs
	// \note: Do not forget Sync with IO_InputNames

	Input_Count
} Input_t;


typedef enum
{
#ifdef CONFIG_MODULE_IO_OUTPUT_EXAMPLE_ENABLE
	Output_Example,
#endif

	// XXX: Add here new outputs
	// \note: Do not forget Sync with IO_OutputNames

	Output_Count
} Output_t;


typedef enum
{
	InputState_Unknown,
	InputState_Active,
	InputState_Inactive,

	// Do not use, max. enum num
	InputState_Count
} InputState_t;


typedef enum
{
	OutputState_Unknown,
	OutputState_Active,
	OutputState_Inactive,
	OutputState_Toggle,

	// Do not use, max. enum num
	OutputState_Count
} OutputState_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_IO_ENABLE
void IO_Init(void);

void IO_SetOutputState(Output_t outputpin, OutputState_t outstate);
void IO_SetInputState(Input_t inputpin, InputState_t inputstate);

InputState_t IO_GetInputState(Input_t inputpin);
OutputState_t IO_GetOutputState(Output_t outputpin);

const char * IO_GetInputName(Input_t inputpin);
const char * IO_GetOutputName(Output_t outputpin);

const char * IO_GetInputStateName(InputState_t input);
const char * IO_GetOutputStateName(OutputState_t output);
#endif



#endif /* IO_H_ */
