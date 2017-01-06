/*
 *		ESP8266.c
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		usart communication
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2017.01.04
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

char ESP8266_ReceiveBuffer[ESP8266_BUFFER_LENGTH];
volatile char ESP8266_TransmitBuffer[ESP8266_HOMEAUT_MESSAGECONTENT_LENGTH+1];


ESP8266_ConnectionStatusType	ESP8266_ConnectionStatus = ESP8266_ConnectionStatus_Unknown;

ESP8266_TcpConnectionStatusType	ESP8266_TcpConnectionStatus = ESP8266_TcpConnectionStatus_Unknown;

///< IP address
uint8_t ESP8266_MyIpAddress[4] = { 0 };

// Receive
uint8_t ESP8266_Receive_Mode_FixLength = 1;
uint8_t ESP8266_ReceiveBuffer_Cnt = 0;

// Debug
uint8_t ESP8266_DebugEnableFlag = 1;


#ifdef CONFIG_USE_FREERTOS
xSemaphoreHandle ESP8266_USART_Rx_Semaphore;
xQueueHandle ESP8266_SendMessage_Queue;	// LOG queue
xQueueHandle ESP8266_ReceivedMessage_Queue;
#endif



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local functions
 *----------------------------------------------------------------------------*/
static void DebugPrint(const char *debugString);
static ReturnType ESP8266_ReceiveUnknownTcpMessage(void);
static void ESP8266_WaitMessageAndCheckSendingQueue(void);

static ReturnType ESP8266_ConvertIpString(char *message, uint8_t *ip);


/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/
extern void Error_Handler( void );



/**
 * \brief	Initialize ESP8266
 */
ReturnType ESP8266_Init(void)
{
	
	//	GPIO Init

	GPIO_InitTypeDef  GPIO_InitStruct;

	ESP8266_PINS_CLK_ENABLES();
	
	
	// GPIO0 & GPIO2
	
	// help for pins: http://www.electrodragon.com/w/ESP8266
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
	
	// Reset (low = reset active)
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_PORT,ESP8266_RST_GPIO_PIN,GPIO_PIN_RESET);	
	//DelayMs(100);

	// Reset end (high = inactive)
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_PORT,ESP8266_RST_GPIO_PIN,GPIO_PIN_SET);
	//DelayMs(1000);


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


	return Return_Ok;
	
}



/**
 * \brief	Configure ESP8266
 */
ReturnType ESP8266_Config(void)
{
	// TODO: Összevonni a ReceiveString-et, SendString-et  és a WaitAnswer-t
	// TODO: Visszatérni false-szal, ha hibás a lefutás

	//////////////////////
	// "ATE0"
	// Echo off

	ESP8266_ReceiveString(StringLength("ATE0\r\r\n\r\nOK\r\n"));

	ESP8266_SendString("ATE0\r\n");

	ESP8266_WaitAnswer();

	if (!StrCmp("ATE0\r\r\n\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
	{
		// Ok
		ESP8266_LED_OK();
	}



	//////////////////////
	// "AT"
	// Check it is alive?

	ESP8266_ReceiveString(StringLength("\r\nOK\r\n"));
	
	ESP8266_SendString("AT\r\n");

	ESP8266_WaitAnswer();

	
	if (!StrCmp("\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
	{
		ESP8266_LED_OK();
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

	ESP8266_WaitAnswer();
	
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
	}
	
	
	return Return_Ok;	
}



/**
 * \brief	Connect to Wifi network
 */
ReturnType ESP8266_ConnectToWifiNetwork(void)
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
	
	ESP8266_WaitAnswer();
	
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
		return Return_Error;
	}
	else if (!StrCmp("\r\nERRO", (const char *)ESP8266_ReceiveBuffer))
	{
		// "ERROR"
		// AT+CWJAP=\"networkname\",\"password\"\r\r\n\r\nERROR\r\n")
		ESP8266_LED_FAIL();
		return Return_Error;
	}
	
	
	
	///////////////////////////////////////////////
	// AT+CIPMUX = 	set multiple connections mode
	// Connection Type:
	// AT+CIPMUX=0		// Single
	// AT+CIPMUX=1		// Multiple
	//////////////////////////////////////////////
		
	ESP8266_ReceiveString(StringLength("\r\nOK\r\n"));
	
	ESP8266_SendString("AT+CIPMUX=1\r\n");
	
	ESP8266_WaitAnswer();
	
	if (!StrCmp("\r\nOK\r\n", (const char *)ESP8266_ReceiveBuffer))
	{
		ESP8266_LED_OK();
	}
	else
	{
		ESP8266_LED_FAIL();
		return Return_Error;
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
	
	ESP8266_WaitAnswer();
	
	// Check and convert IP address
	if (ESP8266_ConvertIpString(ESP8266_ReceiveBuffer, ESP8266_MyIpAddress) == Return_Ok)
	{
		DebugPrint("Successful convert IP address\r\n");
	}
	else
	{
		DebugPrint("Failed convert IP address\r\n");
	}

	
	// Send my IP address debug message
	if (ESP8266_DebugEnableFlag == 1)
	{
		uprintf("My IP address: %d.%d.%d.%d\r\n",
				ESP8266_MyIpAddress[0],
				ESP8266_MyIpAddress[1],
				ESP8266_MyIpAddress[2],
				ESP8266_MyIpAddress[3]);
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
	
	ESP8266_ReceiveString((unsigned char *)ESP8266_ReceiveBuffer,StringLength((uint8_t *)"AT+CIPSTA=\"9.6.5.14\"\r\r\n\r\nOK\r\n"));
	
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
	
	
	return Return_Ok;
}



/**
 * \brief	TODO
 */
ReturnType ESP8266_FindServer ( void )
{
	
	// TODO: Not used... check it

	
	////////////////////////
	//	Connected devices:
	//	AT+CWLIF
	////////////////////////
		
	ESP8266_ReceiveString(StringLength("AT+CWLIF\r\r\n\r\nOK\r\nLinked\r\n"));
	
	ESP8266_SendString("AT+CWLIF\r\n");
	
	ESP8266_WaitAnswer();
	
	// TODO: WTF, what is this parameter
	if (!StrCmp("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",(const char *)ESP8266_ReceiveBuffer))
	{
		// OK\r\nLinked
		LED_GREEN_ON();
		LED_RED_OFF();
		return Return_Ok;
	}
	else
	{
		// ERROR\r\nUnli [nk]
		LED_GREEN_OFF();
		LED_RED_ON();		
		return Return_Error;
	}
	
	//return Return_Ok;
}



#if (CONFIG_ESP8266_IS_SERVER == 0)
/**
 * \brief	Connect ESP8266 to server
 */
ReturnType ESP8266_ConnectToServer(void)
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
	
	ESP8266_WaitAnswer();
	
	
	if (!StrCmp("\r\nOK\r\nLinked\r\n", (const char *)ESP8266_ReceiveBuffer))
	{
		// OK\r\nLinked
		ESP8266_LED_OK();
		return Return_Ok;
	}
	else
	{
		// ERROR\r\nUnli [nk]
		ESP8266_LED_FAIL();
		return Return_Error;
	}
	
}
#endif



#if (CONFIG_ESP8266_IS_SERVER == 1)
/**
 * \brief	Start ESP8266 server
 */
ReturnType ESP8266_StartServer ( void )
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
		return Return_Ok;
	}
	else
	{
		// ERROR
		ESP8266_LED_FAIL();
		return Return_Error;
	}
	
	// Not reached
	//return Return_Ok;
}
#endif


/**
 * \brief	Sending HomeAutMessage
 *			for example:
 *			|HomeAut|010|014|LOGIN__|NMEDIU00000000|
 *			length: 40
 */
ReturnType ESP8266_SendTcpMessage(char *message)
{
	uint8_t length = 0;
	
	if (StringLength(message) !=  ESP8266_TCP_MESSAGE_LENGTH)
	{		
		return Return_Error;
	}
	else
	{	
		length = ESP8266_TCP_MESSAGE_LENGTH;
	}
	
	// TODO: Should be changeable length

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
	
	ESP8266_ReceiveString(StringLength("AT+CIPSEND=0,40\r\r\n> "));
	
	ESP8266_SendString("AT+CIPSEND=0,40\r\n");
	
	ESP8266_WaitAnswer();
	
	
	if (!StrCmp("AT+CIPSEND=0,40\r\r\n> ",(const char *)ESP8266_ReceiveBuffer))
	{
		ESP8266_LED_OK();
	}
	else
	{
		ESP8266_LED_FAIL();
		
		return Return_Error;
	}
	
	
	// GET / HTTP/1.0\r\n\r\n
	// Response:
	// SEND OK

	// TODO: do with parameter
	// message

	//ESP8266_ReceiveString((unsigned char *)ESP8266_ReceiveBuffer,StringLength((uint8_t *)"|HomeAut|010|014|LOGIN__|NMEDIU00000000|\r\r\r\nSEND OK\r\n"));
	ESP8266_ReceiveString(length + StringLength("\r\nSEND OK\r\n"));
	
	//ESP8266_SendString((uint8_t *)"|HomeAut|010|014|LOGIN__|NMEDIU00000000|\r\n\r\n");
	char buffer[60];
	StrCpy(buffer,message);
	//StrAppend(buffer,(uint8_t *)"\r\n\r\n");
	ESP8266_SendString(buffer);
	// TODO: Why need \r\n, after fix size data?
	
	ESP8266_WaitAnswer();
	
	
	//if (!StrCmp("|HomeAut|010|014|LOGIN__|NMEDIU00000000|\r\r\r\nSEND OK\r\n",(const char *)ESP8266_ReceiveBuffer))
	StrCpy(buffer,message);
	StrAppend(buffer,"\r\nSEND OK\r\n");
	if (!StrCmp((const char *)buffer,(const char *)ESP8266_ReceiveBuffer))
	{
		ESP8266_LED_OK();
	}
	else
	{
		ESP8266_LED_FAIL();
		
		return Return_Error;
	}
	
	return Return_Ok;
}



/**
 *\brief		Receive HomeAutMessage
 *				length: 10+40
 *				example: "+IPD,0,40:" + "|HomeAut|010|014|LOGIN__|NMEDIU00000000|"
 */
ReturnType ESP8266_ReceiveFixTcpMessage ( void )
{

	
	ESP8266_ClearReceiveBuffer();
	
	// Old:
	// +IPD,0,18:GET / HTTP/1.0
	// +IPD,0,18:<message>
	
	// HomeAutMessage:
	// "\r\n"											2
	// "+IPD,0,40:"										10
	// "|HomeAut|010|014|LOGIN__|NMEDIU00000000|"		40
	// "\r\nOK\r\n"										6
	// length: 2+10+40+6 = 58
	ESP8266_ReceiveString(ESP8266_HOMEAUT_RECEIVING_MESSAGE_LENGTH );
		
	return Return_Ok;
}



#ifdef CONFIG_USE_FREERTOS
/**
 * \brief	Switch to unknown length Receive mode
 */
static ReturnType ESP8266_ReceiveUnknownTcpMessage(void)
{
	// usart IT-ben váltás, egyenként fogunk karaktert fogadni:
	ESP8266_Receive_Mode_FixLength = 0;
	
	// Clear buffer
	ESP8266_ClearReceiveBuffer();
	
	// BufferCnt = 0;
	ESP8266_ReceiveBuffer_Cnt = 0;
	
	// Start receive
	HAL_UART_Receive_IT(&ESP8266_UartHandle,(uint8_t *)&ESP8266_ReceiveBuffer[0],1);

	// Wait message
	//ESP8266_WaitAnswer();	// Only wait ...
	ESP8266_WaitMessageAndCheckSendingQueue();		// wait for received message or if have to sending message
	
	return Return_Ok;
}
#endif


/*
// It not used
TODO: delete
ReturnType ESP8266_CheckReceiveMessage ( void )
{
	
	#ifdef CONFIG_USE_FREERTOS
	ESP8266_WaitAnswer();	// FreeRTOS blocking (with semaphore)
	return Retrn_Ok;
	return
	#else
	// without semaphore, it is not blocking
	if (ESP8266_Uart_ReceivedCharFlag == 1)
	{
		return Return_Ok;
	}
	else
	{
		return Return_Error;
	}
	#endif
	
}
*/



/**
 * \brief	Send string to ESP8266
 */
void ESP8266_SendString(char *aTxBuffer)
{
	
	uint8_t length = StringLength(aTxBuffer);

	if (HAL_UART_Transmit(&ESP8266_UartHandle, (uint8_t*)aTxBuffer, length,5000) != HAL_OK)
	{
		Error_Handler();
	}
}



/**
 * \brief	Wait/Receive string
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
	uint8_t i;
	for (i =0; i < ESP8266_BUFFER_LENGTH; i++)
	{
		ESP8266_ReceiveBuffer[i] = '\0';
	}
	
}



/**
 *\brief		Wait for answer in blocking mode
 */
void ESP8266_WaitAnswer(void)
{
	#ifdef CONFIG_USE_FREERTOS
	while(1)
	{
		if (xSemaphoreTake(ESP8266_USART_Rx_Semaphore, (portTickType) 10000) == pdTRUE)
		{
			// Successful take, return
			return;
		}
	}

	#else
	while (ESP8266_Uart_ReceivedCharFlag != 1);
	#endif
	
}


/**
 * \brief		Wait receive message, or transmit queue...
 */
static void ESP8266_WaitMessageAndCheckSendingQueue(void)
{

	while(1)
	{
		
		// Received an ended string?
		if ( xSemaphoreTake(ESP8266_USART_Rx_Semaphore, (portTickType) 500) == pdTRUE )
		{
			// Received a message
			return;
		}
		// If not received a lot of char...
		else if ( ESP8266_ReceiveBuffer_Cnt < 2 )
		{
			// Need to sending?
			if ( xQueueReceive( ESP8266_SendMessage_Queue, (void * )ESP8266_TransmitBuffer, ( portTickType )0 ) == pdTRUE  )
			{
				// Sending...
				ESP8266_Receive_Mode_FixLength = 1;	// fix length, bevause receing "> "
				ESP8266_TransmitBuffer[40] = '\0';	// end of buffer need to put an end char
				
				// Clear buffer and etc
				// Delete previous receive:
				__HAL_UART_FLUSH_DRREGISTER(&ESP8266_UartHandle);
				__HAL_UART_CLEAR_FLAG(&ESP8266_UartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);

				ESP8266_UartHandle.ErrorCode = HAL_UART_ERROR_NONE;
				ESP8266_UartHandle.gState = HAL_UART_STATE_READY;
				ESP8266_UartHandle.TxXferCount = 0;
				ESP8266_UartHandle.TxXferSize = 0;
				ESP8266_UartHandle.RxXferCount = 0;
				ESP8266_UartHandle.RxXferSize = 0;
								
				
				// Sending
				if ( ESP8266_SendTcpMessage((char *)ESP8266_TransmitBuffer) == Return_Ok )
				{
					DebugPrint("Successful sended a message:\r\n");
					DebugPrint((const char *)ESP8266_TransmitBuffer);
					DebugPrint("\r\n");
				}
				else
				{	
					DebugPrint("Failed sending message:\r\n");
					DebugPrint((const char *)ESP8266_TransmitBuffer);
					DebugPrint("\r\n");
				}

				
				// Reset the receive buffer:
				ESP8266_ReceiveBuffer_Cnt = 0;
				ESP8266_Receive_Mode_FixLength = 0;
				
				// Delete previous receive:
				//__HAL_UART_CLEAR_FLAG(&ESP8266_UartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
				//__HAL_UART_FLUSH_DRREGISTER(&ESP8266_UartHandle);
				/*
				ESP8266_UartHandle.ErrorCode = HAL_UART_ERROR_NONE;
				ESP8266_UartHandle.State = HAL_UART_STATE_READY;
				ESP8266_UartHandle.TxXferCount = 0;
				ESP8266_UartHandle.TxXferSize = 0;
				ESP8266_UartHandle.RxXferCount = 0;
				ESP8266_UartHandle.RxXferSize = 0;
				*/
				
				// Start receive
				HAL_UART_Receive_IT(&ESP8266_UartHandle,(uint8_t *)&ESP8266_ReceiveBuffer[0],1);
			}
		
		}
		else
		{
			// Do nothing
		}
	}
	
}



/**
 * \brief	TODO
 */
ReturnType ESP8266_SendMessageToQueue ( uint8_t *message )
{
	// Sending
	if( ESP8266_SendMessage_Queue != 0 )
	{

		if ( xQueueSend ( ESP8266_SendMessage_Queue, ( void * )message, ( portTickType ) 1000) != pdPASS )
		{
			return Return_Ok;
		}
		else
		{
			return Return_Error;
		}
	}
	else
	{
		return Return_Error;
	}
	
}



/**
 *\brief		Wait for client connect
 */
#if 0
// TODO: delete
ReturnType ESP8266_WaitClientConnect( void)
{
	
	ESP8266_ClearReceiveBuffer();
	
	// Link\r\n
	// \r\n
	ESP8266_ReceiveString(StringLength("Link\r\n") );
		
	// wait for message
	ESP8266_WaitAnswer();
	
	return Return_Ok;
}
#endif



#if (CONFIG_ESP8266_IS_SERVER == 0)
/**
 * \brief	Connect to server, with blocking
 */
ReturnType ESP8266_ClientConnectBlocking(void)
{

	ReturnType successfulConnected = Return_Error;
	
	// Receive type
	ESP8266_Receive_Mode_FixLength = 1;
	
	
	while (successfulConnected != Return_Ok)
	{
		// Connect to server
		successfulConnected = ESP8266_ConnectToServer();
	
		if (successfulConnected == Return_Ok)
		{
			// Successful connected
			ESP8266_ConnectionStatus = ESP8266_ConnectionStatus_SuccessfulConnected;
			DebugPrint("Successful connected to server\r\n");
		}
		else
		{
			// Failed to connect
			DebugPrint("Failed connected to server\r\n");
			DelayMs(10000);
			DebugPrint("Retry to connect...\r\n");
			
			// Clean and reset...
			// ERROR\r\nUnli [nk] // miatt kell... TODO: valami jobb megoldás kéne...
			
			// TODO: Valami szebb megoldás legyen itt
			// Clear buffer and etc
			// Delete previous receive:
			__HAL_UART_FLUSH_DRREGISTER(&ESP8266_UartHandle);
			__HAL_UART_CLEAR_FLAG(&ESP8266_UartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);

			ESP8266_UartHandle.ErrorCode = HAL_UART_ERROR_NONE;
			//ESP8266_UartHandle. = HAL_UART_STATE_READY;
			ESP8266_UartHandle.TxXferCount = 0;
			ESP8266_UartHandle.TxXferSize = 0;
			ESP8266_UartHandle.RxXferCount = 0;
			ESP8266_UartHandle.RxXferSize = 0;
								
		}	
	}
	
	// Successful Connected ...
	
	// Send Login message
	/*
	uint8_t myIp = ESP8266_MyIpAddress;

	// TODO:
	HOMEAUTMESSAGE_CreateAndSendHomeAutMessage(
		myIp,ESP8266_SERVER_IP_ADDRESS_SHORT,
		Function_Login,Login_ImLoginImNodeMedium,0,1);
	*/
	
	DelayMs(1000);
	
	
	return Return_Ok;
}
#endif



#if (CONFIG_ESP8266_IS_SERVER == 1)
ReturnType ESP8266_StartServerBlocking(void)
{
	// Start server
	while ( ESP8266_StartServer() != Return_Ok)
	{
		DebugPrint("Error! Server isn't started\r\n");
		DelayMs(10000);
	}
	
	ESP8266_ConnectionStatus = ESP8266_ConnectionStatus_SuccessfulServerStarted;
	DebugPrint("Server has started\r\n");

	return Return_Ok;
	
}
#endif



/**
 * \brief	Convert IP string to IP number
 */
static ReturnType ESP8266_ConvertIpString(char *message, uint8_t *ip)
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
		return Return_Error;
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
					ip[i] = (uint8_t)convertValue;
				}
				else
				{
					// Error, Too large value
					return Return_Error;
				}
			}
			else
			{
				// Failed convert to number
				return Return_Error;
			}

		}

		return Return_Ok;
	}
	else
	{
		// Failed separate string
		return Return_Error;
	}

}



#ifdef CONFIG_USE_FREERTOS
/**
 * \brief	ESP8266 Task
 * \note	It will go infinite loop, does not return
 */
void ESP8266_Task(void)
{

	// For received message checking
	ReturnType isValidMessage = Return_Error;

	
	// First, need initialize ESP8266's pins, with	ESP8266_Init();
	// is in main.c
	
	
	// Delay, for hardware initializing
	DelayMs(10000);
	
	
	// Minél késobb kell inicializálni az USART-ot, mert az ESP8266 induláskor sok üzenetet küld, amitol kifagy a handler.
	//(ErrorCallback hívódik meg)
	USART_Init(&ESP8266_UartHandle);
	
	
	DelayMs(100);
	
	
	// Config ESP8266
	// TODO: Check ReturnType ...
	ESP8266_Config();

	// Wait
	DelayMs(5000);
	
	
	// Connect to WiFi
	while (ESP8266_ConnectToWifiNetwork() == Return_Error)
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
		if (ESP8266_ConnectionStatus != ESP8266_ConnectionStatus_SuccessfulConnected)
		{
			// TODO:Find server
			
			// Connect
			ESP8266_ClientConnectBlocking();

		}
		#else
		#error "CONFIG_ESP8266_IS_SERVER define isnt correct."
		#endif
		
		// END OF Connect to server or start server

		

		/////////////////////////////////////
		// Receive any message + Sending:
		/////////////////////////////////////
		ESP8266_ReceiveUnknownTcpMessage();
		
		
		/////////////////////////////////////
		//		Check received message
		/////////////////////////////////////
		
		// Print received message:
		DebugPrint("\r\nReceived a message\r\n");
		// TODO: ha ezt most kiveszem, akkor nem tudja feldolgozni az elso uzenet kivetelevel a többit...
		//DebugPrint((char *)&ESP8266_ReceiveBuffer[ESP8266_HOMEAUTMESSAGE_RECEIVEDMESSAGE_START]);
		//DebugPrint((const char *)&ESP8266_ReceiveBuffer[0]);
		//DebugPrint("\r\n");
				
		
		// HomeAutMessage:
		// "\r\n"											2
		// "+IPD,0,40:"										10
		// "|HomeAut|010|014|LOGIN__|NMEDIU00000000|"		40
		// For HomeAutMessage
		if (ESP8266_ReceiveBuffer_Cnt >= 50)
		{
			//isValidMessage = OMEAUTMESSAGE_CompareMessage((uint8_t *)&ESP8266_ReceiveBuffer[ESP8266_HOMEAUTMESSAGE_RECEIVEDMESSAGE_START]);
			isValidMessage = Return_Ok;
			if (isValidMessage == Return_Ok)
			{
				DebugPrint("Valid HomeAut message received:\r\n");
				DebugPrint((char *)&ESP8266_ReceiveBuffer[ESP8266_HOMEAUTMESSAGE_RECEIVEDMESSAGE_START]);	
				if (xQueueSend(ESP8266_ReceivedMessage_Queue,&ESP8266_ReceiveBuffer[ESP8266_HOMEAUTMESSAGE_RECEIVEDMESSAGE_START],1000) == pdPASS)
				{
					// Successful sended to queue
				}
				else
				{
					// Failed sended to queue
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
			isValidMessage = Return_Ok;
			
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
			isValidMessage = Return_Ok;
			
			// TODO: Process disconnecting, delete IP from list
			#if ( CONFIG_ESP8266_IS_SERVER == 1)
			DebugPrint("A client disconnected.\r\n");
			#else
			DebugPrint("Disconnected. Need to reconnect\r\n");
			ESP8266_ConnectionStatus = ESP8266_ConnectionStatus_ClosedConnection;
			#endif
		}
		else if (!StrCmpWithLength((const char *)ESP8266_ReceiveBuffer, "OK", 2))
		{
			// Received: "OK"
			// Note: This arrived after received an x length message (now, after 40 length homeautmessage)
			isValidMessage = Return_Ok;
			DebugPrint("Received an OK\r\n");
		}
		else if (!StrCmpWithLength((const char *)ESP8266_ReceiveBuffer, "", 1))
		{
			// Received: ""
			// Note: This arrived after received an x length message (now, after 40 length homeautmessage) + "\r\n" after ""
			isValidMessage = Return_Ok;
			DebugPrint("Received an empty string\r\n");
		}		
		else
		{
			// Not valid message
			isValidMessage = Return_Error;
			DebugPrint((char *)&ESP8266_ReceiveBuffer[0]);
			DebugPrint("Not valid message.\r\n");
		}
	
		
		// Received an correct message?
		if (isValidMessage == Return_Ok)
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
	// END OF while(1)


}
#endif	// #ifdef CONFIG_USE_FREERTOS



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
