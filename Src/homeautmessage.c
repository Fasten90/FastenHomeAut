/*
 *		homeautmessage.c
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		HomeAutMessage message handling
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */


#include <HomeAutMessage.h>
#include "include.h"


#ifdef CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

// Constant global variables

// TODO: Write to dynamic

// Now: 40 length
const uint8_t HOMEAUTMESSAGE_DefaultMessageLength = HOMEAUTMESSAGE_MESSAGE_MAX_LENGTH;

const char HOMEAUTMESSAGE_DefaultHeader[] = "HomeAut";
const uint8_t HOMEAUTMESSAGE_DefaultHeader_Length = sizeof(HOMEAUTMESSAGE_DefaultHeader) -1;

const char HOMEAUTMESSAGE_DefaultSeperator[] = "|";
const uint8_t HOMEAUTMESSAGE_DefaultSeparator_Length = sizeof(HOMEAUTMESSAGE_DefaultSeperator) -1;


const uint8_t HOMEAUTMESSAGE_DefaultDataType_Length = 6;

const uint8_t HOMEAUTMESSAGE_DefaultFunction_Length = 7;

const uint8_t HOMEAUTMESSAGE_DefaultAddress_Length = 3;
const uint8_t HOMEAUTMESSAGE_DefaultData_Length = 8;



const FunctionTypeParity FunctionTypeParity_List[] =
{
	{
		.name = "INVALID",
		.function = Function_Invalid
	},
	{
		.name = "LOGIN__",
		.function = Function_Login
	},
	{
		.name = "STATE__",
		.function = Function_State
	},
	{
		.name = "ALARM__",
		.function = Function_Alarm
	},
	{
		.name = "COMMAND",
		.function = Function_Command
	},
	{
		.name = "MODE___",
		.function = Function_Mode
	},
	{
		.name = "CONFIG_",
		.function = Function_Config
	},

	// HERE ADD NEW DateType

	{
		.name = "",
		.function = Function_End
	}

};




const DataTypeParity DataTypeParity_List[] =
{
	// 6 char length

	// LOGIN
	{
		.name = "NSMALL",
		.type = Login_ImLoginImNodeSmall
	},
	{
		.name = "NMEDIU",
		.type = Login_ImLoginImNodeMedium
	},
	{
		.name = "CENTER",
		.type = Login_ImLoginImCenterPanel
	},
	{
		.name = "WELCOM",
		.type = Login_Welcome
	},
	{
		.name = "SYNC__",
		.type = Login_Sync
	},


	// STATE
	{
		.name = "TEMP__",
		.type = State_Temperature
	},
	{
		.name = "LIGHT_",
		.type = State_Brightness
	},
	{
		.name = "SOUND_",
		.type = State_Sound
	},
	{
		.name = "BUTTON",
		.type = State_Button
	},
	{
		.name = "BATT__",
		.type = State_Battery
	},
	{
		.name = "OUTPUT",
		.type = State_Output
	},

	// ALARM
	{
		.name = "PREBUT",
		.type = Alarm_PressedButton
	},
	{
		.name = "LOWBAT",
		.type = Alarm_LowBattery
	},
	{
		.name = "ONCHG_",
		.type = Alarm_OnCharge
	},
	{
		.name = "TOOHOT",
		.type = Alarm_TooHot
	},
	{
		.name = "TOCOLD",
		.type = Alarm_TooCold
	},
	{
		.name = "INTHOT",
		.type = Alarm_InternalTemperature_TooHot
	},
	{
		.name = "INCOLD",
		.type = Alarm_InternalTemperature_TooCold
	},
	{
		.name = "BRIGHT",
		.type = Alarm_TooBright
	},
	{
		.name = "TODARK",
		.type = Alarm_TooDark
	},
	{
		.name = "MOVING",
		.type = Alarm_Moving
	},
	{
		.name = "SOUND_",
		.type = Alarm_SoundImpacted
	},

	
	// COMMAND
	{
		.name = "SETOUT",
		.type = Command_SetOutput
	},
	{
		.name = "REMOTE",
		.type = Command_Remote
	},

	// HERE ADD NEW DateType

	{
		.name = "",
		.type = DataType_End
	}


};


HomeAut_InformationType HOMEAUTMESSAGE_MessageInformation;




uint8_t HomeAutMessage_PrintIpAddress(char *message, HomeAutAddress_t *ip);



/*------------------------------------------------------------------------------
 *									Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Check HomeAutMessage (Convert string to information)
 */
bool HOMEAUTMESSAGE_CheckAndProcessMessage(const char *messageString,
		HomeAut_InformationType *messageInformation)
{

	// Information data
	HomeAut_InformationType information = { 0 };
	
	// For separate
	uint8_t messageLength;
	char message[HOMEAUTMESSAGE_MESSAGE_MAX_LENGTH];
	bool isOk = true;
	char *split[HOMEAUTMESSAGE_SPLIT_NUM] = { 0 };
	uint8_t i;
	
	// |HomeAut|192.168.100.100|192.168.100.014|2017-01-10 18:49:50|COMMAND|REMOTE|00000000|\0

	// Check length
	messageLength = StringLength(messageString);
	if (messageLength >= HOMEAUTMESSAGE_MESSAGE_MIN_LENGTH)
	{
		// Ok, copy message to modify buffer
		StrCpy(message, messageString);
	}
	else
	{
		isOk = false;
	}


	// TODO: Check first character

	// Split
	isOk = (STRING_Splitter(&message[1], HOMEAUTMESSAGE_SEPARATOR_CHARACTER,
			split, HOMEAUTMESSAGE_SPLIT_NUM) == HOMEAUTMESSAGE_SPLIT_NUM);


	// Check header
	if (isOk)
	{
		isOk = !StrCmp(split[HOMEAUTMESSAGE_MESSAGE_STRUCT_HEADER_COUNT],
				HOMEAUTMESSAGE_DefaultHeader);
	}


	// Check source address
	if (isOk)
	{
		isOk = HomeAutMessage_ConvertAddressStringToIP(
				split[HOMEAUTMESSAGE_MESSAGE_STRUCT_SOURCEADDRESS_COUNT],
				&information.SourceAddress);
	}


	// Check target address
	if (isOk)
	{
		isOk = HomeAutMessage_ConvertAddressStringToIP(
				split[HOMEAUTMESSAGE_MESSAGE_STRUCT_TARGETADDRESS_COUNT],
				&information.TargetAddress);
	}


	// DateTime
	if (isOk)
	{
		isOk = DateTime_ConvertStringToDateTime(
				split[HOMEAUTMESSAGE_MESSAGE_STRUCT_DATETIME_COUNT],
				&information.DateTime);
	}


	// Function
	if (isOk)
	{
		information.Function = Function_Invalid;
		for (i = 0; FunctionTypeParity_List[i].function != Function_End; i++)
		{
			if (!StrCmp(
					(char *)split[HOMEAUTMESSAGE_MESSAGE_STRUCT_FUNCTION_COUNT],
					(char *)FunctionTypeParity_List[i].name))
			{
				information.Function = FunctionTypeParity_List[i].function;
				break;
			}
		}

		isOk = (information.Function != Function_Invalid);
	}

	// Data type
	if (isOk)
	{
		information.DataType = DataType_Unknown;
		for (i = 0; DataTypeParity_List[i].type != DataType_End; i++)
		{
			//DataTypeParity_List[i].name
			if (!StrCmp(
					(char *)split[HOMEAUTMESSAGE_MESSAGE_STRUCT_DATATYPE_COUNT],
					(char *)DataTypeParity_List[i].name))
			{
				information.DataType  = DataTypeParity_List[i].type;
				break;
			}

		}

		isOk = (information.DataType != DataType_Unknown);
	}


	// Data
	if (isOk)
	{
		isOk = StringHexToNum(split[HOMEAUTMESSAGE_MESSAGE_STRUCT_DATA_COUNT], &information.Data);
	}


	// Finish
	if (isOk)
	{
		information.isValid = true;
		// If message is good, copy information to parameter
		memcpy(messageInformation, &information, sizeof(HomeAut_InformationType));
	}

	
	return isOk;

}		



/**
 * \brief	Create an HomeAutMessage
 */
uint8_t HOMEAUTMESSAGE_CreateMessage(HomeAut_InformationType *messageInformation, char *createToMessage)
{
	
	uint8_t length = 0;
	uint8_t i;

	// Separator
	length += StrCpy(&createToMessage[length], HOMEAUTMESSAGE_SEPARATOR_STRING);

	// Header Header
	length += StrCpy(&createToMessage[length], HOMEAUTMESSAGE_DefaultHeader);

	// Separator
	length += StrCpy(&createToMessage[length], HOMEAUTMESSAGE_SEPARATOR_STRING);

	///////////////////////
	// Others, variables
	///////////////////////

	// Source Address
	length += HomeAutMessage_PrintIpAddress(&createToMessage[length], &messageInformation->SourceAddress);

	// Separator
	length += StrCpy(&createToMessage[length], HOMEAUTMESSAGE_SEPARATOR_STRING);

	// Target Address
	length += HomeAutMessage_PrintIpAddress(&createToMessage[length], &messageInformation->TargetAddress);

	// Separator
	length += StrCpy(&createToMessage[length], HOMEAUTMESSAGE_SEPARATOR_STRING);

	// DateTime
	length += DateTime_PrintDateTimeToString(&createToMessage[length], &messageInformation->DateTime);

	// Separator
	length += StrCpy(&createToMessage[length], HOMEAUTMESSAGE_SEPARATOR_STRING);
	
	// Function
	for (i = 0; FunctionTypeParity_List[i].function != Function_End; i++)
	{
		if (FunctionTypeParity_List[i].function == messageInformation->Function )
		{
			length += StrCpy(&createToMessage[length], FunctionTypeParity_List[i].name);
			break;
		}
	}
	
	// Separator
	length += StrCpy(&createToMessage[length], HOMEAUTMESSAGE_SEPARATOR_STRING);

	// DataType
	for (i = 0; DataTypeParity_List[i].type != DataType_End; i++)
	{
		if (DataTypeParity_List[i].type == messageInformation->DataType )
		{
			length += StrCpy(&createToMessage[length], DataTypeParity_List[i].name);
			break;
		}
	}
	
	// Separator
	length += StrCpy(&createToMessage[length], HOMEAUTMESSAGE_SEPARATOR_STRING);

	// Data
	length += DecimalToHexaString(messageInformation->Data, &createToMessage[length], 8);
	
	// Separator
	length += StrCpy(&createToMessage[length], HOMEAUTMESSAGE_SEPARATOR_STRING);
	
	
	return length;
}



/**
 * \brief	Create and send message the sending queue
 * \param	uint8_t SourceAddress;
 * \param	uint8_t TargetAddress;
 * \param	HomeAut_FunctionType Function;
 * \param	HomeAut_DataType DataType;
 * \param	uint32_t Data;
 * \param	uint8_t isValid;
 */
ReturnType HOMEAUTMESSAGE_CreateAndSendHomeAutMessage(
	uint8_t *myIp, uint8_t *destIp,
	HomeAut_FunctionType function,
	HomeAut_DataType dataType,
	uint32_t data,
	uint8_t isValid)
{
	
	HomeAut_InformationType messageInformation;
	char message[HOMEAUTMESSAGE_MESSAGE_MAX_LENGTH];
	
	uint8_t i;

	for (i = 0; i < 4; i++)
		messageInformation.SourceAddress.IP[i] = myIp[i];

	for (i = 0; i < 4; i++)
		messageInformation.TargetAddress.IP[i] = destIp[i];
	
	messageInformation.Function = function;
	
	messageInformation.DataType = dataType;
	
	messageInformation.Data = data;

	messageInformation.isValid = isValid;
	
	// Create message ...
	if (HOMEAUTMESSAGE_CreateMessage(&messageInformation, message) == Return_Ok)
	{
		// Send message
#ifdef CONFIG_USE_FREERTOS
		return ESP8266_SendMessageToQueue(message);
#else
		if (ESP8266_TcpConnectionStatus == ESP8266_TcpConnectionStatus_Connected)
		{
			ESP8266_SendTcpMessage((uint8_t *)&message);	// direct sending
		}
		else
		{
			return Return_Error;
		}
#endif
	}	

	return Return_Ok;	// TODO: Should correct return values
	
}



/**
 * \brief	Convert string IP address to uint8_t[4]
 */
bool HomeAutMessage_ConvertAddressStringToIP(char *message, HomeAutAddress_t *address)
{

	// String come like "\r\n192.168.0.1\r\n"

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
					address->IP[i] = (uint8_t)convertValue;
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
 * \brief	Print IP address to string
 */
uint8_t HomeAutMessage_PrintIpAddress(char *message, HomeAutAddress_t *ip)
{
	uint8_t length = 0;
	uint8_t i;

	for (i=0; i<4; i++)
	{
		length += UnsignedDecimalToString(ip->IP[i], &message[length]);
		if (i < 3)
		{
			message[length++] = '.';
		}
	}

	return length;
}



/**
 * \brief	Test an HomeAutMessage
 */
void HOMEAUTMESSAGE_UnitTest(void)
{
	// TODO: Update to unit test

	// |HomeAut|<SourceAddress>|<TargetAddress>|<DateTime>|<Function>|<DataType>:<Data>|
	const char TestMessage[] = "|HomeAut|192.168.100.100|192.168.100.014|2017-01-10 18:49:50|COMMAND|REMOTE|00000000|";
	HomeAut_InformationType testInformation = { 0 };
	char exampleStringMessage[HOMEAUTMESSAGE_MESSAGE_MAX_LENGTH];
	uint8_t isGood = 0;

	// It is valid message?
	if (HOMEAUTMESSAGE_CheckAndProcessMessage(TestMessage, &testInformation))
	{
		// TODO: Check all parameters
		// TODO: Correct the parameters
		if (testInformation.isValid == true)
		{
			if (testInformation.Data == 0x01234567)
			{
#warning "It is not good"
				LED_GREEN_ON();
				isGood = 1;
				// GOOD !!
				// now, you can check "HOMEAUTMESSAGE_MessageInformation"
			}
		}
	}

	// Is good to string message -> information convert?
	if (isGood)
	{
		LED_GREEN_ON();
	}


	HOMEAUTMESSAGE_CreateMessage(&testInformation, exampleStringMessage);

	// TODO: Check it

}



#endif	// CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE
