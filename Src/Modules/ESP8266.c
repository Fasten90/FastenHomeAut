/*
 *		ESP8266.c
 *		Creat on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		ESP8266 communication
 *		Target:			STM32Fx
 *		Version:		v5
 *		Last modified:	2017-03-05
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "include.h"
#include <stdarg.h>		// For va_list
#include "String.h"
#include "CircularBuffer.h"
#include "TaskList.h"
#include "CommandHandler.h"
#include "DateTime.h"
#include "Globals.h"
#include "USART.h"

#include "ESP8266.h"


#ifdef CONFIG_MODULE_ESP8266_ENABLE

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

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
	Esp8266Status_ConfigCipMux,
	Esp8266Status_ConfigCipMuxCheckResponse,
	Esp8266Status_WaitAfterSuccessfulConfig,
#if CONFIG_ESP8266_IS_WIFI_HOST == 1
	Esp8266Status_StartWifiHost,
	Esp8266Status_StartWifiHostCheckResponse,
#else
	Esp8266Status_ConnectWifiNetwork,
	Esp8266Status_ConnectWifiNetworkCheckResponse,
	Esp8266Status_ConnectWifiNetworkCheckFinish,
#endif
	Esp8266Status_PrintMyIpAddress,
	Esp8266Status_IpAddressResponse,
#if CONFIG_ESP8266_IS_TCP_SERVER == 1
	Esp8266Status_StartTcpServer,
	Esp8266Status_StartTcpServerCheckResponse,
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

#ifdef ESP8266_USE_BLOCK_MODE
volatile uint8_t ESP8266_Uart_ReceivedCharFlag;
#endif

volatile char ESP8266_RxBuffer[ESP8266_RX_BUFFER_LENGTH];
char ESP8266_TxBuffer[ESP8266_TX_BUFFER_LENGTH];

// Receive Cnt
volatile uint8_t ESP8266_RxBuffer_WriteCnt = 0;
volatile uint8_t ESP8266_RxBuffer_ReadCnt = 0;

volatile bool ESP8266_SendEnable_flag = false;

// TCP message receive buffer
char ESP8266_TcpTransmitBuffer[ESP8266_TCP_MESSAGE_MAX_LENGTH];
char ESP8266_ReceivedTcpMessage[ESP8266_TCP_MESSAGE_MAX_LENGTH];

// TCP Message sending flags
bool ESP8266_TcpSendBuffer_EnableFlag = true;
bool ESP8266_TcpSendIsStarted_Flag = false;
bool ESP8266_TcpSent_WaitSendOk_Flag = false;



ESP8266_WifiConnectionStatusType	ESP8266_ConnectionStatus
			= ESP8266_WifiConnectionStatus_Unknown;

ESP8266_TcpConnectionStatusType		ESP8266_TcpConnectionStatus
			= ESP8266_TcpConnectionStatus_Unknown;


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


///< My IP address
Network_IP_t ESP8266_MyWifiIpAddress = { 0 };
Network_IP_t ESP8266_ExWifiIpAddress = { 0 };


///< Error counter
uint8_t ESP8266_ErrorCnt = 0;


///< Debug enable
bool ESP8266_DebugEnableFlag = 1;


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


#ifdef CONFIG_USE_FREERTOS
xSemaphoreHandle ESP8266_USART_Rx_Semaphore;
xQueueHandle ESP8266_SendMessage_Queue;	// LOG queue
xQueueHandle ESP8266_ReceivedMessage_Queue;
#endif


static ESP8266_StatusMachine_t ESP8266StatusMachine = Esp8266Status_Unknown;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_USE_FREERTOS
static bool ESP8266_ReceiveUnknownTcpMessage(void);
#endif


#ifdef NOT_USED
static bool ESP8266_CheckReceivedMessage(void);
#endif

#ifdef ESP8266_USE_BLOCK_MODE
static void ESP8266_WaitMessageAndCheckSendingQueue(void);
static void ESP8266_LoopSending(void);
static bool ESP8266_ConnectToServerInBlockingMode(void);
#endif

#ifdef ESP8266_USE_BLOCK_MODE
static bool ESP8266_SendTcpMessageBlockingMode(const char *message);
#else
static bool ESP8266_SendTcpMessageNonBlockingMode_Start(const char *message);
static bool ESP8266_SendTcpMessageNonBlockingMode_SendMessage(void);
#endif

static bool ESP8266_WaitForSend(uint16_t timeoutMilliSecond);

static bool ESP8266_ConvertIpString(char *message);

static void DebugPrint(const char *format, ...);

static void ESP8266_FindLastMessage(void);
static void ESP8266_ClearReceive(bool isFullClear, uint8_t stepLength);

static void ESP8266_CheckIdleStateMessage(char * receiveBuffer, uint8_t receivedMessageLength);

static void ESP8266_FirstStartReceive(void);

// TODO: Delete, if not need
#define ESP8266_StartReceive()		(void)0



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * \brief	Initialize ESP8266
 */
void ESP8266_Init(void)
{
	
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
	//USART_Init(&ESP8266_UartHandle);
	

	//DelayMs(100);

#ifdef CONFIG_USE_FREERTOS
	// ESP8266 FreeRTOS queues
	ESP8266_USART_Rx_Semaphore = xSemaphoreCreateBinary();
	ESP8266_SendMessage_Queue = xQueueCreate(
			ESP8266_SENDMESSAGE_QUEUE_LENGTH,
			ESP8266_MESSAGE_QUEUE_ITEM_SIZE);
	ESP8266_ReceivedMessage_Queue = xQueueCreate(
			ESP8266_RECEIVEMESSAGE_QUEUE_LENGTH,
			ESP8266_MESSAGE_QUEUE_ITEM_SIZE);
#endif
}



/**
 * \brief	Reset ESP8266 module on RST pin
 */
void ESP8266_ResetHardware(void)
{
	ESP8266_RST_ACTIVE();
	DelayMs(1);
	ESP8266_RST_INACTIVE();
}



/**
 * \brief	Send string to ESP8266
 */
void ESP8266_SendString(const char *str)
{
	uint8_t length = StringLength(str);

	if (length > ESP8266_TX_BUFFER_LENGTH)
	{
		length = ESP8266_TX_BUFFER_LENGTH;
	}

	if (ESP8266_WaitForSend(200))
	{
		// Copy if can send
		StrCpyFixLength(ESP8266_TxBuffer, str, length);

		if (HAL_UART_Transmit_IT(&ESP8266_UartHandle, (uint8_t*)ESP8266_TxBuffer, length) == HAL_OK)
		{
			ESP8266_SendEnable_flag = false;
		}
		else
		{
			Error_Handler();
		}
	}
	else
	{
		// Cannot send, because cannot take flag
		DebugPrint("Cannot take sending flag");
	}
}



/**
 * \brief	Wait for USART sending
 */
static bool ESP8266_WaitForSend(uint16_t timeoutMilliSecond)
{
	// Wait for flag or timeout
	while ((ESP8266_SendEnable_flag != true) || (timeoutMilliSecond == 0))
	{
		timeoutMilliSecond--;
		DelayMs(1);
	}

	// TODO: Not a good idea...
	//ESP8266_SendEnable_flag = true;

	return ESP8266_SendEnable_flag;
}



/**
 * \brief	Convert IP string to IP number
 */
static bool ESP8266_ConvertIpString(char *message)
{
	bool isOk = false;
	int16_t pos1;
#if CONFIG_ESP8266_IS_WIFI_HOST == 1
	int16_t pos2;
#endif

	// String come like "192.168.4.1\r\n192.168.1.34\r\n\r\nOK\r\n"
	pos1 = STRING_FindString(message, "\r\n");
	if (pos1 >= 0)
	{
#if CONFIG_ESP8266_IS_WIFI_HOST == 1
		message[pos1] = '\0';
		pos1 += 2;	// Skip "\r\n"
		pos2 = STRING_FindString(&message[pos1], "\r\n");
		if (pos2 >= 0)
		{
			message[pos1+pos2] = '\0';
			isOk = true;
		}
		// else: there is no ending "\r\n"
#else
		message[pos1] = '\0';
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
#if CONFIG_ESP8266_IS_WIFI_HOST == 1
		isOk &= Network_ConvertIpAddressStringToIP(&message[pos1], &ESP8266_ExWifiIpAddress);
#endif
		if (isOk)
		{
			char ipBuffer[80];
			ESP8266_PrintIpAddress(ipBuffer);
			DebugPrint(ipBuffer);
		}
	}

	return isOk;
}



/**
 * \brief	Print ESP8266 IP addresses
 */
uint8_t ESP8266_PrintIpAddress(char * str)
{
	uint8_t length = 0;

	length += StrCpy(&str[length], "MyWifi: ");
	length += Network_PrintIp(&str[length], &ESP8266_MyWifiIpAddress);
#if CONFIG_ESP8266_IS_WIFI_HOST == 1
	length += StrCpy(&str[length], "\r\nExWifi: ");
	length += Network_PrintIp(&str[length], &ESP8266_ExWifiIpAddress);
#endif
	length += StrCpy(&str[length], "\r\n");

	return length;
}



/**
 * \brief	ESP8266 debug print
 */
static void DebugPrint(const char *format, ...)
{
	if (ESP8266_DebugEnableFlag)
	{
		// Working in at:
		char txBuffer[ESP8266_DEBUG_TXBUFFERSIZE];

#ifdef CONFIG_DEBUG_MODE
		txBuffer[ESP8266_DEBUG_TXBUFFERSIZE-1] = 0xEF;
#endif

		va_list ap;									// argument pointer
		va_start(ap, format); 						// ap on arg
		string_printf(txBuffer, format, ap);		// Separate and process
		va_end(ap);						 			// Cleaning after end

#ifdef CONFIG_DEBUG_MODE
		if (txBuffer[ESP8266_DEBUG_TXBUFFERSIZE-1] != 0xEF) DEBUG_BREAKPOINT();
#endif

		DebugUart_SendMessage("ESP8266: ");
		DebugUart_SendMessage(TxBuffer);			// Send on DebugUart

	}
}



#ifdef CONFIG_USE_FREERTOS
/**
 * \brief	ESP8266 Task
 * \note	It will go infinite loop, does not return
 */
void ESP8266_Task(void)
{

	// First, need initialize ESP8266's pins, with	ESP8266_Init();
	// is in main.c


	// Delay, for hardware initializing
	DelayMs(10000);


	// Should init UART at very late (last minute),
	// because ESP8266 send lot of messages at startup and UART has error, and call the ErrorCallback
	USART_Init(&ESP8266_UartHandle);


	DelayMs(100);


	// Configure ESP8266
	while (!ESP8266_Config())
	{
		DebugPrint("Reconfigure ESP8266 module...\r\n");
		// Reinit ESP8266 - Restart it
		ESP8266_ResetHardware();
		DelayMs(10000);
		USART_Init(&ESP8266_UartHandle);
		UART_ResetStatus(&ESP8266_UartHandle);
	}

	// If reached this, ESP8266 configure is successful
	DebugPrint("Successful configured ESP8266\r\n");


	// Wait
	DelayMs(5000);


	// Connect to WiFi
	while (!ESP8266_ConnectToWifiNetwork())
	{
		DebugPrint("Failed connect to WiFi\r\n");
		DelayMs(10000);
	}

	// If reached this, successful connected
	DebugPrint("Successful connected to WiFi\r\n");


	// Delay
	DelayMs(1000);


	//	Infinite loop for task working ...


	while (1)
	{
		// CONNECT / START OF SERVER
		#if (CONFIG_ESP8266_IS_TCP_SERVER == 1)
		// Server mode
		if (ESP8266_ConnectionStatus != ESP8266_WifiConnectionStatus_SuccessfulServerStarted)
		{
			// Start server
			ESP8266_StartServerBlocking();

		}
		#elif (CONFIG_ESP8266_IS_TCP_SERVER == 0)
		// Client mode
		if (ESP8266_ConnectionStatus != ESP8266_WifiConnectionStatus_SuccessfulConnected)
		{
			// TODO: Find server

			// Connect to server
			ESP8266_ConnectToServerInBlockingMode();

		}
		#else
		#error "CONFIG_ESP8266_IS_TCP_SERVER define isnt correct."
		#endif


		// END OF Connect to server or start server

#ifdef CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE
		// Send Login message
		// TODO: Beautify
		HomeAutMessage_CreateAndSendHomeAutMessage(
			&ESP8266_MyIpAddress,
			(Network_IP_t *)&ESP8266_ServerAddress,
			(DateTime_t *)&EventDateTime,
			Function_Login,
			Login_ImLoginImDiscovery,
			0);

		DelayMs(1000);
#endif

		//	Sending infinite loop
		ESP8266_LoopSending();


		/////////////////////////////////////
		// Receive any message + Sending:
		/////////////////////////////////////
		//ESP8266_ReceiveUnknownTcpMessage();


		/////////////////////////////////////
		//		Check received message
		/////////////////////////////////////

		ESP8266_CheckReceivedMessage();

	}
	// END OF while(1)


}
#endif	// #ifdef CONFIG_USE_FREERTOS



#ifdef ESP8266_USE_BLOCK_MODE
/**
 * \brief	Configure ESP8266
 */
bool ESP8266_Config(void)
{

	//////////////////////
	// "ATE0"
	// Echo off

	ESP8266_ReceiveString(StringLength("ATE0\r\r\n\r\nOK\r\n"));

	ESP8266_SendString("ATE0\r\n");

	ESP8266_WaitAnswer(1000);

	if (!StrCmpFirst("ATE0\r\r\n\r\nOK\r\n",(const char *)ESP8266_RxBuffer))
	{
		// Ok
		ESP8266_LED_OK();
	}
	else
	{
		ESP8266_LED_FAIL();
		return false;
	}



	//////////////////////
	// "AT"
	// Check it is alive?

	ESP8266_ReceiveString(StringLength("\r\nOK\r\n"));
	
	ESP8266_SendString("AT\r\n");

	ESP8266_WaitAnswer(1000);

	
	if (!StrCmpFirst("\r\nOK\r\n",(const char *)ESP8266_RxBuffer))
	{
		ESP8266_LED_OK();
	}
	else
	{
		ESP8266_LED_FAIL();
		return false;
	}
	
	
	DelayMs(1000);
		


	/////////////////
	// CWMODE
	// 1 = station
	// 2 = host
	// 3 = dual
	/////////////////
	
	ESP8266_ReceiveString(StringLength("no change\r\n"));

	ESP8266_SendString("AT+CWMODE=3\r\n");

	ESP8266_WaitAnswer(1000);
	
	if (!StrCmpFirst("OK\r\n",(const char *)ESP8266_RxBuffer))
	{
		// "OK"
		ESP8266_LED_OK();
	}
	else if (!StrCmpFirst("no change\r\n",(const char *)ESP8266_RxBuffer))
	{
		// "no change"
		ESP8266_LED_OK();
	}
	else 
	{
		// Other... it is wrong
		ESP8266_LED_FAIL();
		return false;
	}
	
	
	return true;
}
#endif	// #ifdef ESP8266_USE_BLOCK_MODE



#ifdef ESP8266_USE_BLOCK_MODE
/**
 * \brief	Connect to Wifi network
 */
bool ESP8266_ConnectToWifiNetwork(void)
{
	
	
	/////////////////////////
	// Join to WiFi network
	// AT+CWJAP
	// Syntax: AT+CWJAP="networkname","password"
	/////////////////////////
	
	
	ESP8266_ReceiveString(StringLength("\r\nOK\r\n"));
	
	ESP8266_SendString("AT+CWJAP=\""
			CONFIG_ESP8266_WIFI_NETWORK_NAME
			"\",\""
			CONFIG_ESP8266_WIFI_NETWORK_PASSWORD
			"\"\r\n");
	
	ESP8266_WaitAnswer(20000);
	
	if (!StrCmpFirst("\r\nOK\r\n", (const char *)ESP8266_RxBuffer))
	{
		// "OK"
		ESP8266_LED_OK();
	}
	else if (!StrCmpFirst("\r\nFAIL", (const char *)ESP8266_RxBuffer))
	{
		// "FAIL"
		// AT+CWJAP=\"networkname\",\"password\"\r\r\n\r\nFAIL\r\n")
		ESP8266_LED_FAIL();
		return false;
	}
	else if (!StrCmpFirst("\r\nERRO", (const char *)ESP8266_RxBuffer))
	{
		// "ERROR"
		// AT+CWJAP=\"networkname\",\"password\"\r\r\n\r\nERROR\r\n")
		ESP8266_LED_FAIL();
		return false;
	}
	else
	{
		// Received unknown message
		ESP8266_LED_FAIL();
		return false;
	}
	
	
	
	///////////////////////////////////////////////
	// AT+CIPMUX = 	set multiple connections mode
	// Connection Type:
	// AT+CIPMUX=0		// Single
	// AT+CIPMUX=1		// Multiple
	//////////////////////////////////////////////
		
	ESP8266_ReceiveString(StringLength("\r\nOK\r\n"));
	
	ESP8266_SendString("AT+CIPMUX=1\r\n");
	
	ESP8266_WaitAnswer(2000);
	
	if (!StrCmpFirst("\r\nOK\r\n", (const char *)ESP8266_RxBuffer))
	{
		ESP8266_LED_OK();
	}
	else
	{
		ESP8266_LED_FAIL();
		return false;
	}
	
	
	////////////////////////////////////////////
	// Save IP address to ESP8266_MyIpAddressString
	// AT+CIFSR
	// IP address
	// Command:
	// AT+CIFSR
	// Response:	AT+CIFSR 192.168.0.106\r\nOK
	////////////////////////////////////////////
	
	// TODO: Wait unknown length string (IP address length is not fix...
	ESP8266_ReceiveString(StringLength("192.168.0.1\r\n192.168.1.34\r\n\r\nOK\r\n"));

	ESP8266_SendString("AT+CIFSR\r\n");
	
	ESP8266_WaitAnswer(2000);
	
	// Check and convert IP address
	if (ESP8266_ConvertIpString((char *)ESP8266_RxBuffer, &ESP8266_MyIpAddress))
	{
		DebugPrint("Successful convert IP address\r\n");
	}
	else
	{
		DebugPrint("Failed convert IP address\r\n");
	}

	
	// Print my IP address
	if (ESP8266_DebugEnableFlag)
	{
		Network_PrintIpOnDebug("ESP8266",
				&ESP8266_MyIpAddress);
	}
	
	
	// TODO: Check this commented section
	/*
	#ifdef CONFIG_ESP8266_IS_TCP_SERVER
	
	// AT+CWDHCP - Enable/Disable DHCP
	// Set 	AT+CWDHCP=mode,en 	OK
	
	// Parameter Description
	// mode
	// 0 : set ESP8266 softAP
	// 1 : set ESP8266 station
	// 2 : set both softAP and station
	// en
	// 0 : Enable DHCP
	// 1 : Disable DHCP 

	// AT+CWDHCP=1,0\r\n
	// mode = 1, dhcp = 0 = disable
	ESP8266_ReceiveString((unsigned char *)ESP8266_RxBuffer,StringLength((uint8_t *)"AT+CWDHCP=1,0\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString((uint8_t *)"AT+CWDHCP=1,0\r\n");
	
	ESP8266_WaitAnswer();
	
	if (!StrCmpFirst("AT+CWDHCP=1,0\r\r\n\r\nOK\r\n",(const char *)ESP8266_RxBuffer))
	{
		// Ok
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		// Fail
		LED_GREEN_OFF();
		LED_RED_ON();
	}	
	
	

	// Set fix IP
	///////////////////////////////
	//	AT+CIPSTA	set IP address of STAtion
	//	AT+CIPSTA=<ip>
	///////////////////////////////
	
	ESP8266_ReceiveString((unsigned char *)ESP8266_RxBuffer,
	StringLength((uint8_t *)"AT+CIPSTA=\"9.6.5.14\"\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString((uint8_t *)"AT+CIPSTA=\"9.6.5.14\"\r\n");
	
	ESP8266_WaitAnswer();
	
	if (!StrCmpFirst("AT+CIPSTA=\"9.6.5.14\"\r\r\n\r\nOK\r\n",(const char *)ESP8266_RxBuffer))
	{
		// Ok
		ESP8266_LED_OK();
	}
	else
	{
		// Fail
		ESP8266_LED_FAIL();
	}
	
	#endif
	*/
	
	
	return true;
}
#endif	// #ifdef ESP8266_USE_BLOCK_MODE



#ifdef ESP8266_USE_BLOCK_MODE
/**
 * \brief	TODO: Implement this function
 */
bool ESP8266_FindServer ( void )
{
	
	// TODO: Not used... check it

	
	////////////////////////
	//	Connected devices:
	//	AT+CWLIF
	////////////////////////
		
	ESP8266_ReceiveString(StringLength("AT+CWLIF\r\r\n\r\nOK\r\nLinked\r\n"));
	
	ESP8266_SendString("AT+CWLIF\r\n");
	
	ESP8266_WaitAnswer(5000);
	
	// TODO: Delete this parameter
	if (!StrCmpFirst("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			(const char *)ESP8266_RxBuffer))
	{
		// OK\r\nLinked
		ESP8266_LED_OK();
		return true;
	}
	else
	{
		// ERROR\r\nUnli [nk]
		ESP8266_LED_FAIL();
		return false;
	}
	
	//return true;
}
#endif



#if defined(ESP8266_USE_BLOCK_MODE) &&(CONFIG_ESP8266_IS_TCP_SERVER == 0)
/**
 * \brief	Connect ESP8266 to server
 */
bool ESP8266_ConnectToServer(Network_IP_t *ip, Network_Port_t port)
{
	
	////////////////////////////////////////
	// AT+CIPSTART=?
	// (CIPMUX=0) AT+CIPSTART = <type>,<addr>,<port>
	// (CIPMUX=1) AT+CIPSTART=  <id><type>,<addr>, <port>
	// AT+CIPSTART=4,"TCP","google.com",80
	//	Now: AT+CIPSTART=0,"TCP","9.6.5.14",2000
	//		id=0, TCP, 9.6.5.14, port:2000
	//
	// Response:
	// OK
	// Linked
	////////////////////////////////////////
	
	ESP8266_ReceiveString(StringLength("\r\nOK\r\nLinked\r\n"));
	
	// Original string: "AT+CIPSTART=0,\"TCP\",\"192.168.1.62\",2000\r\n"
	//ESP8266_SendString("AT+CIPSTART=0,\"TCP\",\"192.168.1.62\",2000\r\n");
	char buffer[50];
	uint8_t length = 0;
	length += usprintf(buffer, "AT+CIPSTART=0,\"TCP\",\"");
	length += Network_PrintIp(&buffer[length], ip);
	length += usprintf(&buffer[length], "\",%d\r\n", port);
	ESP8266_SendString(buffer);

	ESP8266_WaitAnswer(5000);
	
	if (!StrCmpFirst("\r\nOK\r\nLinked\r\n", (const char *)ESP8266_RxBuffer))
	{
		// OK\r\nLinked
		ESP8266_LED_OK();
		return true;
	}
	if (!StrCmpFirst("ALREADY CONNECT", (const char *)ESP8266_RxBuffer))
	{
		// "ALREADY CONNECT"
		ESP8266_LED_OK();
		return true;
	}
	else
	{
		// ERROR\r\nUnli [nk]
		ESP8266_LED_FAIL();
		return false;
	}
	
}
#endif



#if defined(ESP8266_USE_BLOCK_MODE) && (CONFIG_ESP8266_IS_TCP_SERVER == 1)
/**
 * \brief	Start ESP8266 server
 */
bool ESP8266_StartServer ( void )
{
	
	////////////////////////////////////////
	//		AT+CIPSERVER
	//		set as server/listen()	
	//		AT+CIPSERVER=1,2000
	//
	//		Response:
	//		OK
	//		Linked
	////////////////////////////////////////
	
	
	ESP8266_ClearFullReceiveBuffer();
		
	ESP8266_ReceiveString(StringLength("AT+CIPSERVER=1,2000\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString("AT+CIPSERVER=1,2000\r\n");
	
	ESP8266_WaitAnswer();
	
	
	if (!StrCmpFirst("AT+CIPSERVER=1,2000\r\r\n\r\nOK\r\n",(const char *)ESP8266_RxBuffer))
	{
		// OK
		ESP8266_LED_OK();
		return true;
	}
	else
	{
		// ERROR
		ESP8266_LED_FAIL();
		return false;
	}
	
	// Not reached
	//return true;
}
#endif



#ifdef ESP8266_USE_BLOCK_MODE
/**
 * \brief	Sending HomeAutMessage
 *			for example:
 *			|HomeAut|010|014|LOGIN__|NMEDIU00000000|
 *			length: 40
 */
static bool ESP8266_SendTcpMessageBlockingMode(const char *message)
{
	uint8_t length = StringLength(message);
	char buffer[25]; // For "AT+CIPSEND=0,40\r\n"
	
	// Check length
	if (length >= ESP8266_TCP_MESSAGE_MAX_LENGTH)
	{
		// Wrong length
		DebugPrint("Too long message!\r\n");
		length = ESP8266_TCP_MESSAGE_MAX_LENGTH - 1;
	}
	

	///////////////////////////////////////////////
	//	Send data
	//	AT+CIPSEND
	//	1)single connection(+CIPMUX=0) AT+CIPSEND=<length>;
	//	2) multiple connection (+CIPMUX=1) AT+CIPSEND= <id>,<length>
	//	
	//	AT+CIPSEND=4,18
	//	18 byte to send: GET / HTTP/1.0\r\n\r\n
	//
	// Response:
	// SEND OK
	///////////////////////////////////////////////
	
	ESP8266_ReceiveString(StringLength("> "));
	
	// Send ~ "AT+CIPSEND=0,40\r\n"
	usprintf(buffer, "AT+CIPSEND=0,%d\r\n", length);
	ESP8266_SendString(buffer);
	
	ESP8266_WaitAnswer(5000);
	
	if (!StrCmpFirst("> ", (const char *)ESP8266_RxBuffer))
	{
		ESP8266_LED_OK();
	}
	else if (!StrCmpFirst("link is not\r\n", (const char *)&ESP8266_RxBuffer[1]))
	{
		ESP8266_LED_FAIL();
		return false;
	}
	else
	{
		ESP8266_LED_FAIL();
		return false;
	}
	
	
	/////////////////////////
	// Example:
	// Send
	// "GET / HTTP/1.0\r\n\r\n"
	// Response:
	// "SEND OK"

	// Send message and wait response
	ESP8266_ReceiveString(StringLength("\r\nSEND OK\r\n"));
	
	ESP8266_SendString(message);

	ESP8266_WaitAnswer(5000);
	
	if (!StrCmpFirst("\r\nSEND OK\r\n", (const char *)ESP8266_RxBuffer))
	{
		ESP8266_LED_OK();
	}
	else
	{
		ESP8266_LED_FAIL();
		return false;
	}
	
	return true;
}
#endif



#if NOT_USED
/**
 *\brief		Receive HomeAutMessage
 *				length: 10+40
 *				example: "+IPD,0,40:" + "|HomeAut|010|014|LOGIN__|NMEDIU00000000|"
 */
bool ESP8266_ReceiveFixTcpMessage(void)
{

	
	ESP8266_ClearFullReceiveBuffer();
	
	// Old:
	// +IPD,0,18:GET / HTTP/1.0
	// +IPD,0,18:<message>
	
	// HomeAutMessage:
	// "\r\n"											2
	// "+IPD,0,40:"										10
	// "|HomeAut|010|014|LOGIN__|NMEDIU00000000|"		x
	// "\r\nOK\r\n"										6
	// length: 2+10+40+6 = 58
	ESP8266_ReceiveString(ESP8266_RECEIVEMESSAGE_MAX_LENGTH);
		
	return true;
}
#endif



#ifdef CONFIG_USE_FREERTOS
/**
 * \brief	Switch to unknown length Receive mode
 */
static bool ESP8266_ReceiveUnknownTcpMessage(void)
{
	// In uart IT: receive character by once
	ESP8266_Receive_Mode_FixLength = 0;
	
	// Clear buffer
	ESP8266_ClearFullReceiveBuffer();
	
	// BufferCnt = 0;
	ESP8266_RxBuffer_WriteCnt = 0;
	
	// Start receive (wait 1 character)
	HAL_UART_Receive_IT(&ESP8266_UartHandle, (uint8_t *)&ESP8266_RxBuffer[0], 1);

	// Wait message
	//ESP8266_WaitAnswer();	// Only wait ...
	ESP8266_WaitMessageAndCheckSendingQueue();		// wait for received message or if have to sending message
	
	return true;
}
#endif



#ifdef ESP8266_USE_BLOCK_MODE
/**
 *\brief		Wait for answer in blocking mode
 */
static void ESP8266_WaitAnswer(uint32_t timeout)
{
	#ifdef CONFIG_USE_FREERTOS
#if OLD_BLOCK
	while (1)
	{
		if (xSemaphoreTake(ESP8266_USART_Rx_Semaphore, (portTickType) timeout) == pdPASS)
		{
			// Successful take, return
			return;
		}
	}
#else
	xSemaphoreTake(ESP8266_USART_Rx_Semaphore, (portTickType) timeout);
#endif

	#else
	(void)timeout;
	// TODO: ...
	while (ESP8266_Uart_ReceivedCharFlag != 1);
	#endif
	
}
#endif



#ifdef CONFIG_USE_FREERTOS
/**
 * \brief		Wait receive message, or transmit queue...
 */
static void ESP8266_WaitMessageAndCheckSendingQueue(void)
{

	// Wait for message receiving or queue has a message for sending
	while (1)
	{
		
		// Received an ended string?
		if (xSemaphoreTake(ESP8266_USART_Rx_Semaphore, (portTickType) 500) == pdPASS)
		{
			// Received a message
			return;
		}
		// If not received a lot of char...
		else if (ESP8266_RxBuffer_WriteCnt < 2)
		{
			// Need to sending?
			if (xQueueReceive(ESP8266_SendMessage_Queue,
					(void *)ESP8266_TcpTransmitBuffer,
					(portTickType)0) == pdPASS)
			{
				// Sending...
				// TODO: Csinálni egy receive mode váltás függvényt!!
				ESP8266_Receive_Mode_FixLength = 1;	// fix length, because receiving "> "
				ESP8266_RxBuffer_WriteCnt = 0;
				ESP8266_TcpTransmitBuffer[40] = '\0';	// end of buffer need to put an end char
				
				// Clear UART states
				UART_ResetStatus(&ESP8266_UartHandle);
				
				// Sending
				if (ESP8266_SendTcpMessageBlockingMode((char *)ESP8266_TcpTransmitBuffer))
				{
					if (ESP8266_DebugEnableFlag)
					{
						uprintf("Successful sent a message:\r\n"
								"  %s\r\n",
								(const char *)ESP8266_TcpTransmitBuffer);
					}
				}
				else
				{	
					if (ESP8266_DebugEnableFlag)
					{
						uprintf("Failed sending message:\r\n"
								"  %s\r\n",
								(const char *)ESP8266_TcpTransmitBuffer);
					}
				}

				
				// Reset the receive buffer:
				ESP8266_RxBuffer_WriteCnt = 0;
				ESP8266_Receive_Mode_FixLength = 0;

				// Start receive
				HAL_UART_Receive_IT(&ESP8266_UartHandle, (uint8_t *)ESP8266_RxBuffer, 1);
			}
		
		}
		else
		{
			// Received some characters, we should check messages, after it has been received
			// Do nothing
		}
	}
}



/**
 * \brief	Check sending queue and send messages on TCP
 * \note	Be careful, it is infinite loop, never exit
 */
static void ESP8266_LoopSending(void)
{
	while(1)
	{
		// Need to sending?
		if (xQueueReceive(ESP8266_SendMessage_Queue,
				(void * )ESP8266_TcpTransmitBuffer,
				( portTickType )0 ) == pdPASS)
		{

			// Sending
			if (ESP8266_SendTcpMessageBlockingMode((char *)ESP8266_TcpTransmitBuffer))
			{
				// Successful sent
				if (ESP8266_DebugEnableFlag)
				{
					uprintf("Successful sent a message:\r\n"
							"  %s\r\n",
							(const char *)ESP8266_TcpTransmitBuffer);
				}
			}
			else
			{
				// Failed send
				if (ESP8266_DebugEnableFlag)
				{
					uprintf("Failed sending message:\r\n"
							"  %s\r\n",
							(const char *)ESP8266_TcpTransmitBuffer);
				}
			}
		}
	}
}



/**
 * \brief	Put message to sending queue
 */
bool ESP8266_SendMessageToQueue(char *message)
{
	// Sending
	if (ESP8266_SendMessage_Queue != 0)
	{

		if (xQueueSend(ESP8266_SendMessage_Queue, ( void * )message, ( portTickType ) 1000) == pdPASS)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}



/**
 *\brief		Wait for client connect
 */
#if 0
// TODO: delete
bool ESP8266_WaitClientConnect( void)
{
	
	ESP8266_ClearFullReceiveBuffer();
	
	// Link\r\n
	// \r\n
	ESP8266_ReceiveString(StringLength("Link\r\n") );
		
	// wait for message
	ESP8266_WaitAnswer();
	
	return true;
}
#endif



#if (CONFIG_ESP8266_IS_TCP_SERVER == 0)
/**
 * \brief	Connect to server, with blocking
 */
static bool ESP8266_ConnectToServerInBlockingMode(void)
{

	bool successfulConnected = false;
	
	while (!successfulConnected)
	{
		// Connect to server
		successfulConnected = ESP8266_ConnectToServer(
				(Network_IP_t *)&ESP8266_ServerAddress,
				ESP8266_ServerPort);
	
		if (successfulConnected)
		{
			// Successful connected
			ESP8266_ConnectionStatus = ESP8266_WifiConnectionStatus_SuccessfulConnected;
			DebugPrint("Successful connected to server\r\n");
		}
		else
		{
			// Failed to connect
			DebugPrint("Failed connected to server\r\n");
			DelayMs(10000);
			DebugPrint("Retry to connect...\r\n");
		}	
	}
	
	// Successful Connected ...
	
	return true;
}
#endif



#if (CONFIG_ESP8266_IS_TCP_SERVER == 1)
bool ESP8266_StartServerBlocking(void)
{
	// Start server
	while (!ESP8266_StartServer())
	{
		DebugPrint("Error! Server isn't started\r\n");
		DelayMs(10000);
	}
	
	ESP8266_ConnectionStatus = ESP8266_WifiConnectionStatus_SuccessfulServerStarted;
	DebugPrint("Server has started\r\n");

	return true;
	
}
#endif
#endif	// CONFIG_USE_FREERTOS



#ifdef NOT_USED
/**
 * \brief	Check received a new TCP message
 */
static bool ESP8266_CheckReceivedMessage(void)
{

	bool isValidMessage;


	// Print received message:
	DebugPrint("Received a message\r\n");


	if (ESP8266_RxBuffer_WriteCnt >= ESP8266_RECEIVEMESSAGE_MIN_LENGTH)
	{

		// TODO: Change DebugPrint-s
		// TODO: Modify separated good message......
		DebugPrint("Valid HomeAut message received:\r\n"
					"%s",
					(char *)ESP8266_RxBuffer
					);

#ifdef CONFIG_USE_FREERTOS
		if (xQueueSend(ESP8266_ReceivedMessage_Queue,
				(const void *)ESP8266_RxBuffer,
				1000) != pdPASS)
		{
			// Successful sent to queue
			isValidMessage = true;
		}
		else
		{
			// Failed sent to queue
			DebugPrint("Message failed to sending queue\r\n");
			isValidMessage = false;
		}
#endif
	}
	// Check for unconnect, or other messages...
	else if (!StrCmpFirst("Link", (const char *)ESP8266_RxBuffer))
	{
		// Received: "Link"
		isValidMessage = true;

		#if (CONFIG_ESP8266_IS_TCP_SERVER == 1)
		// TODO: Process connecting, add new IP to list
		DebugPrint("A client connected.\r\n");
		#else
		DebugPrint("Received \"Link\". This device is in client mode, dont understand it.\r\n");
		#endif
	}
	else if (!StrCmpFirst("Unlink", (const char *)ESP8266_RxBuffer))
	{
		// Received: "Unlink"
		isValidMessage = true;

		// TODO: Process disconnecting, delete IP from list
		#if (CONFIG_ESP8266_IS_TCP_SERVER == 1)
		DebugPrint("A client disconnected.\r\n");
		#else
		DebugPrint("Disconnected. Need to reconnect\r\n");
		ESP8266_ConnectionStatus = ESP8266_WifiConnectionStatus_ClosedConnection;
		#endif
	}
	else if (!StrCmpFirst("OK", (const char *)ESP8266_RxBuffer))
	{
		// Received: "OK"
		// Note: This arrived after received an x length message (now, after 40 length homeautmessage)
		isValidMessage = true;
		DebugPrint("Received an OK\r\n");
	}
	else if (!StrCmpFirst("", (const char *)ESP8266_RxBuffer))
	{
		// Received: ""
		// Note: This arrived after received an x length message (now, after 40 length homeautmessage) + "\r\n" after ""
		isValidMessage = true;
		DebugPrint("Received an empty string\r\n");
	}
	else
	{
		// Not valid message
		isValidMessage = false;
		DebugPrint("Received not valid message: %s\r\n", (char *)&ESP8266_RxBuffer[0]);
	}


	// Received an correct message?
	if (isValidMessage)
	{
		ESP8266_LED_OK();
	}
	else
	{
		ESP8266_LED_FAIL();
	}

	return isValidMessage;
}
#endif



#ifdef NOT_USED
// TODO: Delete, because this unfix message waiting is wrong idea
/**
 * \brief	Wait/Receive string
 * \note	It wait fix length
 */
void ESP8266_ReceiveString(uint8_t length)
{
	// Clear buffer
	ESP8266_ClearFullReceiveBuffer();

	// Clear flag
#ifdef CONFIG_USE_FREERTOS
	xSemaphoreTake(ESP8266_USART_Rx_Semaphore, 0);
#else
	ESP8266_Uart_ReceivedCharFlag = 0;
#endif

	// Receive
	HAL_UART_Receive_IT(&ESP8266_UartHandle, (uint8_t *)ESP8266_RxBuffer, length);
}
#endif



/**
 * \brief	Request send TCP message
 */
uint8_t ESP8266_RequestSendTcpMessage(const char * message)
{
	uint8_t length = 0;

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
		DebugPrint("Cannot request TCP message, it is disabled\r\n");
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
	 * 1)single connection(+CIPMUX=0) AT+CIPSEND=<length>;
	 * 2) multiple connection (+CIPMUX=1) AT+CIPSEND= <id>,<length>
	 *
	 * AT+CIPSEND=4,18
	 * 18 byte to send: GET / HTTP/1.0\r\n\r\n
	 *
	 * Response:
	 * 	SEND OK
	 */

	uint8_t length = StringLength(message);
	char buffer[25]; // For "AT+CIPSEND=0,40\r\n"

	// Check length
	if (length >= ESP8266_TCP_MESSAGE_MAX_LENGTH)
	{
		// Wrong length
		DebugPrint("Too long message!\r\n");
		length = ESP8266_TCP_MESSAGE_MAX_LENGTH - 1;
	}


	// Send ~ "AT+CIPSEND=0,40\r\n"
	usprintf(buffer, "AT+CIPSEND=0,%d\r\n", length);
	ESP8266_SendString(buffer);

	ESP8266_TcpSendIsStarted_Flag = true;

	return true;
}



/**
 * \brief	Send TCP message which are in ESP8266_TcpTransmitBuffer
 * \note	Recommend received "> " before this function
 */
static bool ESP8266_SendTcpMessageNonBlockingMode_SendMessage(void)
{

	// Check length
	uint8_t length = StringLength((const char *)ESP8266_TcpTransmitBuffer);

	if (length >= ESP8266_TCP_MESSAGE_MAX_LENGTH)
	{
		// Wrong length
		DebugPrint("Too long message!\r\n");
		// TODO: Use length or delete!
		length = ESP8266_TCP_MESSAGE_MAX_LENGTH - 1;
	}

	/*
	 * Example:
	 * Send:
	 * "GET / HTTP/1.0\r\n\r\n"
	 * Response:
	 * "SEND OK"
	 */

	// Send message and wait response
	ESP8266_SendString(ESP8266_TcpTransmitBuffer);

	DebugPrint("Send TCP message: \"%s\"\r\n", ESP8266_TcpTransmitBuffer);

	return true;
}



/**
 * \brief	ESP8266 receive-sending full reinitialize
 */
static void ESP8266_FirstStartReceive(void)
{
	// Clear buffer
	ESP8266_ClearFullReceiveBuffer();

	// BufferCnt = 0;
	ESP8266_RxBuffer_WriteCnt = 0;
	ESP8266_RxBuffer_ReadCnt = 0;

	UART_ResetStatus(&ESP8266_UartHandle);

	ESP8266_SendEnable_flag = true;

	// Start receive (wait x character)
	HAL_UART_Receive_IT(&ESP8266_UartHandle,
			(uint8_t *)&ESP8266_RxBuffer[0],
			ESP8266_RECEIVE_LENGTH);
}



/**
 * \brief	Reset ESP8266 received buffer
 */
void ESP8266_ClearFullReceiveBuffer(void)
{
	uint16_t i;
	for (i = 0; i < ESP8266_RX_BUFFER_LENGTH; i++)
	{
		ESP8266_RxBuffer[i] = '\0';
	}
}



/**
 * \brief	Step Buffer WriteCnt to last character
 */
static void ESP8266_FindLastMessage(void)
{
	ESP8266_RxBuffer_WriteCnt = ESP8266_RX_BUFFER_LENGTH - ESP8266_UartHandle.RxXferCount;
	// TODO: Buffer Overflow



	// TODO: Not a good solve...
	//
	/*
	uint16_t i = 0;

	// Find last character in the buffer
	while (ESP8266_RxBuffer[ESP8266_RxBuffer_WriteCnt])
	{
		++ESP8266_RxBuffer_WriteCnt;
		++i;

		if ((i > ESP8266_RX_BUFFER_LENGTH) || (((ESP8266_RX_BUFFER_LENGTH - ESP8266_UartHandle.RxXferCount) - ESP8266_RxBuffer_WriteCnt) > 100))
		{
			// Buffer full - Error
			DebugPrint("Error: Buffer full, clear it...\r\n");

			ESP8266_ClearFullReceiveBuffer();
			//ESP8266_RxBuffer_WriteCnt = 0;
			//ESP8266_RxBuffer_ReadCnt = 0;
			ESP8266_RxBuffer_WriteCnt = ESP8266_UartHandle.RxXferCount;
			ESP8266_RxBuffer_ReadCnt = ESP8266_RxBuffer_WriteCnt;
			//ESP8266_ClearReceive(true, 0);	// TODO: Not enough, clear all buffer
			//ESP8266_RxBuffer_ReadCnt = ESP8266_RxBuffer_WriteCnt;
			// TODO: Need restore writeCnt
			break;
		}
	}//
	*/
}



/**
 * \brief	Clear receive buffer
 */
static void ESP8266_ClearReceive(bool isFullClear, uint8_t stepLength)
{
	ESP8266_FindLastMessage();

	// Clear from ReadCnt to WriteCnt
	if (isFullClear)
	{
		// Clear all buffer
		CircularBuffer_Clear((char *)ESP8266_RxBuffer, ESP8266_RX_BUFFER_LENGTH,
				ESP8266_RxBuffer_ReadCnt,
				ESP8266_RxBuffer_WriteCnt);
		ESP8266_RxBuffer_ReadCnt = ESP8266_RxBuffer_WriteCnt;
	}
	else
	{
		// Not full clear from readCnt to writeCnt
		CircularBuffer_Clear((char *)ESP8266_RxBuffer, ESP8266_RX_BUFFER_LENGTH,
				ESP8266_RxBuffer_ReadCnt,
				(uint8_t)(ESP8266_RxBuffer_ReadCnt + stepLength));

		// Increment ReadCnt
		if (ESP8266_RxBuffer_ReadCnt <  ESP8266_RxBuffer_WriteCnt)
		{
			ESP8266_RxBuffer_ReadCnt += stepLength;
			if (ESP8266_RxBuffer_ReadCnt > ESP8266_RxBuffer_WriteCnt)
			{
				ESP8266_RxBuffer_ReadCnt = ESP8266_RxBuffer_WriteCnt;
			}
		}
		else if (ESP8266_RxBuffer_ReadCnt >  ESP8266_RxBuffer_WriteCnt)
		{
			ESP8266_RxBuffer_ReadCnt += stepLength;
		}
	}
}



#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
/**
 * \brief	ESP8266 state machine
 * \note	Call this task a lot of times (it is not az infinite loop)
 */
void ESP8266_StatusMachine(void)
{
	// TODO: Delete ESP8266_StartReceive();

	ESP8266_FindLastMessage();

	// If WriteCnt not equal with ReadCnt, we have received message
	char receiveBuffer[ESP8266_RX_BUFFER_LENGTH+1] = { 0 };
	uint16_t receivedMessageLength = 0;

	if (ESP8266_RxBuffer_WriteCnt != ESP8266_RxBuffer_ReadCnt)
	{
		// Need copy to receiveBuffer
		receivedMessageLength = CircularBuffer_GetCharacters(
				(char *)ESP8266_RxBuffer, receiveBuffer,
				ESP8266_RX_BUFFER_LENGTH,
				ESP8266_RxBuffer_WriteCnt, ESP8266_RxBuffer_ReadCnt,
				true);
	}


#if ESP8266_DEBUG_MODE == 1
	// Print all received chars:
	{
		static uint8_t RxPrintCnt = 0;
		if (RxPrintCnt != ESP8266_UartHandle.RxXferCount)
		{
			DebugUart_Printf("ESP8266 Received: %s", receiveBuffer);
			RxPrintCnt = ESP8266_UartHandle.RxXferCount;
		}
	}
#endif


	// Check ESP8266 status machine state
	switch (ESP8266StatusMachine)
	{

		case Esp8266Status_Unknown:
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Unknown");
			// break;	// Step to next

		case Esp8266Status_Init:
			//HAL_UART_DeInit(&ESP8266_UartHandle);
			ESP8266_ResetHardware();
			ESP8266StatusMachine++;
			ESP8266_ConnectionStatus = ESP8266_WifiConnectionStatus_Unknown;
			// Wait 10sec
			// Disable event
			//TaskHandler_DisableTask(Task_Esp8266);
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 5000);
			ESP8266_DEBUG_PRINT("Init");
			break;

		case Esp8266Status_AfterInit:
			// When we reach this state, we wait for ESP8266's startup
			USART_Init(&ESP8266_UartHandle);
			ESP8266_FirstStartReceive();
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
				//break;	// Step to next
			}
			else
			{
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_Init;
				ESP8266_DEBUG_PRINTF("Config ATE0 response failed, received message: \"%s\"\r\n",
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
				//break;	// Step to next
			}
			else
			{
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_Init;
				ESP8266_DEBUG_PRINTF("Config AT failed, received message: \"%s\"\r\n",
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
				|| (!StrCmpFirst("no change\r\n", (const char *)receiveBuffer)))
			{
				// "OK"
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Config CWMODE successful");
				//break;	// Step to next
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
				//break;	// Step to next
			}
			else if (!StrCmpFirst("no change\r\n",(const char *)receiveBuffer))
			{
				// "no change"
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Config CWDHCP response ok");
				//break;	// Step to next
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
				// TODO: "link is builded\r\n" message is not correct, but it is good for me
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Config CIPMUX response ok");
				//break;	// Step to next
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

#if CONFIG_ESP8266_IS_WIFI_HOST == 1
		case Esp8266Status_StartWifiHost:
			/*
			 * AT+CWSAP="ESP8266HomeAutomation","AUT",1,0
			 * 3. param = channel
			 * 4. param = encryption
			 * ESP8266HomeAutomation
			 * AUT
			 */
			ESP8266_StartReceive();
			ESP8266_SendString("AT+CWSAP=\"ESP8266HomeAutomation\",\"AUT\",1,0\r\n");
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Wifi server create");
			break;

		case Esp8266Status_StartWifiHostCheckResponse:
			if (!StrCmpFirst("\r\nOK\r\n", (const char *)receiveBuffer))
			{
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Wifi server create successful");
				//break;	// Step to next
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
			// AT+CWJAP="networkname","password"
#if CONFIG_ESP8266_CONNECT_DYNAMIC == 1
			char str[50];
			usprintf(str, "AT+CWJAP=\"%s\",\"%s\"\r\n",
					ESP8266_WifiNetworkName, ESP8266_WifiNetworkPassword);
			ESP8266_SendString(str);
#else
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
				ESP8266_ClearReceive(false, sizeof("\r\nOK\r\n")-1);
			}
			else if (!StrCmpFirst("\r\nbusy p...\r\n", (const char *)receiveBuffer))
			{
				// "busy p..."
				// I hope, connection in progress
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Wait... (busy)\r\n");
				ESP8266_ClearReceive(false, sizeof("\r\nbusy p...\r\n")-1);
			}
			else if (!StrCmpFirst("\r\nFAIL", (const char *)receiveBuffer)
					|| !StrCmpFirst("\r\nERROR", (const char *)receiveBuffer))
			{
				// TODO: This is need? Can we drop out?
				// "FAIL"
				// "ERROR"
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
				ESP8266_DEBUG_PRINT("WiFi connect command failed");
				ESP8266_ClearReceive(true, 0);
			}
			else if (!receivedMessageLength)
			{
				// Not received anything
				ESP8266_DEBUG_PRINT("Wifi connection in progress, wait reponse...");
				ESP8266_ErrorCnt++;
				if (ESP8266_ErrorCnt > 10)
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
				ESP8266_DEBUG_PRINTF("Wifi connection in progress, received unknown message (but not processed): \"%s\"\r\n", receiveBuffer);
				ESP8266_ErrorCnt++;
				if (ESP8266_ErrorCnt > 10)
				{
					ESP8266_ErrorCnt = 0;
					ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
					ESP8266_DEBUG_PRINT("Restart connect to wifi network...");
					ESP8266_ClearReceive(true, 0);
				}
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
				ESP8266_ClearReceive(false, sizeof("\r\nOK\r\n")-1);
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
			/* Get IP
			 * AT+CIFSR
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
			 * AT+CIPSERVER
			 * set as server/listen()
			 * AT+CIPSERVER=1,2000
			 * Response:
			 * 	OK
			 * 	Linked
			 */
			ESP8266_StartReceive();
			ESP8266_SendString("AT+CIPSERVER=1,2000\r\n");
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
				DebugPrint("Successful started server\r\n");
				// Disable task, because next state is wait client
				//TaskHandler_DisableTask(Task_Esp8266);
				ESP8266_StartReceive();
			}
			else if (!StrCmpFirst("ERROR", (const char *)receiveBuffer))
			{
				// ERROR
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_StartTcpServer;
				DebugPrint("Failed started server (ERROR)\r\n");
			}
			else
			{
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_StartTcpServer;
				DebugPrint("Failed started server\r\n");
			}
			ESP8266_ClearReceive(true, 0);
			break;
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
			length += usprintf(buffer, "AT+CIPSTART=0,\"TCP\",\"");
			length += Network_PrintIp(&buffer[length], (Network_IP_t *)&ESP8266_ServerAddress);
			length += usprintf(&buffer[length], "\",%d\r\n", ESP8266_ServerPort);
			ESP8266_SendString(buffer);
#else
			ESP8266_SendString("AT+CIPSTART=0,\"TCP\",\""
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
				DebugPrint("OK... Wait finish...\r\n");
				ESP8266_ClearReceive(false, sizeof("\r\nOK\r\n")-1);
			}
			else if (!StrCmpFirst("ALREADY CONNECT\r\n", (const char *)receiveBuffer)
				||   !StrCmpFirst("ALREAY CONNECT\r\n", (const char *)receiveBuffer))
			{
				// OK
				// Already connected
				ESP8266_LED_OK();
				ESP8266StatusMachine += 2;	// Jump CheckFinish
				DebugPrint("Successful connect to TCP server (already connected)\r\n");
				ESP8266_ClearReceive(true, 0);
			}
			else if (!StrCmpFirst("\r\nbusy p...\r\n", (const char *)receiveBuffer))
			{
				// busy p...
				ESP8266StatusMachine++;	// I hope, the command in progress
				DebugPrint("Failed connect to TCP server (busy)... Wait...\r\n");
				// TODO: Wait... ?
				ESP8266_ClearReceive(false, sizeof("\r\nbusy p...\r\n")-1);
			}
			else if (!StrCmpFirst("no ip\r\n", (const char *)receiveBuffer))
			{
				// Error, no IP
				ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
				ESP8266_ErrorCnt = 0;
				DebugPrint("Error... No IP...\r\n");
				ESP8266_ClearReceive(true, 0);
			}
			else
			{
				DebugPrint("Tcp connect command response not received, wait...\r\n");
				ESP8266_ErrorCnt++;
				if (ESP8266_ErrorCnt > 5)
				{
					// Has lot of errors
					ESP8266StatusMachine = Esp8266Status_ConnectTcpServer;
					ESP8266_ErrorCnt = 0;
					DebugPrint("Error... Restart connecting...\r\n");
					if (receivedMessageLength > 0)
						DebugPrint("Received message: %s\r\n", receiveBuffer);
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
				DebugPrint("Successful connect to TCP server\r\n");
				ESP8266_ClearReceive(true, 0);
			}
			else if (!StrCmpFirst("\r\nERROR\r\nUnlink", (const char *)receiveBuffer))
			{
				// Error
				// It is TCP connection error, not WiFi error
				ESP8266_LED_FAIL();
				ESP8266_ErrorCnt++;
				ESP8266StatusMachine = Esp8266Status_ConnectTcpServer;
				DebugPrint("Failed connect to TCP server (unlink)\r\n");
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
				DebugPrint("Failed connect to TCP server (fail)\r\n");
				ESP8266_ClearReceive(true, 0);
			}
			else
			{
				// Unknown error..
				ESP8266_LED_FAIL();
				ESP8266_ErrorCnt++;
				// Do not step state ?
				if (receivedMessageLength > 0)
					DebugPrint("Unknown received message: \"%s\" at connection to TCP server\r\n", receiveBuffer);
				else
					DebugPrint("Not received message at connection to TCP server\r\n");
				if (ESP8266_ErrorCnt > 10)
				{
					// Has lot of errors
					ESP8266StatusMachine = Esp8266Status_ConnectTcpServer;
					ESP8266_ErrorCnt = 0;
					DebugPrint("Error... Restart connecting...\r\n");
					ESP8266_ClearReceive(true, 0);
					// TODO: Csekkolni, hogy van-e hálózat?
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
			static uint8_t IdleCnt = 0;
			IdleCnt++;
			if (IdleCnt > 30)
			{
				IdleCnt = 0;
				DebugPrint("Send an idle message...\r\n");
				ESP8266_RequestSendTcpMessage("version");
				// TODO: Do not response message need to send?
			}
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
static void ESP8266_CheckIdleStateMessage(char * receiveBuffer, uint8_t receivedMessageLength)
{
	/*
	 * Check message, which can be anything...
	 * Link
	 * Unlink
	 * +IPD: Received message
	 */
	if (ESP8266_RxBuffer_WriteCnt != ESP8266_RxBuffer_ReadCnt)
	{

		bool goodMsgRcv = false;

		// Buffer changed

		if (ESP8266_TcpSendIsStarted_Flag)
		{
			// Check, response is "> "
			if (!StrCmpFirst("> ", (const char *)receiveBuffer))
			{
				// Received "> ", we can send message
				ESP8266_DEBUG_PRINT("Received \"> \"");
				ESP8266_SendTcpMessageNonBlockingMode_SendMessage();

				ESP8266_TcpSent_WaitSendOk_Flag = true;
				TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 1000);
				goodMsgRcv = true;
				ESP8266_ClearReceive(false, sizeof("> ") - 1);
			}
			else if (!StrCmpFirst("link is not\r\n", (const char *)receiveBuffer))
			{
				// Cannot send
				ESP8266_TcpSendBuffer_EnableFlag = true;
				DebugPrint("Cannot send (link is not)\r\n");
				ESP8266_ClearReceive(false, sizeof("link is not\r\n") - 1);
				// Request TCP reconnect
#if CONFIG_ESP8266_IS_TCP_SERVER == 0
				ESP8266StatusMachine = Esp8266Status_ConnectTcpServer;
#endif
			}
			else if (!StrCmpFirst("busy inet...\r\nERROR\r\n", (const char *)receiveBuffer))
			{
				// Error...
				// TODO: What we need to do?
				ESP8266_TcpSendBuffer_EnableFlag = true;
				DebugPrint("Cannot send TCP message (busy inet...)\"> \"...\r\n");
				ESP8266_ClearReceive(true, 0);
			}
			else
			{
				// Error
				ESP8266_TcpSendBuffer_EnableFlag = true;
				// TODO: Be careful, it can be buffer overflow !!!
				DebugPrint("Cannot send, not received \"> \"...\r\n"
						"Received message: \"%s\"\r\n", receiveBuffer);
				DebugPrint("Cannot send TCP message (not received \"> \")\r\n");
				ESP8266_ClearReceive(true, 0);
			}
			// Clear send flag
			ESP8266_TcpSendIsStarted_Flag = false;
		}
		else if (ESP8266_TcpSent_WaitSendOk_Flag)
		{
			if (!StrCmpFirst("\r\nSEND OK\r\n", (const char *)receiveBuffer))
			{
				// OK, sending successful
				DebugPrint("Sending successful\r\n");
				goodMsgRcv = true;
				ESP8266_ClearReceive(false, sizeof("\r\nSEND OK\r\n") - 1);
			}
			else
			{
				// TODO: Egyéb válasz is lehet?
				// Error, not received "SEND OK"
				DebugPrint("Sending failed (not received \"SEND OK\"). Received: \"%s\"\r\n", receiveBuffer);
				ESP8266_ClearReceive(true, 0);
			}
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 100);
			ESP8266_TcpSent_WaitSendOk_Flag = false;
			ESP8266_TcpSendBuffer_EnableFlag = true;
		}
		else if (!StrCmpFirst("Link\r\n", (const char *)receiveBuffer))
		{
			// "Link"
			DebugPrint("Received \"Link\": a client connected\r\n");
			// TODO: Step to IP address printing (connected clients)
			//ESP8266StatusMachine = Esp8266Status_XXX;
			goodMsgRcv = true;
			ESP8266_ClearReceive(false, sizeof("Link\r\n") - 1);
			ESP8266_LOG_EVENT(EVENT_LINK);
		}
		else if (!StrCmpFirst("Unlink\r\n", (const char *)receiveBuffer))
		{
			// "Unlink"
			DebugPrint("Received \"Unlink\": a client disconnected\r\n");
			goodMsgRcv = true;
			ESP8266_ClearReceive(false, sizeof("Unlink\r\n") - 1);
			ESP8266_LOG_EVENT(EVENT_UNLINK);
		}
#if CONFIG_ESP8266_IS_TCP_SERVER == 0
		else if (!StrCmpFirst("\r\nERROR\r\nUnlink\r\n", (const char *)receiveBuffer))
		{
			// "Unlink"
			DebugPrint("Received \"Unlink\": a tcp disconnected\r\n");
			goodMsgRcv = true;
			ESP8266StatusMachine = Esp8266Status_ConnectTcpServer;
			ESP8266_ClearReceive(false, sizeof("Received \"Unlink\": a tcp disconnected\r\n") - 1);
			ESP8266_LOG_EVENT(EVENT_UNLINK);
		}
#endif
		else if (!StrCmpFirst("\r\n+IPD,", (const char *)receiveBuffer))
		{
			// Message form: "\r\n+IPD,1,4:msg3\r\nOK"
			// Has got finish "\r\n"?
			if (STRING_FindString(
					(const char *)&receiveBuffer[sizeof("\r\n+IPD,")],
					"\r\n") >= 0)
			{
				// Received good message, with finish "\r\n"

				// TODO: Channel (2 character is skipped:)
				// "+IPD,1,4:msg3"
				// Check next parameter:
				uint8_t length = sizeof("\r\n+IPD,0,") - 1;
				// TODO: Use better function
				int16_t commIndex = STRING_FindString((const char *)&receiveBuffer[length], ":");
				if (commIndex > -1)
				{
					// Has ':'
					// TODO: scanf
					uint32_t messageLength;
					receiveBuffer[length + commIndex] = '\0';
					if (StringToUnsignedDecimalNum((const char *)&receiveBuffer[length], &messageLength))
					{
						if (messageLength <= ESP8266_TCP_MESSAGE_MAX_LENGTH)
						{
							// Set position after ':'
							length += commIndex + 1;

							// We have message source:
							// Save to global variable
							StrCpyFixLength(ESP8266_ReceivedTcpMessage,
									(const char *)&receiveBuffer[length],
									messageLength);
							ESP8266_ReceivedTcpMessage[messageLength] = '\0';

							DebugPrint("Received TCP message: \"%s\"\r\n", ESP8266_ReceivedTcpMessage);

							CommandHandler_PrepareFindExecuteCommand(CommProt_ESP8266Wifi, ESP8266_ReceivedTcpMessage);

							ESP8266_ClearReceive(false, length + messageLength + sizeof("\r\nOK\r\n") - 1);
							goodMsgRcv = true;
							ESP8266_LOG_EVENT(EVENT_RECEIVED_GOOD_TCP_MESSAGE);
						}
						else
						{
							// Received too large message
							DebugPrint("Received too large TCP message: \"%s\"\r\n", receiveBuffer);
							ESP8266_ClearReceive(true, 0);
						}
					}
					else
					{
						DebugPrint("Received +IPD message with wrong length parameter\r\n");
						//ESP8266_RxBuffer_ReadCnt - do not clear
					}
				}
				else
				{
					DebugPrint("Received +IPD message without ':'\r\n");
					//ESP8266_RxBuffer_ReadCnt - do not clear
				}
			}
			else
			{
				// +IPD hasn't got finish "\r\n"
				DebugPrint("Received +IPD message without end:\r\n\"%s\"\r\n", (const char *)receiveBuffer);
			}
		}
		else if (receivedMessageLength < 6)
		{
			// Received too small message
			DebugPrint("Received too small message: \"%s\"\r\n", (const char *)receiveBuffer);
			//ESP8266_RxBuffer_ReadCnt - do not clear
		}
		else
		{
			// Received unknown message
			DebugPrint("Received unknown message: \"%s\"\r\n", (const char *)receiveBuffer);
			//ESP8266_RxBuffer_ReadCnt - do not clear
		}


		// Error handling
		// If has lot of error, clear buffer
		if (!goodMsgRcv)
		{
			// If has a lot of error
			ESP8266_ErrorCnt++;
			if (ESP8266_ErrorCnt > 5 || receivedMessageLength > 50)
			{
				DebugPrint("Has lot of errors, cleared buffer\r\n");
				// ~Reset buffer
				ESP8266_ClearReceive(true, 0);
				ESP8266_ErrorCnt = 0;
				ESP8266_LOG_EVENT(EVENT_ERROR);
			}
		}
		else
		{
			// Received good message
			// TODO: Think about ErrorCnt clearing...
			ESP8266_ErrorCnt = 0;
		}
	}
	/*
	 * else
	 * ESP8266_RxBuffer_WriteCnt == ESP8266_RxBuffer_ReadCnt
	 * Do nothing...
	 */
	else if (ESP8266_TcpSendBuffer_EnableFlag == false)
	{
		// We has message on SendBuffer (we want to send a requested message)
		// ReadCnt = WriteCnt, so we are not receiving
		// TODO: Be careful, it is not truth
		ESP8266_SEND_TCP_MESSAGE((char *)ESP8266_TcpTransmitBuffer);
	}
}



#endif	// #ifdef CONFIG_MODULE_ESP8266_ENABLE
