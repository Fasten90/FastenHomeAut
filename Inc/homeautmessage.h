/*******************************************************************************
 * Purpose:
 ******************************************************************************/

#ifndef HOMEAUTMESSAGE_H_
#define HOMEAUTMESSAGE_H_


#include "include.h"




// DEFINES

#define VALID		( 1 )
#define INVALID		( 0 )


///< HomeAut message structure for message seperating
typedef struct
{
	char Header[9];							///< Header
	// |HomeAut|
	char MyAddress[3];						///< My address (sender)
	// <MyAddress>
	char Seperator1[1];						///< Separator character
	// |
	char TargetAddress[3];					///< Target Address
	// <TargetAddress>
	char Seperator2[1];						///< Separator character
	// |
	char Function[7];						///< Function
	// <Function>
	char Seperator3[1];						///< Separator character
	// |
	char DataType[6];						///< Data type (
	// <DataType> : 6 byte ASCII char
	char Data[8];							///< Data (example: temperature value)
	// <Data> : 8 byte - ASCII Hex
	char Seperator4[1];						///< Last separator character
	// |
} HOMEAUTMESSAGE_MessageType;

// For example message:
// |HomeAut|010|014|LOGIN__|NMEDIU00000000|
//type = Login_ImLoginImNodeMedium





// uint8_t	Function[7],
/*
const uint8_t HOMEAUTMESSAGE_DefaultFunctionLogin[] =   "LOGIN__";
const uint8_t HOMEAUTMESSAGE_DefaultFunctionState[] =   "STATE__";
const uint8_t HOMEAUTMESSAGE_DefaultFunctionAlarm[] =   "ALARM__";
const uint8_t HOMEAUTMESSAGE_DefaultFunctionCommand[] = "COMMAND";
const uint8_t HOMEAUTMESSAGE_DefaultFunctionMode[] =    "MODE___";
const uint8_t HOMEAUTMESSAGE_DefaultFunctionConfig[] =  "CONFIG_";
*/



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
} HOMEAUTMESSAGE_FunctionType;



///< Function name - string parity table
typedef struct
{
	char name[8];								///< Name of function
	HOMEAUTMESSAGE_FunctionType function;		///< Function enum
} FunctionTypeParity;



typedef enum
{
	DataType_Unknown = 0,
	
	// Login
	Login_ImLoginImNodeSmall,
	Login_ImLoginImNodeMedium,
	Login_ImLoginImCenterPanel,
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
	
	// Mode: Master set Slave mode
	Mode_RemoteControl,
	Mode_Spectator,
	Mode_Sleep,
	
	// Config: Master config the slave
	Config_TODO,
	Config_AnythingLimit,
	
	// Too add new DataType here
	
	DataType_End
} HOMEAUTMESSAGE_DataType;



///<  Data type - string table
typedef struct
{
	const char name[7];						///< Name of Data [string]
	HOMEAUTMESSAGE_DataType type;			///< Data type
} DataTypeParity;



///< HomeAut message information type
typedef struct
{
	uint8_t MyAddress;						///< My (sender) address
	uint8_t TargetAddress;					///< Target (receiver) address
	HOMEAUTMESSAGE_FunctionType Function;	///< Function
	HOMEAUTMESSAGE_DataType DataType;		///< Data type
	uint32_t Data;							///< Data
	uint8_t isValid;						///< It is valid information?
} HOMEAUTMESSAGE_InformationType;





// GLOBAL VARIABLES

extern const FunctionTypeParity FunctionTypeParity_List[];	// Function enum - "FUNCTION" string
	

extern const DataTypeParity DataTypeParity_List[];			// DataType enum - "DATATYPE" string


extern HOMEAUTMESSAGE_InformationType HOMEAUTMESSAGE_MessageInformation;



// PROTOTYPES

void HOMEAUTMESSAGE_Test(void);
ReturnType HOMEAUTMESSAGE_CompareMessage(char *messageString);
ReturnType HOMEAUTMESSAGE_CreateMessage(HOMEAUTMESSAGE_MessageType *createToMessage, HOMEAUTMESSAGE_InformationType *messageInformation);

ReturnType HOMEAUTMESSAGE_CreateAndSendHomeAutMessage
	( uint8_t myIp, uint8_t destIp, HOMEAUTMESSAGE_FunctionType function,
	HOMEAUTMESSAGE_DataType dataType, uint32_t data, uint8_t isValid);
	




#endif /* HOMEAUTMESSAGE_H_*/
