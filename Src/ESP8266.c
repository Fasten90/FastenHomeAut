/*
 *		ESP8266.c
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		ESP8266 communication
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2017.01.14
 */


/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "include.h"
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


ESP8266_WifiConnectionStatusType	ESP8266_ConnectionStatus
			= ESP8266_WifiConnectionStatus_Unknown;

ESP8266_TcpConnectionStatusType	ESP8266_TcpConnectionStatus
			= ESP8266_TcpConnectionStatus_Unknown;

///< My IP address
Network_IP_t ESP8266_MyIpAddress = { 0 };
///< Server IP address
const Network_IP_t ESP8266_ServerAddress = { .IP = { 192, 168, 1, 62 } };


// Receive
uint8_t ESP8266_Receive_Mode_FixLength = 1;
uint8_t ESP8266_ReceiveBuffer_Cnt = 0;

/// Debug
uint8_t ESP8266_DebugEnableFlag = 1;

/// for debug
const DateTime_t EventDateTime =
{
	.year = 17,
	.month = 1,
	.day = 14,
	.hour = 20,
	.minute = 0,
	.second = 0
};


#ifdef CONFIG_USE_FREERTOS
xSemaphoreHandle ESP8266_USART_Rx_Semaphore;
xQueueHandle ESP8266_SendMessage_Queue;	// LOG queue
xQueueHandle ESP8266_ReceivedMessage_Queue;
#endif



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/


static bool ESP8266_ReceiveUnknownTcpMessage(void);

static void ESP8266_WaitMessageAndCheckSendingQueue(void);
static void ESP8266_LoopSending(void);
static void ESP8266_CheckReceivedMessage(void);

static bool ESP8266_ConnectToServerInBlockingMode(void);

static bool ESP8266_SendTcpMessage(const char *message);

static bool ESP8266_ConvertIpString(char *message, Network_IP_t *ip);

static void DebugPrint(const char *debugString);



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
	// TODO: Delete this comments
	// Mindketto HIGH szinten volt amikor megmértem
	// Igen, de GPIO0 az LOW volt induláskor és HIGH lett csatlakozás után ...

	// TODO: Need?
	GPIO_InitStruct.Pin       = ESP8266_GPIO2_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP ;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_GPIO2_GPIO_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ESP8266_GPIO2_GPIO_PORT,ESP8266_GPIO2_GPIO_PIN,GPIO_PIN_SET);	

	// TODO: Need?
	GPIO_InitStruct.Pin       = ESP8266_GPIO0_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_GPIO0_GPIO_PORT, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(ESP8266_GPIO0_GPIO_PORT,ESP8266_GPIO0_GPIO_PIN,GPIO_PIN_RESET);

	
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
	HAL_GPIO_WritePin(ESP8266_CH_GPIO_PORT,ESP8266_CH_GPIO_PIN,GPIO_PIN_SET);
	//DelayMs(2000);
	
	
	// USART TX - RX	
	// Init late USART, because we receive a lot of message at ESP8266 start
	//USART_Init(&ESP8266_UartHandle);
	

	//DelayMs(100);
		
	/////////////////////////////////////////////


	return;
	
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


	// TODO: Delete this comment
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
		if (ESP8266_ConnectionStatus != ESP8266_ConnectionStatus_SuccessfulServerStarted)
		{
			// Start server
			ESP8266_StartServerBlocking();

		}
		#elif (CONFIG_ESP8266_IS_SERVER == 0)
		if (ESP8266_ConnectionStatus != ESP8266_WifiConnectionStatus_SuccessfulConnected)
		{
			// TODO: Find server

			// Connect
			ESP8266_ConnectToServerInBlockingMode();

		}
		#else
		#error "CONFIG_ESP8266_IS_SERVER define isnt correct."
		#endif


		// END OF Connect to server or start server


		// Send Login message
		// TODO: Beautify
		HomeAutMessage_CreateAndSendHomeAutMessage(
			&ESP8266_MyIpAddress,
			(Network_IP_t *)&ESP8266_ServerAddress,
			(DateTime_t *)&EventDateTime,
			Function_Login,
			Login_ImLoginImDiscovery,
			0,
			1);

		DelayMs(1000);


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



/**
 * \brief	Configure ESP8266
 */
bool ESP8266_Config(void)
{
	// TODO: Összevonni a ReceiveString-et, SendString-et  és a WaitAnswer-t
	// TODO: Visszatérni false-szal, ha hibás a lefutás

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
	ESP8266_ReceiveString(StringLength("\r\n192.168.0.1\r\n9.6.5.10\r\n\r\nOK\r\n"));

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
	if (ESP8266_DebugEnableFlag == 1)
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
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		// Fail
		LED_GREEN_OFF();
		LED_RED_ON();
	}
	
	#endif
	*/
	
	
	return true;
}



/**
 * \brief	TODO
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
		LED_GREEN_ON();
		LED_RED_OFF();
		return true;
	}
	else
	{
		// ERROR\r\nUnli [nk]
		LED_GREEN_OFF();
		LED_RED_ON();		
		return false;
	}
	
	//return true;
}



#if (CONFIG_ESP8266_IS_SERVER == 0)
/**
 * \brief	Connect ESP8266 to server
 */
bool ESP8266_ConnectToServer(void)
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
	
	// TODO: Change IP address to changeable
	ESP8266_ReceiveString(StringLength("\r\nOK\r\nLinked\r\n"));
	
	ESP8266_SendString("AT+CIPSTART=0,\"TCP\",\"192.168.1.62\",2000\r\n");
	
	ESP8266_WaitAnswer(5000);
	
#warning "Has an error, not receive good response, but device connect to server"
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


/**
 * \brief	Sending HomeAutMessage
 *			for example:
 *			|HomeAut|010|014|LOGIN__|NMEDIU00000000|
 *			length: 40
 */
static bool ESP8266_SendTcpMessage(const char *message)
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
	
#warning "First character is 0, but why...?... Now it is \r and only received 1 character"
	if (!StrCmp("> ", (const char *)&ESP8266_ReceiveBuffer[1]))
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
#warning: "uncomment it"
		//return false;
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
	
#warning "Wrong received characters, 0.-2. characters are '\0'... Now only receive '>' on first index"
	if (!StrCmp("\r\nSEND OK\r\n", (const char *)&ESP8266_ReceiveBuffer[3]))
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
	ESP8266_ReceiveString(ESP8266_BUFFER_LENGTH);
		
	return true;
}



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
	ESP8266_ReceiveBuffer_Cnt = 0;
	
	// Start receive (wait 1 character)
	HAL_UART_Receive_IT(&ESP8266_UartHandle, (uint8_t *)&ESP8266_ReceiveBuffer[0], 1);

	// Wait message
	//ESP8266_WaitAnswer();	// Only wait ...
	ESP8266_WaitMessageAndCheckSendingQueue();		// wait for received message or if have to sending message
	
	return true;
}
#endif


/*
// It not used
TODO: delete
bool ESP8266_CheckReceiveMessage ( void )
{
	
	#ifdef CONFIG_USE_FREERTOS
	ESP8266_WaitAnswer();	// FreeRTOS blocking (with semaphore)
	return Retrn_Ok;
	return
	#else
	// without semaphore, it is not blocking
	if (ESP8266_Uart_ReceivedCharFlag == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
	#endif
	
}
*/



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
 * \brief	Wait/Receive string
 */
void ESP8266_ReceiveString(uint8_t length)
{
	// Receive type
	//ESP8266_Receive_Mode_FixLength = 1; // TODO:

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
	uint8_t i;
	for (i = 0; i < ESP8266_BUFFER_LENGTH; i++)
	{
		ESP8266_ReceiveBuffer[i] = '\0';
	}
	
}



/**
 *\brief		Wait for answer in blocking mode
 */
void ESP8266_WaitAnswer(uint32_t timeout)
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
	while (ESP8266_Uart_ReceivedCharFlag != 1);
	#endif
	
}



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
		else if (ESP8266_ReceiveBuffer_Cnt < 2)
		{
			// Need to sending?
			if (xQueueReceive( ESP8266_SendMessage_Queue, (void * )ESP8266_TransmitBuffer, ( portTickType )0 ) == pdPASS)
			{
				// Sending...
				ESP8266_Receive_Mode_FixLength = 1;	// fix length, because receiving "> "
				ESP8266_ReceiveBuffer_Cnt = 0;		// TODO: Csinálni egy receive mode váltás függvényt!!
				ESP8266_TransmitBuffer[40] = '\0';	// end of buffer need to put an end char
				
				// Clear UART states
				UART_ResetStatus(&ESP8266_UartHandle);
				
				// Sending
				if (ESP8266_SendTcpMessage((char *)ESP8266_TransmitBuffer))
				{
					// TODO: printf
					DebugPrint("Successful sent a message:\r\n");
					DebugPrint((const char *)ESP8266_TransmitBuffer);
					DebugPrint("\r\n");
				}
				else
				{	
					// TODO: printf
					DebugPrint("Failed sending message:\r\n");
					DebugPrint((const char *)ESP8266_TransmitBuffer);
					DebugPrint("\r\n");
				}

				
				// Reset the receive buffer:
				ESP8266_ReceiveBuffer_Cnt = 0;
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
			if (ESP8266_SendTcpMessage((char *)ESP8266_TransmitBuffer))
			{
				// TODO: printf
				DebugPrint("Successful sent a message:\r\n");
				DebugPrint((const char *)ESP8266_TransmitBuffer);
				DebugPrint("\r\n");
			}
			else
			{
				// TODO: printf
				DebugPrint("Failed sending message:\r\n");
				DebugPrint((const char *)ESP8266_TransmitBuffer);
				DebugPrint("\r\n");
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
		successfulConnected = ESP8266_ConnectToServer();
#warning "delete this code below !!!!!"
		successfulConnected = true;
#warning "Need bugfix !!!!!"
	
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



/**
 * \brief	Convert IP string to IP number
 */
static bool ESP8266_ConvertIpString(char *message, Network_IP_t *ip)
{
	// String come like "\r\n192.168.0.1\r\n"

	int16_t pos = STRING_FindString(&message[2], "\r\n");
	if (pos >= 0)
	{
		pos += 2;
		message[pos] = '\0';
	}
	else
	{
		// There is no ending "\r\n"
		return false;
	}

	char *separated[4];
	if (STRING_Splitter(message, '.',  separated, 4) == 4)
	{
		// Successful separated
		// Convert IP string to number
		uint8_t i;
		for (i = 0; i < 4; i++)
		{
			uint32_t convertValue;
			if (StringToUnsignedDecimalNum(separated[i], &convertValue))
			{
				// Successful convert to number
				if (convertValue <= 255)
				{
					ip->IP[i] = (uint8_t)convertValue;
				}
				else
				{
					// Error, Too large value
					return false;
				}
			}
			else
			{
				// Failed convert to number
				return false;
			}

		}

		return true;
	}
	else
	{
		// Failed separate string
		return false;
	}
}



/**
 * \brief	Reset ESP8266 module on RST pin
 */
void ESP8266_ResetHardware(void)
{
	ESP8266_RST_ACTIVE();
	ESP8266_RST_INACTIVE();
}


/**
 * \brief	TODO
 */
static void ESP8266_CheckReceivedMessage(void)
{

	// TODO: Check these codes below...

	bool isValidMessage;


	// Print received message:
	DebugPrint("\r\nReceived a message\r\n");


	// HomeAutMessage:
	// "\r\n"											2
	// "+IPD,0,40:"										10
	// "|HomeAut|010|014|LOGIN__|NMEDIU00000000|"		40
	// For HomeAutMessage
	if (ESP8266_ReceiveBuffer_Cnt >= 50)
	{

		// TODO: Correct this!
		isValidMessage = true;
		if (isValidMessage)
		{
			// TODO: Change DebugPrint-s
			// TODO: Modify separated good message......
			DebugPrint("Valid HomeAut message received:\r\n");
			DebugPrint((char *)ESP8266_ReceiveBuffer);
			if (xQueueSend(ESP8266_ReceivedMessage_Queue,
					(const void *)ESP8266_ReceiveBuffer,
					1000) == pdPASS)
			{
				// Successful sent to queue
			}
			else
			{
				// Failed sent to queue
				DebugPrint("Message failed to sending queue");
			}
		}
		else
		{
			DebugPrint("Invalid HomeAut message received:\r\n");
			DebugPrint((char *)&ESP8266_ReceiveBuffer[0]);
		}
	}
	// Check for unconnect, or other messages...
	else if (!StrCmpWithLength((const char *)ESP8266_ReceiveBuffer, "Link", 4))
	{
		// Received: "Link"
		isValidMessage = true;

		#if ( CONFIG_ESP8266_IS_SERVER == 1)
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
		#if ( CONFIG_ESP8266_IS_SERVER == 1)
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
		DebugPrint((char *)&ESP8266_ReceiveBuffer[0]);
		DebugPrint("Not valid message.\r\n");
	}


	// Received an correct message?
	if (isValidMessage)
	{
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		LED_GREEN_OFF();
		LED_RED_ON();
	}

}



/**
 * \brief	ESP8266 debug print
 */
static void DebugPrint(const char *debugString)
{
	if (ESP8266_DebugEnableFlag == 1)
	{
		USART_SendMessage(debugString);
	}
}



#endif
// END of File
