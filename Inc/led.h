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


// TODO: PWM LED

#define  LED_TIMER_PERIOD_VALUE       (uint32_t)(666 - 1)  /* Period Value  */


#define LED_BLUE_SET_PERCENT(percent)	LED_PWM_ChangePercent(&TimLedBlueHandle,LED_BLUE_TIMER_CHANNEL,percent);
#define LED_GREEN_SET_PERCENT(percent)	LED_PWM_ChangePercent(&TimLedGreenHandle,LED_GREEN_TIMER_CHANNEL,percent);	


/*
#define  PULSE1_VALUE       (uint32_t)(PERIOD_VALUE/2)        //Capture Compare 1 Value
#define  PULSE2_VALUE       (uint32_t)(PERIOD_VALUE*37.5/100) // Capture Compare 2 Value
#define  PULSE3_VALUE       (uint32_t)(PERIOD_VALUE/4)        // Capture Compare 3 Value
#define  PULSE4_VALUE       (uint32_t)(PERIOD_VALUE*12.5/100) // Capture Compare 4 Value
*/



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

extern TIM_HandleTypeDef    TimLedBlueHandle;
extern TIM_HandleTypeDef    TimLedGreenHandle;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void LED_Init(void);
void LED_Test(void);
bool LED_SetLed(uint8_t num, LED_SetType ledSet);
bool LED_GetStatus(uint8_t num);
uint8_t LED_GetNumFromName(const char*name);
LED_SetType LED_GetTypeFromString(const char*typeString);


void LED_InitTimers(void);
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim);
void LED_PWM_ChangePercent(TIM_HandleTypeDef *TimHandle,
		uint32_t timerChannel, uint8_t percent);



#endif // LED_H
