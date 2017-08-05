#ifndef LED_H_
#define LED_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "include.h"
#include "options.h"
#include "board.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
	LED_Unknown,

	LED_Green,
#if LED_NUM_MAX > 1
	LED_Blue,
	LED_Red,
#endif

	LED_Count
} LED_Pin_t;


typedef enum
{
	LED_Set_DontCare,
	LED_Set_On,
	LED_Set_Off,
	LED_Set_Toggle,
	LED_Get_Status,
	// Do not use:
	LED_Type_Count
} LED_SetType_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void LED_Init(void);
void LED_Test(void);
bool LED_SetLed(LED_Pin_t pin, LED_SetType_t ledSet);
bool LED_GetStatus(LED_Pin_t pin);
LED_Pin_t LED_GetNumFromName(const char *name);
LED_SetType_t LED_GetTypeFromString(const char *typeString);
uint8_t LED_GetLedStates(char *str);



#endif // LED_H
