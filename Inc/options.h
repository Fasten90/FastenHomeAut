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


/// Debug
#define CONFIG_DEBUG_MODE


/// Unit tests

#define MODULE_STRING_UNIT_TEST_ENABLED






#endif /* OPTIONS_H_ */
