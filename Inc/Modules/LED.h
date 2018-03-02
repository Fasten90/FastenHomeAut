/*
 *		LED.h
 *		Creat on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		LED functions
 *		Target:			STM32Fx
 *		Version:		v2
 *		Last modified:	2016-11-23
 */


#ifndef IO_H_
#define IO_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"
#include "board.h"
#include "LedList.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

// Configs:
#define CONFIG_IO_OUTPUT_BLINK_ENABLE



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< LED command
typedef enum
{
	IO_Output_Cmd_DontCare,

	IO_Output_Cmd_SetOn,
	IO_Output_Cmd_SetOff,
	IO_Output_Cmd_SetToggle,
#if defined(CONFIG_IO_OUTPUT_BLINK_ENABLE)
	IO_Output_Cmd_SetBlink,
#endif
	IO_Output_Cmd_GetStatus,

	// Do not use:
	IO_Output_Cmd_Count
} IO_Output_Cmd_t;


///< LED State
typedef enum
{
	IO_Status_Unknown,
	IO_Status_Off,
	IO_Status_On,
} IO_Status_t;


///< LED registration record struct
typedef struct
{
	// GPIO registrations
	const GPIO_TypeDef * const GPIO_Port;	///< GPIO port
	const uint32_t GPIO_Pin;				///< GPIO Pin
	const IO_Status_t lowVoltageState;		///< Low voltage state

	const char * const name;				///< Name of LED
} IO_Output_Record_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_IO_ENABLE

void IO_Init(void);
void IO_Output_Test(void);
IO_Status_t IO_Output_SetStatus(IO_Output_Name_t ioName, IO_Output_Cmd_t ioCmd);
IO_Status_t IO_Output_GetStatus(IO_Output_Name_t ioName);
IO_Output_Name_t IO_GetOutputNumFromName(const char *name);
IO_Output_Cmd_t IO_GetOutputTypeFromString(const char *typeString);
size_t IO_GetOutputStates(char *str);

#else

// Empty IO set definition
#define IO_Output_SetStatus(_pin, _set)

#endif

void IO_LED_PWMTask(void);
void IO_Output_Handler(void);



#endif /* IO_H_ */
