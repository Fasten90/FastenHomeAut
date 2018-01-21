/*
 *		options.h
 * 		Created on:		2016-09-28
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		compiler options for project
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_



/*------------------------------------------------------------------------------
 *								Config Defines
 *----------------------------------------------------------------------------*/
 
/// XXX: Select your panel
// NOTE: Recommend define at project settings:
//#define CONFIG_USE_PANEL_NUCLEOF401RE
//#define CONFIG_USE_PANEL_STM32F4DISCOVERY
//#define CONFIG_USE_PANEL_HOMEAUTCENTERPANEL
//#define CONFIG_USE_PANEL_HOMEAUTNODESMALL
//#define CONFIG_USE_PANEL_FASTENNODE
//#define CONFIG_USE_PANEL_PC

#if !defined(CONFIG_USE_PANEL_NUCLEOF401RE) \
	&& !defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) \
	&& !defined(CONFIG_USE_PANEL_HOMEAUTCENTERPANEL) \
	&& !defined(CONFIG_USE_PANEL_HOMEAUTNODESMALL) \
	&& !defined(CONFIG_USE_PANEL_FASTENNODE) \
	&& !defined(CONFIG_USE_PANEL_PC)
#error "Miss config define! Use CONFIG_USE_PANEL_..."
#endif



/*------------------------------------------------------------------------------
 *								Logical functions
 *----------------------------------------------------------------------------*/


///< Remote controller (for car) function
//#define CONFIG_FUNCTION_REMOTECONTROLLER_CAR
#ifdef CONFIG_FUNCTION_REMOTECONTROLLER_CAR
	#ifndef CONFIG_MODULE_MOTOR_ENABLE
		#define CONFIG_MODULE_MOTOR_ENABLE
	#endif
	#define CONFIG_MODULE_BLUETOOTH_ENABLE
#endif


///< Remote controller (for controller) function
//#define CONFIG_FUNCTION_REMOTECONTROLLER
#ifdef CONFIG_FUNCTION_REMOTECONTROLLER
	#ifndef CONFIG_MODULE_BUTTON_ENABLE
		#define CONFIG_MODULE_BUTTON_ENABLE
	#endif
	#define CONFIG_MODULE_BLUETOOTH_ENABLE
	/*#ifndef CONFIG_MODULE_ESP8266_ENABLE
		#define CONFIG_MODULE_ESP8266_ENABLE
	#endif
	#define CONFIG_FUNCTION_ESP8266_WRITE_IP_TO_DISPLAY
	*/
#endif


#if defined(CONFIG_FUNCTION_REMOTECONTROLLER_CAR) && defined(CONFIG_FUNCTION_REMOTECONTROLLER)
#error "RemoteController - controller and car is too defined. Do not use them together!"
#endif


///< Display: Menu
#define CONFIG_FUNCTION_DISPLAY_MENU
#ifdef CONFIG_FUNCTION_DISPLAY_MENU
	#ifndef CONFIG_MODULE_DISPLAY_ENABLE
		#define CONFIG_MODULE_DISPLAY_ENABLE
	#endif
	// Display: clock
	#define CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK
	// Display: Snake game
	#define CONFIG_FUNCTION_GAME_SNAKE
	// Display: Input
	#define CONFIG_FUNCTION_DISPLAY_INPUT
	// Display: Show screen
	#define CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN

	// Display: Scrolling menu (Use only for longer then 3 menu)
	#define CONFIG_FUNCTION_DISPLAY_MENU_SCROLLING
#endif


///< Display: Show screen
//#define CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
// TODO: What we want to displaying


///< Display: Show clock
//#define CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK
#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK
	#define CONFIG_DISPLAY_CLOCK_LARGE
	#define CONFIG_DISPLAY_CLOCK_SMALL
	#ifndef CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE
		#define CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE
	#endif
#endif


///< Display: Clock change function
//#define CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK
#ifdef CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK
	#ifndef CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK
		#define CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK
	#endif
	#ifndef CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE
		#define CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE
	#endif
#endif


///< Display: Input function
//#define CONFIG_FUNCTION_DISPLAY_INPUT
#ifdef CONFIG_FUNCTION_DISPLAY_INPUT
	#ifndef CONFIG_MODULE_DISPLAY_ENABLE
		#define CONFIG_MODULE_DISPLAY_ENABLE
	#endif
	#ifndef CONFIG_MODULE_BUTTON_ENABLE
		#define CONFIG_MODULE_BUTTON_ENABLE
	#endif
#endif


///< Charger function
//#define CONFIG_FUNCTION_CHARGER
#ifdef CONFIG_FUNCTION_CHARGER
	#ifndef CONFIG_MODULE_DISPLAY_ENABLE
		#define CONFIG_MODULE_DISPLAY_ENABLE
	#endif
	#ifndef CONFIG_MODULE_ADC_ENABLE
		#define CONFIG_MODULE_ADC_ENABLE
	#endif
	#ifndef CONFIG_MODULE_IO_ENABLE
		#define CONFIG_MODULE_IO_ENABLE
	#endif
	#ifndef CONFIG_MODULE_IO_BATTERY_CHARGER_ENABLE
		#define CONFIG_MODULE_IO_BATTERY_CHARGER_ENABLE
	#endif
#endif


///< Snake game
//#define CONFIG_FUNCTION_GAME_SNAKE
#ifdef CONFIG_FUNCTION_GAME_SNAKE
	#ifndef CONFIG_MODULE_DISPLAY_ENABLE
		#define CONFIG_MODULE_DISPLAY_ENABLE
	#endif
	#ifndef CONFIG_DISPLAY_FONT8X5_ENABLE
		#define CONFIG_DISPLAY_FONT8X5_ENABLE
	#endif
	#ifndef CONFIG_DISPLAY_FONT12X8_ENABLE
		#define CONFIG_DISPLAY_FONT12X8_ENABLE
	#endif
#endif


///< Periodical sending UART message function
//#define CONFIG_FUNCTION_PERIODICAL_SENDING



/*------------------------------------------------------------------------------
 *									Modules
 *----------------------------------------------------------------------------*/

/// Enabled modules

#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL

	#define CONFIG_PLATFORM_MCU_STM32F4xx
	#define CONFIG_USE_PANEL_HOMEAUTPANELS

	#define CONFIG_MODULE_LED_ENABLE

	#define CONFIG_MODULE_BUTTON_ENABLE

	#define CONFIG_MODULE_DEBUGUART_ENABLE

	#define CONFIG_MODULE_ESP8266_ENABLE
	#define CONFIG_USE_FREERTOS

	#define CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE

	//#define CONFIG_MODULE_FLASH_ENABLE
	//#define CONFIG_MODULE_TEMPERATURE_ENABLE
	//#define CONFIG_MODULE_ADC_ENABLE

#endif



#ifdef CONFIG_USE_PANEL_NUCLEOF401RE
	// STM32 Nucleo F401RE

	#define CONFIG_PLATFORM_MCU_STM32F4xx

	#define CONFIG_MODULE_DEBUGUART_ENABLE

	#define CONFIG_MODULE_COMMANDHANDLER_ENABLE

	//#define CONFIG_USE_FREERTOS

	#define CONFIG_MODULE_LED_ENABLE

	#define CONFIG_MODULE_BUTTON_ENABLE

	//#define CONFIG_MODULE_IO_ENABLE

	//#define CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE

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

	#define CONFIG_PLATFORM_MCU_STM32F4xx


	//#define CONFIG_USE_FREERTOS

	// Modules:

	#define CONFIG_MODULE_DEBUGUART_ENABLE
	#define CONFIG_MODULE_TERMINAL_ENABLE

	#define CONFIG_MODULE_COMMANDHANDLER_ENABLE
	#define CONFIG_MODULE_GLOBALVARHANDLER_ENABLE

	#define CONFIG_MODULE_LED_ENABLE

	//#define CONFIG_MODULE_LEDPWM_ENABLE

	#define CONFIG_MODULE_BUTTON_ENABLE

	//#define CONFIG_MODULE_IO_ENABLE

	//#define CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE

	//#define CONFIG_MODULE_ESP8266_ENABLE

	//#define CONFIG_MODULE_FLASH_ENABLE

	//#define CONFIG_MODULE_TEMPERATURE_ENABLE

	//#define CONFIG_MODULE_ADC_ENABLE

	//#define CONFIG_MODULE_SYSMANAGER_ENABLE

	#define CONFIG_MODULE_COMMON_DAC_ENABLE

	#define CONFIG_MODULE_COMMON_IO_ENABLE

	#define CONFIG_MODULE_COMMON_ADC_ENABLE

	//#define CONFIG_MODULE_COMMON_PWM_ENABLE

	//#define CONFIG_MODULE_MOTOR_ENABLE

	//#define CONFIG_MODULE_RTC_ENABLE

	#define CONFIG_MODULE_TASKHANDLER_ENABLE

	#define CONFIG_MODULE_EVENTLOG_ENABLE

	//#define CONFIG_MODULE_WATCHDOG_ENABLE

	//#define CONFIG_MODULE_DISPLAY_ENABLE

	//#define CONFIG_MODULE_IO_ENABLE
	//#define CONFIG_MODULE_IO_INPUT_MOTION_ENABLE
	//#define CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE
	//#define CONFIG_MODULE_IO_OUTPUT_EXAMPLE_ENABLE
#endif



#ifdef CONFIG_USE_PANEL_HOMEAUTNODESMALL
	// HomeAutNodeSmall
	#define CONFIG_PLATFORM_MCU_STM32F0xx

	#define CONFIG_USE_PANEL_HOMEAUTPANELS


	// Enabled modules
	//#define CONFIG_MODULE_DEBUGUART_ENABLE
	//#define CONFIG_MODULE_TERMINAL_ENABLE

	#define CONFIG_MODULE_TASKHANDLER_ENABLE

	#define CONFIG_MODULE_COMMANDHANDLER_ENABLE
	#define CONFIG_MODULE_GLOBALVARHANDLER_ENABLE

	#define CONFIG_MODULE_LED_ENABLE

	#define CONFIG_MODULE_BUTTON_ENABLE

	//#define CONFIG_MODULE_ESP8266_ENABLE

#endif



#ifdef CONFIG_USE_PANEL_FASTENNODE
	// FastenNode
	#define CONFIG_PLATFORM_MCU_STM32F0xx

	#define CONFIG_USE_PANEL_HOMEAUTPANELS


	/*			Enabled modules			*/

	#define CONFIG_MODULE_DEBUGUART_ENABLE
	//#define CONFIG_MODULE_TERMINAL_ENABLE
	//#define CONFIG_MODULE_COLOREDMESSAGE_ENABLE

	#define CONFIG_MODULE_TASKHANDLER_ENABLE

	#define CONFIG_MODULE_COMMANDHANDLER_ENABLE
	//#define CONFIG_MODULE_GLOBALVARHANDLER_ENABLE

	#define CONFIG_MODULE_LED_ENABLE

	//#define CONFIG_MODULE_COMMON_IO_ENABLE
	//#define CONFIG_MODULE_COMMON_UART_ENABLE

	//#define CONFIG_MODULE_DISPLAY_ENABLE

	#define CONFIG_MODULE_BUTTON_ENABLE

	//#define CONFIG_MODULE_ESP8266_ENABLE
	//#define CONFIG_MODULE_BLUETOOTH_ENABLE

	//#define CONFIG_MODULE_MOTOR_ENABLE

	//#define CONFIG_MODULE_IO_ENABLE
	//#define CONFIG_MODULE_IO_BATTERY_CHARGER_ENABLE

	//#define CONFIG_MODULE_LINKEDLIST_ENABLE

	//#define CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE

	//#define CONFIG_MODULE_COMMUNICATION_ENABLE

	//#define CONFIG_MODULE_CONVERTTABLE_ENABLE

	//#define CONFIG_USE_FREERTOS

	#define CONFIG_MODULE_RESETREASON_ENABLE

	#define CONFIG_MODULE_HARDFAULTHANDLER_ENABLE

	//#define CONFIG_MODULE_WATCHDOG_ENABLE
#endif



#ifdef CONFIG_USE_PANEL_PC
	#define CONFIG_PLATFORM_PC_WINDOWS

	//#define CONFIG_MODULE_DEBUGUART_ENABLE

	//#define CONFIG_MODULE_TERMINAL_ENABLE
	//#define CONFIG_MODULE_COLOREDMESSAGE_ENABLE

	#define CONFIG_MODULE_TASKHANDLER_ENABLE

	#define CONFIG_MODULE_COMMANDHANDLER_ENABLE
	//#define CONFIG_MODULE_GLOBALVARHANDLER_ENABLE

	#define CONFIG_MODULE_LINKEDLIST_ENABLE

	#define CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE

	#define CONFIG_MODULE_CONVERTTABLE_ENABLE

	#define CONFIG_MODULE_UNITTEST_ENABLE
#endif


#if defined(CONFIG_MODULE_COMMON_ADC_ENABLE) && defined(CONFIG_MODULE_ADC_ENABLE)
#warning "ADC and CommonADC will exclude each other"
#endif


#if defined(CONFIG_MODULE_DEBUGUART_ENABLE) && !defined(CONFIG_MODULE_COMMANDHANDLER_ENABLE)
#warning "DebugUart cannot used without CommandHandler, because received UART messages will unprocessed"
#endif


#if defined(CONFIG_MODULE_TASKHANDLER_ENABLE) && defined(CONFIG_USE_FREERTOS)
#warning "TaskHandler and FreeRTOS are enabled, but they exclude each other"
#endif


#ifdef CONFIG_USE_FREERTOS
	// FreeRTOS task defines

	#define TERMINAL_TASK_STACK_SIZE		(configMINIMAL_STACK_SIZE * 10)
	#define TERMINAL_TASK_PRIORITY			(tskIDLE_PRIORITY + 3UL)

	#define ESP8266_TASK_STACK_SIZE			(configMINIMAL_STACK_SIZE * 3)
	#define ESP8266_TASK_PRIORITY			(tskIDLE_PRIORITY + 3UL)

	#define SYSMANAGER_TASK_STACK_SIZE		(configMINIMAL_STACK_SIZE * 2)
	#define SYSMANAGER_TASK_PRIORITY		(tskIDLE_PRIORITY + 3UL)
#endif



/*------------------------------------------------------------------------------
 *								Debug settings
 *----------------------------------------------------------------------------*/

///< ASSERT functions for HAL (Or set in stm32f0xx_hal_conf.h file)
//#define USE_FULL_ASSERT

///< Debug mode: enable breakpoint and other debug support function
#define CONFIG_DEBUG_MODE

///< Hard Assert mode - with file name and line
//#define ASSERT_MODE_HARD

///< Test Mode: Enable "test" command...
//#define CONFIG_TEST_MODE

///< Module test: Enable "moduletest" command ~hwtest
//#define CONFIG_MODULE_TEST_ENABLE

///< Simulation mode: Enable "simulate" command
//#define CONFIG_MODULE_SIMULATION_ENABLE

///< SWO: Serial Wire Output enable
//#define CONFIG_SWO_ENABLE

///< SW Watchdog - check Task scheduling - called from SysTick (Debug function)
//#define CONFIG_DEBUG_SW_WATCHDOG_ENABLE

///< SelfTests
//#define CONFIG_DEBUG_SELFTEST


///< Debug - Formatted text (colored texts)
#define CONFIG_MODULE_DEBUG_ENABLE
//#define CONFIG_MODULE_COLOREDMESSAGE_ENABLE



/*------------------------------------------------------------------------------
 *								Unit tests
 *----------------------------------------------------------------------------*/


///< Unit tests
//#define CONFIG_MODULE_UNITTEST_ENABLE
#if defined(CONFIG_MODULE_UNITTEST_ENABLE) && !defined(CONFIG_USE_PANEL_PC)
	//#define MODULE_STRING_UNITTEST_ENABLE
	//#define MODULE_HOMEAUTMESSAGE_UNITTEST_ENABLE
	//#define MODULE_DATETIME_UNITTEST_ENABLE
	//#define MODULE_COMMANDHANDLER_UNITTEST_ENABLE
	//#define MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE
	//#define CONFIG_PROTOCOL_BUFFER_ENABLE
	//#define MODULE_CIRCULARBUFFER_UNITTEST_ENABLE
	//#define MODULE_TASKHANDLER_UNNITEST_ENABLE
	//#define MODULE_EVENTLOG_UNITTEST_ENABLE
	//#define MODULE_EVENTHANDLER_UNITTEST_ENABLE
	//#define MODULE_CALC_UNITTEST_ENABLE
	//#define MODULE_MEM_UNITTEST_ENABLE
	//#define MODULE_LINKEDLIST_UNITTEST_ENABLE
	//#define MODULE_CONVERTTABLE_UNITTEST_ENABLE
	//#define MODULE_SECUREDDATATYPES_UNITTEST_ENABLE

	#if (defined(MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE) || defined(MODULE_COMMANDHANDLER_UNITTEST_ENABLE))&& !defined(CONFIG_PROTOCOL_BUFFER_ENABLE)
	#define CONFIG_PROTOCOL_BUFFER_ENABLE
	#endif
#elif defined(CONFIG_USE_PANEL_PC)
	// Forced all UnitTest
	#define MODULE_STRING_UNITTEST_ENABLE
	#define MODULE_HOMEAUTMESSAGE_UNITTEST_ENABLE
	#define MODULE_DATETIME_UNITTEST_ENABLE
	#define MODULE_COMMANDHANDLER_UNITTEST_ENABLE
	#define MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE
	#define MODULE_CIRCULARBUFFER_UNITTEST_ENABLE
	#define MODULE_TASKHANDLER_UNNITEST_ENABLE
	#define MODULE_EVENTLOG_UNITTEST_ENABLE
	#define MODULE_EVENTHANDLER_UNITTEST_ENABLE
	#define MODULE_CALC_UNITTEST_ENABLE
	#define MODULE_MEM_UNITTEST_ENABLE
	#define MODULE_LINKEDLIST_UNITTEST_ENABLE
	#define MODULE_CONVERTTABLE_UNITTEST_ENABLE
	#define MODULE_SECUREDDATATYPES_UNITTEST_ENABLE
#endif



/*------------------------------------------------------------------------------
 *								Terminal settings
 *----------------------------------------------------------------------------*/


//#define CONFIG_MODULE_TERMINAL_ENABLE
#ifdef CONFIG_MODULE_TERMINAL_ENABLE

	//#define CONFIG_DEBUGUSART_MODE_ONEPERONERCHARACTER

	/// XXX: Select your terminal
	//#define CONFIG_TERMINAL_USE_HYPERTERMINAL
	#define CONFIG_TERMINAL_USE_ZOC
	//#define CONFIG_TERMINAL_USE_PUTTY

	///< Wait password and until not received good password, commands are not evaluated
	//#define CONFIG_TERMINAL_GET_PASSWORD_ENABLE

	#define CONFIG_TERMINAL_PROMT_ENABLE

	// Escape sequences (cursors, history, cls, ...)
	#define CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE

	#ifdef CONFIG_TERMINAL_ESCAPE_SEQUENCE_ENABLE
		// If you want use terminal program's history
		// Turn off, if has small memory, now it need 1.5k RAM
		//#define CONFIG_TERMINAL_HISTORY_ENABLE
	#endif
#endif	// #ifdef CONFIG_MODULE_TERMINAL_ENABLE



/*------------------------------------------------------------------------------
 *								Other module settings
 *----------------------------------------------------------------------------*/


///< CommandHandler settings
#define CONFIG_MODULE_COMMANDHANDLER_ENABLE
#ifdef CONFIG_MODULE_COMMANDHANDLER_ENABLE
	///< Notify, if received a command (but not from Debug port)
	#define CONFIG_COMMANDHANDLER_NOTIFY_COMMAND_RECEIVED_FROM_NOT_DEBUGPORT

	///< CommandList settings
	//#define CONFIG_COMMANDLIST_RAMREAD_WRITE_COMMAND_ENABLE
	//#define CONFIG_COMMANDLIST_GO_COMMAND_ENABLE
#endif


///< GlobalVarHandler settings
//#define CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
	//#define CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
	//#define CONFIG_GLOBALVARHANDLER_TRACE_RAM_BUFFER
	#define CONFIG_GLOBALVARHANDLER_CHECK_ENABLE
#endif


///< ESP8266 settings
//#define CONFIG_MODULE_ESP8266_ENABLE
#ifdef CONFIG_MODULE_ESP8266_ENABLE

	// ESP8266 Debug mode:
	// 1 - on
	// 0 - off
	#define ESP8266_DEBUG_MODE					(1)


	// ESP8266 - Connection type
	// 1 - Dynamic (Can change WiFi and TCP client/server
	// 0 - Static, fix connection type (only server or only client)
	#define CONFIG_ESP8266_CONNECT_DYNAMIC		0


	// Module will be WiFi network "server"
	// 1 - WifiHost
	// 0 - Not WifiHost (client)
	#define CONFIG_ESP8266_IS_WIFI_HOST			0

	// Module TCP state
	// 1 - ESP8266 will be TCP server,
	// 0 - will be TCP client
	#define CONFIG_ESP8266_IS_TCP_SERVER		0


	//#define CONFIG_ESP8266_TCP_SERVER_ON_ESP8266
	#define CONFIG_ESP8266_TCP_SERVER_ON_HOMENETWORK


	#if CONFIG_ESP8266_IS_WIFI_HOST == 0
		#ifdef CONFIG_ESP8266_TCP_SERVER_ON_ESP8266
			#define CONFIG_ESP8266_WIFI_NETWORK_NAME			"ESP8266HomeAutomation"
			#define CONFIG_ESP8266_WIFI_NETWORK_PASSWORD		"AUT"
		#elif defined(CONFIG_ESP8266_TCP_SERVER_ON_HOMENETWORK)
			#define CONFIG_ESP8266_WIFI_NETWORK_NAME			"ARTN16"
			//#define CONFIG_ESP8266_WIFI_NETWORK_NAME			"USR5461"
			#define CONFIG_ESP8266_WIFI_NETWORK_PASSWORD		"Easdg2011"
		#endif

		#ifndef CONFIG_ESP8266_WIFI_NETWORK_PASSWORD
			#warning "CONFIG_ESP8266_WIFI_NETWORK_PASSWORD defined not defined!"
			#define CONFIG_ESP8266_WIFI_NETWORK_PASSWORD	"Password"
		#endif
	#endif


	#if CONFIG_ESP8266_IS_TCP_SERVER == 0
		#ifdef CONFIG_ESP8266_TCP_SERVER_ON_ESP8266
			//'192.168.4.1'
			#define CONFIG_ESP8266_TCP_SERVER_IP_1	(192)
			#define CONFIG_ESP8266_TCP_SERVER_IP_2	(168)
			#define CONFIG_ESP8266_TCP_SERVER_IP_3	(4)
			#define CONFIG_ESP8266_TCP_SERVER_IP_4	(1)
			#define CONFIG_ESP8266_TCP_SERVER_IP_STRING		"192.168.4.1"
		#elif defined(CONFIG_ESP8266_TCP_SERVER_ON_HOMENETWORK)
			#define CONFIG_ESP8266_TCP_SERVER_IP_1	(192)
			#define CONFIG_ESP8266_TCP_SERVER_IP_2	(168)
			#define CONFIG_ESP8266_TCP_SERVER_IP_3	(1)
			#define CONFIG_ESP8266_TCP_SERVER_IP_4	(62)
			#define CONFIG_ESP8266_TCP_SERVER_IP_STRING		"192.168.1.62"
		#endif

		#define CONFIG_ESP8266_TCP_SERVER_PORT			(2000)
		#define CONFIG_ESP8266_TCP_SERVER_PORT_STRING	"2000"
	#endif

#endif


///< EventHandler settings
#define CONFIG_MODULE_EVENTHANDLER_ENABLE
#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE
	//#define CONFIG_EVENTHANDLER_REQUIRED_TASK_MODE
#endif


///< EventLog settings
//#define CONFIG_MODULE_EVENTLOG_ENABLE
#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
	//#define CONFIG_EVENTLOG_TASKHANDLER_LOG_ENABLE
	//#define CONFIG_EVENTLOG_DISPLAY_LOG_ENABLE
	//#define CONFIG_EVENTLOG_PRINT_IMMEDIATELY
	#define CONFIG_EVENTLOG_CMDHANDLER_ENABLE
#endif


///< Display settings
//#define CONFIG_MODULE_DISPLAY_ENABLE
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
	#define CONFIG_DISPLAY_FONT8X5_ENABLE
	#define CONFIG_DISPLAY_FONT12X8_ENABLE
	#define CONFIG_DISPLAY_FONT32X20_ENABLE

	// Tests
	#define CONFIG_MODULE_DISPLAY_TEST
	#define CONFIG_MODULE_DISPLAY_TEST_WITH_TERMINAL

	#define CONFIG_DISPLAY_SPI_USE_DMA
#endif


///< Button settings
//#define CONFIG_MODULE_BUTTON_ENABLE
#ifdef CONFIG_MODULE_BUTTON_ENABLE
	#define CONFIG_BUTTON_DEBUG_ENABLE
	#define CONFIG_MODULE_BUTTON_LONG_PRESS
	#define CONFIG_BUTTON_LONG_PRESS_TIME_TICK_LIMIT			(400U)
	#define CONFIG_BUTTON_CONTINUOUS_PRESS_TIME_TICK_LIMIT		(1000U)
#endif


///< TaskHandler settings
//#define CONFIG_MODULE_TASKHANDLER_ENABLE
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE

	///< Debug modes - Print debug message
	//#define CONFIG_TASKHANDLER_DEBUG_ENABLE
	///< Other debug functions
	#define CONFIG_TASKHANDLER_DEBUG_RUN_ENABLE

	///< Statistics (CPU usage, runtimes...)
	#define CONFIG_MODULE_TASKHANDLER_STATISTICS
	#define TASKHANDLER_STATISTICS_LIMIT						(100)

	///< UART handler require Task scheduling
	#define CONFIG_MODULE_UART_REQUIRE_TASKSCHEDULE_ENABLE

	//  TaskList:

	///< Software Watchdog (task) - Debug function
	//#define CONFIG_MODULE_TASK_SOFTWARE_WATCHDOG_ENABLE

	///< System time task
	#define CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE

	#ifdef CONFIG_MODULE_LED_ENABLE
		#define CONFIG_MODULE_LED_TASK_ENABLE
	#endif
#endif


///< LED settings
//#define CONFIG_MODULE_LED_ENABLE
#ifdef CONFIG_MODULE_LED_ENABLE
	//#define CONFIG_MODULE_LED_TASK_ENABLE
	#ifdef CONFIG_MODULE_LED_TASK_ENABLE
		//#define LED_TASK_OLD_STYLE
		#define LED_TASK_PWM_STYLE
	#endif
#endif


///< Communication
//#define CONFIG_COMMUNICATION_HAS_MORE_COMM_PORT
#if ((defined(CONFIG_MODULE_DEBUGUART_ENABLE) && defined(CONFIG_MODULE_BLUETOOTH_ENABLE)) \
	|| (defined(CONFIG_MODULE_DEBUGUART_ENABLE) && defined(CONFIG_MODULE_ESP8266_ENABLE)) \
	|| (defined(CONFIG_MODULE_BLUETOOTH_ENABLE) && defined(CONFIG_MODULE_ESP8266_ENABLE)))
	#define CONFIG_COMMUNICATION_HAS_MORE_COMM_PORT
#endif


#endif /* OPTIONS_H_ */
