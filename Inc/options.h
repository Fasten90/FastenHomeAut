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


/*******************************************************************************
								CONFIG DEFINES
*******************************************************************************/

/// XXX: Select your panel
// NOTE: Recommend define at project settings:
//#define CONFIG_USE_PANEL_NUCLEOF401RE
//#define CONFIG_USE_PANEL_HOMEAUTCENTERPANEL
//#define CONFIG_USE_PANEL_STM32F4DISCOVERY

#if !defined(CONFIG_USE_PANEL_NUCLEOF401RE) \
	&& !defined(CONFIG_USE_PANEL_HOMEAUTCENTERPANEL) \
	&& !defined(CONFIG_USE_PANEL_STM32F4DISCOVERY)
#error "Miss config define! Use CONFIG_USE_PANEL_..."
#endif



/*******************************************************************************
									Modules
*******************************************************************************/

/// Enabled modules

#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL

	#define CONFIG_MICROCONTROLLER_STM32F4xx
	#define CONFIG_USE_PANEL_HOMEAUTPANELS

	#define CONFIG_MODULE_LED_ENABLE

	#define CONFIG_MODULE_BUTTON_ENABLE

	#define CONFIG_MODULE_DEBUGUSART_ENABLE

	#define CONFIG_MODULE_ESP8266_ENABLE
	#define CONFIG_USE_FREERTOS

	#define CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE

	//#define CONFIG_MODULE_FLASH_ENABLE
	//#define CONFIG_MODULE_TEMPERATURE_ENABLE
	//#define CONFIG_MODULE_ADC_ENABLE

#endif



#ifdef CONFIG_USE_PANEL_NUCLEOF401RE
	// STM32 Nucleo F401RE

	#define CONFIG_MICROCONTROLLER_STM32F4xx

	#define CONFIG_MODULE_DEBUGUSART_ENABLE

	#define CONFIG_MODULE_COMMANDHANDLER_ENABLE

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

	//#define CONFIG_MODULE_COMMON_DAC_ENABLE
	// Because, there is no  DAC in NUCLEOF401RE

	#define CONFIG_MODULE_COMMON_IO_ENABLE

	#define CONFIG_MODULE_COMMON_ADC_ENABLE

#endif



#ifdef CONFIG_USE_PANEL_STM32F4DISCOVERY
	// STM32F4 Discovery

	#define CONFIG_MICROCONTROLLER_STM32F4xx

	#define CONFIG_MODULE_DEBUGUSART_ENABLE

	#define CONFIG_MODULE_COMMANDHANDLER_ENABLE
	#define CONFIG_MODULE_TERMINAL_ENABLE

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

	//#define CONFIG_MODULE_COMMON_DAC_ENABLE

	//#define CONFIG_MODULE_COMMON_IO_ENABLE

	//#define CONFIG_MODULE_COMMON_ADC_ENABLE

	#define CONFIG_MODULE_RTC_ENABLE

	#define CONFIG_MODULE_TASKHANDLER_ENABLE

	#define CONFIG_MODULE_EVENTHANDLER_ENABLE

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



/*******************************************************************************
								Monitor settings
*******************************************************************************/

#ifdef CONFIG_MODULE_TERMINAL_ENABLE

	/// XXX: Select your terminal
	/// for CommandHandler.c
	//#define CONFIG_COMMANDHANDLER_USE_TERMINAL_HYPERTERMINAL
	#define CONFIG_COMMANDHANDLER_USE_TERMINAL_ZOC
	//#define CONFIG_COMMANDHANDLER_USE_TERMINAL_PUTTY


	/// For wait password
	//#define CONFIG_COMMANDHANDLER_GET_PASSWORD_ENABLE


	// Escape sequences
	// comment out, if you dont need escape sequence (cursors, history, cls, ...)
	#define CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE

	#ifdef CONFIG_COMMANDHANDLER_ESCAPE_SEQUENCE_ENABLE
		// If you want use monitor program's history
		// Turn off, if has small memory, now it need 1.5k RAM
		#define CONFIG_COMMANDHANDLER_USE_HISTORY
	#endif

#endif	// #ifdef CONFIG_MODULE_TERMINAL_ENABLE


/*******************************************************************************
								Other settings
*******************************************************************************/


/// Debug
#define CONFIG_DEBUG_MODE
#define CONFIG_SWO_ENABLE


/// Debug modes
//#define CONFIG_TASKHANDLER_DEBUG_ENABLE


/// Unit tests
//#define MODULE_STRING_UNITTEST_ENABLE
//#define MODULE_HOMEAUTMESSAGE_UNITTEST_ENABLE
//#define MODULE_DATETIME_UNITTEST_ENABLE


// ESP8266 settings
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	// XXX: IfC ONFIG_ESP8266_IS_SERVER defined to 1, ESP8266 will be server,
	// if not (0), it will be client
	#define CONFIG_ESP8266_IS_SERVER	0

	#define CONFIG_ESP8266_WIFI_NETWORK_NAME			"ARTN16"
	#define CONFIG_ESP8266_WIFI_NETWORK_PASSWORD		"Easdg2011"
	#ifndef CONFIG_ESP8266_WIFI_NETWORK_PASSWORD
		#warning "CONFIG_ESP8266_WIFI_NETWORK_PASSWORD defined not defined!"
		#define CONFIG_ESP8266_WIFI_NETWORK_PASSWORD	"Password"
	#endif

#endif


#endif /* OPTIONS_H_ */
