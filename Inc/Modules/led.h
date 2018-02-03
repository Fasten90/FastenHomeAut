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


#ifndef LED_H_
#define LED_H_


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
//#define CONFIG_LED_BLINK_ENABLE



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< LED command
typedef enum
{
	LED_Cmd_DontCare,

	LED_Cmd_SetOn,
	LED_Cmd_SetOff,
	LED_Cmd_SetToggle,
#if defined(CONFIG_LED_BLINK_ENABLE)
	LED_Cmd_SetBlink,
#endif
	LED_Cmd_GetStatus,

	// Do not use:
	LED_Cmd_Count
} LED_Cmd_t;


///< LED State
typedef enum
{
	LED_State_Unknown,
	LED_State_Off,
	LED_State_On,
} LED_Status_t;


///< LED registration record struct
typedef struct
{
	// GPIO registrations
	const GPIO_TypeDef * const GPIO_Port;	///< GPIO port
	const uint32_t GPIO_Pin;				///< GPIO Pin
	const LED_Status_t lowVoltageState;		///< Low voltage state

	const char * const name;				///< Name of LED
} LED_Record_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_LED_ENABLE
void LED_Init(void);
void LED_Test(void);
LED_Status_t LED_SetLed(LED_Name_t ledName, LED_Cmd_t ledCmd);
LED_Status_t LED_GetStatus(LED_Name_t ledName);
LED_Name_t LED_GetNumFromName(const char *name);
LED_Cmd_t LED_GetTypeFromString(const char *typeString);
uint8_t LED_GetLedStates(char *str);
#else
// Empty Led set definition
#define LED_SetLed(_pin, _set)
#endif

void LED_PWMTask(void);
void LED_Handler(void);



#endif // LED_H
