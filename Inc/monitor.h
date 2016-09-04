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
#include "command.h"


///////////////////////////// MONITOR


/////////////////////////////
// DEFINES
// TYPEDEFS
// EXTERNED GLOBAL VARIABLES
// EXTERNED PROTOTYPES
/////////////////////////////



/////////////////////////////  DEFINES



#define MONITOR_MAX_COMMAND_LENGTH			(255U)
#define MONITOR_COMMAND_ARG_COUNT			(3)
#define MONITOR_COMMAND_ARG_MAX_LENGTH		(80)
#define MONITOR_HISTORY_MAX_LENGTH			(5)


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



/////////////////////////////  GLOBAL VARIABLES

extern volatile char MONITOR_CommandActual[MONITOR_MAX_COMMAND_LENGTH];
extern volatile char MONITOR_CommandActualEscape[3];

extern const bool MONITOR_CommandReceiveEnable;


#ifdef CONFIG_USE_FREERTOS
extern xSemaphoreHandle DEBUG_USART_Rx_Semaphore;
extern xSemaphoreHandle DEBUG_USART_Tx_Semaphore;
#endif





/////////////////////////////  PROTOTYPES



void MONITOR_Init ( void );
void MONITOR_SendWelcome ( void );
void MONITOR_SendPrimitiveWelcome ( void );

void MONITOR_CheckCommand ( void );
bool MONITOR_PrepareFindExecuteCommand ( void );
uint8_t MONITOR_CommandSeparate ( void );
bool MONITOR_CommandFind ( void );

void MONITOR_CommandBackspace ( void );
void MONITOR_CommandResendLine ( void );
void MONITOR_NewCommandResendLine ( void );
bool MONITOR_CommandEscapeCharValidation ( void );

// Monitor history
void MONITOR_HISTORY_Save ( void );
bool MONITOR_HISTORY_IsInIt ( void );
void MONITOR_HISTORY_Load ( uint8_t direction );


void MONITOR_ConvertSmallLetter ( void );

void MONITOR_CheckResultAndRespond(CommandResult_t result);
void MONITOR_RunCommand ( uint8_t commandID );
void MONITOR_WriteAnCommandHelp ( uint8_t commandID );
CommandResult_t MONITOR_ArgumentNumIsGood ( uint8_t receivedArgNum, uint8_t commandArgNum);



#endif /* MONITOR_H_ */
