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


// FreeRTOS - at panel selection
// Do not use from this line, use the CONFIG_USE_PANEL ...
//#define CONFIG_USE_FREERTOS


// ESP8266
//#define USE_ESP8266_TEST_FASTENHOME
#define USE_ESP8266_MODE_CLIENT



/////////////////////////////
// 		Select your panel
/////////////////////////////

// Recommend define at project settings:
// XXX: Select your panel
//#define CONFIG_USE_PANEL_NODESMALL
//#define CONFIG_USE_PANEL_NODEMEDIUM
//#define CONFIG_USE_PANEL_CENTERPANEL
//#define CONFIG_USE_PANEL_DISCOVERY


/// Debug
#define CONFIG_DEBUG_MODE


/// Unit tests

#define MODULE_STRING_UNIT_TEST_ENABLED






#endif /* OPTIONS_H_ */
