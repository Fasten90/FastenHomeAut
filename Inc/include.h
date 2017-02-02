/*
 *		include.h
 *
 *  	Created on: 	2013.11.19.
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		include file for project
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef INCLUDE_H_
#define INCLUDE_H_


#include "options.h"


// #define assert_param(expr) ((void)0)	// incompatible redefinition ...


// Include libs, important headers


// bool typedef
#include <stdbool.h>


/// Integer typedefs

typedef unsigned char				uint8_t;	
typedef signed char					int8_t;		
typedef short unsigned int			uint16_t;
typedef short signed int			int16_t;
#ifndef CONFIG_USE_COMPILER_GCC
typedef unsigned int				uint32_t;
typedef signed int					int32_t;
#endif



/*******************************************************************************
 * 									INCLUDES
 ******************************************************************************/


/// Includes microcontroller family libraries
#ifdef CONFIG_MICROCONTROLLER_STM32F4xx
	// STM32F4 Discovery
	#include "stm32f4xx_hal.h"
	#include "stm32_hal_legacy.h"	// for defines
#endif



/// FreeRTOS includes
#ifdef CONFIG_USE_FREERTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "list.h"

/*
FreeRTOS/Source/tasks.c
FreeRTOS/Source/queue.c
FreeRTOS/Source/list.c
FreeRTOS/Source/portable/[compiler]/[architecture]/port.c.
FreeRTOS/Source/portable/MemMang/heap_x.c where 'x' is 1, 2, 3, 4 or 5.
*/

#endif	// #ifdef CONFIG_USE_FREERTOS


	
/// For board defines (pins, ports)
#include "board.h"


/// MODULE INCLUDES

#include "MEM.h"

#include "globals.h"
#include "string.h"
#include "calc.h"
#include "DateTime.h"
#include "network.h"

#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
#include "usart.h"
#include "Terminal.h"
#include "CommandHandler.h"
#include "commandList.h"
#endif

#ifdef CONFIG_MODULE_LED_ENABLE
#include "led.h"
#endif

#ifdef CONFIG_MODULE_BUTTON_ENABLE
#include "button.h"
#endif

#ifdef CONFIG_MODULE_IO_ENABLE
#include "IO.h"
#endif

#ifdef CONFIG_MODULE_ESP8266_ENABLE
#include "ESP8266.h"
#endif

#ifdef CONFIG_MODULE_SYSMANAGER_ENABLE
#include "homeautmessage.h"
#include "SysManager.h"
#endif

#ifdef MODUL_TEMPERATURE_ENABLE
#include "temperature.h"
#endif

#ifdef CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE
#include "homeautmessage.h"
#endif

#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
#include "raspberrypi.h"
#endif

#ifdef CONFIG_MODULE_ADC_ENABLE
#include "adc.h"
#endif

#ifdef CONFIG_MODULE_FLASH_ENABLE
#include <Flash_S25FL.h>	// Work, but not need
#endif

#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE
#include "commonDac.h"
#endif

#ifdef CONFIG_MODULE_COMMON_IO_ENABLE
#include "commonIO.h"
#endif

#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
#include "commonAdc.h"
#endif

#ifdef CONFIG_SWO_ENABLE
#include "SWO.h"
#endif

#ifdef CONFIG_MODULE_RTC_ENABLE
#include "RTC.h"
#endif

void Error_Handler( void );


#endif /* INCLUDE_H_ */
