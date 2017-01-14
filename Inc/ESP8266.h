/*
 *		ESP8266.h
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		ESP8266 communication
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2017.01.14
 */

#ifndef ESP8266_H_
#define ESP8266_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/// Sending/Receiving content message max length
#define ESP8266_TCP_MESSAGE_MAX_LENGTH			( HOMEAUTMESSAGE_MESSAGE_MAX_LENGTH )
#define ESP8266_TCP_MESSAGE_MIN_LENGTH			( HOMEAUTMESSAGE_MESSAGE_MIN_LENGTH )

/// ESP8266 queue item size
#define ESP8266_MESSAGE_QUEUE_ITEM_SIZE			( HOMEAUTMESSAGE_MESSAGE_MAX_LENGTH )


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
#define ESP8266_BUFFER_LENGTH					( ESP8266_RECEIVEMESSAGE_MAX_LENGTH )



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


/// WiFi Connection status types
typedef enum
{
	ESP8266_WifiConnectionStatus_Unknown = 0,
	ESP8266_WifiConnectionStatus_OnConnecting,
	ESP8266_WifiConnectionStatus_SuccessfulConnected,
	ESP8266_WifiConnectionStatus_SuccessfulServerStarted,
	ESP8266_WifiConnectionStatus_ClosedConnection,
	ESP8266_WifiConnectionStatus_Error
	
} ESP8266_WifiConnectionStatusType;



/// Server connection status types
typedef enum
{
	ESP8266_TcpConnectionStatus_Unknown = 0,
	ESP8266_TcpConnectionStatus_Connected,
	ESP8266_TcpConnectionStatus_Error
	
} ESP8266_TcpConnectionStatusType;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern UART_HandleTypeDef Esp8266UartHandle;
extern volatile uint8_t ESP8266_Uart_ReceivedCharFlag;
extern uint8_t ESP8266_Receive_Mode_FixLength;
extern uint8_t ESP8266_ReceiveBuffer_Cnt;
extern volatile char ESP8266_ReceiveBuffer[];


#ifdef CONFIG_USE_FREERTOS
extern xSemaphoreHandle ESP8266_USART_Rx_Semaphore;
extern xQueueHandle ESP8266_SendMessage_Queue;
extern xQueueHandle ESP8266_ReceivedMessage_Queue;
#endif


extern ESP8266_WifiConnectionStatusType	ESP8266_ConnectionStatus;
extern ESP8266_TcpConnectionStatusType	ESP8266_TcpConnectionStatus;


extern Network_IP_t ESP8266_MyIpAddress;


extern uint8_t ESP8266_DebugEnableFlag;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void ESP8266_SendString(const char *str);
void ESP8266_ReceiveString(uint8_t length);
void ESP8266_ClearReceiveBuffer(void);
void ESP8266_Task(void);

void ESP8266_Init(void);

bool ESP8266_Config(void);
bool ESP8266_ConnectToWifiNetwork(void);

bool ESP8266_StartServer(void);
bool ESP8266_FindServer(void);
bool ESP8266_ConnectToServer(void);

bool ESP8266_SendMessageToQueue(char *message);

bool ESP8266_ReceiveFixTcpMessage(void);

bool ESP8266_WaitClientConnect(void);

bool ESP8266_CheckReceiveMessage(void);

void ESP8266_WaitAnswer(uint32_t timeout);
void ESP8266_ResetHardware(void);



#endif /* ESP8266_H_*/
