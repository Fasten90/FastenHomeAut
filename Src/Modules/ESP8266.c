/*
 *		ESP8266.c
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		ESP8266 communication
 *		Target:			STM32Fx
 *		Version:		v5
 *		Last modified:	2017.03.05
 */


/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "include.h"
#include "CircularBuffer.h"
#include "TaskList.h"
#include "CommandHandler.h"
#include "ESP8266.h"

#ifdef CONFIG_MODULE_ESP8266_ENABLE

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

UART_HandleTypeDef ESP8266_UartHandle;

volatile uint8_t ESP8266_Uart_ReceivedCharFlag;

volatile char ESP8266_ReceiveBuffer[ESP8266_BUFFER_LENGTH];
volatile char ESP8266_TransmitBuffer[ESP8266_BUFFER_LENGTH];
bool ESP8266_SendBuffer_EnableFlag = true;
bool ESP8266_SendIsStarted_Flag = false;
bool ESP8266_Sent_WaitSendOk_Flag = false;


char ESP8266_ReceivedTcpMessage[ESP8266_TCP_MESSAGE_MAX_LENGTH];


ESP8266_WifiConnectionStatusType	ESP8266_ConnectionStatus
			= ESP8266_WifiConnectionStatus_Unknown;

ESP8266_TcpConnectionStatusType	ESP8266_TcpConnectionStatus
			= ESP8266_TcpConnectionStatus_Unknown;

///< My IP address
Network_IP_t ESP8266_MyWifiIpAddress = { 0 };
Network_IP_t ESP8266_ExWifiIpAddress = { 0 };

///< Server IP address
const Network_IP_t ESP8266_ServerAddress = { .IP = { 192, 168, 1, 62 } };

const Network_Port_t ESP8266_ServerPort = 2000;


// Receive
volatile uint8_t ESP8266_ReceiveBuffer_WriteCnt = 0;
volatile uint8_t ESP8266_ReceiveBuffer_ReadCnt = 0;

uint8_t ESP8266_ErrorCnt = 0;


/// Debug
bool ESP8266_DebugEnableFlag = 1;


/// for debug
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


#ifdef CONFIG_USE_FREERTOS
xSemaphoreHandle ESP8266_USART_Rx_Semaphore;
xQueueHandle ESP8266_SendMessage_Queue;	// LOG queue
xQueueHandle ESP8266_ReceivedMessage_Queue;
#endif


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
#if CONFIG_ESP8266_WIFISERVER_ENABLE == 1
	Esp8266Status_StartWifiServer,
	Esp8266Status_StartWifiServerCheckResponse,
#else
	Esp8266Status_ConnectWifiNetwork,
	Esp8266Status_ConnectWifiNetworkCheckResponse,
#endif
	Esp8266Status_StartServer,
	Esp8266Status_StartServerCheckResponse,
	Esp8266Status_PrintMyIpAddress,
	Esp8266Status_IpAddressResponse,

	Esp8266Status_BeforeIdle,
	Esp8266Status_Idle,

} ESP8266_StatusMachine_t;


static ESP8266_StatusMachine_t ESP8266StatusMachine = Esp8266Status_Unknown;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_USE_FREERTOS
static bool ESP8266_ReceiveUnknownTcpMessage(void);
#endif

static void ESP8266_FirstStartReceive(void);
static void ESP8266_StartReceive(void);

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

static bool ESP8266_ConvertIpString(char *message);

static void DebugPrint(const char *format, ...);

static void ESP8266_FindLastMessage(void);
static void ESP8266_ClearReceive(bool isFullClear, uint8_t stepLength);

static void ESP8266_CheckIdleStateMessage(char * receiveBuffer, uint8_t receivedMessageLength);



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

	return;
	
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

	if (HAL_UART_Transmit(&ESP8266_UartHandle, (uint8_t*)str, length, 5000) != HAL_OK)
	{
		Error_Handler();
	}
}



/**
 * \brief	Convert IP string to IP number
 */
static bool ESP8266_ConvertIpString(char *message)
{
	bool isOk = false;
	int16_t pos1;
	int16_t pos2;

	// String come like "192.168.4.1\r\n192.168.1.34\r\n\r\nOK\r\n"
	pos1 = STRING_FindString(message, "\r\n");
	if (pos1 >= 0)
	{
		message[pos1] = '\0';
		pos1 += 2;	// Skip "\r\n"
		pos2 = STRING_FindString(&message[pos1], "\r\n");
		if (pos2 >= 0)
		{
			message[pos1+pos2] = '\0';
			isOk = true;
		}
		// else: there is no ending "\r\n"
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
		isOk &= Network_ConvertIpAddressStringToIP(&message[pos1], &ESP8266_ExWifiIpAddress);
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
	length += StrCpy(&str[length], "\r\nExWifi: ");
	length += Network_PrintIp(&str[length], &ESP8266_ExWifiIpAddress);

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
		char TxBuffer[DEBUGUART_TXBUFFERSIZE];

		va_list ap;									// argument pointer
		va_start(ap, format); 						// ap on arg
		string_printf(TxBuffer, format,ap);			// Separate and process
		va_end(ap);						 			// Cleaning after end

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
		#if (CONFIG_ESP8266_IS_SERVER == 1)
		// Server mode
		if (ESP8266_ConnectionStatus != ESP8266_ConnectionStatus_SuccessfulServerStarted)
		{
			// Start server
			ESP8266_StartServerBlocking();

		}
		#elif (CONFIG_ESP8266_IS_SERVER == 0)
		// Client mode
		if (ESP8266_ConnectionStatus != ESP8266_WifiConnectionStatus_SuccessfulConnected)
		{
			// TODO: Find server

			// Connect to server
			ESP8266_ConnectToServerInBlockingMode();

		}
		#else
		#error "CONFIG_ESP8266_IS_SERVER define isnt correct."
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

	if (!StrCmp("ATE0\r\r\n\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
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

	
	if (!StrCmp("\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
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
	
	if (!StrCmp("OK\r\n",(const char *)ESP8266_ReceiveBuffer))
	{
		// "OK"
		ESP8266_LED_OK();
	}
	else if (!StrCmp("no change\r\n",(const char *)ESP8266_ReceiveBuffer))
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
	
	if (!StrCmp("\r\nOK\r\n", (const char *)ESP8266_ReceiveBuffer))
	{
		// "OK"
		ESP8266_LED_OK();
	}
	else if (!StrCmp("\r\nFAIL", (const char *)ESP8266_ReceiveBuffer))
	{
		// "FAIL"
		// AT+CWJAP=\"networkname\",\"password\"\r\r\n\r\nFAIL\r\n")
		ESP8266_LED_FAIL();
		return false;
	}
	else if (!StrCmp("\r\nERRO", (const char *)ESP8266_ReceiveBuffer))
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
	
	if (!StrCmp("\r\nOK\r\n", (const char *)ESP8266_ReceiveBuffer))
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
	if (ESP8266_ConvertIpString((char *)ESP8266_ReceiveBuffer, &ESP8266_MyIpAddress))
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
	#ifdef CONFIG_ESP8266_IS_SERVER
	
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
	ESP8266_ReceiveString((unsigned char *)ESP8266_ReceiveBuffer,StringLength((uint8_t *)"AT+CWDHCP=1,0\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString((uint8_t *)"AT+CWDHCP=1,0\r\n");
	
	ESP8266_WaitAnswer();
	
	if (!StrCmp("AT+CWDHCP=1,0\r\r\n\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
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
	
	ESP8266_ReceiveString((unsigned char *)ESP8266_ReceiveBuffer,
	StringLength((uint8_t *)"AT+CIPSTA=\"9.6.5.14\"\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString((uint8_t *)"AT+CIPSTA=\"9.6.5.14\"\r\n");
	
	ESP8266_WaitAnswer();
	
	if (!StrCmp("AT+CIPSTA=\"9.6.5.14\"\r\r\n\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
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
	
	// TODO: WTF, what is this parameter
	if (!StrCmp("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			(const char *)ESP8266_ReceiveBuffer))
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



#ifdef ESP8266_USE_BLOCK_MODE
#if (CONFIG_ESP8266_IS_SERVER == 0)
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
	
	if (!StrCmp("\r\nOK\r\nLinked\r\n", (const char *)ESP8266_ReceiveBuffer))
	{
		// OK\r\nLinked
		ESP8266_LED_OK();
		return true;
	}
	if (!StrCmp("ALREADY CONNECT", (const char *)ESP8266_ReceiveBuffer))
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
#endif



#ifdef ESP8266_USE_BLOCK_MODE
#if (CONFIG_ESP8266_IS_SERVER == 1)
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
	
	
	ESP8266_ClearReceiveBuffer();
		
	ESP8266_ReceiveString(StringLength("AT+CIPSERVER=1,2000\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString("AT+CIPSERVER=1,2000\r\n");
	
	ESP8266_WaitAnswer();
	
	
	if (!StrCmp("AT+CIPSERVER=1,2000\r\r\n\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
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
#endif



/**
 * \brief	Request send TCP message
 */
uint8_t ESP8266_RequestSendTcpMessage(const char * message)
{
	uint8_t length = 0;

	// Copy to send buffer
	if (ESP8266_SendBuffer_EnableFlag)
	{
		// Lock buffer
		ESP8266_SendBuffer_EnableFlag = false;
		// Copy message to buffer
		length = StrCpyMax((char *)ESP8266_TransmitBuffer, message, ESP8266_TCP_MESSAGE_MAX_LENGTH);
	}
	else
	{
		// Cannot access to buffer
		length = 0;
	}

	return length;
}



/**
 * \brief	Send message (blocking or not blocking mode)
 */
bool ESP8266_SendTcpMessage(const char *message)
{
#ifdef ESP8266_USE_BLOCK_MODE
	return ESP8266_SendTcpMessageBlockingMode(message);
#else
	return ESP8266_SendTcpMessageNonBlockingMode_Start(message);
#endif
}



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
	
	if (!StrCmp("> ", (const char *)ESP8266_ReceiveBuffer))
	{
		ESP8266_LED_OK();
	}
	else if (!StrCmp("link is not\r\n", (const char *)&ESP8266_ReceiveBuffer[1]))
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
	
	if (!StrCmp("\r\nSEND OK\r\n", (const char *)ESP8266_ReceiveBuffer))
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



/**
 * \brief	Send message on TCP
 */
static bool ESP8266_SendTcpMessageNonBlockingMode_Start(const char *message)
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


	// Send ~ "AT+CIPSEND=0,40\r\n"
	usprintf(buffer, "AT+CIPSEND=0,%d\r\n", length);
	ESP8266_SendString(buffer);

	ESP8266_SendIsStarted_Flag = true;

	return true;
}



static bool ESP8266_SendTcpMessageNonBlockingMode_SendMessage(void)
{

	// Check length
	char * message = (char *)ESP8266_TransmitBuffer;
	uint8_t length = StringLength((const char *)message);

	if (length >= ESP8266_TCP_MESSAGE_MAX_LENGTH)
	{
		// Wrong length
		DebugPrint("Too long message!\r\n");
		length = ESP8266_TCP_MESSAGE_MAX_LENGTH - 1;
	}


	/////////////////////////
	// Example:
	// Send
	// "GET / HTTP/1.0\r\n\r\n"
	// Response:
	// "SEND OK"

	// Send message and wait response

	ESP8266_SendString(message);

	DebugPrint("Sent TCP message: \"%s\"\r\n", message);

	return true;
}



#if NOT_USED
/**
 *\brief		Receive HomeAutMessage
 *				length: 10+40
 *				example: "+IPD,0,40:" + "|HomeAut|010|014|LOGIN__|NMEDIU00000000|"
 */
bool ESP8266_ReceiveFixTcpMessage(void)
{

	
	ESP8266_ClearReceiveBuffer();
	
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
	ESP8266_ClearReceiveBuffer();
	
	// BufferCnt = 0;
	ESP8266_ReceiveBuffer_WriteCnt = 0;
	
	// Start receive (wait 1 character)
	HAL_UART_Receive_IT(&ESP8266_UartHandle, (uint8_t *)&ESP8266_ReceiveBuffer[0], 1);

	// Wait message
	//ESP8266_WaitAnswer();	// Only wait ...
	ESP8266_WaitMessageAndCheckSendingQueue();		// wait for received message or if have to sending message
	
	return true;
}
#endif



static void ESP8266_FirstStartReceive(void)
{
	// Clear buffer
	ESP8266_ClearReceiveBuffer();

	// BufferCnt = 0;
	ESP8266_ReceiveBuffer_WriteCnt = 0;
	ESP8266_ReceiveBuffer_ReadCnt = 0;

	UART_ResetStatus(&ESP8266_UartHandle);

	// Start receive (wait x character)
	HAL_UART_Receive_IT(&ESP8266_UartHandle,
			(uint8_t *)&ESP8266_ReceiveBuffer[0],
			ESP8266_RECEIVE_LENGTH);
}



/**
 * \brief	Switch to unknown length Receive mode
 */
static void ESP8266_StartReceive(void)
{
	// Clear buffer
	//ESP8266_ClearReceiveBuffer();

	// BufferCnt = 0;
	//ESP8266_ReceiveBuffer_WriteCnt = 0;

	//UART_ResetStatus(&ESP8266_UartHandle);

	// Start receive (wait x character)
	/*HAL_UART_Receive_IT(&ESP8266_UartHandle,
			(uint8_t *)&ESP8266_ReceiveBuffer[0],
			ESP8266_RECEIVE_LENGTH);
	*/
	return;
}



/**
 * \brief	Wait/Receive string
 * \note	It wait fix length
 */
void ESP8266_ReceiveString(uint8_t length)
{
	// Clear buffer
	ESP8266_ClearReceiveBuffer();

	// Clear flag
#ifdef CONFIG_USE_FREERTOS
	xSemaphoreTake(ESP8266_USART_Rx_Semaphore, 0);
#else
	ESP8266_Uart_ReceivedCharFlag = 0;
#endif

	// Receive
	HAL_UART_Receive_IT(&ESP8266_UartHandle, (uint8_t *)ESP8266_ReceiveBuffer, length);
}



/**
 * \brief	Reset ESP8266 received buffer
 */
void ESP8266_ClearReceiveBuffer(void)
{
	uint16_t i;
	for (i = 0; i < ESP8266_BUFFER_LENGTH; i++)
	{
		ESP8266_ReceiveBuffer[i] = '\0';
	}
	
}



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
		else if (ESP8266_ReceiveBuffer_WriteCnt < 2)
		{
			// Need to sending?
			if (xQueueReceive(ESP8266_SendMessage_Queue,
					(void *)ESP8266_TransmitBuffer,
					(portTickType)0) == pdPASS)
			{
				// Sending...
				// TODO: Csinálni egy receive mode váltás függvényt!!
				ESP8266_Receive_Mode_FixLength = 1;	// fix length, because receiving "> "
				ESP8266_ReceiveBuffer_WriteCnt = 0;
				ESP8266_TransmitBuffer[40] = '\0';	// end of buffer need to put an end char
				
				// Clear UART states
				UART_ResetStatus(&ESP8266_UartHandle);
				
				// Sending
				if (ESP8266_SendTcpMessageBlockingMode((char *)ESP8266_TransmitBuffer))
				{
					if (ESP8266_DebugEnableFlag)
					{
						uprintf("Successful sent a message:\r\n"
								"  %s\r\n",
								(const char *)ESP8266_TransmitBuffer);
					}
				}
				else
				{	
					if (ESP8266_DebugEnableFlag)
					{
						uprintf("Failed sending message:\r\n"
								"  %s\r\n",
								(const char *)ESP8266_TransmitBuffer);
					}
				}

				
				// Reset the receive buffer:
				ESP8266_ReceiveBuffer_WriteCnt = 0;
				ESP8266_Receive_Mode_FixLength = 0;

				// Start receive
				HAL_UART_Receive_IT(&ESP8266_UartHandle, (uint8_t *)ESP8266_ReceiveBuffer, 1);
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
				(void * )ESP8266_TransmitBuffer,
				( portTickType )0 ) == pdPASS)
		{

			// Sending
			if (ESP8266_SendTcpMessageBlockingMode((char *)ESP8266_TransmitBuffer))
			{
				// Successful sent
				if (ESP8266_DebugEnableFlag)
				{
					uprintf("Successful sent a message:\r\n"
							"  %s\r\n",
							(const char *)ESP8266_TransmitBuffer);
				}
			}
			else
			{
				// Failed send
				if (ESP8266_DebugEnableFlag)
				{
					uprintf("Failed sending message:\r\n"
							"  %s\r\n",
							(const char *)ESP8266_TransmitBuffer);
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
	
	ESP8266_ClearReceiveBuffer();
	
	// Link\r\n
	// \r\n
	ESP8266_ReceiveString(StringLength("Link\r\n") );
		
	// wait for message
	ESP8266_WaitAnswer();
	
	return true;
}
#endif



#if (CONFIG_ESP8266_IS_SERVER == 0)
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



#if (CONFIG_ESP8266_IS_SERVER == 1)
bool ESP8266_StartServerBlocking(void)
{
	// Start server
	while (!ESP8266_StartServer())
	{
		DebugPrint("Error! Server isn't started\r\n");
		DelayMs(10000);
	}
	
	ESP8266_ConnectionStatus = ESP8266_ConnectionStatus_SuccessfulServerStarted;
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


	if (ESP8266_ReceiveBuffer_WriteCnt >= ESP8266_RECEIVEMESSAGE_MIN_LENGTH)
	{

		// TODO: Change DebugPrint-s
		// TODO: Modify separated good message......
		DebugPrint("Valid HomeAut message received:\r\n"
					"%s",
					(char *)ESP8266_ReceiveBuffer
					);

#ifdef CONFIG_USE_FREERTOS
		if (xQueueSend(ESP8266_ReceivedMessage_Queue,
				(const void *)ESP8266_ReceiveBuffer,
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
	else if (!StrCmpWithLength((const char *)ESP8266_ReceiveBuffer, "Link", 4))
	{
		// Received: "Link"
		isValidMessage = true;

		#if (CONFIG_ESP8266_IS_SERVER == 1)
		// TODO: Process connecting, add new IP to list
		DebugPrint("A client connected.\r\n");
		#else
		DebugPrint("Received \"Link\". This device is in client mode, dont understand it.\r\n");
		#endif
	}
	else if (!StrCmpWithLength((const char *)ESP8266_ReceiveBuffer, "Unlink", 6))
	{
		// Received: "Unlink"
		isValidMessage = true;

		// TODO: Process disconnecting, delete IP from list
		#if (CONFIG_ESP8266_IS_SERVER == 1)
		DebugPrint("A client disconnected.\r\n");
		#else
		DebugPrint("Disconnected. Need to reconnect\r\n");
		ESP8266_ConnectionStatus = ESP8266_WifiConnectionStatus_ClosedConnection;
		#endif
	}
	else if (!StrCmpWithLength((const char *)ESP8266_ReceiveBuffer, "OK", 2))
	{
		// Received: "OK"
		// Note: This arrived after received an x length message (now, after 40 length homeautmessage)
		isValidMessage = true;
		DebugPrint("Received an OK\r\n");
	}
	else if (!StrCmpWithLength((const char *)ESP8266_ReceiveBuffer, "", 1))
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
		DebugPrint("Received not valid message: %s\r\n", (char *)&ESP8266_ReceiveBuffer[0]);
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



#ifdef ESP8266_USE_BLOCK_MODE
/**
 * \brief	Send message on wifi
 */
bool ESP8266_SendMessageOnWifi(char *message)
{
#ifdef CONFIG_USE_FREERTOS
	return ESP8266_SendMessageToQueue(message);
#else
	return ESP8266_SendTcpMessageBlockingMode(message);
#endif
}
#endif



/**
 * \brief	Step Buffer WriteCnt to last character
 */
static void ESP8266_FindLastMessage(void)
{
	// TODO: Not a good solve...
	uint16_t i = 0;
	while (ESP8266_ReceiveBuffer[ESP8266_ReceiveBuffer_WriteCnt])
	{
		++ESP8266_ReceiveBuffer_WriteCnt;
		++i;

		if (i > ESP8266_BUFFER_LENGTH)
		{
			// Error
			DebugPrint("Error: Buffer full, clear it...\r\n");
			ESP8266_ClearReceive(true, 0);
			break;
		}
	}
}



/**
 * \brief	Clear receive buffer
 */
static void ESP8266_ClearReceive(bool isFullClear, uint8_t stepLength)
{
	// Clear from ReadCnt to WriteCnt
	if (isFullClear)
	{
		// Clear buffer
		CircularBuffer_Clear((char *)ESP8266_ReceiveBuffer, ESP8266_BUFFER_LENGTH,
				ESP8266_ReceiveBuffer_ReadCnt, ESP8266_ReceiveBuffer_WriteCnt);
		ESP8266_ReceiveBuffer_ReadCnt = ESP8266_ReceiveBuffer_WriteCnt;
	}
	else
	{
		// Not full clear from readCnt to writeCnt
		CircularBuffer_Clear((char *)ESP8266_ReceiveBuffer, ESP8266_BUFFER_LENGTH,
				ESP8266_ReceiveBuffer_ReadCnt,
				ESP8266_ReceiveBuffer_ReadCnt + stepLength);
		ESP8266_ReceiveBuffer_ReadCnt += stepLength;
	}
}



#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
/**
 * \brief	Configure ESP8266
 */
void ESP8266_StatusMachine(void)
{
	// TODO: Delete ESP8266_StartReceive();

	ESP8266_FindLastMessage();

	// If WriteCnt not equal with ReadCnt, we have received message
	char receiveBuffer[ESP8266_BUFFER_LENGTH+1];
	uint16_t receivedMessageLength = 0;

	if (ESP8266_ReceiveBuffer_WriteCnt != ESP8266_ReceiveBuffer_ReadCnt)
	{
		// Need copy to receiveBuffer
		receivedMessageLength = CircularBuffer_GetCharacters(
				(char *)ESP8266_ReceiveBuffer, receiveBuffer,
				ESP8266_BUFFER_LENGTH,
				ESP8266_ReceiveBuffer_WriteCnt, ESP8266_ReceiveBuffer_ReadCnt,
				true);
	}


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
			// Wait 10sec
			// Disable event
			//TaskHandler_DisableTask(Task_Esp8266);
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 10000);
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
			if (!StrCmp("ATE0\r\r\n\r\nOK\r\n", (const char *)receiveBuffer)
				|| !StrCmp("\r\nOK\r\n", (const char *)receiveBuffer))
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
			if (!StrCmp("\r\nOK\r\n", (const char *)receiveBuffer))
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
			ESP8266_SendString("AT+CWMODE=3\r\n");
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Config CWMODE sent");
			break;

		case Esp8266Status_ConfigCwModeCheckResponse:
			if ((!StrCmp("OK\r\n", (const char *)receiveBuffer))
				|| (!StrCmp("no change\r\n", (const char *)receiveBuffer)))
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
			if (!StrCmp("OK\r\n", (const char *)receiveBuffer))
			{
				// "OK"
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Config CWDHCP response ok");
				//break;	// Step to next
			}
			else if (!StrCmp("no change\r\n",(const char *)receiveBuffer))
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
			if ((!StrCmp("\r\nOK\r\n", (const char *)receiveBuffer))
				|| (!StrCmp("link is builded\r\n", (const char *)receiveBuffer)))
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
#if CONFIG_ESP8266_WIFISERVER_ENABLE == 1
		case Esp8266Status_StartWifiServer:
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

		case Esp8266Status_StartWifiServerCheckResponse:
			if (!StrCmp("\r\nOK\r\n", (const char *)receiveBuffer))
			{
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Wifi server create successful");
				//break;	// Step to next
			}
			else
			{
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_StartWifiServer;
				ESP8266_DEBUG_PRINTF("Wifi server create failed, received message: \"%s\"\r\n",
													 (const char *)receiveBuffer);
			}
			ESP8266_ClearReceive(true, 0);
			break;
		// End of "#if CONFIG_ESP8266_WIFISERVER_ENABLE == 1"
#else
		// Not wifi server (connect to network)
		case Esp8266Status_ConnectWifiNetwork:
			/*
			 * Join to WiFi network
			 * AT+CWJAP
			 * Syntax: AT+CWJAP="networkname","password"
			 */
			ESP8266_StartReceive();
			// TODO: Make variable connecting?
			ESP8266_SendString("AT+CWJAP=\""
					CONFIG_ESP8266_WIFI_NETWORK_NAME
					"\",\""
					CONFIG_ESP8266_WIFI_NETWORK_PASSWORD
					"\"\r\n");
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Start connect to wifi network...");
			break;

		case Esp8266Status_ConnectWifiNetworkCheckResponse:

			if (!StrCmp("\r\nOK\r\n", (const char *)receiveBuffer))
			{
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				ESP8266_DEBUG_PRINT("Wifi connect successful");
				ESP8266_ClearReceive(true, 0);
				//break;	// Step to next
			}
			else if (!StrCmp("\r\nFAIL", (const char *)receiveBuffer)
					|| !StrCmp("\r\nERROR", (const char *)receiveBuffer))
			{
				// "FAIL"
				// "ERROR"
				ESP8266_LED_FAIL();
				ESP8266_DEBUG_PRINT("Wifi connect failed");
				ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
				ESP8266_ClearReceive(true, 0);
			}
			else
			{
				// Not received response?
				ESP8266_DEBUG_PRINT("Wifi connecting...");
				ESP8266_DEBUG_PRINT(receiveBuffer);
				ESP8266_ErrorCnt++;
				if (ESP8266_ErrorCnt > 10)
				{
					ESP8266StatusMachine = Esp8266Status_ConnectWifiNetwork;
					ESP8266_ClearReceive(true, 0);
				}
			}

			break;
#endif	// End of "CONFIG_ESP8266_WIFISERVER_ENABLE == 0"

		case Esp8266Status_StartServer:
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

		case Esp8266Status_StartServerCheckResponse:
			// Check CIPSERVER response
			if (!StrCmp("\r\nOK\r\n", (const char *)receiveBuffer)
				|| !StrCmp("no change\r\n", (const char *)receiveBuffer))
			{
				// OK
				ESP8266_LED_OK();
				ESP8266StatusMachine++;
				DebugPrint("Successful started server\r\n");
				// Disable task, because next state is wait client
				//TaskHandler_DisableTask(Task_Esp8266);
				ESP8266_StartReceive();
			}
			else if (!StrCmp("ERROR", (const char *)receiveBuffer))
			{
				// ERROR
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_StartServer;
				DebugPrint("Failed started server (ERROR)\r\n");
			}
			else
			{
				ESP8266_LED_FAIL();
				ESP8266StatusMachine = Esp8266Status_StartServer;
				DebugPrint("Failed started server\r\n");
			}
			ESP8266_ClearReceive(true, 0);
			break;

		case Esp8266Status_PrintMyIpAddress:
			/* Get IP
			 * AT+CIFSR
			 */
			ESP8266_StartReceive();
			ESP8266_SendString(" AT+CIFSR\r\n");
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Get IP address");
			break;

		case Esp8266Status_IpAddressResponse:
			if (ESP8266_ConvertIpString(receiveBuffer))
			{
				// Good processing
				ESP8266_DEBUG_PRINT("IP address processed");
				ESP8266StatusMachine++;
				ESP8266_ClearReceive(true, 0);
			}
			else
			{
				// Wrong / Not full response received
				ESP8266_ErrorCnt++;
				if (ESP8266_ErrorCnt > 3)
				{
					ESP8266_DEBUG_PRINT("IP address process failed");
					ESP8266StatusMachine = Esp8266Status_PrintMyIpAddress;
					ESP8266_ClearReceive(true, 0);
				}
			}
			break;

		case Esp8266Status_BeforeIdle:
			// Print IP address
			DebugPrint((const char *)receiveBuffer);
			ESP8266_ClearReceive(true, 0);
			ESP8266_StartReceive();
			// Set TaskHandler to faster
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 100);
			ESP8266StatusMachine++;
			ESP8266_DEBUG_PRINT("Idle");
			break;

		case Esp8266Status_Idle:
			// Idle state (Check link, unlink, received message...)
			ESP8266_CheckIdleStateMessage(receiveBuffer, receivedMessageLength);
			break;

		default:
			// Unknown state = Error --> Go to Init state
			ESP8266StatusMachine = Esp8266Status_Init;
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 1000);
			ESP8266_DEBUG_PRINT("Error state!");
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
	if (ESP8266_ReceiveBuffer_WriteCnt != ESP8266_ReceiveBuffer_ReadCnt)
	{

		bool goodMsgRcv = false;

		// Buffer changed

		if (ESP8266_SendIsStarted_Flag)
		{
			// Check, response is "> "
			if (!StrCmp("> ", (const char *)receiveBuffer))
			{
				// Received "> ", we can send message
				ESP8266_DEBUG_PRINT("\"> \" Received");
				ESP8266_SendTcpMessageNonBlockingMode_SendMessage();

				ESP8266_Sent_WaitSendOk_Flag = true;
				TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 1000);
				goodMsgRcv = true;
				ESP8266_ClearReceive(false, sizeof("> ") - 1);
			}
			else if (!StrCmp("link is not\r\n", (const char *)receiveBuffer))
			{
				// Cannot send
				ESP8266_SendBuffer_EnableFlag = true;
				DebugPrint("ESP8266: Cannot send, link is nothing\r\n");
				ESP8266_ClearReceive(false, sizeof("link is not\r\n") - 1);
			}
			// busy inet...\r\nERROR  TODO:
			else
			{
				// Error
				ESP8266_SendBuffer_EnableFlag = true;
				DebugPrint("Cannot send, not received \"> \"...\r\n"
						"Received message: \"%s\"\r\n", receiveBuffer);
			}
			// Clear send flag
			ESP8266_SendIsStarted_Flag = false;
		}
		else if (ESP8266_Sent_WaitSendOk_Flag)
		{
			if (!StrCmp("\r\nSEND OK\r\n", (const char *)receiveBuffer))
			{
				// OK, sending successful
				DebugPrint("Sending successful\r\n");
				goodMsgRcv = true;
				ESP8266_ClearReceive(false, sizeof("\r\nSEND OK\r\n") - 1);
			}
			else
			{
				// Error, not received "SEND OK"
				DebugPrint("Sending failed: \"%s\"\r\n", receiveBuffer);
			}
			TaskHandler_SetTaskPeriodicTime(Task_Esp8266, 100);
			ESP8266_Sent_WaitSendOk_Flag = false;
			ESP8266_SendBuffer_EnableFlag = true;
		}
		else if (!StrCmp("Link\r\n", (const char *)receiveBuffer))
		{
			// "Link"
			DebugPrint("Received \"Link\": a client connected\r\n");
			// TODO: Step to IP address printing (connected clients)
			//ESP8266StatusMachine = Esp8266Status_XXX;
			goodMsgRcv = true;
			ESP8266_ClearReceive(false, sizeof("Link\r\n") - 1);
			ESP8266_LOG_EVENT(EVENT_LINK);
		}
		else if (!StrCmp("Unlink\r\n", (const char *)receiveBuffer))
		{
			// "Unlink"
			DebugPrint("Received \"Unlink\": a client disconnected\r\n");
			goodMsgRcv = true;
			ESP8266_ClearReceive(false, sizeof("Unlink\r\n") - 1);
			ESP8266_LOG_EVENT(EVENT_UNLINK);
		}
		else if (!StrCmp("\r\n+IPD,", (const char *)receiveBuffer))
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
							// TODO: Event / set flag
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
						//ESP8266_ReceiveBuffer_ReadCnt - do not clear
					}
				}
				else
				{
					DebugPrint("Received +IPD message without ':'\r\n");
					//ESP8266_ReceiveBuffer_ReadCnt - do not clear
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
			//ESP8266_ReceiveBuffer_ReadCnt - do not clear
		}
		else
		{
			// Received unknown message
			DebugPrint("Received unknown message: \"%s\"\r\n", (const char *)receiveBuffer);
			//ESP8266_ReceiveBuffer_ReadCnt - do not clear
		}


		// Error handling
		// If has lot of error, clear buffer
		if (!goodMsgRcv)
		{
			// If has a lot of error
			ESP8266_ErrorCnt++;
			if (ESP8266_ErrorCnt > 5 || receivedMessageLength > 50)
			{
				DebugPrint("ESP8266 Has lot of errors, cleared buffer\r\n");
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
	 * ESP8266_ReceiveBuffer_WriteCnt == ESP8266_ReceiveBuffer_ReadCnt
	 * Do nothing...
	 */
	else if (ESP8266_SendBuffer_EnableFlag == false)
	{
		// We has message on SendBuffer
		// ReadCnt = WriteCnt, so we are not receiving
		// TODO: Be careful, it is not truth
		ESP8266_SendTcpMessage((char *)ESP8266_TransmitBuffer);
	}
}



#endif	// #ifdef CONFIG_MODULE_ESP8266_ENABLE
