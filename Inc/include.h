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



// Include standard types
#include "GenericTypeDefs.h"



/*------------------------------------------------------------------------------
 *									Includes
 *----------------------------------------------------------------------------*/

/// Includes microcontroller family libraries
#ifdef CONFIG_MICROCONTROLLER_STM32F4xx
	// STM32F4 Discovery
	#include "stm32f4xx_hal.h"
	#include "stm32_hal_legacy.h"	// for defines
#endif


#ifdef CONFIG_MICROCONTROLLER_STM32F0xx
	// STM32F4 Discovery
	#include "stm32f0xx_hal.h"
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


/*------------------------------------------------------------------------------
 *									Include modules
 *----------------------------------------------------------------------------*/


// Common modules

#include "MEM.h"

#include "globals.h"
#include "String.h"
#include "Calc.h"
#include "DateTime.h"
#include "Network.h"
#include "Communication.h"

#if defined(CONFIG_MODULE_DEBUGUSART_ENABLE) || defined(CONFIG_MODULE_ESP8266_ENABLE)
#include "USART.h"
#endif

#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
#include "Terminal.h"
#include "DebugUart.h"
#endif

#ifdef CONFIG_MODULE_COMMANDHANDLER_ENABLE
#include "CommandHandler.h"
#include "CommandList.h"
#endif

#ifdef CONFIG_MODULE_LED_ENABLE
#include "LED.h"
#endif

#ifdef CONFIG_MODULE_LEDPWM_ENABLE
#include "LedPWM.h"
#endif

#ifdef CONFIG_MODULE_BUTTON_ENABLE
#include "Button.h"
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
#include "Flash_S25FL.h"	// Work, but not need
#endif

#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE
#include "CommonDac.h"
#endif

#ifdef CONFIG_MODULE_COMMON_IO_ENABLE
#include "CommonIO.h"
#endif

#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
#include "CommonAdc.h"
#endif

#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE
#include "CommonPWM.h"
#endif

#ifdef CONFIG_MODULE_MOTOR_ENABLE
#include "Motor.h"
#endif

#ifdef CONFIG_SWO_ENABLE
#include "SWO.h"
#endif

#ifdef CONFIG_MODULE_RTC_ENABLE
#include "RTC.h"
#endif

#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
#include "TaskHandler.h"
#include "TaskList.h"
#endif

#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
#include "EventLog.h"
#endif

#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
#include "Watchdog.h"
#else
#define Watchdog_Clear()
#endif


#ifdef CONFIG_MODULE_DISPLAY_ENABLE
#include "Display_SSD1306.h"
#include "Display.h"
#include "DisplayImages.h"
#endif

#include "Reset.h"


// Macro for checking
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))


// #define assert_param(expr) ((void)0)	// incompatible redefinition ...



extern void Error_Handler(void);



#endif /* INCLUDE_H_ */
