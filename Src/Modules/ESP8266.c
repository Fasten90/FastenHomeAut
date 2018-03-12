/*
 *		ESP8266.c
 *		Creat on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		ESP8266 communication
 *		Target:			STM32Fx
 *		Version:		v6
 *		Last modified:	2018-03-04
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"
#include "board.h"
#include "StringHelper.h"
#include "CircularBuffer.h"
#include "TaskList.h"
#include "CommandHandler.h"
#include "DateTime.h"
#include "ErrorHandler.h"
#include "UART.h"
#include "Timing.h"

#ifdef CONFIG_MODULE_WEBPAGE_ENABLE
#include "WebpageHandler.h"
#endif

#include "ESP8266.h"


#ifdef CONFIG_MODULE_ESP8266_ENABLE

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define STRING_LENGTH(_str)			(sizeof(_str) - 1)


#ifdef ESP8266_USE_BLOCK_MODE
#define ESP8266_SEND_TCP_MESSAGE(msg)	ESP8266_SendTcpMessageBlockingMode(msg)
#else
#define ESP8266_SEND_TCP_MESSAGE(msg)	ESP8266_SendTcpMessageNonBlockingMode_Start(msg);
#endif



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
	// Do not change order, it will crash ESP8266 state machine!
	Esp8266Status_Unknown,
	Esp8266Status_Init,
	Esp8266Status_AfterInit,
	Esp8266Status_ConfigAte0,
	Esp2866Status_ConfigAte0CheckResponse,
	Esp8266Status_ConfigAt,
	Esp8266Status_ConfigAtCheckResponse,
	Esp8266Status_ConfigCwMode,
	Esp8266Status_ConfigCwModeCheckResponse,
#ifdef CONFIG_ESP8266_CWDHCP_ENABLE
	Esp8266Status_ConfigCwDhcp,
	Esp8266Status_ConfigCwDhcpCheckResponse,
#endif
#if (CONFIG_ESP8266_FIX_IP == 1)
	Esp8266Status_ConfigFixIp,
	Esp8266Status_ConfigFixIpCheckResponse,
#endif
	Esp8266Status_ConfigCipMux,
	Esp8266Status_ConfigCipMuxCheckResponse,
	Esp8266Status_WaitAfterSuccessfulConfig,
#if (CONFIG_ESP8266_IS_WIFI_HOST == 1)
	Esp8266Status_StartWifiHost,
	Esp8266Status_StartWifiHostCheckResponse,
#else
	Esp8266Status_ConnectWifiNetwork,
	Esp8266Status_ConnectWifiNetworkCheckResponse,
	Esp8266Status_ConnectWifiNetworkCheckFinish,
#endif
	Esp8266Status_PrintMyIpAddress,
	Esp8266Status_IpAddressResponse,
#if (CONFIG_ESP8266_IS_TCP_SERVER == 1)
	Esp8266Status_StartTcpServer,
	Esp8266Status_StartTcpServerCheckResponse,
	#ifdef CONFIG_MODULE_WEBPAGE_ENABLE
	Esp8266Status_StartTcpHttpServer,
	Esp8266Status_StartTcpHttpServerCheckResponse,
	#endif
#else
	Esp8266Status_ConnectTcpServer,
	Esp8266Status_ConnectTcpServerCheckResponse,
	Esp8266Status_ConnectTcpServerCheckFinish,
#endif

	Esp8266Status_BeforeIdle,
	Esp8266Status_Idle,

} ESP8266_StatusMachine_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

UART_HandleTypeDef ESP8266_UartHandle;

// Buffers
static volatile char ESP8266_TxBuffer[ESP8266_TX_BUFFER_LENGTH] = { 0 };
static volatile char ESP8266_RxBuffer[ESP8266_RX_BUFFER_LENGTH] = { 0 };

static CircularBufferInfo_t ESP8266_TxBuffStruct =
{
	.buffer = (char *)ESP8266_TxBuffer,
	.name = "ESP8266_TxBuffer",
	.size = ESP8266_TX_BUFFER_LENGTH
};

static CircularBufferInfo_t ESP8266_RxBuffStruct =
{
	.buffer = (char *)ESP8266_RxBuffer,
	.name = "ESP8266_RxBuffer",
	.size = ESP8266_RX_BUFFER_LENGTH
};

///< UART + CircularBuffer handler structure
UART_Handler_t ESP8266_Uart =
{
	.huart = &ESP8266_UartHandle,
	.tx = &ESP8266_TxBuffStruct,
	.rx = &ESP8266_RxBuffStruct,
	.txIsEnabled = true,
	.rxIsEnalbed = true,
#ifdef CONFIG_MODULE_UART_REQUIRE_TASKSCHEDULE_ENABLE
	// TODO: Do not use ESP8266 task scheduling from UART handler: Timeout used from Task...
	.requiredTask = Task_Count,
#endif
};


// TCP message receive buffer
static char ESP8266_TcpTransmitBuffer[ESP8266_TCP_MESSAGE_MAX_LENGTH]  = { 0 };

// TCP Message sending flags
static bool ESP8266_TcpSendBuffer_EnableFlag = true;
static bool ESP8266_TcpSendIsStarted_Flag = false;
static bool ESP8266_TcpSent_WaitSendOk_Flag = false;

static uint8_t ESP8266_TcpMessageId = 0;
static bool ESP8266_TcpCloseAfterSent = false;

// Statuses
static ESP8266_StatusMachine_t ESP8266StatusMachine = Esp8266Status_Unknown;

ESP8266_WifiConnectionStatusType ESP8266_ConnectionStatus = ESP8266_WifiConnectionStatus_Unknown;

ESP8266_TcpConnectionStatusType	ESP8266_TcpConnectionStatus = ESP8266_TcpConnectionStatus_Unknown;


///< My IP address
Network_IP_t ESP8266_MyWifiIpAddress = { 0 };
Network_IP_t ESP8266_ExWifiIpAddress = { 0 };


///< Error counter
static uint8_t ESP8266_ErrorCnt = 0;


#if (CONFIG_ESP8266_IS_WIFI_HOST == 0) && (CONFIG_ESP8266_CONNECT_DYNAMIC == 1)
///< Connection Wifi network name
static const char ESP8266_WifiNetworkName[] = CONFIG_ESP8266_WIFI_NETWORK_NAME;

///< Connection Wifi network password
static const char ESP8266_WifiNetworkPassword[] = CONFIG_ESP8266_WIFI_NETWORK_PASSWORD;
#endif


#if (CONFIG_ESP8266_IS_TCP_SERVER == 0) && (CONFIG_ESP8266_CONNECT_DYNAMIC == 1)
///< Server TCP IP address, port (if I am client and need connect to server)
static const Network_IP_t ESP8266_ServerAddress = { .IP =
{
	CONFIG_ESP8266_TCP_SERVER_IP_1,
	CONFIG_ESP8266_TCP_SERVER_IP_2,
	CONFIG_ESP8266_TCP_SERVER_IP_3,
	CONFIG_ESP8266_TCP_SERVER_IP_4
} };

static const Network_Port_t ESP8266_ServerPort = CONFIG_ESP8266_TCP_SERVER_PORT;
#endif


#if defined(CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE) && !defined(CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE)
///< Event DateTime for debug
const DateTime_t EventDateTime =
{
	.date =
	{
		.year = 17,
		.month = 1,
		.day = 14
	},
	.time =
	{
		.hour = 20,
		.minute = 0,
		.second = 0
	}
};
#endif



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static inline void ESP8266_ReceiveEnable(void);
static inline void ESP8266_SendEnable(void);
static void ESP8266_ResetHardware(void);

static bool ESP8266_ConvertIpString(char *message);

static void ESP8266_StartReceive(void);
static void ESP8266_ClearReceive(bool isFullClear, size_t stepLength);

static bool ESP8266_SendTcpMessageNonBlockingMode_Start(const char *message);
static size_t ESP8266_SendTcpMessageNonBlockingMode_SendMessage(void);
static void ESP8266_SendTcpClose(void);

static void ESP8266_CheckIdleStateMessage(char * receiveBuffer, size_t receivedMessageLength);
static bool ESP8266_ProcessReceivedTcpMessage(char *receiveBuffer);

#ifdef CONFIG_MODULE_WEBPAGE_ENABLE
static bool ESP8266_SearchGetRequest(const char *recvMsg);
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Initialize ESP8266
 */
void ESP8266_Init(void)
{
	// Circular buffer initialization
	CircularBuffer_Init(ESP8266_Uart.tx);
	CircularBuffer_Init(ESP8266_Uart.rx);

	
	//	GPIO Init
	GPIO_InitTypeDef  GPIO_InitStruct;

	ESP8266_PINS_CLK_ENABLES();
	
	
	// GPIO0 & GPIO2
	
	// help for pins: http://www.electrodragon.com/w/ESP8266
	// GPIO0 start with LOW level, after connecting will be on HIGH level
	// GPIO2 pin is on HIGH level...

	// TODO: Need this pin?
	GPIO_InitStruct.Pin       = ESP8266_GPIO2_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP ;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_GPIO2_GPIO_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ESP8266_GPIO2_GPIO_PORT, ESP8266_GPIO2_GPIO_PIN, GPIO_PIN_SET);

	// TODO: Need this pin?
	GPIO_InitStruct.Pin       = ESP8266_GPIO0_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_GPIO0_GPIO_PORT, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(ESP8266_GPIO0_GPIO_PORT, ESP8266_GPIO0_GPIO_PIN, GPIO_PIN_RESET);

	
	// RST
	GPIO_InitStruct.Pin       = ESP8266_RST_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_RST_GPIO_PORT, &GPIO_InitStruct);
	
	ESP8266_ResetHardware();


	// CH
	GPIO_InitStruct.Pin       = ESP8266_CH_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_CH_GPIO_PORT, &GPIO_InitStruct);	


	// CH UP
	HAL_GPIO_WritePin(ESP8266_CH_GPIO_PORT, ESP8266_CH_GPIO_PIN, GPIO_PIN_SET);
	//DelayMs(2000);
	
	
	// USART TX - RX	
	// Init late USART, because we receive a lot of message at ESP8266 start
	UART_Init(&ESP8266_UartHandle);

	ESP8266_ReceiveEnable();
}



/**
 * \brief	Receive enable
 */
static inline void ESP8266_ReceiveEnable(void)
{
	UART_ReceiveEnable(&ESP8266_Uart);
}



/**
 * \brief	Send enable
 */
static inline void ESP8266_SendEnable(void)
{
	UART_SendEnable(&ESP8266_Uart);
}



/**
 * \brief	Reset ESP8266 module on RST pin
 */
static void ESP8266_ResetHardware(void)
{
	ESP8266_RST_ACTIVE();
	DelayMs(1);
	ESP8266_RST_INACTIVE();
}



/**
 * \brief	Send string to ESP8266
 */
size_t ESP8266_SendString(const char *msg)
{
	size_t length = 0;
	size_t putLength;

	length = StringLength(msg);

	if (length == 0)
	{
		return 0;
	}

	putLength = CircularBuffer_PutString(ESP8266_Uart.tx, msg, length);

	if (putLength > 0)
		ESP8266_SendEnable();

	return putLength;
}



/**
 * \brief	Convert IP string to IP number
 */
static bool ESP8266_ConvertIpString(char *message)
{
	bool isOk = false;

#if (CONFIG_ESP8266_IS_WIFI_HOST == 1)
	char *pos2 = NULL;
#endif

	// String come like "192.168.4.1\r\n192.168.1.34\r\n\r\nOK\r\n"
	char *pos1 = (char *)STRING_FindString((const char *)message, "\r\n");
	if (pos1 != NULL)
	{
#if (CONFIG_ESP8266_IS_WIFI_HOST == 1)
		*pos1 = '\0';
		pos1 += 2;	// Skip "\r\n"
		pos2 = (char *)STRING_FindString(pos1, "\r\n");
		if (pos2 != NULL 0)
		{
			*pos2 = '\0';
			isOk = true;
		}
		// else: there is no ending "\r\n"
#else
		*pos1 = '\0';
		isOk = true;
#endif
	}
	else
	{
		// There is no ending "\r\n"
		isOk = false;
	}

	if (isOk)
	{
		// Process IP address string like "192.168.0.1" to Network_IP_t
		isOk = Network_ConvertIpAddressStringToIP(message, &ESP8266_MyWifiIpAddress);
#if (CONFIG_ESP8266_IS_WIFI_HOST == 1)
		isOk &= Network_ConvertIpAddressStringToIP(&message[pos1], &ESP8266_ExWifiIpAddress);
#endif
		if (isOk)
		{
			char ipBuffer[80];
			ESP8266_PrintIpAddress(ipBuffer);
			ESP8266_DEBUG_PRINT(ipBuffer);
		}
	}

	return isOk;
}



/**
 * \brief	Print ESP8266 IP addresses
 */
size_t ESP8266_PrintIpAddress(char * str)
{
	size_t length = 0;

	length += StrCpy(&str[length], "MyWifi: ");
	length += Network_PrintIp(&str[length], &ESP8266_MyWifiIpAddress);
#if (CONFIG_ESP8266_IS_WIFI_HOST == 1)
	length += StrCpy(&str[length], "\r\nExWifi: ");
	length += Network_PrintIp(&str[length], &ESP8266_ExWifiIpAddress);
#endif

	return length;
}



/**
 * \brief	ESP8266 receive reinitialize
 */
static void ESP8266_StartReceive(void)
{
	// Clear buffer
	CircularBuffer_Clear(ESP8266_Uart.rx);
}



/**
 * \brief	Clear receive buffer
 */
static void ESP8266_ClearReceive(bool isFullClear, size_t stepLength)
{
	if (isFullClear)
	{
		// Clear all buffer
		CircularBuffer_Clear(ESP8266_Uart.rx);
	}
	else
	{
		// Not full clear from readCnt to writeCnt
		CircularBuffer_DropCharacters(ESP8266_Uart.rx, stepLength);
	}
}



/**
 * \brief	Request send TCP message
 */
size_t ESP8266_RequestSendTcpMessage(const char * message)
{
	size_t length = 0;

	// Copy to send buffer
	if (ESP8266_TcpSendBuffer_EnableFlag)
	{
		// Lock buffer
		ESP8266_TcpSendBuffer_EnableFlag = false;
		// Copy message to buffer
		length = StrCpyMax((char *)ESP8266_TcpTransmitBuffer, message, ESP8266_TCP_MESSAGE_MAX_LENGTH);
	}
	else
	{
		// Cannot access to buffer
		length = 0;
		ESP8266_DEBUG_PRINT("Cannot request TCP message, it is disabled");
	}

	return length;
}



/**
 * \brief	Send message on TCP
 */
static bool ESP8266_SendTcpMessageNonBlockingMode_Start(const char *message)
{
	/*
	 * Send data
	 * AT+CIPSEND
	 * 1) single connection(+CIPMUX=0) AT+CIPSEND=<length>;
	 * 2) multiple connection (+CIPMUX=1) AT+CIPSEND= <id>,<length>
	 *
	 * AT+CIPSEND=4,18
	 * 18 byte to send: GET / HTTP/1.0\r\n\r\n
	 *
	 * Response:
	 * 	SEND OK
	 */

	size_t length = StringLength(message);
	char buffer[25]; // For "AT+CIPSEND=0,40\r\n"

	// Check length
	if (length >= ESP8266_TCP_MESSAGE_MAX_LENGTH)
	{
		// Wrong length
		ESP8266_DEBUG_PRINT("Too long message!");
		length = ESP8266_TCP_MESSAGE_MAX_LENGTH - 1;
	}


	// Send ~ "AT+CIPSEND=0,40\r\n"
	//usprintf(buffer, "AT+CIPSEND=0,%d\r\n", length);
	usprintf(buffer, "AT+CIPSEND=%d,%d\r\n", ESP8266_TcpMessageId, length);

	ESP8266_SendString(buffer);

	ESP8266_TcpSendIsStarted_Flag = true;

	ESP8266_DEBUG_PRINT("Start send TCP message");
	// Wait response immediately
	TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 100);

	return true;
}



/**
 * \brief	Send TCP message which are in ESP8266_TcpTransmitBuffer
 * \note	Recommend received "> " before this function
 */
static size_t ESP8266_SendTcpMessageNonBlockingMode_SendMessage(void)
{

	// Check length
	size_t length = StringLength((const char *)ESP8266_TcpTransmitBuffer);

	/*
	// TODO: Not need check length, because
	if (length >= ESP8266_TCP_MESSAGE_MAX_LENGTH)
	{
		// Wrong length
		ESP8266_DEBUG_PRINT("Too long message!");
		// TODO: Not the best solution
		length = ESP8266_TCP_MESSAGE_MAX_LENGTH - 1;
	}*/

	/*
	 * Example:
	 * Send:
	 * "GET / HTTP/1.0\r\n\r\n"
	 * Response:
	 * "SEND OK"
	 */

	// Send message and wait response
	ESP8266_SendString(ESP8266_TcpTransmitBuffer);

	ESP8266_DEBUG_PRINTF("Send TCP message: \"%s\"", ESP8266_TcpTransmitBuffer);

	return length;
}



/**
 * \brief	Close TCP connection: last <id> connection
 */
static void ESP8266_SendTcpClose(void)
{
	/*
	 * AT+CIPCLOSE
	 * Close TCP connection
	 * AT+CIPCLOSE=<id>
	 * If enabled more connection
	 */
	char cmd[20];
	usnprintf(cmd, 20, "AT+CIPCLOSE=%d\r\n", ESP8266_TcpMessageId);
	ESP8266_SendString(cmd);
}



#ifdef NOT_USED
/**
 * \brief	Start send TCP message with blocked
 */
bool ESP8266_StartSendTcpMessageBlocked(size_t length)
{
	/*
	 * Send data
	 * AT+CIPSEND
	 * 1) single connection(+CIPMUX=0) AT+CIPSEND=<length>;
	 * 2) multiple connection (+CIPMUX=1) AT+CIPSEND= <id>,<length>
	 *
	 * AT+CIPSEND=4,18
	 * 18 byte to send: GET / HTTP/1.0\r\n\r\n
	 *
	 * Response:
	 * 	SEND OK
	 */

	char buffer[25]; // For "AT+CIPSEND=0,40\r\n"

	// Send ~ "AT+CIPSEND=0,40\r\n"
	usprintf(buffer, "AT+CIPSEND=0,%d\r\n", length);
	ESP8266_SendString(buffer);

	uint8_t timeout = 200;
	bool sendIsEnable = false;
	while (timeout > 0)
	{
		char recvMsg[5];
		uint16_t recvLength = CircularBuffer_GetString(ESP8266_Uart.rx, recvMsg, 5);
		if (recvLength > 0)
		{
			if (!StrCmpFirst("> ", (const char *)recvMsg))
			{
				sendIsEnable = true;
				CircularBuffer_DropCharacters(ESP8266_Uart.rx, recvLength);
				break;
			}
		}

		// Wait
		timeout--;
		DelayMs(1);
	}

	return sendIsEnable;
}
#endif



#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
/**
 * \brief	ESP8266 state machine
 * \note	Call this task a lot of times (it is not an infinite loop)
 */
void ESP8266_StatusMachine(void)
{
	// If WriteCnt not equal with ReadCnt, we have received message
	char receiveBuffer[ESP8266_RX_BUFFER_LENGTH+1] = { 0 };
	uint16_t receivedMessageLength = 0;

	// Need copy to receiveBuffer
	receivedMessageLength = CircularBuffer_GetString(
			ESP8266_Uart.rx,
			receiveBuffer,
			ESP8266_RX_BUFFER_LENGTH);


#if (ESP8266_DEBUG_MODE == 1) && (ESP8266_DEBUG_PRINT_ALL_RECEIVED_MSG == 1)
	// Print all received chars:
	if (receivedMessageLength > 0)
	{
		DebugUart_Printf("ESP8266 Received: %s", receiveBuffer);
	}
#endif


	// Check ESP8266 status machine state
	switch (ESP8266StatusMachine)
	{
		case Esp8266Status_Unknown:
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Error: Unknown state");
			// TODO: Add warning suppression to here
			// break;	// Step to next

		case Esp8266Status_Init:
			ESP8266_ResetHardware();
			ESP8266StatusMachine++;
			ESP8266_ConnectionStatus = ESP8266_WifiConnectionStatus_Unknown;
			// Wait more second for startup device
			// Disable event
			//TaskHandler_DisableTask(Task_Esp8266);
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 5000);
			ESP8266_DEBUG_PRINT("Init");
			break;

		case Esp8266Status_AfterInit:
			// When we reach this state, we wait for ESP8266's startup
			ESP8266_StartReceive();
			ESP8266StatusMachine++;
			//TaskHandler_EnableTask(Task_Esp8266);
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 1000);
			ESP8266_DEBUG_PRINT("After init");
			break;

		case Esp8266Status_ConfigAte0:
			/*
			 * "ATE0"
			 * Echo off
			 */
			ESP8266_StartReceive();
			ESP8266_SendString("ATE0\r\n");
			ESP8266StatusMachine++;
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 1000);
			ESP8266_DEBUG_PRINT("Config ATE0");
			break;

		case Esp2866Status_ConfigAte0CheckResponse:
			if (!StrCmpFirst("ATE0\r\r\n\r\nOK\r\n", (const char *)receiveBuffer)
				|| !StrCmpFirst("\r\nOK\r\n", (const char *)receiveBuffer))
			{
				// Ok
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Config ATE0 response Ok");
			}
			else
			{
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_Init;
				ESP8266_DEBUG_PRINTF("Config ATE0 response failed, received message: \"%s\"",
									 (const char *)receiveBuffer);
			}
			ESP8266_ClearReceive(true, 0);
			break;

		case Esp8266Status_ConfigAt:

			/*
			 * "AT"
			 * Check it is alive?
			 */
			ESP8266_StartReceive();
			ESP8266_SendString("AT\r\n");
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Config AT");
			break;

		case Esp8266Status_ConfigAtCheckResponse:
			if (!StrCmpFirst("\r\nOK\r\n", (const char *)receiveBuffer))
			{
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Config AT successful");
			}
			else
			{
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_Init;
				ESP8266_DEBUG_PRINTF("Config AT failed, received message: \"%s\"",
									 (const char *)receiveBuffer);
			}
			ESP8266_ClearReceive(true, 0);
			break;

		case Esp8266Status_ConfigCwMode:

			/*
			 * CWMODE
			 * 	1 = station
			 * 	2 = host / AP (Access Point)
			 * 	3 = dual
			 */
			ESP8266_StartReceive();
#if CONFIG_ESP8266_IS_WIFI_HOST == 1
			// Dual = Station + host
			ESP8266_SendString("AT+CWMODE=3\r\n");
#else
			// Only Station
			ESP8266_SendString("AT+CWMODE=1\r\n");
#endif
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Config CWMODE sent");
			break;

		case Esp8266Status_ConfigCwModeCheckResponse:
			if ((!StrCmpFirst("OK\r\n", (const char *)receiveBuffer))
				|| (!StrCmpFirst("no change\r\n", (const char *)receiveBuffer))
				|| (!StrCmpFirst("no chae\r\n", (const char *)receiveBuffer)))	// Note: ESP8266 fault ?
			{
				// "OK"
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Config CWMODE successful");
			}
			else
			{
				// Other... it is wrong
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_Init;
				ESP8266_DEBUG_PRINT("Config CWMODE failed");
			}
			ESP8266_ClearReceive(true, 0);
			break;

#ifdef CONFIG_ESP8266_CWDHCP_ENABLE
		case Esp8266Status_ConfigCwDhcp:
			/*
			 * - Static IP: AT+CWDHCP=1,0\r\n
			 * - Dynamic IP: AT+CWDHCP=2,1\r\n
			 * <mode>
			 * 0 : set ESP8266 soft-AP
			 * 1 : set ESP8266 station
			 * 2 : set both softAP and station
			 * <en>
			 * 0 : Disable DHCP
			 * 1 : Enable DHCP
			*/
			ESP8266_StartReceive();
			ESP8266_SendString("AT+CWDHCP=1,1\r\n");
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Config CWDHCP sent");
			break;

		case Esp8266Status_ConfigCwDhcpCheckResponse:
			if (!StrCmpFirst("OK\r\n", (const char *)receiveBuffer))
			{
				// "OK"
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Config CWDHCP response ok");
			}
			else if (!StrCmpFirst("no change\r\n",(const char *)receiveBuffer))
			{
				// "no change"
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Config CWDHCP response ok");
			}
			else
			{
				// Other... it is wrong
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_ConfigAt;
				ESP8266_DEBUG_PRINT("Config CWDHCP response failed");
			}
			ESP8266_ClearReceive(true, 0);
			break;
#endif	// #ifdef CONFIG_ESP8266_CWDHCP_ENABLE

#if (CONFIG_ESP8266_FIX_IP == 1)
#warning "Not tested function"
		/*****************
		 * Set fix IP
		 *	AT+CIPSTA	set IP address of STAtion
		 *	AT+CIPSTA=<ip>
		 ******************/
		case Esp8266Status_ConfigFixIp:
			ESP8266_StartReceive();
			ESP8266_SendString("AT+CIPSTA=\"" CONFIG_ESP8266_FIX_IP_ADDRESS "\"\r\n");
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Config Fix IP sent");
			break;

		case Esp8266Status_ConfigFixIpCheckResponse:
			if (!StrCmpFirst("OK\r\n", (const char *)receiveBuffer)
				|| !StrCmpFirst("no change\r\n",(const char *)receiveBuffer))
			{
				// "OK"
				// "no change"
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Config CWDHCP response ok");
			}
			else
			{
				// Other... it is wrong
				ESP8266_LED_FAIL();
				ESP8266StatusMachine--;
				ESP8266_DEBUG_PRINT("Config CWDHCP response failed");
			}
			ESP8266_ClearReceive(true, 0);
			break;
#endif	// #if (CONFIG_ESP8266_FIX_IP == 1)

		case Esp8266Status_ConfigCipMux:
			/*
			 * AT+CIPMUX = 	set multiple connections mode
			 * Connection Type:
			 * AT+CIPMUX=0		// Single
			 * AT+CIPMUX=1		// Multiple
			 *
			 * Now: "AT+CIPMUX=1\r\n"
			 */
			ESP8266_StartReceive();
			ESP8266_SendString("AT+CIPMUX=1\r\n");
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Config CIPMUX sent");
			break;

		case Esp8266Status_ConfigCipMuxCheckResponse:
			if ((!StrCmpFirst("\r\nOK\r\n", (const char *)receiveBuffer))
				|| (!StrCmpFirst("link is builded\r\n", (const char *)receiveBuffer)))
			{
				// "OK"
				// TODO: "link is builded\r\n" message is not correct, but it is good for us
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Config CIPMUX response ok");
			}
			else
			{
				// Other... it is wrong
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_Init;
				ESP8266_DEBUG_PRINT("Config CIPMUX response failed");
			}
			ESP8266_ClearReceive(true, 0);
			break;

		case Esp8266Status_WaitAfterSuccessfulConfig:
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Wait After successful config");
			break;

		// End of config

#if (CONFIG_ESP8266_IS_WIFI_HOST == 1)
		case Esp8266Status_StartWifiHost:
			/*
			 * Create Soft Access Point
			 * AT+CWSAP="networkname","password",1,0
			 * 3. param = channel
			 * 4. param = encryption
			 */
			ESP8266_StartReceive();
			ESP8266_SendString("AT+CWSAP=\""
								CONFIG_ESP8266_WIFI_NETWORK_NAME
								""\",\""
								CONFIG_ESP8266_WIFI_NETWORK_PASSWORD
								"\",1,0\r\n");
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Wifi server create");
			break;

		case Esp8266Status_StartWifiHostCheckResponse:
			if (!StrCmpFirst("\r\nOK\r\n", (const char *)receiveBuffer))
			{
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Wifi server create successful");
			}
			else
			{
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_StartWifiHost;
				ESP8266_DEBUG_PRINTF("Wifi server create failed, received message: \"%s\"\r\n",
													 (const char *)receiveBuffer);
			}
			ESP8266_ClearReceive(true, 0);
			break;
		// End of "#if CONFIG_ESP8266_IS_WIFI_HOST == 1"
#else
		// Not WiFi host (connect to network)
		case Esp8266Status_ConnectWifiNetwork:
			/*
			 * Join to WiFi network
			 * AT+CWJAP
			 * Syntax: AT+CWJAP="networkname","password"
			 */
			ESP8266_StartReceive();
			ESP8266_ClearReceive(true, 0);
	#if (CONFIG_ESP8266_CONNECT_DYNAMIC == 1)
			// Dynamic value of WiFi network name and password
			char str[50];
			usprintf(str, "AT+CWJAP=\"%s\",\"%s\"\r\n",
					ESP8266_WifiNetworkName, ESP8266_WifiNetworkPassword);
			ESP8266_SendString(str);
	#else
			// Fix WiFi network name and password
			ESP8266_SendString("AT+CWJAP=\""
					CONFIG_ESP8266_WIFI_NETWORK_NAME
					"\",\""
					CONFIG_ESP8266_WIFI_NETWORK_PASSWORD
					"\"\r\n");
	#endif
			ESP8266StatusMachine++;
			ESP8266_ErrorCnt = 0;
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 1000);
			ESP8266_DEBUG_PRINT("Start connect to wifi network...");
			break;

		case Esp8266Status_ConnectWifiNetworkCheckResponse:

			if (!StrCmpFirst("\r\nOK\r\n", (const char *)receiveBuffer))
			{
				// Command OK, step to wait response
				ESP8266StatusMachine += 2;	// TODO: Now, it skipped wait finish
				ESP8266_DEBUG_PRINT("Wifi connect command OK");
				ESP8266_ClearReceive(false, STRING_LENGTH("\r\nOK\r\n"));
			}
			else if (!StrCmpFirst("\r\nbusy p...\r\n", (const char *)receiveBuffer))
			{
				// "busy p..."
				// I hope, connection in progress
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Wait... (busy)\r\n");
				ESP8266_ClearReceive(false, STRING_LENGTH("\r\nbusy p...\r\n"));
			}
			else if (!StrCmpFirst("\r\nFAIL", (const char *)receiveBuffer)
					|| !StrCmpFirst("\r\nERROR", (const char *)receiveBuffer))
			{
				// "FAIL"
				// "ERROR"
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
				ESP8266_DEBUG_PRINT("WiFi connect command failed");
				ESP8266_ClearReceive(true, 0);
			}
			else if (receivedMessageLength == 0)
			{
				// Not received anything
				ESP8266_DEBUG_PRINT("Wifi connection in progress, wait reponse...");
				ESP8266_ErrorCnt++;
			}
			else
			{
				// Received character, but not received response?
				ESP8266_DEBUG_PRINTF("Wifi connection in progress, received unknown message (but not processed): \"%s\"\r\n", receiveBuffer);
				ESP8266_ErrorCnt++;
			}

			// For try-safe (avoid the infinite connection trying loop)
			if (ESP8266_ErrorCnt > 10)
			{
				ESP8266_ErrorCnt = 0;
				ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
				ESP8266_DEBUG_PRINT("Restart connect to wifi network...");
				ESP8266_ClearReceive(true, 0);
			}

			break;

		case Esp8266Status_ConnectWifiNetworkCheckFinish:

			if (!StrCmpFirst("\r\nOK\r\n", (const char *)receiveBuffer))
			{
				// OK
				// Command OK, step to wait response
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Wifi connect successful");
				ESP8266_ClearReceive(false, STRING_LENGTH("\r\nOK\r\n"));
			}
			else if (!StrCmpFirst("\r\nFAIL", (const char *)receiveBuffer)
					|| !StrCmpFirst("\r\nERROR", (const char *)receiveBuffer))
			{
				// "FAIL"
				// "ERROR"
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
				ESP8266_DEBUG_PRINT("Wifi connect failed");
				ESP8266_ClearReceive(true, 0);
			}
			else if (!receivedMessageLength)
			{
				ESP8266_DEBUG_PRINT("Wifi connecting...");
				ESP8266_ErrorCnt++;
				if (ESP8266_ErrorCnt > 15)
				{
					ESP8266_ErrorCnt = 0;
					ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
					ESP8266_DEBUG_PRINT("Restart connect to wifi network...");
					ESP8266_ClearReceive(true, 0);
				}
			}
			else
			{
				// Not received response?
				ESP8266_DEBUG_PRINTF("Wifi connecting... received unknown message: \"%s\"\r\n", receiveBuffer);
				ESP8266_ErrorCnt++;
				if (ESP8266_ErrorCnt > 20)
				{
					ESP8266_ErrorCnt = 0;
					ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
					ESP8266_DEBUG_PRINT("Restart connect to wifi network...");
					ESP8266_ClearReceive(true, 0);
				}
			}
			break;

#endif	// End of "CONFIG_ESP8266_IS_WIFI_HOST == 0"

		case Esp8266Status_PrintMyIpAddress:
			/* Get IP address
			 * AT+CIFSR
			 *
			 * Response:
			 * 192.168.0.106\r\nOK
			 */
			ESP8266_StartReceive();
			ESP8266_SendString("AT+CIFSR\r\n");
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Get IP address");
			ESP8266_ClearReceive(true, 0);
			break;

		case Esp8266Status_IpAddressResponse:
			if (ESP8266_ConvertIpString(receiveBuffer))
			{
				// Good processing
				// Check IP
				if (ESP8266_MyWifiIpAddress.IP[0] != 0)
				{
					// Good IP
					ESP8266StatusMachine++;
					ESP8266_DEBUG_PRINT("IP address processed, and good");
				}
				else
				{
					// Wrong IP, reconnect to WiFi
					ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
				}

				ESP8266_ClearReceive(true, 0);
			}
			else
			{
				// Wrong / Not full response received
				ESP8266_ErrorCnt++;
				if (ESP8266_ErrorCnt > 3 && ESP8266_ErrorCnt < 10)
				{
					ESP8266StatusMachine = Esp8266Status_PrintMyIpAddress;
					ESP8266_DEBUG_PRINT("IP address process failed");
					ESP8266_ClearReceive(true, 0);
				}
				else if (ESP8266_ErrorCnt >= 10)
				{
					// Has lot of error
					ESP8266_ErrorCnt = 0;
					#if CONFIG_ESP8266_IS_WIFI_HOST == 1
					ESP8266StatusMachine = Esp8266Status_StartWifiHost;
					#else
					ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
					#endif
					ESP8266_DEBUG_PRINT("IP address process has lot of fails");
					ESP8266_ClearReceive(true, 0);
				}
			}
			break;

#if CONFIG_ESP8266_IS_TCP_SERVER == 1
		case Esp8266Status_StartTcpServer:
			/*
			 * Set as server/listen()
			 * AT+CIPSERVER
			 *
			 * Syntax: AT+CIPSERVER=<mode>,<port>
			 * E.g.:   AT+CIPSERVER=1,2000
			 *
			 * Response:
			 * 	 OK
			 * 	 Linked
			 */
			ESP8266_StartReceive();
			// E.g. "AT+CIPSERVER=1,2000"
			ESP8266_SendString("AT+CIPSERVER=1," CONFIG_ESP8266_TCP_SERVER_PORT_STRING "\r\n");
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Start server");
			break;

		case Esp8266Status_StartTcpServerCheckResponse:
			// Check CIPSERVER response
			if (!StrCmpFirst("\r\nOK\r\n", (const char *)receiveBuffer)
				|| !StrCmpFirst("no change\r\n", (const char *)receiveBuffer))
			{
				// OK
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Successful started server");
				// Disable task, because next state is wait client
				//TaskHandler_DisableTask(Task_Esp8266);
				ESP8266_StartReceive();
			}
			else if (!StrCmpFirst("ERROR", (const char *)receiveBuffer))
			{
				// ERROR
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_StartTcpServer;
				ESP8266_DEBUG_PRINT("Failed started server (ERROR)");
			}
			else
			{
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_StartTcpServer;
				ESP8266_DEBUG_PRINT("Failed started server");
			}
			ESP8266_ClearReceive(true, 0);
			break;
	#ifdef CONFIG_MODULE_WEBPAGE_ENABLE
		case Esp8266Status_StartTcpHttpServer:
			/*
			 * Set as server/listen()
			 * AT+CIPSERVER
			 *
			 * Syntax: AT+CIPSERVER=<mode>,<port>
			 * E.g.:   AT+CIPSERVER=1,2000
			 * mode:   0 - close
			 *         1 - open
			 *
			 * Response:
			 * 	 OK
			 * 	 Linked
			 */
			ESP8266_StartReceive();
			// E.g. "AT+CIPSERVER=1,2000"
			ESP8266_SendString("AT+CIPSERVER=1," CONFIG_ESP8266_TCP_HTTP_PORT_STRING "\r\n");
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Start server");
			break;

		case Esp8266Status_StartTcpHttpServerCheckResponse:
			// Check CIPSERVER response
			if (!StrCmpFirst("\r\nOK\r\n", (const char *)receiveBuffer)
				|| !StrCmpFirst("no change\r\n", (const char *)receiveBuffer))
			{
				// OK
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Successful started server");
				// Disable task, because next state is wait client
				//TaskHandler_DisableTask(Task_Esp8266);
				ESP8266_StartReceive();
			}
			else if (!StrCmpFirst("ERROR", (const char *)receiveBuffer))
			{
				// ERROR
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_StartTcpHttpServer;
				ESP8266_DEBUG_PRINT("Failed started server (ERROR)");
			}
			else
			{
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_StartTcpHttpServer;
				ESP8266_DEBUG_PRINT("Failed started server");
			}
			ESP8266_ClearReceive(true, 0);
			break;
	#endif	// #ifdef CONFIG_MODULE_WEBPAGE_ENABLE

#else	// End of #if CONFIG_ESP8266_IS_TCP_SERVER == 1
//  #if CONFIG_ESP8266_IS_TCP_SERVER == 0

		case Esp8266Status_ConnectTcpServer:

			/*
			 * AT+CIPSTART=?
			 * (CIPMUX=0) AT+CIPSTART = <type>,<addr>,<port>
			 * (CIPMUX=1) AT+CIPSTART=  <id><type>,<addr>, <port>
			 * AT+CIPSTART=4,"TCP","google.com",80
			 *	Now: AT+CIPSTART=0,"TCP","9.6.5.14",2000
			 *		id=0, TCP, 9.6.5.14, port:2000
			 *
			 * Response:
			 * OK
			 * Linked
			 */

			ESP8266_StartReceive();
			// Original string: "AT+CIPSTART=0,\"TCP\",\"192.168.1.62\",2000\r\n"
#if CONFIG_ESP8266_CONNECT_DYNAMIC == 1
			char buffer[60];
			uint8_t length = 0;
			length += usprintf(buffer, "AT+CIPSTART=" CONFIG_ESP8266_TCP_CLIENT_CONNECTION_ID ",\"TCP\",\"");
			length += Network_PrintIp(&buffer[length], (Network_IP_t *)&ESP8266_ServerAddress);
			length += usprintf(&buffer[length], "\",%d\r\n", ESP8266_ServerPort);
			ESP8266_SendString(buffer);
#else
			// Note: 0. id
			ESP8266_SendString("AT+CIPSTART=" CONFIG_ESP8266_TCP_CLIENT_CONNECTION_ID ",\"TCP\",\""
					CONFIG_ESP8266_TCP_SERVER_IP_STRING
					"\","
					CONFIG_ESP8266_TCP_SERVER_PORT_STRING
					"\r\n");
#endif

			ESP8266StatusMachine++;
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 1000);
			ESP8266_DEBUG_PRINT("Start connect TCP");

			break;

		case Esp8266Status_ConnectTcpServerCheckResponse:

			if (!StrCmpFirst("\r\nOK\r\n", (const char *)receiveBuffer))
			{
				// OK
				// Need Wait...
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("OK... Wait finish...");
				ESP8266_ClearReceive(false, STRING_LENGTH("\r\nOK\r\n"));
			}
			else if (!StrCmpFirst("ALREADY CONNECT\r\n", (const char *)receiveBuffer)
				||   !StrCmpFirst("ALREAY CONNECT\r\n", (const char *)receiveBuffer))
			{
				// OK
				// Already connected
				ESP8266_LED_OK();
				ESP8266StatusMachine += 2;	// Jump CheckFinish
				ESP8266_DEBUG_PRINT("Successful connect to TCP server (already connected)");
				ESP8266_ClearReceive(true, 0);
			}
			else if (!StrCmpFirst("\r\nbusy p...\r\n", (const char *)receiveBuffer))
			{
				// busy p...
				ESP8266StatusMachine++;	// I hope, the command in progress
				ESP8266_DEBUG_PRINT("Failed connect to TCP server (busy)... Wait...");
				// TODO: Wait... ?
				ESP8266_ClearReceive(false, STRING_LENGTH("\r\nbusy p...\r\n"));

#warning "Need close! Beautify!"
				ESP8266_SendString("AT+CIPCLOSE=0\r\n");
				DelayMs(50);
				ESP8266_ClearReceive(true, 0);
			}
			else if (!StrCmpFirst("no ip\r\n", (const char *)receiveBuffer))
			{
				// Error, no IP
				ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
				ESP8266_ErrorCnt = 0;
				ESP8266_DEBUG_PRINT("Error: No IP...");
				ESP8266_ClearReceive(true, 0);
			}
			else
			{
				ESP8266_DEBUG_PRINT("TCP connect command response not received, wait...");
				ESP8266_ErrorCnt++;
				if (ESP8266_ErrorCnt > 5)
				{
					// Has lot of errors
					ESP8266StatusMachine = Esp8266Status_ConnectTcpServer;
					ESP8266_ErrorCnt = 0;
					ESP8266_DEBUG_PRINT("Error: Restart connecting...");
					if (receivedMessageLength > 0)
						ESP8266_DEBUG_PRINTF("Received message: %s", receiveBuffer);
					ESP8266_ClearReceive(true, 0);
				}
			}
			break;

		case Esp8266Status_ConnectTcpServerCheckFinish:
			if (!StrCmpFirst("\r\nOK\r\nLinked\r\n", (const char *)receiveBuffer)
				|| !StrCmpFirst("Linked\r\n", (const char *)receiveBuffer))
			{
				// OK Linked
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Successful connect to TCP server");
				ESP8266_ClearReceive(true, 0);
			}
			else if (!StrCmpFirst("\r\nERROR\r\nUnlink", (const char *)receiveBuffer))
			{
				// Error
				// It is TCP connection error, not WiFi error
				ESP8266_LED_FAIL();
				ESP8266_ErrorCnt++;
				ESP8266StatusMachine = Esp8266Status_ConnectTcpServer;
				ESP8266_DEBUG_PRINT("Failed connect to TCP server (unlink)");
				ESP8266_ClearReceive(true, 0);
			}
			else if (!StrCmpFirst("\r\nFAIL\r\n", (const char *)receiveBuffer))
			{
				// Error
				// It is TCP connection error, not WiFi error
				// TODO: It is true???
				ESP8266_LED_FAIL();
				ESP8266_ErrorCnt++;
				ESP8266StatusMachine = Esp8266Status_ConnectTcpServer;
				ESP8266_DEBUG_PRINT("Failed connect to TCP server (fail)");
				ESP8266_ClearReceive(true, 0);
			}
			else
			{
				// Unknown error..
				ESP8266_LED_FAIL();
				ESP8266_ErrorCnt++;
				// Do not step state ?
				if (receivedMessageLength > 0)
					ESP8266_DEBUG_PRINTF("Unknown received message: \"%s\" at connection to TCP server", receiveBuffer);
				else
					ESP8266_DEBUG_PRINT("Not received message at connection to TCP server");
				if (ESP8266_ErrorCnt > 10)
				{
					// Has lot of errors
					ESP8266StatusMachine = Esp8266Status_ConnectTcpServer;
					ESP8266_ErrorCnt = 0;
					ESP8266_DEBUG_PRINT("Error: Restart connecting...");
					ESP8266_ClearReceive(true, 0);
					// TODO: Check, there is network?
				}
			}

			break;

			// TODO: TCP connection checking
#endif	//#if CONFIG_ESP8266_IS_TCP_SERVER == 0

		case Esp8266Status_BeforeIdle:
			ESP8266_StartReceive();
			ESP8266_ClearReceive(true, 0);
			// Set TaskHandler to faster
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 100);
			ESP8266StatusMachine++;
			ESP8266_ConnectionStatus = ESP8266_WifiConnectionStatus_SuccessfulServerStarted;
			ESP8266_DEBUG_PRINT("Idle");
			break;

		case Esp8266Status_Idle:
			// Idle state (Check link, unlink, received message...)
			ESP8266_CheckIdleStateMessage(receiveBuffer, receivedMessageLength);
#ifdef ESP8266_PERIODICALLY_SEND_VERSION
			static uint8_t IdleCnt = 0;
			IdleCnt++;
			if (IdleCnt > 30)
			{
				IdleCnt = 0;
				ESP8266_DEBUG_PRINT("Send an idle message...");
				ESP8266_RequestSendTcpMessage("version");
			}
#endif
			break;

		default:
			// Unknown state = Error --> Go to Init state
			ESP8266StatusMachine = Esp8266Status_Init;
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 1000);
			ESP8266_DEBUG_PRINT("Error! Unknown or uninplemented StateMachine state!");
			break;

	}	// End of switch

	return;
}
#endif



/**
 * \brief	Check idle state message
 * 			- Link
 * 			- Unlink
 * 			- +IPD: Received message
 */
static void ESP8266_CheckIdleStateMessage(char * receiveBuffer, size_t receivedMessageLength)
{
	/*
	 * Check message, which can be anything...
	 * Link
	 * Unlink
	 * +IPD: Received message
	 */
	if (CircularBuffer_IsNotEmpty(ESP8266_Uart.rx))
	{
		// There is some character in rx buffer

		bool goodMsgRcv = false;

		// Buffer changed

		if (ESP8266_TcpSendIsStarted_Flag)
		{
			// TCP message sending started, wait "> "
			// Check, response is "> "
			if (!StrCmpFirst("> ", (const char *)receiveBuffer))
			{
				// Received "> ", we can send message
				ESP8266_DEBUG_PRINT("Received \"> \"");
				ESP8266_SendTcpMessageNonBlockingMode_SendMessage();

				ESP8266_TcpSent_WaitSendOk_Flag = true;
				TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 1000);
				goodMsgRcv = true;
				ESP8266_ClearReceive(false, STRING_LENGTH("> "));
			}
			else if (!StrCmpFirst("link is not\r\n", (const char *)receiveBuffer))
			{
				// Cannot send
				ESP8266_DEBUG_PRINT("Cannot send (link is not)");
				ESP8266_TcpSendBuffer_EnableFlag = true;
				ESP8266_ClearReceive(false, STRING_LENGTH("link is not\r\n"));
				// Request TCP reconnect
#if CONFIG_ESP8266_IS_TCP_SERVER == 0
				ESP8266StatusMachine = Esp8266Status_ConnectTcpServer;
#endif
			}
			else if (!StrCmpFirst("busy p...", (const char *)receiveBuffer))
			{
				// Error: We cannot send now
				// TODO: What we need to do? Wait a moment?
				//ESP8266_TcpSendBuffer_EnableFlag = true; // TODO: Do not clear, send later
				ESP8266_DEBUG_PRINT("Cannot send TCP message (busy p...)");
				ESP8266_ClearReceive(false, STRING_LENGTH("busy p..."));
			}
			else if (!StrCmpFirst("\r\nbusy p...", (const char *)receiveBuffer))
			{
				// TODO: Duplicated code
				ESP8266_DEBUG_PRINT("Cannot send TCP message (busy p...)");
				ESP8266_ClearReceive(false, STRING_LENGTH("\r\nbusy p..."));
			}
			else if (!StrCmpFirst("busy inet...\r\nERROR\r\n", (const char *)receiveBuffer))
			{
				// Error...
				// TODO: What we need to do? Reconnection
				//ESP8266_TcpSendBuffer_EnableFlag = true;	// TODO: Send later
				ESP8266_DEBUG_PRINT("Cannot send TCP message (busy inet...)");
				ESP8266_ClearReceive(false, STRING_LENGTH("busy inet...\r\nERROR\r\n"));
			}
			else
			{
				// Error
				//ESP8266_TcpSendBuffer_EnableFlag = true;	// TODO: Send later
				// TODO: Be careful, it can be buffer overflow !!!
				ESP8266_DEBUG_PRINTF("Cannot send TCP message, not received \"> \"\r\n"
						"Received message: \"%s\"", receiveBuffer);
				// TODO: Do not clear, because sometimes we receive any important message ...
				//ESP8266_ClearReceive(true, 0);
			}
			// Clear send flag
			ESP8266_TcpSendIsStarted_Flag = false;
		}
		else if (ESP8266_TcpSent_WaitSendOk_Flag)
		{
			// Finish of TCP message sending
			if (!StrCmpFirst("\r\nSEND OK\r\n", (const char *)receiveBuffer))
			{
				// OK, sending successful
				ESP8266_DEBUG_PRINT("Sending successful");
				goodMsgRcv = true;
				ESP8266_ClearReceive(false, STRING_LENGTH("\r\nSEND OK\r\n"));

				if (ESP8266_TcpCloseAfterSent)
				{
					ESP8266_TcpCloseAfterSent = false;
					ESP8266_SendTcpClose();
				}
			}
			else
			{
				// TODO: Other answer is possible?
				// Error, not received "SEND OK"
				ESP8266_DEBUG_PRINTF("Sending failed (not received \"SEND OK\"). Received: \"%s\"", receiveBuffer);
				ESP8266_ClearReceive(true, 0);
			}
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 100);
			ESP8266_TcpSent_WaitSendOk_Flag = false;
			ESP8266_TcpSendBuffer_EnableFlag = true;
		}
		else if (!StrCmpFirst("Link\r\n", (const char *)receiveBuffer))
		{
			// "Link"
			ESP8266_DEBUG_PRINT("Received \"Link\": a client connected");
			// TODO: Step to IP address printing (connected clients)
			//ESP8266StatusMachine = Esp8266Status_XXX;
			goodMsgRcv = true;
			ESP8266_ClearReceive(false, STRING_LENGTH("Link\r\n"));
			ESP8266_LOG_EVENT(EVENT_LINK);
		}
		else if (!StrCmpFirst("Unlink\r\n", (const char *)receiveBuffer))
		{
			// "Unlink"
			ESP8266_DEBUG_PRINT("Received \"Unlink\": a client disconnected");
			goodMsgRcv = true;
			ESP8266_ClearReceive(false, STRING_LENGTH("Unlink\r\n"));
			ESP8266_LOG_EVENT(EVENT_UNLINK);
		}
#if CONFIG_ESP8266_IS_TCP_SERVER == 0
		else if (!StrCmpFirst("\r\nERROR\r\nUnlink\r\n", (const char *)receiveBuffer))
		{
			// "Unlink"
			ESP8266_DEBUG_PRINT("Received \"Unlink\": a TCP disconnected");
			goodMsgRcv = true;
			ESP8266StatusMachine = Esp8266Status_ConnectTcpServer;
			ESP8266_ClearReceive(false, STRING_LENGTH("Received \"Unlink\": a tcp disconnected\r\n"));
			ESP8266_LOG_EVENT(EVENT_UNLINK);
		}
#endif
		else if (!StrCmpFirst("\r\n+IPD,", (const char *)receiveBuffer))	// TODO: Change to "+IPD"... ?
		{
			ESP8266_DEBUG_PRINT("Received +IPD");
			if (ESP8266_ProcessReceivedTcpMessage(receiveBuffer))
			{
				// Successful processed message
				//ESP8266_ClearReceive(true, 0); // TODO: Do not clear! Cleared from Process() function
				goodMsgRcv = true;
				//ESP8266_ErrorCnt = 0;
			}
			// else: wrong TCP message. Not need debug print it, because ESP8266_ProcessReceivedTcpMessage() did that
		}
		else if (!StrCmpFirst("busy p...", (const char *)receiveBuffer))
		{
			// "busy p..."
			ESP8266_ClearReceive(false, STRING_LENGTH("busy p..."));
		}
		else if (!StrCmpFirst("\r\n", (const char *)receiveBuffer))
		{
			// "\r\n"
			ESP8266_ClearReceive(false, STRING_LENGTH("\r\n"));
		}
		else if (!StrCmpFirst("OK", (const char *)receiveBuffer))
		{
			// TODO: This is need for drop \r\nOK\r\n message. But why need? ESP8266_ProcessReceivedTcpMessage() handled wrongly?
			// "\r\n"
			ESP8266_ClearReceive(false, STRING_LENGTH("OK"));
		}
		else if (receivedMessageLength < 6)
		{
			// Received too small message
			ESP8266_DEBUG_PRINTF("Received too small message, wait more: \"%s\"", (const char *)receiveBuffer);
			//ESP8266_RxBuffer_ReadCnt - do not clear
		}
		else
		{
			// Received unknown message
			ESP8266_DEBUG_PRINTF("Received unknown message, wait more: \"%s\"", (const char *)receiveBuffer);
			// TODO: Delete these!
			/*size_t processedLength = ESP8266_ProcessReceivedTcpMessage(receiveBuffer);
			ESP8266_ClearReceive(false, processedLength);*/

			//ESP8266_RxBuffer_ReadCnt - do not clear
		}


		// Error handling
		// If has lot of error, clear buffer
		if (!goodMsgRcv)
		{
			// If has a lot of error
			ESP8266_ErrorCnt++;
			if ((ESP8266_ErrorCnt > 5) || (receivedMessageLength > (ESP8266_RX_BUFFER_LENGTH - 10)))
			{
				ESP8266_DEBUG_PRINT("Has lot of errors, cleared buffer");
				// ~Reset buffer
				ESP8266_ClearReceive(true, 0);
				ESP8266_ErrorCnt = 0;
				ESP8266_LOG_EVENT(EVENT_ERROR);

				// Clear sends...
				ESP8266_TcpSendBuffer_EnableFlag = true;
				ESP8266_TcpSendIsStarted_Flag = false;
			}
		}
		else
		{
			// Received good message
			// TODO: Think about ErrorCnt clearing...
			ESP8266_ErrorCnt = 0;
		}
	}
	else if (ESP8266_TcpSendBuffer_EnableFlag == false)
	{
		// We has message on SendBuffer (we want to send a requested message)
		// ReadCnt = WriteCnt, so we are not receiving
		// TODO: Be careful, it is not truth
		// TODO: Check last received time?
		if (ESP8266_TcpSendIsStarted_Flag == false)
		{
			// Not started to send message, we can send
			ESP8266_SEND_TCP_MESSAGE((char *)ESP8266_TcpTransmitBuffer);
		}
		else
		{
			ESP8266_DEBUG_PRINT("Sending in process...");	// TODO: ...
		}
	}
	else
	{
		/*
		 * Not received character
		 * Do nothing...
		 */
	}
}



/**
 * \brief	Process received TCP message
 * 			Actual message start with: \r\n+IPD,
 * \retval	Processed (Need drop chars)
 */
static bool ESP8266_ProcessReceivedTcpMessage(char *receiveBuffer)
{
	bool isOk = false;

	// Message form: "\r\n+IPD,1,4:msg3\r\nOK"

	receiveBuffer += STRING_LENGTH("\r\n+IPD,");
	// Has got finish "\r\n"?
	if (STRING_FindString((const char *)receiveBuffer, "\r\n") != NULL)
	{
		// Received good message, with finish "\r\n"

		char *splittedTcpMsg[3] = { NULL };

		if (STRING_Splitter(receiveBuffer, ",:", splittedTcpMsg, 2) == 2)
		{
			// Successful split

			uint32_t id;
			uint32_t messageLength;

			// 0. splitted string is ID
			if (StringToUnsignedDecimalNum((const char *)splittedTcpMsg[0], &id))
			{
				// ID converted successfully
				if (StringToUnsignedDecimalNum((const char *)splittedTcpMsg[1], &messageLength))
				{
					// Message length converted successfully

					// Create 3. string = message
					splittedTcpMsg[2] = splittedTcpMsg[1] + StringLength(splittedTcpMsg[1]) + 1;

					if (messageLength <= ESP8266_RX_TCP_MSG_MAX_LENGTH)
					{
						// Received correct length message (can be process it)
						size_t recvMsgLength = StringLength(splittedTcpMsg[2]);

						// For security
						static uint8_t waitCount = 0;

						if (messageLength < recvMsgLength)
						{
							// We have message source:
							ESP8266_DEBUG_PRINTF("Received TCP message (id: %d, length: %d): \"%s\"", id, messageLength, splittedTcpMsg[2]);

							// We need answer in good <id> !
							ESP8266_TcpMessageId = id;

			#ifdef CONFIG_MODULE_WEBPAGE_ENABLE
							if (ESP8266_SearchGetRequest(splittedTcpMsg[2]))
							{
								// Webpage request
								// Found, clear buffer
								isOk = true;
							}
							else
							{
			#endif
								// Received ~telnet command
								char responseBuffer[ESP8266_TCP_MESSAGE_MAX_LENGTH];
								// TODO: Use the global buffer immediately?

								// Execute the command
								CmdH_ExecuteCommand(splittedTcpMsg[2], responseBuffer, ESP8266_TCP_MESSAGE_MAX_LENGTH);

								ESP8266_RequestSendTcpMessage(responseBuffer);

								isOk = true;
			#ifdef CONFIG_MODULE_WEBPAGE_ENABLE
							}
			#endif

							ESP8266_ClearReceive(false,
									STRING_LENGTH("\r\n+IPD,")
									+ StringLength(splittedTcpMsg[0])
									+ 1
									+ StringLength(splittedTcpMsg[1])
									+ 1
									+ messageLength
									+ 1
									+ STRING_LENGTH("\r\nOK")
									+ 1); // This last is understand

							ESP8266_LOG_EVENT(EVENT_RECEIVED_GOOD_TCP_MESSAGE);

							waitCount = 0;
							TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 100);
						}
						else
						{
							ESP8266_DEBUG_PRINT("Not received all message yet. Wait...");
							isOk = true;
							TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 100);
							// Do not clear!

							// Security
							waitCount++;
							if (waitCount >= 10)
							{
								isOk = false;
							}
						}
					}
					else
					{
						// Received too large message
						ESP8266_DEBUG_PRINTF("Received too large TCP message: \"%s\"", receiveBuffer);
						ESP8266_ClearReceive(true, 0);
					}
				}
				else
				{
					ESP8266_DEBUG_PRINT("Error: TCP message has wrong length parameter");
				}
			}
			else
			{
				ESP8266_DEBUG_PRINT("Error: TCP message has wrong ID parameter");
			}
		}
		else
		{
			// Failed split
			ESP8266_DEBUG_PRINT("Received +IPD message split fail!");
			//ESP8266_RxBuffer_ReadCnt - do not clear
		}
	}
	else
	{
		// +IPD hasn't got finish "\r\n"
		ESP8266_DEBUG_PRINTF("Received +IPD message without end:\r\n\"%s\"", (const char *)receiveBuffer);
	}

	return isOk;
}



#ifdef CONFIG_MODULE_WEBPAGE_ENABLE
static bool ESP8266_SearchGetRequest(const char *recvMsg)
{
	bool isFound = false;
	const char *getString = STRING_FindString(recvMsg, "GET /");
	if (getString != NULL)
	{
		char responseBuffer[ESP8266_TX_BUFFER_LENGTH - ESP8266_TCP_MESSAGEHEADER_LENGTH];

		// Received "GET /...", it can be webpage request
		WebpageHandler_GetRequrest((const char *)&getString[STRING_LENGTH("GET /")], responseBuffer);

		// Send HTML webpage
		ESP8266_RequestSendTcpMessage(responseBuffer);

		// Close TCP connection
		//ESP8266_SendTcpClose(); // TODO: Only after sent...
		ESP8266_TcpCloseAfterSent = true;

		isFound = true;
	}

	return isFound;
}
#endif



#endif	// #ifdef CONFIG_MODULE_ESP8266_ENABLE
