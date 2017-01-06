/*******************************************************************************
 * Purpose:
 ******************************************************************************/

#ifndef ESP8266_H_
#define ESP8266_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



#define ESP8266_BUFFER_LENGTH	( 70 )
// for ESP8266 receive buffer length


#define ESP8266_SERVER_IP_ADDRESS_SHORT		( 10 )


#define ESP8266_HOMEAUTMESSAGE_RECEIVEMESSAGE_QUEUE_LENGTH	( 5 )
#define ESP8266_HOMEAUTMESSAGE_SENDMESSAGE_QUEUE_LENGTH		( 5 )


#define ESP8266_HOMEAUT_MESSAGECONTENT_LENGTH	( 40 )
// example: "|HomeAut|010|014|LOGIN__|NMEDIU00000000|"
#define ESP8266_HOMEAUTMESSAGE_ITEM_SIZE		( ESP8266_HOMEAUT_MESSAGECONTENT_LENGTH + 1 )

#define ESP8266_HOMEAUT_MESSAGEHEADER_LENGTH	( 12 )
// "\r\n"
// "+IPD,0,20:"

#define ESP8266_HOMEAUT_MESSAGEBOTTOM_LENGTH	( 6 )	
// "\r\nOK\r\n"

#define ESP8266_HOMEAUT_RECEIVING_MESSAGE_LENGTH	( ESP8266_HOMEAUT_MESSAGECONTENT_LENGTH + \
			ESP8266_HOMEAUT_MESSAGEHEADER_LENGTH + ESP8266_HOMEAUT_MESSAGEBOTTOM_LENGTH )


#define	ESP8266_HOMEAUTMESSAGE_RECEIVEDMESSAGE_NULLCHAR		( 52 )
#define	ESP8266_HOMEAUTMESSAGE_RECEIVEDMESSAGE_START		( 12 )


#define ESP8266_LED_OK()		LED_GREEN_ON()
#define ESP8266_LED_FAIL()		LED_RED_ON()


#define ESP8266_TCP_MESSAGE_LENGTH				( 40 )


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum {
	ESP8266_ConnectionStatus_Unknown = 0,
	ESP8266_ConnectionStatus_OnConnecting,
	ESP8266_ConnectionStatus_SuccessfulConnected,
	ESP8266_ConnectionStatus_SuccessfulServerStarted,
	ESP8266_ConnectionStatus_ClosedConnection,
	ESP8266_ConnectionStatus_Error
	
} ESP8266_ConnectionStatusType;


typedef enum {
	ESP8266_TcpConnectionStatus_Unknown = 0,
	ESP8266_TcpConnectionStatus_Connected,
	ESP8266_TcpConnectionStatus_Error
	
} ESP8266_TcpConnectionStatusType;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
extern UART_HandleTypeDef Esp8266UartHandle;
extern volatile uint8_t ESP8266_Uart_ReceivedCharFlag;

#ifdef CONFIG_USE_FREERTOS
extern xSemaphoreHandle ESP8266_USART_Rx_Semaphore;
extern xQueueHandle ESP8266_SendMessage_Queue;
extern xQueueHandle ESP8266_ReceivedMessage_Queue;
#endif


extern ESP8266_ConnectionStatusType	ESP8266_ConnectionStatus;
extern ESP8266_TcpConnectionStatusType	ESP8266_TcpConnectionStatus;

extern uint8_t ESP8266_MyIpAddress[4];
extern uint8_t ESP8266_DebugEnableFlag;

extern uint8_t ESP8266_Receive_Mode_FixLength;
extern uint8_t ESP8266_ReceiveBuffer_Cnt;
extern char ESP8266_ReceiveBuffer[ESP8266_BUFFER_LENGTH];


/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void ESP8266_SendString(char *aTxBuffer);
void ESP8266_ReceiveString(uint8_t length);
void ESP8266_ClearReceiveBuffer(void);
void ESP8266_Task(void);

ReturnType ESP8266_Init(void);

ReturnType ESP8266_Config(void);
ReturnType ESP8266_ConnectToWifiNetwork(void);

ReturnType ESP8266_StartServer(void);
ReturnType ESP8266_FindServer(void);
ReturnType ESP8266_ConnectToServer(void);

ReturnType ESP8266_SendTcpMessage(char *message);
ReturnType ESP8266_SendMessageToQueue(uint8_t *message);

ReturnType ESP8266_ReceiveFixTcpMessage(void);

ReturnType ESP8266_WaitClientConnect(void);

ReturnType ESP8266_CheckReceiveMessage(void);

void ESP8266_WaitAnswer(void);



#endif /* ESP8266_H_*/
