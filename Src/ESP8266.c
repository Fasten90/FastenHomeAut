/*******************************************************************************
 * Purpose:
 ******************************************************************************/

/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "ESP8266.h"
#include "include.h"


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


char ESP8266_MyIpAddressString[4];
uint8_t ESP8266_MyIpAddressDecimal = 0;

uint8_t ESP8266_Receive_Mode_FixLength = 1;
uint8_t ESP8266_ReceiveBuffer_Cnt = 0;

uint8_t ESP8266_DebugEnableFlag = 1;


#ifdef CONFIG_USE_FREERTOS
xSemaphoreHandle ESP8266_USART_Rx_Semaphore;
xQueueHandle ESP8266_SendMessage_Queue;	// LOG queue
xQueueHandle ESP8266_ReceivedMessage_Queue;
#endif


// For server type (server=master, or not server=client=slave)
#ifdef CONFIG_USE_PANEL_CENTERPANEL
uint8_t ESP8266_ImServer = 1;
#define CONFIG_ESP8266_IS_SERVER	1
#else
uint8_t ESP8266_ImServer = 0;
#define CONFIG_ESP8266_IS_SERVER	0
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
static void DebugPrint( const char *debugString );
ReturnType ESP8266_ReceiveUnknownTcpMessage ( void );
void ESP8266_WaitMessageAndCheckSendingQueue(void);
;

/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/
extern void Error_Handler( void );
//extern ReturnType HOMEAUTMESSAGE_CompareMessage(uint8_t *messageString);		// TODO: delete


/***************************************************************************//**
 * @brief
 * @details
 * @param[in]
 * @param[out]
 * @return
 ******************************************************************************/
void ESP8266_Test ( void )
{
	
	


	/////////////////////////////////////////////

	GPIO_InitTypeDef  GPIO_InitStruct;

	ESP8266_PINS_CLK_ENABLES();
	

	
	// GPIO0 & GPIO2
	
	// help for pins: http://www.electrodragon.com/w/ESP8266
	// Mindketto HIGH szinten volt amikor megmértem
	// Igen, de GPIO0 az LOW volt induláskor és HIGH lett csatlakozás után ...


	GPIO_InitStruct.Pin       = ESP8266_GPIO2_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP ;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	//GPIO_InitStruct.Alternate =
	HAL_GPIO_Init(ESP8266_GPIO2_GPIO_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ESP8266_GPIO2_GPIO_PORT,ESP8266_GPIO2_GPIO_PIN,GPIO_PIN_SET);	


	//
	/*
	GPIO_InitStruct.Pin       = ESP8266_GPIO0_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_GPIO0_GPIO_PORT, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(ESP8266_GPIO0_GPIO_PORT,ESP8266_GPIO0_GPIO_PIN,GPIO_PIN_RESET);
	//
	*/
	
	//
	/*
	GPIO_InitStruct.Pin       = ESP8266_GPIO0_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_GPIO0_GPIO_PORT, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(ESP8266_GPIO0_GPIO_PORT,ESP8266_GPIO0_GPIO_PIN,GPIO_PIN_SET);
	//
	*/
	
	//
	/*
	GPIO_InitStruct.Pin       = ESP8266_GPIO0_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_GPIO0_GPIO_PORT, &GPIO_InitStruct);	
	//
	*/


	// RST

	///*
	GPIO_InitStruct.Pin       = ESP8266_RST_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_RST_GPIO_PORT, &GPIO_InitStruct);
	
	// Reset (low = reset active)
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_PORT,ESP8266_RST_GPIO_PIN,GPIO_PIN_RESET);	
	HAL_Delay(100);
	
	// Reset end (high = inactive)
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_PORT,ESP8266_RST_GPIO_PIN,GPIO_PIN_SET);
	HAL_Delay(1000);
	//*/
	
	/*
	// RST - input
	GPIO_InitStruct.Pin       = ESP8266_RST_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_RST_GPIO_PORT, &GPIO_InitStruct);
	*/
	

	// CH
	
	GPIO_InitStruct.Pin       = ESP8266_CH_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_CH_GPIO_PORT, &GPIO_InitStruct);	


	// CH UP
	HAL_GPIO_WritePin(ESP8266_CH_GPIO_PORT,ESP8266_CH_GPIO_PIN,GPIO_PIN_SET);
	
	HAL_Delay(2000);
	
	


	
	// Wait
	HAL_Delay(1000);
	
	
	// USART TX - RX	
	// Most kell inicializálni, különben a reset miatt kapunk egy csomó üzenetet, amitol kifagy az UART
	USART_Init(&ESP8266_UartHandle);
	

	HAL_Delay(100);
		
	/////////////////////////////////////////////

	//AT+RST



	//AT+ CIPSTATUS


	char Buffer[ESP8266_BUFFER_LENGTH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
		
		
		
	//ESP8266_Uart_ReceivedCharFlag = 0;
	// Inkább legyen a ReceiveString() függvényben
	
	
	
	
	/*
	ESP8266_ReceiveString(Buffer,StringLength((uint8_t *)"AT+RST\r\r\n\r\nOK\r\n"
		"..................................................."));
		
	ESP8266_SendString((uint8_t*)"AT+RST\r\n");		// Reset	
	while(ESP8266_Uart_ReceivedCharFlag != 1);	
		
		
	HAL_Delay(1000);
	*/	
	
	
	
	
	//ESP8266_ReceiveString(Buffer);
	ESP8266_ReceiveString(Buffer,StringLength("AT\r\r\n\r\nOK\r\n"));
	// Buffer + waitedString
	
	ESP8266_SendString("AT\r\n");

	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	if (!StrCmp("AT\r\r\n\r\nOK\r\n",(const char *)Buffer)) LED_RED_ON();
	
	
	
	HAL_Delay(200);

		

	// CWMODE
	// 1 = station
	// 2 = host
	// 3 = dual 
	
	ESP8266_ReceiveString(Buffer,StringLength("AT+CWMODE=3\r\r\nno change\r\n"));
	// Buffer + waitedString
	
	ESP8266_SendString("AT+CWMODE=3\r\n");

	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	if (!StrCmp("AT+CWMODE=3\r\r\n\r\nOK\r\n",(const char *)Buffer))
	{
		LED_RED_OFF();
	}
	else if (!StrCmp("AT+CWMODE=3\r\r\nno change\r\n",(const char *)Buffer))
	{
		LED_RED_OFF();
	}
	else 
	{
		LED_RED_ON();
	}
	
	HAL_Delay(1000);

	
	
	
	
	
	// Test
	//ESP8266_ReceiveString(Buffer,StringLength((uint8_t *)"AT\r\r\n\r\nasdddddddddddddddddddddddddddddddOK\r\n"));
	
	// Check one from these:
	//ESP8266_SendString((uint8_t*)"AT+CWLAP\r\n");		// List
	//ESP8266_SendString((uint8_t*)"AT+CIPSTATUS\r\n");	// Chip status
	// now: 4
	//ESP8266_SendString((uint8_t*)"AT+GMR\r\n");		// Firmware version
	// now: 0018000902-AI03
	//ESP8266_SendString((uint8_t*)"AT+CWJAP?\r\n");	// Connection (saved) list
	//ESP8266_SendString((uint8_t*)"AT+RST\r\n");		// Reset
	
	
	//while(ESP8266_Uart_ReceivedCharFlag != 1);	
	
	// End of Listing test



	

	#ifdef USE_ESP8266_TEST_FASTENHOME
	/////////////////////////////////
	//		TEST  -  FASTENHOME
	/////////////////////////////////
	
	
	ESP8266_ReceiveString(Buffer,StringLength((uint8_t *)"AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString((uint8_t *)"AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\n");
	
	//ESP8266_WaitAndCheckString();
	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	if (!StrCmp("AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nOK\r\n",(const char *)Buffer))
	{
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		if (!StrCmp("AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nFAIL",(const char *)Buffer))
		{
			// AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nFAIL\r\n")
			LED_GREEN_ON();
			LED_RED_ON();
		}
		else if (!StrCmp("AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nERRO",(const char *)Buffer))
		{
			// AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nERROR\r\n")
			LED_GREEN_OFF();
			LED_RED_ON();
		}
	}
	
	

	
	
		
		
		
	// for client
	// AT+CIFSR	// Check IP address
		
	// for server ?
	// AT+CWLIF		// get IP's of devices connected to AP
	
	
	// Connection Type:
	// AT+CIPMUX=0		// Single
	// AT+CIPMUX=1		// Multiple



	ESP8266_BufferReset(Buffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(Buffer,StringLength((uint8_t *)"AT+CIPMUX=1\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString((uint8_t *)"AT+CIPMUX=1\r\n");
	
	//ESP8266_WaitAndCheckString();
	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	
	if (!StrCmp("AT+CIPMUX=1\r\r\n\r\nOK\r\n",(const char *)Buffer))
	{
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		LED_GREEN_OFF();
		LED_RED_ON();
	}
	
	
	
	
	
	// AT+CIPSTART=?
	// (CIPMUX=0) AT+CIPSTART = <type>,<addr>,<port>
	// (CIPMUX=1) AT+CIPSTART=  <id><type>,<addr>, <port>
	// AT+CIPSTART=4,"TCP","google.com",80
	
	// Response:
	// OK
	// Linked
	
	
	ESP8266_BufferReset(Buffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(Buffer,StringLength((uint8_t *)"AT+CIPSTART=0,\"TCP\",\"www.fasten.elindor.eu\",80\r\r\n\r\nOK\r\nLinked\r\n"));
	
	ESP8266_SendString((uint8_t *)"AT+CIPSTART=0,\"TCP\",\"www.fasten.elindor.eu\",80\r\n");
	
	//ESP8266_WaitAndCheckString();
	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	
	if (!StrCmp("AT+CIPSTART=0,\"TCP\",\"www.fasten.elindor.eu\",80\r\r\n\r\nOK\r\nLinked\r\n",(const char *)Buffer))
	{
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		LED_GREEN_OFF();
		LED_RED_ON();
	}
	
	
	
	// AT+CIPSEND=4,18
	// 18 byte to send: GET / HTTP/1.0\r\n\r\n
	
	
	// Response:
	// SEND OK
	
	
	ESP8266_BufferReset(Buffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(Buffer,StringLength((uint8_t *)"AT+CIPSEND=0,18\r\r\n> "));
	
	ESP8266_SendString((uint8_t *)"AT+CIPSEND=0,18\r\n");
	
	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	
	if (!StrCmp("AT+CIPSEND=0,18\r\r\n> ",(const char *)Buffer))
	{
		LED_GREEN_ON();
		LED_RED_OFF();

	}
	else
	{
		LED_GREEN_OFF();
		LED_RED_ON();
	}
	
	
	
	// GET / HTTP/1.0\r\n\r\n
	// Response:
	// SEND OK

	ESP8266_BufferReset(Buffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(Buffer,StringLength((uint8_t *)"GET / HTTP/1.0\r\r\r\nSEND OK\r\n\r\n"));
	
	ESP8266_SendString((uint8_t *)"GET / HTTP/1.0\r\n\r\n");
	
	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	if (!StrCmp("GET / HTTP/1.0\r\r\r\nSEND OK\r\n\r\n",(const char *)Buffer))
	{
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		LED_GREEN_OFF();
		LED_RED_ON();
	}

	
	
	// +IPD
	// (CIPMUX=0): + IPD, <len>:
	// (CIPMUX=1): + IPD, <id>, <len>: <data>
	ESP8266_BufferReset(Buffer, ESP8266_BUFFER_LENGTH);
	
	ESP8266_ReceiveString(Buffer,StringLength((uint8_t *)"IPD, 0, 200: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));
	
	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	
	#endif // #ifdef USE_ESP8266_TEST_FASTENHOME
	////////////////////////////////////////////////
	
	
	
	

	
	
	
	
	///////////////////////////////////////////
	//		CLIENT 
	
	#ifdef USE_ESP8266_MODE_CLIENT
	
	
	// Join to wifi network
	// AT+CWJAP
	
	
	ESP8266_ReceiveString(Buffer,StringLength("AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString("AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\n");
	
	//ESP8266_WaitAndCheckString();
	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	if (!StrCmp("AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nOK\r\n",(const char *)Buffer))
	{
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		if (!StrCmp("AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nFAIL",(const char *)Buffer))
		{
			// AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nFAIL\r\n")
			LED_GREEN_ON();
			LED_RED_ON();
		}
		else if (!StrCmp("AT+CWJAP=\"ESP8266HomeAutomation\",\"AUT\"\r\r\n\r\nERRO",(const char *)Buffer))
		{
			// AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nERROR\r\n")
			LED_GREEN_OFF();
			LED_RED_ON();
		}
	}
	
	
	


	// Connection Type:
	// AT+CIPMUX=0		// Single
	// AT+CIPMUX=1		// Multiple



	ESP8266_BufferReset(Buffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(Buffer,StringLength("AT+CIPMUX=1\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString("AT+CIPMUX=1\r\n");
	
	//ESP8266_WaitAndCheckString();
	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	
	if (!StrCmp("AT+CIPMUX=1\r\r\n\r\nOK\r\n",(const char *)Buffer))
	{
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		LED_GREEN_OFF();
		LED_RED_ON();
	}
	
	
	
	// AT+CIPSTART=?
	// (CIPMUX=0) AT+CIPSTART = <type>,<addr>,<port>
	// (CIPMUX=1) AT+CIPSTART=  <id><type>,<addr>, <port>
	// AT+CIPSTART=4,"TCP","google.com",80
	
	// Response:
	// OK
	// Linked
	
	
	ESP8266_BufferReset(Buffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(Buffer,StringLength("AT+CIPSTART=0,\"TCP\",\"9.6.5.13\",80\r\r\n\r\nOK\r\nLinked\r\n"));
	
	ESP8266_SendString("AT+CIPSTART=0,\"TCP\",\"9.6.5.13\",80\r\n");
	
	//ESP8266_WaitAndCheckString();
	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	
	if (!StrCmp("AT+CIPSTART=0,\"TCP\",\"9.6.5.13\",80\r\r\n\r\nOK\r\nLinked\r\n",(const char *)Buffer))
	{
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		LED_GREEN_OFF();
		LED_RED_ON();
	}
	
	
	
	// AT+CIPSEND=4,18
	// 18 byte to send: GET / HTTP/1.0\r\n\r\n
	
	
	// Response:
	// SEND OK
	
	
	ESP8266_BufferReset(Buffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(Buffer,StringLength("AT+CIPSEND=0,18\r\r\n> "));
	
	ESP8266_SendString("AT+CIPSEND=0,18\r\n");
	
	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	
	if (!StrCmp("AT+CIPSEND=0,18\r\r\n> ",(const char *)Buffer))
	{
		LED_GREEN_ON();
		LED_RED_OFF();

	}
	else
	{
		LED_GREEN_OFF();
		LED_RED_ON();
	}
	
	
	
	// GET / HTTP/1.0\r\n\r\n
	// Response:
	// SEND OK

	ESP8266_BufferReset(Buffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(Buffer,StringLength("GET / HTTP/1.0\r\r\r\nSEND OK\r\n"));
	
	ESP8266_SendString("GET / HTTP/1.0\r\n\r\n");
	
	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	if (!StrCmp("GET / HTTP/1.0\r\r\r\nSEND OK\r\n",(const char *)Buffer))
	{
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		LED_GREEN_OFF();
		LED_RED_ON();
	}
	
	
	
	#endif	// #ifdef USE_ESP8266_MODE_CLIENT
	//
	///////////////////////////////////
	
	
	
	
	// Receiving:
	// +IPD,0,18:GET / HTTP/1.0
	
	

	// AT+CIPCLOSE
	// AT+CIPCLOSE=<id>
	

	
	
	// SZERVER
	#ifdef USE_WIFI_ESP8266_SERVER
	
	
	//AT+CWSAP="ESP8266HomeAutomation","AUT",3,0
	// 3. param = channel
	// 4. param = encryption
	

	// Connected devies:
	//AT+CWLIF
	
	
	
	
	//AT+CIPSERVER=1,80
	
	
	
	#endif

		
		
	
	
	// Tesztelésre
	while(1);
	
	
	
}










ReturnType ESP8266_Init ( void )
{
	

	/////////////////////////////////////////////
	//		GPIO INIT

	GPIO_InitTypeDef  GPIO_InitStruct;

	ESP8266_PINS_CLK_ENABLES();
	

	
	// GPIO0 & GPIO2
	
	// help for pins: http://www.electrodragon.com/w/ESP8266
	// Mindketto HIGH szinten volt amikor megmértem
	// Igen, de GPIO0 az LOW volt induláskor és HIGH lett csatlakozás után ...


	GPIO_InitStruct.Pin       = ESP8266_GPIO2_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP ;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	//GPIO_InitStruct.Alternate =
	HAL_GPIO_Init(ESP8266_GPIO2_GPIO_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ESP8266_GPIO2_GPIO_PORT,ESP8266_GPIO2_GPIO_PIN,GPIO_PIN_SET);	


	//
	/*
	GPIO_InitStruct.Pin       = ESP8266_GPIO0_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_GPIO0_GPIO_PORT, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(ESP8266_GPIO0_GPIO_PORT,ESP8266_GPIO0_GPIO_PIN,GPIO_PIN_RESET);
	//
	*/
	
	//
	/*
	GPIO_InitStruct.Pin       = ESP8266_GPIO0_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_GPIO0_GPIO_PORT, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(ESP8266_GPIO0_GPIO_PORT,ESP8266_GPIO0_GPIO_PIN,GPIO_PIN_SET);
	//
	*/
	
	//
	/*
	GPIO_InitStruct.Pin       = ESP8266_GPIO0_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_GPIO0_GPIO_PORT, &GPIO_InitStruct);	
	//
	*/


	// RST

	///*
	GPIO_InitStruct.Pin       = ESP8266_RST_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_RST_GPIO_PORT, &GPIO_InitStruct);
	
	// Reset (low = reset active)
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_PORT,ESP8266_RST_GPIO_PIN,GPIO_PIN_RESET);	
	//HAL_Delay(100);

	
	
	// Reset end (high = inactive)
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_PORT,ESP8266_RST_GPIO_PIN,GPIO_PIN_SET);
	//HAL_Delay(1000);



	/*
	// RST - input
	GPIO_InitStruct.Pin       = ESP8266_RST_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_RST_GPIO_PORT, &GPIO_InitStruct);
	*/
	

	// CH
	
	GPIO_InitStruct.Pin       = ESP8266_CH_GPIO_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(ESP8266_CH_GPIO_PORT, &GPIO_InitStruct);	


	// CH UP
	HAL_GPIO_WritePin(ESP8266_CH_GPIO_PORT,ESP8266_CH_GPIO_PIN,GPIO_PIN_SET);
	
	//HAL_Delay(2000);
	
	


	
	// Wait
	//HAL_Delay(1000);
	
	
	// USART TX - RX	
	// Most kell inicializálni, különben a reset miatt kapunk egy csomó üzenetet, amitol kifagy az UART
	//USART_Init(&ESP8266_UartHandle);
	

	//HAL_Delay(100);
		
	/////////////////////////////////////////////



	//AT+RST	
	/*
	ESP8266_ReceiveString(Buffer,StringLength((uint8_t *)"AT+RST\r\r\n\r\nOK\r\n"
		"..................................................."));
		
	ESP8266_SendString((uint8_t*)"AT+RST\r\n");		// Reset	
	while(ESP8266_Uart_ReceivedCharFlag != 1);	
		
		
	HAL_Delay(1000);
	*/	
	
	
	
	
	/*
	ESP8266_ReceiveString((unsigned char *)ESP8266_ReceiveBuffer,StringLength((uint8_t *)"AT\r\r\n\r\nOK\r\n"));
	// Buffer + waitedString
	
	ESP8266_SendString((uint8_t*)"AT\r\n");

	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	if (!StrCmp("AT\r\r\n\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer)) LED_RED_ON();
	
	
	
	HAL_Delay(200);

		

	// CWMODE
	// 1 = station
	// 2 = host
	// 3 = dual 
	
	ESP8266_ReceiveString((unsigned char *)ESP8266_ReceiveBuffer,StringLength((uint8_t *)"AT+CWMODE=3\r\r\nno change\r\n"));
	// Buffer + waitedString
	
	ESP8266_SendString((uint8_t*)"AT+CWMODE=3\r\n");

	while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	if (!StrCmp("AT+CWMODE=3\r\r\n\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
	{
		LED_RED_OFF();
	}
	else if (!StrCmp("AT+CWMODE=3\r\r\nno change\r\n",(const char *)ESP8266_ReceiveBuffer))
	{
		LED_RED_OFF();
	}
	else 
	{
		LED_RED_ON();
	}
	
	
	HAL_Delay(1000);
	*/
	
	return Return_Ok;
	
}




ReturnType ESP8266_Config ( void )
{
	
	ESP8266_ReceiveString(ESP8266_ReceiveBuffer,StringLength("AT\r\r\n\r\nOK\r\n"));
	// Buffer + waitedString
	
	ESP8266_SendString("AT\r\n");

	ESP8266_WaitAnswer();

	
	if (!StrCmp("AT\r\r\n\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
	{
		LED_RED_ON();
	}
	
	
	//HAL_Delay(200);
	vTaskDelay(1000);
		

	
	// CWMODE
	// 1 = station
	// 2 = host
	// 3 = dual 
	
	ESP8266_ReceiveString(ESP8266_ReceiveBuffer,StringLength("AT+CWMODE=3\r\r\nno change\r\n"));
	// Buffer + waitedString
	
	ESP8266_SendString("AT+CWMODE=3\r\n");

	//while(ESP8266_Uart_ReceivedCharFlag != 1);
	ESP8266_WaitAnswer();
	
	if (!StrCmp("AT+CWMODE=3\r\r\n\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
	{
		// Ok 
		LED_RED_OFF();
	}
	else if (!StrCmp("AT+CWMODE=3\r\r\nno change\r\n",(const char *)ESP8266_ReceiveBuffer))
	{
		// no change
		LED_RED_OFF();
	}
	else 
	{
		LED_RED_ON();
	}
	
	
	return Return_Ok;	
}




ReturnType ESP8266_ConnectToWifiNetwork ( void )
{
	
	
	/////////////////////////
	// Join to wifi network
	// AT+CWJAP
	/////////////////////////
	
	
	ESP8266_ReceiveString(ESP8266_ReceiveBuffer,StringLength("AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString("AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\n");
	
	ESP8266_WaitAnswer();
	
	if (!StrCmp("AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
	{
		// Ok
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else if (!StrCmp("AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nFAIL",(const char *)ESP8266_ReceiveBuffer))
	{
		// Fail
		// AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nFAIL\r\n")
		LED_GREEN_ON();
		LED_RED_ON();
		return Return_False;
	}
	else if (!StrCmp("AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nERRO",(const char *)ESP8266_ReceiveBuffer))
	{
		// Error
		// AT+CWJAP=\"Fastenwifi\",\"fastenhome\"\r\r\n\r\nERROR\r\n")
		LED_GREEN_OFF();
		LED_RED_ON();
		return Return_False;
	}
	
	
	
	///////////////////////////////////////////////
	// AT+CIPMUX = 	set multiple connections mode
	// Connection Type:
	// AT+CIPMUX=0		// Single
	// AT+CIPMUX=1		// Multiple
	//////////////////////////////////////////////


	ESP8266_BufferReset(ESP8266_ReceiveBuffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(ESP8266_ReceiveBuffer,StringLength("AT+CIPMUX=1\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString("AT+CIPMUX=1\r\n");
	
	//ESP8266_WaitAndCheckString();
	//while(ESP8266_Uart_ReceivedCharFlag != 1);
	ESP8266_WaitAnswer();
	
	if (!StrCmp("AT+CIPMUX=1\r\r\n\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
	{
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		LED_GREEN_OFF();
		LED_RED_ON();
		return Return_False;
	}
	
	
	////////////////////////////////////////////
	// Save IP address to ESP8266_MyIpAddressString
	// AT+CIFSR
	// IP cím
	// Command:
	// AT+CIFSR
	// Response:	AT+CIFSR 192.168.0.106\r\nOK
	////////////////////////////////////////////
	
	ESP8266_BufferReset(ESP8266_ReceiveBuffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(ESP8266_ReceiveBuffer,StringLength("AT+CIFSR\r\r\n192.168.0.1\r\n9.6.5.10\r\n\r\nOK\r\n"));
	
	ESP8266_SendString("AT+CIFSR\r\n");
	
	ESP8266_WaitAnswer();
	
	uint8_t point = 0;
	uint8_t IPcnt = 0;
	const uint8_t selected_point = 6;		// TODO: ... ennél jobb IP beolvasás kéne
	for (int i=0; ESP8266_ReceiveBuffer[i] != '\0'; i++ )
	{
		if (ESP8266_ReceiveBuffer[i] == '.')
		{
			point++;
		}
		else if ( point == selected_point && ESP8266_ReceiveBuffer[i] >= '0' && ESP8266_ReceiveBuffer[i] <= '9' && IPcnt < 3 )
		{
			ESP8266_MyIpAddressString[IPcnt++] = ESP8266_ReceiveBuffer[i];
		}
		else if ( point == selected_point && ESP8266_ReceiveBuffer[i] == '\r' )
		{
			ESP8266_MyIpAddressString[IPcnt] = '\0';
			break;
		}
		else if (IPcnt >= 3)
		{
			// End char
			ESP8266_MyIpAddressString[3] = '\0';
			break;
		}	
	}
	// IP address had saved
	
	// Convert IP address (string) to decimal
	uint32_t ip;
	if (!UnsignedDecimalStringToNum(ESP8266_MyIpAddressString,&ip))
	{
		// Failed to convert
		return Return_False;
	}
	ESP8266_MyIpAddressDecimal = (uint8_t)ip;
	
	// Send to debug uart
	/*
	DebugPrint("My IP address:\r\n");
	DebugPrint((char *)ESP8266_MyIpAddressString);
	DebugPrint("\r\n");
	*/
	if (ESP8266_DebugEnableFlag == 1)
	{
		uprintf("My IP address: %d\r\n",ESP8266_MyIpAddressDecimal);
	}
	
	
	
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



ReturnType ESP8266_FindServer ( void )
{
	
	
	////////////////////////
	//	Connected devies:
	//	AT+CWLIF
	////////////////////////
	
	ESP8266_BufferReset(ESP8266_ReceiveBuffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(ESP8266_ReceiveBuffer,StringLength("AT+CWLIF\r\r\n\r\nOK\r\nLinked\r\n"));
	
	ESP8266_SendString("AT+CWLIF\r\n");
	
	ESP8266_WaitAnswer();
	
	
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
		return Return_False;
	}
	
	//return Return_Ok;
}



#if ( CONFIG_ESP8266_IS_SERVER == 0)
ReturnType ESP8266_ConnectToServer ( void )
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
	
	
	ESP8266_BufferReset(ESP8266_ReceiveBuffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(ESP8266_ReceiveBuffer,StringLength("AT+CIPSTART=0,\"TCP\",\"9.6.5.10\",2000\r\r\n\r\nOK\r\nLinked\r\n"));
	
	ESP8266_SendString("AT+CIPSTART=0,\"TCP\",\"9.6.5.10\",2000\r\n");
	
	ESP8266_WaitAnswer();
	
	
	if (!StrCmp("AT+CIPSTART=0,\"TCP\",\"9.6.5.10\",2000\r\r\n\r\nOK\r\nLinked\r\n",(const char *)ESP8266_ReceiveBuffer))
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
		return Return_False;
	}
	
	
	//return Return_Ok;
	
}
#endif



#if ( CONFIG_ESP8266_IS_SERVER == 1)
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
	
	
	ESP8266_BufferReset(ESP8266_ReceiveBuffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(ESP8266_ReceiveBuffer,StringLength("AT+CIPSERVER=1,2000\r\r\n\r\nOK\r\n"));
	
	ESP8266_SendString("AT+CIPSERVER=1,2000\r\n");
	
	ESP8266_WaitAnswer();
	
	
	if (!StrCmp("AT+CIPSERVER=1,2000\r\r\n\r\nOK\r\n",(const char *)ESP8266_ReceiveBuffer))
	{
		// OK
		LED_GREEN_ON();
		LED_RED_OFF();
		return Return_Ok;
	}
	else
	{
		// ERROR
		LED_GREEN_OFF();
		LED_RED_ON();
		return Return_False;
	}
	
	// Not reached
	//return Return_Ok;
}
#endif


/*
\brief	Sending HomeAutMessage
for example:
|HomeAut|010|014|LOGIN__|NMEDIU00000000|
length: 40
*/
ReturnType ESP8266_SendTcpMessage ( char *message )
{
	uint8_t length = 0;
	
	// TODO: HOMEAUTMESSAGE_DefaultMessageLength define-nal kéne	// 40
	if ( StringLength(message) !=  40 )	
	{		
		return Return_False;
	}
	else
	{	
		length = 40;
	}
	
	// TODO: HOMEAUTMESSAGE_DefaultMessageLength
	
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
	
	ESP8266_BufferReset(ESP8266_ReceiveBuffer, ESP8266_BUFFER_LENGTH);
		
	ESP8266_ReceiveString(ESP8266_ReceiveBuffer,StringLength("AT+CIPSEND=0,40\r\r\n> "));
	
	ESP8266_SendString("AT+CIPSEND=0,40\r\n");
	
	ESP8266_WaitAnswer();
	
	
	if (!StrCmp("AT+CIPSEND=0,40\r\r\n> ",(const char *)ESP8266_ReceiveBuffer))
	{
		LED_GREEN_ON();
		LED_RED_OFF();

	}
	else
	{
		LED_GREEN_OFF();
		LED_RED_ON();
		
		return Return_False;
	}
	
	
	
	// GET / HTTP/1.0\r\n\r\n
	// Response:
	// SEND OK

	// TODO: do with parameter
	// message
	ESP8266_BufferReset(ESP8266_ReceiveBuffer, ESP8266_BUFFER_LENGTH);
		
	//ESP8266_ReceiveString((unsigned char *)ESP8266_ReceiveBuffer,StringLength((uint8_t *)"|HomeAut|010|014|LOGIN__|NMEDIU00000000|\r\r\r\nSEND OK\r\n"));
	ESP8266_ReceiveString(ESP8266_ReceiveBuffer, length + StringLength("\r\nSEND OK\r\n"));
	
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
		LED_GREEN_ON();
		LED_RED_OFF();
	}
	else
	{
		LED_GREEN_OFF();
		LED_RED_ON();
		
		return Return_False;
	}
	
	return Return_Ok;
}



/*
\brief		Receive HomeAutMessage
length: 10+40
example: "+IPD,0,40:" + "|HomeAut|010|014|LOGIN__|NMEDIU00000000|"
*/
ReturnType ESP8266_ReceiveFixTcpMessage ( void )
{

	
	ESP8266_BufferReset(ESP8266_ReceiveBuffer, ESP8266_BUFFER_LENGTH);
	
	// Old:
	// +IPD,0,18:GET / HTTP/1.0
	// +IPD,0,18:<message>
	
	// HomeAutMessage:
	// "\r\n"											2
	// "+IPD,0,40:"										10
	// "|HomeAut|010|014|LOGIN__|NMEDIU00000000|"		40
	// "\r\nOK\r\n"										6
	// length: 2+10+40+6 = 58
	ESP8266_ReceiveString(ESP8266_ReceiveBuffer, ESP8266_HOMEAUT_RECEIVING_MESSAGE_LENGTH );
		
	//while(ESP8266_Uart_ReceivedCharFlag != 1);
	
	return Return_Ok;
}



#ifdef CONFIG_USE_FREERTOS
ReturnType ESP8266_ReceiveUnknownTcpMessage ( void )
{
	// usart IT-ben váltás, egyenként fogunk karaktert fogadni:
	ESP8266_Receive_Mode_FixLength = 0;
	
	// Clear buffer
	ESP8266_BufferReset(ESP8266_ReceiveBuffer, ESP8266_BUFFER_LENGTH);
	
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
		return Return_False;
	}
	#endif
	
}
*/



void ESP8266_SendString (char *aTxBuffer)
{
	
	//USART_SendMessage(aTxBuffer);
	uint8_t length = StringLength(aTxBuffer);
	//HAL_UART_Transmit(&ESP8266_UartHandle, (uint8_t*)aTxBuffer, length,5000);
	if ( HAL_UART_Transmit(&ESP8266_UartHandle, (uint8_t*)aTxBuffer, length,5000) != HAL_OK)
	{
		Error_Handler();
	}
}



void ESP8266_ReceiveString (char *aRxBuffer, uint8_t length)
{
	ESP8266_Uart_ReceivedCharFlag = 0;
	
	//uint8_t length = 11;
	//uint8_t length = StringLength(waitedString);
	//HAL_UART_Receive(&ESP8266_UartHandle, (uint8_t *)aRxBuffer, length,1000);
	
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
	
	// RECEIVE
	HAL_UART_Receive_IT(&ESP8266_UartHandle, (uint8_t *)aRxBuffer, length);
	
}



void ESP8266_BufferReset (char *Buffer, uint8_t length)
{
	uint8_t i;
	for (i =0; i < ESP8266_BUFFER_LENGTH; i++)
	{
		Buffer[i] = '\0';
	}
	
}



/*
\brief		Wait for answer with blocking !!!
*/
void ESP8266_WaitAnswer(void)
{
	#ifdef CONFIG_USE_FREERTOS
	while(1)
	{
		
		if ( xSemaphoreTake(ESP8266_USART_Rx_Semaphore, (portTickType) 10000) == pdTRUE )
		{
			return;
		}
		else
		{
		}
	}

	#else
	while(ESP8266_Uart_ReceivedCharFlag != 1);
	#endif
	
}


/*
\brief		Wait receive message, or transmit queue...
*/
void ESP8266_WaitMessageAndCheckSendingQueue(void)
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
			return Return_False;
		}
	}
	else
	{
		return Return_False;
	}
	
}



/*
\brief		Wait for client connect
*/
#if 0
ReturnType ESP8266_WaitClientConnect( void)
{
	
	ESP8266_BufferReset(ESP8266_ReceiveBuffer, ESP8266_BUFFER_LENGTH);
	
	// Link\r\n
	// \r\n
	ESP8266_ReceiveString(ESP8266_ReceiveBuffer, StringLength("Link\r\n") );
		
	// wait for message
	ESP8266_WaitAnswer();
	
	return Return_Ok;
}
#endif


#if ( CONFIG_ESP8266_IS_SERVER == 0)
/*
\brief	Connect to server, with blocking
*/
ReturnType ESP8266_ClientConnectBlocking(void)
{

	ReturnType successfulConnected = Return_False;
	
	// Receive type
	ESP8266_Receive_Mode_FixLength = 1;
	
	
	while ( successfulConnected != Return_Ok)
	{
		// Connect to server
		successfulConnected = ESP8266_ConnectToServer();
	
		if ( successfulConnected == Return_Ok )
		{
			// Successfull connected
			ESP8266_ConnectionStatus = ESP8266_ConnectionStatus_SuccessfulConnected;
			DebugPrint("Successful connected to server\r\n");
		}
		else
		{
			// Failed to connect
			DebugPrint("Failed connected to server\r\n");
			vTaskDelay(10000);
			DebugPrint("Retry to connect...\r\n");
			
			// Clean and reset...
			// ERROR\r\nUnli [nk] // miatt kell... TODO: valami jobb megoldás kéne...
			
			// Clear buffer and etc
			// Delete previous receive:
			__HAL_UART_FLUSH_DRREGISTER(&ESP8266_UartHandle);
			__HAL_UART_CLEAR_FLAG(&ESP8266_UartHandle, UART_FLAG_CTS | UART_FLAG_RXNE | UART_FLAG_TXE | UART_FLAG_TC | UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);

			ESP8266_UartHandle.ErrorCode = HAL_UART_ERROR_NONE;
			ESP8266_UartHandle.State = HAL_UART_STATE_READY;
			ESP8266_UartHandle.TxXferCount = 0;
			ESP8266_UartHandle.TxXferSize = 0;
			ESP8266_UartHandle.RxXferCount = 0;
			ESP8266_UartHandle.RxXferSize = 0;
								
		}	
	}
	
	// Successfull Connected ...
	
	// Send Login message
	uint8_t myIp = ESP8266_MyIpAddressDecimal;
	HOMEAUTMESSAGE_CreateAndSendHomeAutMessage(
		myIp,ESP8266_SERVER_IP_ADDRESS_SHORT,
		Function_Login,Login_ImLoginImNodeMedium,0,1);
	
	vTaskDelay(1000);
	
	
	return Return_Ok;
}
#endif


#if ( CONFIG_ESP8266_IS_SERVER == 1)
ReturnType ESP8266_StartServerBlocking(void)
{
	// Start server
	while ( ESP8266_StartServer() != Return_Ok)
	{
		DebugPrint("Error! Server isn't started\r\n");
		vTaskDelay(10000);
	}
	
	ESP8266_ConnectionStatus = ESP8266_ConnectionStatus_SuccessfulServerStarted;
	DebugPrint("Server has started\r\n");

	return Return_Ok;
	
}
#endif


#ifdef CONFIG_USE_FREERTOS
void ESP8266_Task(void)
{

	// For received message checking
	ReturnType isValidMessage = Return_False;

	
	// First, need initialize ESP8266's pins, with	ESP8266_Init();
	// is in main.c
	
	
	// Delay, for hardware initializing
	vTaskDelay(10000);
	
	
	// Minél késobb kell inicializálni az USART-ot, mert az ESP8266 induláskor sok üzenetet küld, amitol kifagy a handler.
	//(ErrorCallback hívódik meg)
	USART_Init(&ESP8266_UartHandle);
	
	
	vTaskDelay(100);
	
	
	// Config ESP8266
	// ReturnType ...
	ESP8266_Config ();

	
	
	//HAL_Delay(1000);
	vTaskDelay(5000);
	
	/////////////////////////////
	// Connect to Wifi
	/////////////////////////////
	
	while ( ESP8266_ConnectToWifiNetwork() == Return_False)
	{
		DebugPrint("Failed connect successful to Wifi\r\n");
		vTaskDelay(10000);
	}
	DebugPrint("Successful connected to Wifi\r\n");
	
	// Delay
	vTaskDelay(1000);
	
	


	////////////////////////////////////////
	// Infinite loop for task working ...
	////////////////////////////////////////
	
	while(1)
	{
		// CONNECT / START OF SERVER
		#if ( CONFIG_ESP8266_IS_SERVER == 1 )
		if ( ESP8266_ConnectionStatus != ESP8266_ConnectionStatus_SuccessfulServerStarted )
		{
			// Start server
			ESP8266_StartServerBlocking();

		}
		#elif ( CONFIG_ESP8266_IS_SERVER == 0 )
		if ( ESP8266_ConnectionStatus != ESP8266_ConnectionStatus_SuccessfulConnected)
		{
			// TODO:Find server
			
			// Connect
			ESP8266_ClientConnectBlocking();

		}
		#else
		#error CONFIG_ESP8266_IS_SERVER define isnt correct.
		#endif
		// END OF CONNECT OR START OF SERVER
		
		

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
		if ( ESP8266_ReceiveBuffer_Cnt >= 50 )
		{
			//isValidMessage = OMEAUTMESSAGE_CompareMessage((uint8_t *)&ESP8266_ReceiveBuffer[ESP8266_HOMEAUTMESSAGE_RECEIVEDMESSAGE_START]);
			isValidMessage = Return_Ok;
			if ( isValidMessage == Return_Ok)
			{
				DebugPrint("Valid HomeAut message received:\r\n");
				DebugPrint((char *)&ESP8266_ReceiveBuffer[ESP8266_HOMEAUTMESSAGE_RECEIVEDMESSAGE_START]);	
				if ( xQueueSend(ESP8266_ReceivedMessage_Queue,&ESP8266_ReceiveBuffer[ESP8266_HOMEAUTMESSAGE_RECEIVEDMESSAGE_START],1000) == pdPASS )
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
		else if ( !StrCmpWithLength((const char *)ESP8266_ReceiveBuffer,"Link",4 ))
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
		else if ( !StrCmpWithLength((const char *)ESP8266_ReceiveBuffer,"Unlink",6 ))
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
		else if ( !StrCmpWithLength((const char *)ESP8266_ReceiveBuffer,"OK",2 ))
		{
			// Received: "OK"
			// Note: This arrived after received an x length message (now, after 40 length homeautmessage)
			isValidMessage = Return_Ok;
			DebugPrint("Received an OK\r\n");
		}
		else if ( !StrCmpWithLength((const char *)ESP8266_ReceiveBuffer,"",1 ))
		{
			// Received: ""
			// Note: This arrived after received an x length message (now, after 40 length homeautmessage) + "\r\n" after ""
			isValidMessage = Return_Ok;
			DebugPrint("Received an empty string\r\n");
		}		
		else
		{
			// Not valid message
			isValidMessage = Return_False;
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


static void DebugPrint( const char *debugString )
{
	if (ESP8266_DebugEnableFlag == 1)
	{
		USART_SendMessage(debugString);
	}
}


#endif
// END of File
