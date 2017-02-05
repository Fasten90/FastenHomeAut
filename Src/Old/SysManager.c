/*******************************************************************************
 * Purpose:
 ******************************************************************************/

/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "SysManager.h"
#include "include.h"

#include "homeautmessage.h"

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_MODULE_SYSMANAGER_ENABLE
/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/
typedef struct {
	uint8_t myIp;
	uint8_t serverIp;
	uint8_t ImTheServer;
	uint8_t IConnected;
	
	// my Voltage
	float myBatteryVoltage;
	float AlarmLowBatteryLimit;
	uint8_t AlarmLowBatteryIsSended;
	
	// Internal Temperature
	float myTemperature;
	uint8_t AlarmTooHotLimit;
	int8_t AlarmTooColdLimit;
	uint8_t AlarmTooHotOrColdIsSended;
	
	// External Temperature
	float extTemperature;
	uint8_t ExternalTemperatureAlarmTooHotLimit;
	uint8_t ExternalTemperatureAlarmTooColdLimit;
	uint8_t ExternalTemperatureAlarmTooHotOrColdIsSended;
	
	
	// Lighting
	float Lighting;
	float AlarmLightingTooDarkLimit;
	float AlarmLightingTooBrightLimit;
	uint8_t AlarmLightingDarkOrBrightIsSended;
	
	uint32_t ActualSec;
	uint8_t LastMessageSendedSec;
} DeviceStatusStruct;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
HomeAutDeviceDataType HomeAutDevices[HOMEAUTMESSAGE_DEVICE_CONNECT_MAX_NUM];
DeviceStatusStruct DeviceStatus;

volatile uint8_t SYSMANAGER_ReceivedAnValidHomeAutMessageAndNeedProcess = 0;

uint8_t SysManager_DebugEnableFlag = 1;


char SysManager_HomeAutMessage_Buffer[ESP8266_MESSAGE_QUEUE_ITEM_SIZE];
	

/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local functions
 *----------------------------------------------------------------------------*/
ReturnType SysManager_GetMyNetworkState ( void );
static void DebugPrint ( char *debugString );

ReturnType SysManager_CheckMyState ( void );
ReturnType SysManager_Sync ( void );
ReturnType SysManager_CheckReceivedMessage ( void );

ReturnType SysManager_Server_CheckButton ( void );

ReturnType SYSMANAGER_GetMyNetworkState ( void );

ReturnType SysManager_CheckButtonStatus ( void );
ReturnType SysManager_CheckAdcValues ( void );
ReturnType SysManager_CheckIOs ( void );
ReturnType SysManager_CheckTemperature ( void );

ReturnType SysManager_ProcessHomeAutMessage ( void );
ReturnType SysManager_SendStates ( void );


/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/


/**
\brief	SysManager Task
*/
void SYSMANAGER_Task(void)
{
	
	// My state
	DeviceStatus.myIp = 0;
	DeviceStatus.IConnected = 0;
	HomeAutDevices[0].State = State_Unknown;
	DeviceStatus.serverIp = ESP8266_SERVER_IP_ADDRESS_SHORT;
	
	// Server mode
	#ifdef CONFIG_USE_PANEL_CENTERPANEL
	DeviceStatus.ImTheServer = 1;
	#else
	DeviceStatus.ImTheServer = 0;
	#endif

	// Voltage
	DeviceStatus.AlarmLowBatteryLimit = 6.5;
	DeviceStatus.AlarmLowBatteryIsSended = 0;	
	
	// Temperature
	DeviceStatus.AlarmTooHotLimit = 80;
	DeviceStatus.AlarmTooColdLimit = -10;
	DeviceStatus.AlarmTooHotOrColdIsSended = 0;
	
	DeviceStatus.ExternalTemperatureAlarmTooHotLimit = 30;
	DeviceStatus.ExternalTemperatureAlarmTooColdLimit = 15;
	DeviceStatus.ExternalTemperatureAlarmTooHotOrColdIsSended = 0;
	
	
	// Lighting
	DeviceStatus.AlarmLightingTooDarkLimit = 3.0;
	DeviceStatus.AlarmLightingTooBrightLimit = 2.4;
	DeviceStatus.AlarmLightingDarkOrBrightIsSended = 0;
	
	// Time
	DeviceStatus.ActualSec = 0;
	DeviceStatus.LastMessageSendedSec = 0;
	
	
	
	//////////////////////////////
	//		INFINITE TASK LOOP
	//////////////////////////////
	while(1)
	{
		
		#ifdef CONFIG_USE_PANEL_NODEMEDIUM
		// Important! ADC_Run() function measure each channel, run a lot of! on NodeMedium
		ADC_Run();
		#endif
		
		// Check my IP (I connected?)
		if ( SysManager_GetMyNetworkState() == Return_Ok )
		{
			DeviceStatus.IConnected = 1;
		}
		
		while (DeviceStatus.IConnected)
		{
					
			// Toggle GREEN LED
			LED_GREEN_TOGGLE();
			
			
			// Check my state for Alarm
			// Read datas, IO-s, ADC-s, Buttons
			#ifdef CONFIG_USE_PANEL_NODEMEDIUM
			SysManager_CheckMyState();
			#endif
			#ifdef CONFIG_USE_PANEL_CENTERPANEL
			SysManager_Server_CheckButton();
			#endif

	
			// SYNC + STATE SENDING
			#ifdef CONFIG_USE_PANEL_NODEMEDIUM
			SysManager_Sync();
			#endif


			// Check received messages
			SysManager_CheckReceivedMessage();

				

			// End of work
			// Delay 1 sec
			vTaskDelay(1000);		

			// TODO: Sec++ áttétele máshova
			DeviceStatus.ActualSec++;			
			
			
		}	// END OF  while (DeviceStatus.IConnected)
		// If not connected, delay

		// Delay 1 sec
		vTaskDelay(1000);
		
		// TODO: Sec++ áttétele máshova
		DeviceStatus.ActualSec++;
		
	}
	
}



/**
\brief	Get wifi state & IP address
*/
ReturnType SysManager_GetMyNetworkState ( void )
{

	// Has connected / Started
	//#if ( CONFIG_ESP8266_IS_SERVER == 1 )
	#ifdef CONFIG_USE_PANEL_CENTERPANEL
	// Server
	if ( ESP8266_ConnectionStatus == ESP8266_ConnectionStatus_SuccessfulServerStarted )
	{
		// HomeAutDevices[0] = I am
		
		// IP address
		DeviceStatus.myIp = ESP8266_MyIpAddress;
		HomeAutDevices[0].IP_addressDec = ESP8266_MyIpAddress;
		
		// State
		HomeAutDevices[0].State = State_Server;
		return Return_Ok;
	}
	
	#else
	
	// Client	
	if ( ESP8266_ConnectionStatus == ESP8266_ConnectionStatus_SuccessfulConnected )
	{
		// HomeAutDevices[0] = I am
		
		// IP address
		DeviceStatus.myIp = ESP8266_MyIpAddress;
		HomeAutDevices[0].IP_addressDec = ESP8266_MyIpAddress;
		
		// State
		HomeAutDevices[0].State = State_Connected;
		return Return_Ok;
	}
	#endif
	
	else
	{
		return Return_False;
	}
	
	
}



#ifdef CONFIG_USE_PANEL_NODEMEDIUM
/**
\brief	Get device's status
*/
ReturnType SysManager_CheckMyState ( void )
{
					
	// Read ADC
	// Vsource, InternalTemp
	// TODO: Light sense
	// Not: microphone
	SysManager_CheckAdcValues();	// Check BatteryVoltage & Send
	
	// Check IO-s
	// Motion, Sound Impact
	SysManager_CheckIOs();			// Check & Send
	
	
	// Check temperature
	SysManager_CheckTemperature();	// Check & Send
	
	
	// Check buttons
	// Button
	SysManager_CheckButtonStatus();	// Check & Send

	return Return_Ok;
}
#endif



#ifdef CONFIG_USE_PANEL_NODEMEDIUM
/**
\brief	Check button status
*/
ReturnType SysManager_CheckButtonStatus ( void )
{
	
	// Pressed any button?
	if ( BUTTON_Clicked & ( 1 << PressedButton_Pressed ) )
	{
		// If pressed any button ...
		
		// Create message ...
		uint32_t buttonData = BUTTON_Clicked;
		BUTTON_Clicked = 0;
		HomeAutMessage_CreateAndSendHomeAutMessage(
			DeviceStatus.myIp,DeviceStatus.serverIp,
			Function_Alarm, Alarm_PressedButton,
			buttonData, 1);
			
	}
	
	return Return_False;
	
}
#endif


#ifdef CONFIG_USE_PANEL_CENTERPANEL
/**
\brief	Check button status
*/
ReturnType SysManager_Server_CheckButton ( void )
{
	// Pressed any button?
	if ( BUTTON_Clicked & ( 1 << PressedButton_Pressed ) )
	{			
		uint8_t myIp = DeviceStatus.myIp;
		uint8_t targetIP = 255;
		BUTTON_Clicked = 0;
		HomeAutMessage_CreateAndSendHomeAutMessage(
			myIp,targetIP,
			Function_Alarm,Alarm_PressedButton,
			BUTTON_Clicked,1);
		
	}
	
	return Return_Ok;
}
#endif



#ifdef CONFIG_USE_PANEL_NODEMEDIUM
/**
\brief	Check the ADC values
*/
ReturnType SysManager_CheckAdcValues ( void )
{
	
	// Read ADC voltages
	ADC_Run();
	
	
	// my Battery Voltage
	DeviceStatus.myBatteryVoltage = ADC_ConvertedValue_VSource;
	
	if ( DeviceStatus.myBatteryVoltage < DeviceStatus.AlarmLowBatteryLimit &&
		!DeviceStatus.AlarmLowBatteryIsSended )
	{
		// Create message ...
		DeviceStatus.AlarmLowBatteryIsSended = 1;
		uint32_t voltage = (uint32_t)( DeviceStatus.myBatteryVoltage * 100);
		
		HomeAutMessage_CreateAndSendHomeAutMessage(
			DeviceStatus.myIp,DeviceStatus.serverIp,
			Function_Alarm, Alarm_LowBattery,
			voltage, 1);
		
		DebugPrint("Low Battery Voltage message has been sended.\r\n");
	}

	
	// my Temperature
	DeviceStatus.myTemperature = ADC_ConvertedValue_InternalTemperature;
	
	if ( DeviceStatus.myTemperature < DeviceStatus.AlarmTooColdLimit &&
		!DeviceStatus.AlarmTooHotOrColdIsSended )
	{
		// Create message ...
		DeviceStatus.AlarmTooHotOrColdIsSended = 1;
		uint32_t temp = (uint32_t)( DeviceStatus.myTemperature * 100);
		
		HomeAutMessage_CreateAndSendHomeAutMessage(
			DeviceStatus.myIp,DeviceStatus.serverIp,
			Function_Alarm, Alarm_InternalTemperature_TooCold,
			temp, 1);		
		
		DebugPrint("Too Cold Alarm message has been sended.\r\n");
	}
	else if ( DeviceStatus.myTemperature > DeviceStatus.AlarmTooHotLimit &&
		!DeviceStatus.AlarmTooHotOrColdIsSended )
	{
		// Create message ...
		DeviceStatus.AlarmTooHotOrColdIsSended = 1;
		uint32_t temp = (uint32_t)( DeviceStatus.myTemperature * 100);
		
		HomeAutMessage_CreateAndSendHomeAutMessage(
			DeviceStatus.myIp,DeviceStatus.serverIp,
			Function_Alarm, Alarm_InternalTemperature_TooHot,
			temp, 1);

		DebugPrint("Too Hot Alarm message has been sended.\r\n");		
	}
	
	// Lighting
	DeviceStatus.Lighting = ADC_ConvertedValue_SensorLight;
	
	if ( ( DeviceStatus.Lighting < DeviceStatus.AlarmLightingTooBrightLimit ) &&
		( DeviceStatus.AlarmLightingDarkOrBrightIsSended != 1 ) )
	{
		// Create message ...
		DeviceStatus.AlarmLightingDarkOrBrightIsSended = 1;
		uint32_t light = (uint32_t) (DeviceStatus.Lighting * 100);
		
		HomeAutMessage_CreateAndSendHomeAutMessage(
			DeviceStatus.myIp,DeviceStatus.serverIp,
			Function_Alarm, Alarm_TooBright,
			light, 1);		
		
		DebugPrint("Too Bright Alarm message has been sended.\r\n");
	}
	else if ( ( DeviceStatus.Lighting > DeviceStatus.AlarmLightingTooDarkLimit ) &&
		( DeviceStatus.AlarmLightingDarkOrBrightIsSended != 2 ) )
	{
		// Create message ...
		DeviceStatus.AlarmLightingDarkOrBrightIsSended = 2;
		uint32_t light = (uint32_t)( DeviceStatus.Lighting * 100);
		
		HomeAutMessage_CreateAndSendHomeAutMessage(
			DeviceStatus.myIp,DeviceStatus.serverIp,
			Function_Alarm, Alarm_TooDark,
			light, 1);

		DebugPrint("Too Dark Alarm message has been sended.\r\n");		
	}	
	
	
	return Return_Ok;
}
#endif // #ifdef CONFIG_USE_PANEL_NODEMEDIUM



#ifdef CONFIG_USE_PANEL_NODEMEDIUM
/**
\brief	Check IOs
*/
ReturnType SysManager_CheckIOs ( void )
{
	if ( GLOBAL_IO_Sensor_Motion_Move )
	{
		GLOBAL_IO_Sensor_Motion_Move = 0;
		
		// Send
		// Create message ...		
		HomeAutMessage_CreateAndSendHomeAutMessage(
			DeviceStatus.myIp,DeviceStatus.serverIp,
			Function_Alarm, Alarm_Moving,
			0, 1);		
		
		DebugPrint("Moving Alarm message has been sended.\r\n");
	}
	if ( GLOBAL_IO_Sensor_Sound_Impact_Sound)
	{
		GLOBAL_IO_Sensor_Sound_Impact_Sound = 0;
		// IMPORTANT: Azért nem kell nagyon nullázni, mert IT, így csak felfutáskor állítódik egybe, ezt a változót
		// 1-be állítja, majd mi ezt kinullázzuk, nyugtázva

		// Send
		HomeAutMessage_CreateAndSendHomeAutMessage(
			DeviceStatus.myIp,DeviceStatus.serverIp,
			Function_Alarm, Alarm_SoundImpacted,
			0, 1);
		
		DebugPrint("Sound impacted Alarm message has been sended.\r\n");
	}

	return Return_Ok;
	
}
#endif	// #ifdef CONFIG_USE_PANEL_NODEMEDIUM



#ifdef CONFIG_USE_PANEL_NODEMEDIUM
/**
\brief	Check the temperature
*/
ReturnType SysManager_CheckTemperature ( void )
{
	
	TEMPERATURE_GetTemperature();
	DeviceStatus.extTemperature = GLOBAL_TemperatureValue_LM75;
	
	if ( DeviceStatus.extTemperature > DeviceStatus.ExternalTemperatureAlarmTooHotLimit
		&& !DeviceStatus.ExternalTemperatureAlarmTooHotOrColdIsSended)
	{
		// Too hot
		DeviceStatus.ExternalTemperatureAlarmTooHotOrColdIsSended = 1;
		HomeAutMessage_CreateAndSendHomeAutMessage(
			DeviceStatus.myIp,DeviceStatus.serverIp,
			Function_Alarm, Alarm_TooHot,
			DeviceStatus.extTemperature * 100, 1);	
	}
	else if ( DeviceStatus.extTemperature < DeviceStatus.ExternalTemperatureAlarmTooColdLimit
		&& !DeviceStatus.ExternalTemperatureAlarmTooHotOrColdIsSended)
	{
		// Too cold
		DeviceStatus.ExternalTemperatureAlarmTooHotOrColdIsSended = 2;
		HomeAutMessage_CreateAndSendHomeAutMessage(
			DeviceStatus.myIp,DeviceStatus.serverIp,
			Function_Alarm, Alarm_TooCold,
			DeviceStatus.extTemperature * 100, 1);
	}
	else if ( DeviceStatus.extTemperature < DeviceStatus.ExternalTemperatureAlarmTooHotLimit
			&& DeviceStatus.extTemperature > DeviceStatus.ExternalTemperatureAlarmTooColdLimit )
	{
		// Temperature is ok
		DeviceStatus.ExternalTemperatureAlarmTooHotOrColdIsSended = 0;
	}
	
	
	return Return_Ok;
	
}

#endif	// #ifdef CONFIG_USE_PANEL_NODEMEDIUM



#ifdef CONFIG_USE_PANEL_NODEMEDIUM
/**
\brief	Send sync message x seconds and send state y seconds
*/
ReturnType SysManager_Sync ( void )
{
	
	// Need Sync message? (every 30 sec / half minute)
	if ( ( DeviceStatus.ActualSec % 30 ) == 0)
	{
		uint8_t myIp = DeviceStatus.myIp;
		uint8_t serverIp = DeviceStatus.serverIp;
		HomeAutMessage_CreateAndSendHomeAutMessage(myIp,serverIp,Function_Login,Login_Sync,0,1);
		DebugPrint("Sync message has been sended\r\n");
	}
	//DeviceStatus.LastMessageSendedSec // TODO:
	
	// Need Sync message? (every 180 sec / 3 minute)
	if ( ( DeviceStatus.ActualSec % 180 ) == 0)
	{
		SysManager_SendStates();
		DebugPrint("State messages has been sended\r\n");
	}	

	return Return_Ok;	
}
#endif


/**
\brief	Check received messages
*/
ReturnType SysManager_CheckReceivedMessage ( void )
{
	// Check received message and Process if need
#ifdef CONFIG_USE_FREERTOS
	if ( xQueueReceive(ESP8266_ReceivedMessage_Queue,SysManager_HomeAutMessage_Buffer,0) == pdPASS )
	{
		// Read & Parse
		if ( HomeAutMessage_CheckAndProcessMessage(SysManager_HomeAutMessage_Buffer) == Return_Ok )
		{
			// Successful parsed
			// Process
			SysManager_ProcessHomeAutMessage();
		}
		else
		{
			DebugPrint("Failed HomeAutMessage parsing\r\n");
		}		
	}
#endif
	
	return Return_Ok;
}			



/**
\brief	Processing the received HomeAutMessage (on server, received message from client)
*/
ReturnType SysManager_ProcessHomeAutMessage ( void )
{

	//DebugPrint("SYSMANAGER_ReceivedAnValidHomeAutMessageAndNeedProcess = 1\r\n");
	
	// Is Valid message?
	if ( HomeAutMessage_MessageInformation.isValid == 1)
	{
		//DebugPrint("HomeAutMessage_MessageInformation.isValid == 1\r\n");
		#ifdef CONFIG_USE_PANEL_CENTERPANEL
		uint8_t ipAddress = HomeAutMessage_MessageInformation.SourceAddress;
		uint32_t data = HomeAutMessage_MessageInformation.Data;
		#endif
		
		// Is Alarm + Button + Left ?
		if (HomeAutMessage_MessageInformation.Function == Function_Alarm
			&&
			HomeAutMessage_MessageInformation.DataType == Alarm_PressedButton
			&&
			HomeAutMessage_MessageInformation.Data == 0x11)
		{	 // Left button
			LED_GREEN_TOGGLE();

			#if CONFIG_USE_PANEL_CENTERPANEL
			DebugPrint("NodeMedium clicked Left button\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_Alarm, Alarm_PressedButton , 17);
			#elif CONFIG_USE_PANEL_NODEMEDIUM
			DebugPrint("CenterPanel clicked Left button, RELAY ON\r\n");
			#endif
		}
		// Is Alarm + Button + Down ?
		else if (HomeAutMessage_MessageInformation.Function == Function_Alarm
			&&
			HomeAutMessage_MessageInformation.DataType == Alarm_PressedButton
			&&
			HomeAutMessage_MessageInformation.Data == 0x5) // Down button
		{
			LED_GREEN_TOGGLE();
			
			#if CONFIG_USE_PANEL_CENTERPANEL
			DebugPrint("NodeMedium clicked Down button\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_Alarm, Alarm_PressedButton , 5);
			#elif CONFIG_USE_PANEL_NODEMEDIUM
			DebugPrint("CenterPanel clicked Down button, RELAY OFF\r\n");
			RELAY_1_OFF();
			#endif
		}
		// Is Alarm + Button + Up ?
		else if (HomeAutMessage_MessageInformation.Function == Function_Alarm
			&&
			HomeAutMessage_MessageInformation.DataType == Alarm_PressedButton
			&&
			HomeAutMessage_MessageInformation.Data == 0x3) // Up button
		{
			#if CONFIG_USE_PANEL_CENTERPANEL
			DebugPrint("NodeMedium clicked Up button\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_Alarm, Alarm_PressedButton , 3);
			#elif CONFIG_USE_PANEL_NODEMEDIUM
			DebugPrint("CenterPanel clicked Up button\r\n");
			RELAY_1_ON();
			#endif
		}
		// Is Alarm + Button + Right ?
		else if (HomeAutMessage_MessageInformation.Function == Function_Alarm
			&&
			HomeAutMessage_MessageInformation.DataType == Alarm_PressedButton
			&&
			HomeAutMessage_MessageInformation.Data == 0x9) // Right button
		{
			#if CONFIG_USE_PANEL_CENTERPANEL
			DebugPrint("NodeMedium clicked Right button\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_Alarm, Alarm_PressedButton , 9);
			#elif CONFIG_USE_PANEL_NODEMEDIUM
			DebugPrint("CenterPanel clicked Right button\r\n");
			#endif					
		}
		// Is Alarm + TooCold
		#if CONFIG_USE_PANEL_CENTERPANEL				// For CenterPanel processing...
		else if (HomeAutMessage_MessageInformation.Function == Function_Alarm
			&&
			HomeAutMessage_MessageInformation.DataType == Alarm_TooCold)
		{

			DebugPrint("NodeMedium too cold\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_Alarm, Alarm_TooCold, data);
			
		}
		// Is Alarm + TooHot
		else if (HomeAutMessage_MessageInformation.Function == Function_Alarm
			&&
			HomeAutMessage_MessageInformation.DataType == Alarm_TooHot)
		{
			DebugPrint("NodeMedium too hot\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_Alarm, Alarm_TooHot, data);
		}
		// Is Alarm + LowBatteryVoltage
		else if (HomeAutMessage_MessageInformation.Function == Function_Alarm
			&&
			HomeAutMessage_MessageInformation.DataType == Alarm_LowBattery)
		{
			DebugPrint("NodeMedium low battery voltage\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_Alarm, Alarm_LowBattery, data);
		}
		
		// Is Alarm + TooBright
		else if (HomeAutMessage_MessageInformation.Function == Function_Alarm
			&&
			HomeAutMessage_MessageInformation.DataType == Alarm_TooBright)
		{
			DebugPrint("NodeMedium too Bright\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_Alarm, Alarm_TooBright, data);
		}
		// Is Alarm + TooDark
		else if (HomeAutMessage_MessageInformation.Function == Function_Alarm
			&&
			HomeAutMessage_MessageInformation.DataType == Alarm_TooDark)
		{
			DebugPrint("NodeMedium too Dark\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_Alarm, Alarm_TooDark, data);
		}
		
		// Is Alarm + Moving
		else if (HomeAutMessage_MessageInformation.Function == Function_Alarm
			&&
			HomeAutMessage_MessageInformation.DataType == Alarm_Moving)
		{
			DebugPrint("NodeMedium moving sensed\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_Alarm, Alarm_Moving, data);
		}
		// Is Alarm + Sound impacted
		else if (HomeAutMessage_MessageInformation.Function == Function_Alarm
			&&
			HomeAutMessage_MessageInformation.DataType == Alarm_SoundImpacted)
		{
			DebugPrint("NodeMedium sound impacting sensed\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_Alarm, Alarm_SoundImpacted , data);
		}
		// Alarm : door opened
		else if (HomeAutMessage_MessageInformation.Function == Function_Alarm
			&&
			HomeAutMessage_MessageInformation.DataType == Alarm_DoorOpened)
		{
			DebugPrint("NodeMedium door opened\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_Alarm, Alarm_SoundImpacted , data);
		}	
		#endif	// #if CONFIG_USE_PANEL_CENTERPANEL	// For CenterPanel processing...
		
		// TODO: STATE ÜZENETEK FELDOLGOZÁSA
		#if CONFIG_USE_PANEL_CENTERPANEL			// For CenterPanel processing...
		// Is State + Battery voltage
		else if (HomeAutMessage_MessageInformation.Function == Function_State
			&&
			HomeAutMessage_MessageInformation.DataType == State_Battery)
		{
			DebugPrint("NodeMedium state battery\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_State, State_Battery, data);
		}
		// Is State + Light
		else if (HomeAutMessage_MessageInformation.Function == Function_State
			&&
			HomeAutMessage_MessageInformation.DataType == State_Brightness)
		{
			DebugPrint("NodeMedium state brightness\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_State, State_Brightness, data);
		}
		// Is State + Temperature
		else if (HomeAutMessage_MessageInformation.Function == Function_State
			&&
			HomeAutMessage_MessageInformation.DataType == State_Temperature)
		{
			DebugPrint("NodeMedium state temperature\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_State, State_Temperature, data);
		}
		// Is State + Output
		else if (HomeAutMessage_MessageInformation.Function == Function_State
			&&
			HomeAutMessage_MessageInformation.DataType == State_Output)
		{
			DebugPrint("NodeMedium state output\r\n");
			RASPBERRYPI_SendMessage(ipAddress, Function_State, State_Output, data);
		}
		#endif // #if CONFIG_USE_PANEL_CENTERPANEL	// For CenterPanel processing...
		
		#ifdef CONFIG_USE_PANEL_NODEMEDIUM
		// Is Command + SetOutput ?
		else if (HomeAutMessage_MessageInformation.Function == Function_Command
			&&
			HomeAutMessage_MessageInformation.DataType == Command_SetOutput)
		{
			if ( HomeAutMessage_MessageInformation.Data )
			{
				RELAY_1_ON();
				DebugPrint("CenterPanel sended Command Output: relay 1 on\r\n");
			}
			else
			{
				RELAY_1_OFF();
				DebugPrint("CenterPanel sended Command Output: relay 1 off\r\n");
			}
			HomeAutMessage_CreateAndSendHomeAutMessage(
				DeviceStatus.myIp,DeviceStatus.serverIp,
				Function_State,State_Output,
				HomeAutMessage_MessageInformation.Data,1);
		}
		#endif	// #ifdef CONFIG_USE_PANEL_NODEMEDIUM	// FOR COMMAND
		
		// END OF PROCESS, no find		
		else
		{
			return Return_False;
		}
		
		
	}
	
	LED_RED_TOGGLE();
	return Return_Ok;
	
}


/**
\brief	Print on debug serial port
*/
static void DebugPrint( char *debugString )
{
	if (SysManager_DebugEnableFlag == 1)
	{
		USART_SendMessage(debugString);
	}
}



#ifdef CONFIG_USE_PANEL_NODEMEDIUM
/**
\brief		Send device's status: Battery voltage, brightness, temperature
*/
ReturnType SysManager_SendStates ( void )
{
	uint8_t myIp = DeviceStatus.myIp;
	uint8_t serverIp = DeviceStatus.serverIp;
	
	// Battery
	uint32_t battery = (uint32_t) ( DeviceStatus.myBatteryVoltage * 100);
	HomeAutMessage_CreateAndSendHomeAutMessage(myIp,serverIp,Function_State,State_Battery,battery,1);
	
	vTaskDelay(1000);
	DeviceStatus.ActualSec++;	// TODO: Sec++ átrakás máshova
	
	// Light
	uint32_t light = (uint32_t) ( DeviceStatus.Lighting * 100);
	HomeAutMessage_CreateAndSendHomeAutMessage(myIp,serverIp,Function_State,State_Brightness,light,1);

	vTaskDelay(1000);
	DeviceStatus.ActualSec++;	// TODO: Sec++ átrakás máshova
	
	// Temperature
	int32_t temp = (uint32_t) ( DeviceStatus.extTemperature * 100);	
	HomeAutMessage_CreateAndSendHomeAutMessage(myIp,serverIp,Function_State,State_Temperature,temp,1);
	
	vTaskDelay(1000);
	DeviceStatus.ActualSec++;	// TODO: Sec++ átrakás máshova
	
	//HomeAutMessage_CreateAndSendHomeAutMessage(0,0,Function_State,State_Sound,0,1);
	
	
	return Return_Ok;
}

#endif	// #ifdef CONFIG_MODULE_SYSMANAGER_ENABLE

#endif

