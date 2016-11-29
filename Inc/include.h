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


// #define assert_param(expr) ((void)0)	// incopatible redefinition ...


// typedefs
#include <stdbool.h>

//
typedef unsigned char				uint8_t;	
typedef signed char					int8_t;		
typedef short unsigned int			uint16_t;
typedef short signed int			int16_t;
#ifndef CONFIG_USE_DEV_ECLIPSE
typedef unsigned int				uint32_t;
typedef signed int					int32_t;
#endif
//



typedef enum
{
	Return_Ok = 0,
	Return_Success,	// Ez kell ????
	Return_False,
	Return_Error,
	Return_Timeout
	
} ReturnType;



// TODO: Áttenni máshova
typedef enum
{
	Source_Unknown,
	Source_DebugUart,
	Source_OtherUart,
	// Do not use:
	Source_Count
} CommProtocol_t;




///////////////////////////// CONFIGURE



/////////////////////////////
// CONFIG DEFINES
// INCLUDES
/////////////////////////////


#if !defined(CONFIG_USE_PANEL_NODEMSMALL) && !defined(CONFIG_USE_PANEL_NODEMEDIUM) \
	&& !defined(CONFIG_USE_PANEL_CENTERPANEL) && !defined(CONFIG_USE_PANEL_DISCOVERY)
#error "Miss config define! Use CONFIG_USE_PANEL_..."
#endif

///////////////////////////// INCLUDES



#ifdef CONFIG_USE_PANEL_NODESMALL
	#include "stm32f0xx_hal.h"
	#include "stm32f0xx_hal_def.h"
	
	#include "stm32f0xx_it.h"
	#include "stm32f0xx_hal_conf.h"
	
	#define CONFIG_MODULE_ESP8266_ENABLE
	#define CONFIG_MODULE_FLASH_ENABLE
	#define CONFIG_MODULE_TEMPERATURE_ENABLE
	#define CONFIG_MODULE_ADC_ENABLE
#endif



#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	#include "stm32l1xx_hal.h"
	
	#include "stm32_hal_legacy.h"	// define-ok miatt?

	#define CONFIG_MODULE_ESP8266_ENABLE
	#define CONFIG_MODULE_DEBUGUSART_ENABLE
	#define CONFIG_USE_FREERTOS
	#define CONFIG_MODULE_FLASH_ENABLE
	#define CONFIG_MODULE_TEMPERATURE_ENABLE
	#define CONFIG_MODULE_ADC_ENABLE
	//#define CONFIG_USE_MONITOR_HISTORY
#endif




#ifdef CONFIG_USE_PANEL_CENTERPANEL
	#include "stm32f4xx_hal.h"
	
	#include "stm32_hal_legacy.h"	// for defines
	
	#define CONFIG_MODULE_ESP8266_ENABLE

	#define CONFIG_MODULE_DEBUGUSART_ENABLE
	//#define CONFIG_USE_FREERTOS
	
	#define CONFIG_MODULE_ESP8266_ENABLE
	#define CONFIG_MODULE_DEBUGUSART_ENABLE
	#define CONFIG_USE_FREERTOS
	#define CONFIG_MODULE_FLASH_ENABLE
	#define CONFIG_MODULE_TEMPERATURE_ENABLE
	#define CONFIG_MODULE_ADC_ENABLE

#endif



#ifdef CONFIG_USE_PANEL_DISCOVERY
	// STM32F4 Discovery

	#include "stm32f4xx_hal.h"
	#include "stm32_hal_legacy.h"	// for defines

	#define CONFIG_MODULE_DEBUGUSART_ENABLE

	//#define CONFIG_USE_FREERTOS

	#define CONFIG_MODULE_LED_ENABLE

	#define CONFIG_MODULE_BUTTON_ENABLE

	//#define CONFIG_MODULE_IO_ENABLE

	#define CONFIG_MODULE_RASPBERRYPI_ENABLE

	//#define CONFIG_MODULE_ESP8266_ENABLE

	//#define CONFIG_MODULE_FLASH_ENABLE

	//#define CONFIG_MODULE_TEMPERATURE_ENABLE

	//#define CONFIG_MODULE_ADC_ENABLE

	//#define CONFIG_MODULE_SYSMANAGER_ENABLE

	#define CONFIG_MODULE_DAC_ENABLE

	#define CONFIG_MODULE_COMMON_IO

#endif






/** Kernel */
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

#define MONITOR_TASK_STACK_SIZE			configMINIMAL_STACK_SIZE * 3
#define MONITOR_TASK_PRIORITY			( tskIDLE_PRIORITY + 3UL )

#define ESP8266_TASK_STACK_SIZE			configMINIMAL_STACK_SIZE * 3
#define ESP8266_TASK_PRIORITY			( tskIDLE_PRIORITY + 3UL )

#define SYSMANAGER_TASK_STACK_SIZE		configMINIMAL_STACK_SIZE * 2
#define SYSMANAGER_TASK_PRIORITY		( tskIDLE_PRIORITY + 3UL )



#endif	// #ifdef CONFIG_USE_FREERTOS


	
// For board defines (pins, ports)
#include "board.h"


// MODUL INCLUDES


// Nincs kész:
//#include "buzzer.h"
//#include "lcd.h"
//#include "eeprom.h"
//#include "rfm12b.h"
//#include "lis302dl.h"
//#include "lowpower.h"
//#include "rtc.h"

//#include "clock.h"

//#include "log.h"



// Need include, because used LED_x_ON / OFF() macros
#include "led.h"

#include "usart.h"
#include "string.h"


#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
#include "CommandHandler.h"
#include "command.h"
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

#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
#include "homeautmessage.h"
#include "raspberrypi.h"
#endif

#ifdef CONFIG_MODULE_ADC_ENABLE
#include "adc.h"
#endif

#ifdef CONFIG_MODULE_FLASH_ENABLE
#include "flash.h"	// Work, but not need
#endif

#ifdef CONFIG_MODULE_DAC_ENABLE
#include "dac.h"
#endif

#ifdef CONFIG_MODULE_COMMON_IO
#include "commonIO.h"
#endif

void Error_Handler( void );



#endif /* INCLUDE_H_ */
