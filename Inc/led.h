#ifndef LED_H_
#define LED_H_


#include "include.h"
#include "options.h"
#include "board.h"




#ifdef CONFIG_MODULE_LED_ENABLE


#define LED_NUM_MIN				(1)
#define LED_NUM_MAX				(3)


#define LED_RED_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN)
#define LED_BLUE_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN)
#define LED_GREEN_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN)

#define LED_RED_STATUS()		HAL_GPIO_ReadPin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN)
#define LED_BLUE_STATUS()		HAL_GPIO_ReadPin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN)
#define LED_GREEN_STATUS()		HAL_GPIO_ReadPin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN)


#else

// if not defined CONFIG_MODULE_LED_ENABLE
#define LED_RED_ON()
#define LED_BLUE_ON()
#define LED_GREEN_ON()
#define LED_RED_OFF()
#define LED_BLUE_OFF()
#define LED_GREEN_OFF()

#endif



#define  LED_TIMER_PERIOD_VALUE       (uint32_t)(666 - 1)  /* Period Value  */


#define LED_BLUE_SET_PERCENT(percent)	LED_PWM_ChangePercent(&TimLedBlueHandle,LED_BLUE_TIMER_CHANNEL,percent);
#define LED_GREEN_SET_PERCENT(percent)	LED_PWM_ChangePercent(&TimLedGreenHandle,LED_GREEN_TIMER_CHANNEL,percent);	



/*
#define  PULSE1_VALUE       (uint32_t)(PERIOD_VALUE/2)        //Capture Compare 1 Value
#define  PULSE2_VALUE       (uint32_t)(PERIOD_VALUE*37.5/100) // Capture Compare 2 Value
#define  PULSE3_VALUE       (uint32_t)(PERIOD_VALUE/4)        // Capture Compare 3 Value
#define  PULSE4_VALUE       (uint32_t)(PERIOD_VALUE*12.5/100) // Capture Compare 4 Value
*/



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



extern TIM_HandleTypeDef    TimLedBlueHandle;
extern TIM_HandleTypeDef    TimLedGreenHandle;




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
