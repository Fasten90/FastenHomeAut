/*******************************************************************************
 * Purpose:
 ******************************************************************************/

#ifndef IO_H_
#define IO_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


#define	RELAY_1_OFF()		HAL_GPIO_WritePin(RELAY_1_GPIO_PORT,RELAY_1_GPIO_PIN,GPIO_PIN_RESET)
#define RELAY_1_ON()		HAL_GPIO_WritePin(RELAY_1_GPIO_PORT,RELAY_1_GPIO_PIN,GPIO_PIN_SET)
#define RELAY_1_TOGGLE()	HAL_GPIO_TogglePin(RELAY_1_GPIO_PORT,RELAY_1_GPIO_PIN)


#define	RELAY_2_OFF()		HAL_GPIO_WritePin(RELAY_2_GPIO_PORT,RELAY_2_GPIO_PIN,GPIO_PIN_RESET)
#define RELAY_2_ON()		HAL_GPIO_WritePin(RELAY_2_GPIO_PORT,RELAY_2_GPIO_PIN,GPIO_PIN_SET)


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
extern uint8_t GLOBAL_IO_Sensor_Motion_Move;
extern uint8_t GLOBAL_IO_Sensor_Sound_Impact_Sound;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/
#ifdef CONFIG_MODULE_IO_ENABLE
void IO_Init ( void );
#endif



#endif /* IO_H_ */