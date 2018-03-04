/*
 *		ESP8266.h
 *		Creat on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		ESP8266 communication
 *		Target:			STM32Fx
 *		Version:		v5
 *		Last modified:	2017-03-05
 */

#ifndef ESP8266_H_
#define ESP8266_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "NetworkHelper.h"
#include "DebugUart.h"
#include "EventLog.h"
#include "EventList.h"
#include "DebugList.h"
#include "Debug.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define ESP8266_DEBUG_TXBUFFERSIZE		(255U)


//#define ESP8266_USE_BLOCK_MODE


#ifdef ESP8266_USE_BLOCK_MODE
#define ESP8266_SEND_TCP_MESSAGE(msg)	ESP8266_SendTcpMessageBlockingMode(msg)
#else
#define ESP8266_SEND_TCP_MESSAGE(msg)	ESP8266_SendTcpMessageNonBlockingMode_Start(msg);
#endif


/*
 * ESP8266 Debug mode:	1 on
 * 						0 off
 */
#ifndef ESP8266_DEBUG_MODE
	#define ESP8266_DEBUG_MODE			(1)
#endif


#if (ESP8266_DEBUG_MODE == 1)
	#if defined(CONFIG_MODULE_DEBUG_ENABLE)
	#define ESP8266_DEBUG_PRINT(msg)					Debug_Print(Debug_ESP8266, msg)
	#else
	#define ESP8266_DEBUG_PRINT(msg)					DebugUart_SendMessage("ESP8266: "); \
														DebugUart_SendLine(msg)
	#endif
#else
#define ESP8266_DEBUG_PRINT(msg)
#endif

#if (ESP8266_DEBUG_MODE == 1)
// Variadic Macro
	#if defined(CONFIG_MODULE_DEBUG_ENABLE)
	#define ESP8266_DEBUG_PRINTF(...)					Debug_Printf(Debug_ESP8266, __VA_ARGS__)
	#else
	#define ESP8266_DEBUG_PRINTF(...)					DebugUart_SendMessage("ESP8266: "); \
														uprintf(__VA_ARGS__)
	#endif
#else
#define ESP8266_DEBUG_PRINTF(...)
#endif

#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
#define ESP8266_LOG_EVENT(val)					EventLog_LogEvent(Event_Esp8266UserEvent, \
													val, \
													Task_Esp8266, \
													EventType_Raised \
													)
#else
#define ESP8266_LOG_EVENT(val)
#endif


#define	EVENT_LINK								( 1 )
#define	EVENT_UNLINK							( 2 )
#define	EVENT_RECEIVED_GOOD_TCP_MESSAGE			( 3 )
#define EVENT_ERROR								( 4 )


/// Sending/Receiving content message max length
#ifdef HOMEAUTMESSAGE_MESSAGE_MAX_LENGTH
#define ESP8266_TCP_MESSAGE_MAX_LENGTH			( HOMEAUTMESSAGE_MESSAGE_MAX_LENGTH )
#define ESP8266_TCP_MESSAGE_MIN_LENGTH			( HOMEAUTMESSAGE_MESSAGE_MIN_LENGTH )
/// ESP8266 queue item size
#define ESP8266_MESSAGE_QUEUE_ITEM_SIZE			( ESP8266_TCP_MESSAGE_MAX_LENGTH )
#else
// If not defined HOMEAUTMESSAGE...
#define ESP8266_TCP_MESSAGE_MAX_LENGTH			( 90 )
#define ESP8266_TCP_MESSAGE_MIN_LENGTH			( 5 )
/// ESP8266 queue item size
#define ESP8266_MESSAGE_QUEUE_ITEM_SIZE			( ESP8266_TCP_MESSAGE_MAX_LENGTH )
#endif


/// Max messages num in queue
#define ESP8266_RECEIVEMESSAGE_QUEUE_LENGTH		( 5 )
#define ESP8266_SENDMESSAGE_QUEUE_LENGTH		( 5 )


/// Receive message header
// "\r\n"
// "+IPD,0,20:"
#define ESP8266_HOMEAUT_MESSAGEHEADER_LENGTH	( 12 )

/// Receive message end
// "\r\nOK\r\n"
#define ESP8266_HOMEAUT_MESSAGEBOTTOM_LENGTH	( 6 )	


#define ESP8266_RECEIVEMESSAGE_MAX_LENGTH			\
			( ESP8266_HOMEAUT_MESSAGEHEADER_LENGTH	\
			+ ESP8266_TCP_MESSAGE_MAX_LENGTH		\
			+ ESP8266_HOMEAUT_MESSAGEBOTTOM_LENGTH )

#define ESP8266_RECEIVEMESSAGE_MIN_LENGTH			\
			( ESP8266_HOMEAUT_MESSAGEHEADER_LENGTH	\
			+ ESP8266_TCP_MESSAGE_MIN_LENGTH		\
			+ ESP8266_HOMEAUT_MESSAGEBOTTOM_LENGTH )

/// ESP8266 receive buffer length
#define ESP8266_RX_BUFFER_LENGTH				( 256UL )

#define ESP8266_RECEIVE_LENGTH					( ESP8266_RX_BUFFER_LENGTH )

#define ESP8266_TX_BUFFER_LENGTH				( 128U )


#define ESP8266_LED_OK()		LED_GREEN_ON();	\
								LED_RED_OFF()

#define ESP8266_LED_FAIL()		LED_RED_ON();	\
								LED_GREEN_OFF()


// Reset (low = reset active)
#define ESP8266_RST_ACTIVE()	\
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_PORT, ESP8266_RST_GPIO_PIN, GPIO_PIN_RESET)
// Reset end (high = inactive)
#define ESP8266_RST_INACTIVE()	\
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_PORT, ESP8266_RST_GPIO_PIN, GPIO_PIN_SET)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


///< WiFi Connection status types
typedef enum
{
	ESP8266_WifiConnectionStatus_Unknown = 0,
	ESP8266_WifiConnectionStatus_OnConnecting,
	ESP8266_WifiConnectionStatus_SuccessfulConnected,
	ESP8266_WifiConnectionStatus_SuccessfulServerStarted,
	ESP8266_WifiConnectionStatus_ClosedConnection,
	ESP8266_WifiConnectionStatus_Error
	
} ESP8266_WifiConnectionStatusType;



///< Server connection status types
typedef enum
{
	ESP8266_TcpConnectionStatus_Unknown = 0,
	ESP8266_TcpConnectionStatus_Connected,
	ESP8266_TcpConnectionStatus_Error
	
} ESP8266_TcpConnectionStatusType;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern UART_HandleTypeDef ESP8266_UartHandle;
extern UART_Handler_t ESP8266_Uart;

#ifdef CONFIG_USE_FREERTOS
extern xSemaphoreHandle ESP8266_USART_Rx_Semaphore;
extern xQueueHandle ESP8266_SendMessage_Queue;
extern xQueueHandle ESP8266_ReceivedMessage_Queue;
#endif


extern ESP8266_WifiConnectionStatusType	ESP8266_ConnectionStatus;
extern ESP8266_TcpConnectionStatusType	ESP8266_TcpConnectionStatus;


extern bool ESP8266_DebugEnableFlag;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void ESP8266_Init(void);

size_t ESP8266_SendString(const char *msg);

void ESP8266_Task(void);

#ifdef ESP8266_USE_BLOCK_MODE
bool ESP8266_Config(void);
bool ESP8266_ConnectToWifiNetwork(void);

bool ESP8266_StartServer(void);
bool ESP8266_FindServer(void);
bool ESP8266_ConnectToServer(Network_IP_t *ip, Network_Port_t port);

bool ESP8266_ReceiveFixTcpMessage(void);

bool ESP8266_WaitClientConnect(void);

bool ESP8266_CheckReceiveMessage(void);

static void ESP8266_WaitAnswer(uint32_t timeout);
#endif

uint8_t ESP8266_RequestSendTcpMessage(const char * message);

void ESP8266_StatusMachine(void);

uint8_t ESP8266_PrintIpAddress(char * str);



#endif /* ESP8266_H_*/
