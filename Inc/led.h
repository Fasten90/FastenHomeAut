#ifndef LED_H_
#define LED_H_


#include "include.h"
#include "board.h"




#if BOARD_LED_LOGIC_HIGH_IS_ON == 1

#define LED_RED_ON()			HAL_GPIO_WritePin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN,GPIO_PIN_SET)
#define LED_BLUE_ON()			HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN,GPIO_PIN_SET)
#define LED_GREEN_ON()			HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN,GPIO_PIN_SET)
#define LED_RED_OFF()			HAL_GPIO_WritePin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN,GPIO_PIN_RESET)
#define LED_BLUE_OFF()			HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN,GPIO_PIN_RESET);
#define LED_GREEN_OFF()			HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN,GPIO_PIN_RESET)

#elif BOARD_LED_LOGIC_HIGH_IS_ON == 0

#define LED_RED_ON()			HAL_GPIO_WritePin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN,GPIO_PIN_RESET)
#define LED_BLUE_ON()			HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN,GPIO_PIN_RESET)
#define LED_GREEN_ON()			HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN,GPIO_PIN_RESET)
#define LED_RED_OFF()			HAL_GPIO_WritePin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN,GPIO_PIN_SET)
#define LED_BLUE_OFF()			HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN,GPIO_PIN_SET);
#define LED_GREEN_OFF()			HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN,GPIO_PIN_SET)	

#else

	#error	"There is not selected BOARD_LED_LOGIC ... "

#endif


#define LED_RED_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN)
#define LED_BLUE_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN)
#define LED_GREEN_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN)
#define LED_RED_STATUS()			HAL_GPIO_ReadPin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN)
#define LED_BLUE_STATUS()			HAL_GPIO_ReadPin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN)
#define LED_GREEN_STATUS()			HAL_GPIO_ReadPin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN)





#define  LED_TIMER_PERIOD_VALUE       (uint32_t)(666 - 1)  /* Period Value  */


#define LED_BLUE_SET_PERCENT(percent)	LED_PWM_ChangePercent(&TimLedBlueHandle,LED_BLUE_TIMER_CHANNEL,percent);
#define LED_GREEN_SET_PERCENT(percent)	LED_PWM_ChangePercent(&TimLedGreenHandle,LED_GREEN_TIMER_CHANNEL,percent);	



/*
#define  PULSE1_VALUE       (uint32_t)(PERIOD_VALUE/2)        //Capture Compare 1 Value
#define  PULSE2_VALUE       (uint32_t)(PERIOD_VALUE*37.5/100) // Capture Compare 2 Value
#define  PULSE3_VALUE       (uint32_t)(PERIOD_VALUE/4)        // Capture Compare 3 Value
#define  PULSE4_VALUE       (uint32_t)(PERIOD_VALUE*12.5/100) // Capture Compare 4 Value
*/




extern TIM_HandleTypeDef    TimLedBlueHandle;
extern TIM_HandleTypeDef    TimLedGreenHandle;




void LED_Init( void );
void LED_Run ( void );
void LED_InitTimers( void );
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim);
void LED_PWM_ChangePercent(TIM_HandleTypeDef *TimHandle, uint32_t timerChannel, uint8_t percent );







#endif // LED_H
