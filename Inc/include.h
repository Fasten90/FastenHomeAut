/*
 *		include.h
 *
 *  	Created on: 	2013.11.19.
 *    Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		University:		BME (BUTE)
 *		Project: 		RadioAlarm - Radio Alarm System modules
 *		Function:		configure: defines for panels, or functions
 *		Target:			STM32F107RC
 *		Version:		v2
 *		Last modified:	2014.05.16
 */

#ifndef INCLUDE_H_
#define INCLUDE_H_

// #define assert_param(expr) ((void)0)	// incopatible redefinition ...


// typedefs
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










///////////////////////////// CONFIGURE



/////////////////////////////
// CONFIG DEFINES
// INCLUDES
/////////////////////////////


///////////////////////////// CONFIG DEFINES


// !!IMPORTANT!!  TODO: Check this!
//#define HSE_VALUE 	(uint32_t)8000000
// not need, its included


/////////////////////////////
////// XXX: Select your terminal
////// for monitor.c
//#define CONFIG_USE_TERMINAL_HYPERTERMINAL
#define CONFIG_USE_TERMINAL_ZOC
//#define CONFIG_USE_TERMINAL_PUTTY


/////////////////////////////
////// XXX: Time init from USB
////// for log.c
//#define RADIOALARM_CONFIG_TIMEINITUSB


/////////////////////////////
////// XXX: Stop mode
////// for log.c, lowpower.c
//#define RADIOALARM_CONFIG_ENABLE_STOPMODE


/////////////////////////////
////// XXX: Select transmitter or receiver
////// for rfm12b.c
//#define RFM12B_TRANSMITTER
//#define RFM12B_RECEIVER


/////////////////////////////
////// XXX: If you want to send messages on UART (USB)
////// for main.c, log.c
//#define USE_RADIOALARM_OTG



// %%%%%%%%%%%%%%

// Select your panel

// Recommend define at project settings:
//#define CONFIG_USE_PANEL_NODESMALL
//#define CONFIG_USE_PANEL_NODEMEDIUM
//#define CONFIG_USE_PANEL_CENTERPANEL


///////////////////////////// INCLUDES


#ifdef CONFIG_USE_PANEL_NODESMALL
	#include "stm32f0xx_hal.h"
	#include "stm32f0xx_hal_def.h"
	
	#include "stm32f0xx_it.h"
	#include "stm32f0xx_hal_conf.h"
	
	#define CONFIG_ENABLE_ESP8266
	#define CONFIG_MODULE_FLASH_ENABLE
	#define CONFIG_MODULE_TEMPERATURE_ENABLE
	#define CONFIG_MODULE_ADC_ENABLE
#endif



#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	#include "stm32l1xx_hal.h"
	
	#include "stm32_hal_legacy.h"	// define-ok miatt?

	#define CONFIG_ENABLE_ESP8266
	#define CONFIG_ENABLE_DEBUGUSART
	#define CONFIG_USE_FREERTOS
	#define CONFIG_MODULE_FLASH_ENABLE
	#define CONFIG_MODULE_TEMPERATURE_ENABLE
	#define CONFIG_MODULE_ADC_ENABLE
	//#define CONFIG_USE_MONITOR_HISTORY
#endif




#ifdef CONFIG_USE_PANEL_CENTERPANEL
	#include "stm32f4xx_hal.h"
	
	#include "stm32_hal_legacy.h"	// define-ok miatt?
	
	#define CONFIG_ENABLE_ESP8266
	#define CONFIG_ENABLE_DEBUGUSART
	#define CONFIG_USE_FREERTOS
	
#endif

// TODO: Kultúráltra megírni
#ifndef CONFIG_USE_PANEL_NODEMSMALL
#ifndef CONFIG_USE_PANEL_NODEMEDIUM
#ifndef CONFIG_USE_PANEL_CENTERPANEL
#error "Miss config define! Use CONFIG_USE_PANEL_..."
#endif
#endif
#endif
//#if ( defined(CONFIG_USE_PANEL_NODEMSMALL) && !defined(CONFIG_USE_PANEL_NODEMEDIUM) && !defined(CONFIG_USE_PANEL_CENTERPANEL) ) )
//#error "Miss config define! Use CONFIG_USE_PANEL_..."
//#endif

// If you want use monitor program's history
// turn off, if has small memory, now it need 1.5k RAM
//#define USE_MONITOR_HISTORY


// FreeRTOS - at panel selection
//#define CONFIG_USE_FREERTOS

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




// ESP8266

//#define USE_ESP8266_TEST_FASTENHOME
#define USE_ESP8266_MODE_CLIENT
	
	

#include "board.h"

// MODUL INCLUDES

#ifdef CONFIG_USE_PANEL_NODESMALL

#include "usart.h"
#include "string.h"

#include "monitor.h"
#include "command.h"

//#include "homeautmessage.h"

#include "led.h"
#include "adc.h"
#include "button.h"
#include "temperature.h"
#include "flash.h"


#include "ESP8266.h"

#endif // #ifdef CONFIG_USE_PANEL_NODESMALL





#ifdef CONFIG_USE_PANEL_NODEMEDIUM


#include "led.h"
#include "button.h"


#include "usart.h"
#include "string.h"

#include "monitor.h"
#include "command.h"

#include "ESP8266.h"

#include "SysManager.h"
#include "homeautmessage.h"


#include "IO.h"

#include "adc.h"
//#include "flash.h"	// Work, but not need



#include "temperature.h"




#endif // #ifdef CONFIG_USE_PANEL_NODEMEDIUM



#ifdef CONFIG_USE_PANEL_CENTERPANEL


#include "led.h"
#include "button.h"


#include "usart.h"
#include "string.h"

#include "monitor.h"
#include "command.h"

#include "ESP8266.h"

#include "homeautmessage.h"
#include "SysManager.h"


#include "IO.h"

#include "raspberrypi.h"


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


#endif // #ifdef CONFIG_USE_PANEL_CENTERPANEL




#endif /* INCLUDE_H_ */
