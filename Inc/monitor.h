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



#define MONITOR_HISTORY_STANDARDCOMMAND_1	"get speed"
#define MONITOR_HISTORY_STANDARDCOMMAND_2	"get velocity"
#define MONITOR_HISTORY_STANDARDCOMMAND_3	"log on status"
#define MONITOR_HISTORY_STANDARDCOMMAND_4	"stop"
#define MONITOR_HISTORY_STANDARDCOMMAND_5	"set speed 15"




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


#define MONITOR_WELCOME_DATE_VERSION	"2015.09.21 - v4.0\r\n"

#ifdef CONFIG_USE_PANEL_NODEMEDIUM
#define MONITOR_BOARD_TYPE		"Used panel: NodeMedium\r\n"
#endif
#ifdef CONFIG_USE_PANEL_CENTERPANEL
#define MONITOR_BOARD_TYPE		"Used panel: CenterPanel\r\n"
#endif

#define MONITOR_DUMMY_STRING				"____________________________________________\r\n"


#define MONITOR_SEND_WELCOME()								\
		USART_SendString("\r\n"								\
		"Welcome in Monitor program!\r\n"					\
		MONITOR_WELCOME_DATE_VERSION						\
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

extern uint8_t MONITOR_CommandActual[MONITOR_MAX_COMMAND_LENGTH];
extern uint8_t MONITOR_CommandActualEscape[3];
extern volatile uint8_t MONITOR_CommandEnable;
extern volatile uint8_t MONITOR_CommandLength;
extern volatile uint8_t MONITOR_CommandCursorPosition;
extern volatile uint8_t MONITOR_CommandEvent;
extern volatile uint8_t MONITOR_CommandReceivedLastChar;
extern volatile uint8_t MONITOR_CommandReceivedNotLastChar;
extern volatile uint8_t MONITOR_CommandReadable;
extern volatile uint8_t	MONITOR_CommandEscapeEnd_flag;
extern volatile uint8_t	MONITOR_CommandEscapeStart_flag;
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
