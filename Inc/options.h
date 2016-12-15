/*
 *		options.h
 *
 * 		Created on:		2016.09.28
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		compiler options for project
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_


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
/// XXX: Monitor settings

// For wait password
//#define MONITOR_GET_PASSWORD_ENABLE


// comment out, if you dont need escape sequence (cursors, history, cls, ...)
#define MONITOR_ESCAPE_SEQUENCE_ENABLE

#ifdef MONITOR_ESCAPE_SEQUENCE_ENABLE
	// If you want use monitor program's history
	// Turn off, if has small memory, now it need 1.5k RAM
	#define USE_MONITOR_HISTORY
#endif


// ESP8266
//#define USE_ESP8266_TEST_FASTENHOME
#define USE_ESP8266_MODE_CLIENT



/////////////////////////////
// 		Select your panel
/////////////////////////////

// XXX: Select your panel
// NOTE: Recommend define at project settings:
//#define CONFIG_USE_PANEL_NODESMALL
//#define CONFIG_USE_PANEL_NODEMEDIUM
//#define CONFIG_USE_PANEL_CENTERPANEL
//#define CONFIG_USE_PANEL_DISCOVERY


#if !defined(CONFIG_USE_PANEL_NODEMSMALL) && !defined(CONFIG_USE_PANEL_NODEMEDIUM) \
	&& !defined(CONFIG_USE_PANEL_CENTERPANEL) && !defined(CONFIG_USE_PANEL_DISCOVERY)
#error "Miss config define! Use CONFIG_USE_PANEL_..."
#endif


// Enabled modules

#ifdef CONFIG_USE_PANEL_NODESMALL

	#define CONFIG_MODULE_ESP8266_ENABLE
	#define CONFIG_MODULE_FLASH_ENABLE
	#define CONFIG_MODULE_TEMPERATURE_ENABLE
	#define CONFIG_MODULE_ADC_ENABLE
#endif



#ifdef CONFIG_USE_PANEL_NODEMEDIUM

	#define CONFIG_MODULE_ESP8266_ENABLE
	#define CONFIG_MODULE_DEBUGUSART_ENABLE
	#define CONFIG_USE_FREERTOS
	#define CONFIG_MODULE_FLASH_ENABLE
	#define CONFIG_MODULE_TEMPERATURE_ENABLE
	#define CONFIG_MODULE_ADC_ENABLE
	//#define CONFIG_USE_MONITOR_HISTORY
#endif




#ifdef CONFIG_USE_PANEL_CENTERPANEL

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

	#define CONFIG_MODULE_DEBUGUSART_ENABLE

	#define CONFIG_MODULE_MONITOR_ENABLE

	//#define CONFIG_USE_FREERTOS

	#define CONFIG_MODULE_LED_ENABLE

	#define CONFIG_MODULE_BUTTON_ENABLE

	//#define CONFIG_MODULE_IO_ENABLE

	//#define CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE

	//#define CONFIG_MODULE_RASPBERRYPI_ENABLE

	//#define CONFIG_MODULE_ESP8266_ENABLE

	//#define CONFIG_MODULE_FLASH_ENABLE

	//#define CONFIG_MODULE_TEMPERATURE_ENABLE

	//#define CONFIG_MODULE_ADC_ENABLE

	//#define CONFIG_MODULE_SYSMANAGER_ENABLE

	#define CONFIG_MODULE_COMMON_DAC_ENABLE

	#define CONFIG_MODULE_COMMON_IO_ENABLE

	#define CONFIG_MODULE_COMMON_ADC_ENABLE

	//#define CONFIG_MODULE_STL_SELFTEST_ENABLE
#endif



#ifdef CONFIG_USE_FREERTOS

// FreeRTOS task defines

#define MONITOR_TASK_STACK_SIZE			configMINIMAL_STACK_SIZE * 10
#define MONITOR_TASK_PRIORITY			( tskIDLE_PRIORITY + 3UL )

#define ESP8266_TASK_STACK_SIZE			configMINIMAL_STACK_SIZE * 3
#define ESP8266_TASK_PRIORITY			( tskIDLE_PRIORITY + 3UL )

#define SYSMANAGER_TASK_STACK_SIZE		configMINIMAL_STACK_SIZE * 2
#define SYSMANAGER_TASK_PRIORITY		( tskIDLE_PRIORITY + 3UL )

#endif



/// Debug
#define CONFIG_DEBUG_MODE
#define CONFIG_SWO_ENABLE


/// Unit tests
#define MODULE_STRING_UNIT_TEST_ENABLE



#endif /* OPTIONS_H_ */
