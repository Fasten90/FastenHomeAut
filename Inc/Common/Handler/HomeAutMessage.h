/*
 *		HomeAutMessage.h
 *		Created on:		2016.01.01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		HomeAutMessage message handling
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef HOMEAUTMESSAGE_H_
#define HOMEAUTMESSAGE_H_


#include "DateTime.h"
#include "NetworkHelper.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

// "|HomeAut|192.168.100.100|192.168.100.014|2017-01-10 18:49:50|COMMAND|REMOTE|00000000|\0"
// This example length: 87
#define HOMEAUTMESSAGE_MESSAGE_MAX_LENGTH					( 90 )
// "|HomeAut|1.1.1.1|1.1.1.2|2017-01-10 18:49:50|COMMAND|REMOTE|00000000|\0"
// This example length: 70
#define HOMEAUTMESSAGE_MESSAGE_MIN_LENGTH					( 70 )

#define	HOMEAUTMESSAGE_SEPARATOR_CHARACTER					( '|' )
#define	HOMEAUTMESSAGE_SEPARATOR_STRING						( "|" )

#define HOMEAUTMESSAGE_SPLIT_NUM							( 7 )

#define HOMEAUTMESSAGE_MESSAGE_STRUCT_HEADER_COUNT			( 0 )
#define HOMEAUTMESSAGE_MESSAGE_STRUCT_SOURCEADDRESS_COUNT	( 1 )
#define HOMEAUTMESSAGE_MESSAGE_STRUCT_TARGETADDRESS_COUNT	( 2 )
#define HOMEAUTMESSAGE_MESSAGE_STRUCT_DATETIME_COUNT		( 3 )
#define HOMEAUTMESSAGE_MESSAGE_STRUCT_FUNCTION_COUNT		( 4 )
#define HOMEAUTMESSAGE_MESSAGE_STRUCT_DATATYPE_COUNT		( 5 )
#define HOMEAUTMESSAGE_MESSAGE_STRUCT_DATA_COUNT			( 6 )

// 2017-01-10 19:38:00
#define HOMEAUTMESSAGE_MESSAGE_DATETIME_LENGTH				( 19 )



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


///< HomeAut function
typedef enum
{
	Function_Invalid = 0,
	Function_Login,
	Function_State,
	Function_Alarm,
	Function_Command,
	Function_Mode,
	Function_Config,
	Function_End
} HomeAut_FunctionType;


///< Function name - string parity table
typedef struct
{
	const char * const name;				///< Name of function
	const HomeAut_FunctionType function;	///< Function enum
} FunctionTypeParity;


///< DataType enumeration for HomeAutMessage
typedef enum
{
	DataType_Unknown = 0,
	
	// Login
	Login_ImLoginImNodeSmall,
	Login_ImLoginImNodeMedium,
	Login_ImLoginImCenterPanel,
	Login_ImLoginImDiscovery,
	Login_Welcome,
	Login_Sync,
	
	// State
	State_Temperature,
	State_Brightness,
	State_Sound,
	State_Button,
	State_Battery,
	State_Vin,
	State_Accelerometer,
	State_Input,
	State_Output,
	
	// Alarm
	Alarm_TooHot,
	Alarm_TooCold,
	Alarm_InternalTemperature_TooHot,
	Alarm_InternalTemperature_TooCold,
	Alarm_TooBright,
	Alarm_TooDark,
	Alarm_PressedButton,
	Alarm_OnCharge,
	Alarm_OffCharge,
	Alarm_LowBattery,
	Alarm_Accelerometer,
	Alarm_InputRise,
	Alarm_InputFalling,
	Alarm_InputAllState,
	Alarm_Moving,
	Alarm_SoundImpacted,
	Alarm_DoorOpened,
	
	// Command: Master send command to Slave
	Command_SetOutput,
	Command_AllOutput,
	Command_ResetOutput,
	Command_SetLed,
	Command_AllLed,
	Command_ResetLed,
	Command_Remote,
	
	// Mode: Master set Slave mode
	Mode_RemoteControl,
	Mode_Spectator,
	Mode_Sleep,
	
	// Config: Master config the slave
	Config_TODO,
	Config_AnythingLimit,
	
	// Too add new DataType here
	
	DataType_End
} HomeAut_DataType;


///<  Data type - string table
typedef struct
{
	const char * const name;				///< Name of Data [string]
	const HomeAut_DataType type;			///< Data type
} DataTypeParity;


///< HomeAut message information type
typedef struct
{
	Network_IP_t SourceAddress;				///< My (sender) address
	Network_IP_t TargetAddress;				///< Target (receiver) address
	DateTime_t DateTime;					///< Event date and time
	HomeAut_FunctionType Function;			///< Function
	HomeAut_DataType DataType;				///< Data type
	uint32_t Data;							///< Data
	bool isValid;							///< It is valid information?
} HomeAut_InformationType;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

///< Function enum - "FUNCTION" string
extern const FunctionTypeParity FunctionTypeParity_List[];
	
///< DataType enum - "DATATYPE" string
extern const DataTypeParity DataTypeParity_List[];


extern HomeAut_InformationType HomeAutMessage_MessageInformation;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

bool HomeAutMessage_CheckAndProcessMessage(const char *messageString,
		HomeAut_InformationType *messageInformation);
uint8_t HomeAutMessage_CreateMessage(HomeAut_InformationType *messageInformation, char *createToMessage);

bool HomeAutMessage_CreateAndSendHomeAutMessage(
	Network_IP_t *myIp,
	Network_IP_t *destIp,
	DateTime_t *dateTime,
	HomeAut_FunctionType function,
	HomeAut_DataType dataType,
	uint32_t data);

void HomeAutMessage_SendMessage(uint8_t myAddress,
		HomeAut_FunctionType functionType, HomeAut_DataType dataType, float data);

uint32_t HomeAutMessage_UnitTest(void);



#endif /* HOMEAUTMESSAGE_H_*/
