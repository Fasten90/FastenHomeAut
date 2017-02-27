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
	LED_SET_DONTCARE,
	LED_SET_ON,
	LED_SET_OFF,
	LED_SET_TOGGLE,
	LED_GET_STATUS,
	// Do not use:
	LED_TYPE_COUNT
} LED_SetType;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void LED_Init(void);
void LED_Test(void);
bool LED_SetLed(uint8_t num, LED_SetType ledSet);
bool LED_GetStatus(uint8_t num);
uint8_t LED_GetNumFromName(const char *name);
LED_SetType LED_GetTypeFromString(const char *typeString);
uint8_t LED_GetLedStates(char *str);



#endif // LED_H
