/*
 * monitor.h
 *
 *		Created on		2013.10.08.
 *      Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		University:		BME (BUTE)
 *		Project: 		RadioAlarm - Radio Alarm System modules
 *		Function:		Monitor program, communication with UART, execute commands
 *		Target:			STM32F107RC
 *		Version:		v2
 *		Last modified:	2014.05.16
 */

#ifndef MONITOR_H_
#define MONITOR_H_


#include "include.h"


///////////////////////////// MONITOR


/////////////////////////////
// TYPEDEFS
// PROTOTYPES
// DEFINES
/////////////////////////////


/////////////////////////////  TYPEDEFS






/*
// For GlobalVars
// TODO: NOT USED

typedef void * ( *GetFunctionPointer )( void * );
typedef void * ( *SetFunctionPointer )( void * );

typedef uint8_t CommandGlobalVarType;

typedef struct {
	char * VarName;
	CommandGlobalVarType type;
	GetFunctionPointer *GetCommandFunctionPointer;
	SetFunctionPointer *SetCommandFunctionPointer;
} GlobalVariable_t;

#define COMMAND_GLOBALVAR_TYPE_UINT8	(CommandGlobalVarType)1
*/






/////////////////////////////  PROTOTYPES



uint8_t MONITOR_Init ( void );
uint8_t MONITOR_SendWelcome ( void );
uint8_t MONITOR_SendPrimitiveWelcome ( void );

uint8_t MONITOR_CheckCommand ( void );
uint8_t MONITOR_EndCommand ( char *CommandActual, unsigned int argc, char** argv  );
uint8_t MONITOR_CommandSeparate ( char** argv, char *CommandActual );
uint8_t MONITOR_CommandFind (unsigned int argc, char** argv );
//uint8_t MONITOR_CommandListInitialize (void);

uint8_t MONITOR_CommandBackspace ( void );
uint8_t MONITOR_CommandResendLine ( void );
uint8_t MONITOR_CommandEscapeCharValidation ( void );


uint8_t MONITOR_HISTORY_Save ( void );
uint8_t MONITOR_HISTORY_IsInIt ( void );
uint8_t MONITOR_HISTORY_Load ( uint8_t direction );


uint8_t MONITOR_ConvertSmallLetter( void );



/////////////////////////////  DEFINES



#define MONITOR_MAX_COMMAND_LENGTH			255
#define MONITOR_MAX_ARG_LENGTH				40
#define MONITOR_MAX_HISTORY_LENGTH			5


#define MONITOR_WELCOME_DATE_VERSION	__DATE__
#define MONITOR_WELCOME_TIME_VERSION	__TIME__

#ifdef CONFIG_USE_PANEL_NODEMEDIUM
#define MONITOR_BOARD_TYPE		"Used panel: NodeMedium\r\n"
#endif
#ifdef CONFIG_USE_PANEL_CENTERPANEL
#define MONITOR_BOARD_TYPE		"Used panel: CenterPanel\r\n"
#endif
#ifdef CONFIG_USE_PANEL_DISCOVERY
#define MONITOR_BOARD_TYPE		"Used panel: STM32F4Discovery\r\n"
#endif


#define MONITOR_DUMMY_STRING				"____________________________________________\r\n"


#define MONITOR_SEND_WELCOME()								\
		USART_SendString("\r\n"								\
		"Welcome in Monitor program!\r\n"					\
		MONITOR_WELCOME_DATE_VERSION "\r\n"					\
		MONITOR_WELCOME_TIME_VERSION "\r\n"					\
		MONITOR_BOARD_TYPE									\
		"(c) Copyright, Vizi Gabor\r\n"						\
		"\r\n"												\
		"Try \"help\" command for help!\r\n")


#define MONITOR_SEND_PROMT()		\
		USART_SendString("\r\n# ")


#define USART_SendString(message)		USART_SendMessage((char *)message)


#define USART_SEND_DUMMYSTRING()		USART_SendString(MONITOR_DUMMY_STRING)


#define	RETURN_SUCCESS	1
#define RETURN_FALSE	0



/////////////////////////////  GLOBAL VARIABLES

extern volatile char MONITOR_CommandActual[MONITOR_MAX_COMMAND_LENGTH];
extern volatile char MONITOR_CommandActualEscape[3];
extern volatile uint8_t MONITOR_CommandEnable;
extern volatile uint8_t MONITOR_CommandActualLength;
extern volatile uint8_t MONITOR_CommandCursorPosition;
extern volatile uint8_t MONITOR_CommandEvent;
extern volatile uint8_t MONITOR_CommandReceivedLastChar;
extern volatile uint8_t MONITOR_CommandReceivedNotLastChar;
extern volatile uint8_t MONITOR_CommandReadable;
extern volatile uint8_t	MONITOR_CommandEscapeSequenceReceived;
extern volatile uint8_t	MONITOR_CommandEscapeSequenceInProgress;
extern volatile uint8_t	MONITOR_CommandEscape_cnt;
extern volatile uint8_t MONITOR_CommandReceivedBackspace;
extern volatile uint8_t MONITOR_CommandSendBackChar_Enable;
extern volatile uint8_t MONITOR_RemoteControl;
extern char MONITOR_RemoteControlBuffer[5];
extern char MONITOR_RemoteControlCharacter;


#ifdef CONFIG_USE_FREERTOS
extern xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
extern xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
#endif




#endif /* MONITOR_H_ */
