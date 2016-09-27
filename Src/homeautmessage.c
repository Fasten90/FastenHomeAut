

#include "homeautmessage.h"
#include "include.h"




////////////////////////////
//		GLOBAL VARIABLES
////////////////////////////


////////// CONST

// TODO: Write to dynamic

// Now: 40 length
const uint8_t HOMEAUTMESSAGE_DefaultMessageLength = sizeof(HOMEAUTMESSAGE_MessageType);

const char HOMEAUTMESSAGE_DefaultHeader[] = "|HomeAut|";
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

	// HERE ADD NEW DateType

	{
		.name = "",
		.type = DataType_End
	}


};







////////////////////////////
//		GLOBAL VARIABLES

HOMEAUTMESSAGE_InformationType HOMEAUTMESSAGE_MessageInformation;



////////////////////////////
//		FUNCTIONS
////////////////////////////


/*
\brief	Check HomeAutMessage
*/
ReturnType HOMEAUTMESSAGE_CompareMessage(char *messageString)
{
	HOMEAUTMESSAGE_MessageType *message = (HOMEAUTMESSAGE_MessageType *)messageString;
	
	HOMEAUTMESSAGE_MessageInformation.isValid = INVALID;
	uint32_t readedMyAddress = 0;
	uint32_t readedTargetAddress = 0;
	HOMEAUTMESSAGE_FunctionType function = Function_Invalid;
	uint32_t readedData = 0;
	char buffer[10];
	HOMEAUTMESSAGE_DataType dataType = DataType_Unknown;
	
	
	// Header
	if (!StrCmpWithLength((char *)message->Header,(char *)HOMEAUTMESSAGE_DefaultHeader,HOMEAUTMESSAGE_DefaultHeader_Length))
	{
		
		// MyAddress
		StrCpyFixLength(buffer,message->MyAddress,HOMEAUTMESSAGE_DefaultAddress_Length);
		buffer[HOMEAUTMESSAGE_DefaultAddress_Length] = '\0';
		if(!UnsignedDecimalStringToNum (buffer, &readedMyAddress))
		{
			return Return_False;
		}
		if (readedMyAddress >= 0 && readedMyAddress <= 256)
		{
			if (!StrCmpWithLength((char *)message->Seperator1,(char *)HOMEAUTMESSAGE_DefaultSeperator,HOMEAUTMESSAGE_DefaultSeparator_Length))
			{
				StrCpyFixLength(buffer,message->TargetAddress,HOMEAUTMESSAGE_DefaultAddress_Length);
				buffer[HOMEAUTMESSAGE_DefaultAddress_Length] = '\0';
				if(!UnsignedDecimalStringToNum (buffer,&readedTargetAddress))
				{
					return Return_False;
				}
				if (readedMyAddress >= 0 && readedMyAddress <= 256)
				{
					//uint8_t Seperator2[1];
					// |				
					if (!StrCmpWithLength((char *)message->Seperator2,(char *)HOMEAUTMESSAGE_DefaultSeperator,HOMEAUTMESSAGE_DefaultSeparator_Length))
					{						

						
						//uint8_t	Function[7];
						// <Function>		
						StrCpyFixLength(buffer,message->Function,HOMEAUTMESSAGE_DefaultFunction_Length);
						buffer[HOMEAUTMESSAGE_DefaultFunction_Length] = '\0';
						for (int i = 0; FunctionTypeParity_List[i].function != Function_End; i++)
						{
							if (!StrCmpWithLength((char *)buffer,(char *)FunctionTypeParity_List[i].name,HOMEAUTMESSAGE_DefaultFunction_Length))
							{
								function = FunctionTypeParity_List[i].function;
								break;
							}
						}
											
						if ( function != Function_Invalid )
						{
							
							//uint8_t Seperator3[1];
							// |
							if (!StrCmpWithLength((char *)message->Seperator3,(char *)HOMEAUTMESSAGE_DefaultSeperator,HOMEAUTMESSAGE_DefaultSeparator_Length))
							{							
								
								
								
								// <DataType> : 6 byte ASCII char
								StrCpyFixLength(buffer,message->DataType,HOMEAUTMESSAGE_DefaultDataType_Length);
								buffer[HOMEAUTMESSAGE_DefaultFunction_Length] = '\0';
								for (int i = 0; DataTypeParity_List[i].type != DataType_End; i++)
								{		
									//DataTypeParity_List[i].name
									if (!StrCmpWithLength((char *)buffer,(char *)DataTypeParity_List[i].name,HOMEAUTMESSAGE_DefaultDataType_Length))
									{
										dataType = DataTypeParity_List[i].type;
										break;
									}
									
								}
								
								if (dataType == DataType_Unknown)
								{
									return Return_False;
								}
									
								
								//uint8_t Data[8];
								// <Data>
								StrCpyFixLength(buffer,message->Data,HOMEAUTMESSAGE_DefaultData_Length);
								buffer[HOMEAUTMESSAGE_DefaultData_Length] = '\0';

								if(StringHexToNum (buffer,&readedData,4))
								{
									// Successful converting
								}
								else
								{
									// Error with converting hex
									return Return_Error;
								}
																
								//uint8_t Seperator4[1];
								// |	
								if (!StrCmpWithLength((char *)message->Seperator4,(char *)HOMEAUTMESSAGE_DefaultSeperator,HOMEAUTMESSAGE_DefaultSeparator_Length))
								{	
									
									HOMEAUTMESSAGE_MessageInformation.MyAddress = readedMyAddress;
									HOMEAUTMESSAGE_MessageInformation.TargetAddress = readedTargetAddress;
									HOMEAUTMESSAGE_MessageInformation.Function = function;
									HOMEAUTMESSAGE_MessageInformation.DataType = dataType;
									HOMEAUTMESSAGE_MessageInformation.Data = readedData;
									HOMEAUTMESSAGE_MessageInformation.isValid = VALID;
									
									return Return_Ok;
								}
							}								
							
						}		
						
					}
				}
			}
		}		
	}			
	
	return Return_False;
	
}		




void HOMEAUTMESSAGE_Test(void)
{
	
	// |HomeAut|<MyAddress>|<TargetAddress>|<Function>|<DataType>:<Data>|
	char TestMessage[] = "|HomeAut|100|121|ALARM__|TEMP__01234567|";
	
	uint8_t isGood = 0;
	
	// It is valid message?
	//if ( HOMEAUTMESSAGE_CompareMessage((HOMEAUTMESSAGE_MessageType *)TestMessage) == Return_Ok)	// TODO: delete, if below good
	if ( HOMEAUTMESSAGE_CompareMessage(TestMessage) == Return_Ok)
	{
		if ( HOMEAUTMESSAGE_MessageInformation.isValid == VALID )
		{
			if ( HOMEAUTMESSAGE_MessageInformation.Data == 0x01234567)
			{
				LED_GREEN_ON();
				isGood = 1;
				// GOOD !!
				// now, you can check "HOMEAUTMESSAGE_MessageInformation"
			}
		}
	}
	
	if(isGood)
	{
		LED_GREEN_ON();
	}
	
	HOMEAUTMESSAGE_MessageType anMessage;
	HOMEAUTMESSAGE_MessageType *pAnMessage;
	pAnMessage = &anMessage;
	
	HOMEAUTMESSAGE_CreateMessage(pAnMessage,&HOMEAUTMESSAGE_MessageInformation);
	
	
	while(1);
	
}





ReturnType HOMEAUTMESSAGE_CreateMessage(HOMEAUTMESSAGE_MessageType *createToMessage, HOMEAUTMESSAGE_InformationType *messageInformation)
{
	
	
	// Default Header
	StrCpyFixLength(createToMessage->Header,HOMEAUTMESSAGE_DefaultHeader, HOMEAUTMESSAGE_DefaultHeader_Length);


	// Others, variables
	
	// MyAddress: uint8_t --> ASCII_3
	// Problem: if not 3 char?
	// TODO: Error with it! Fix length, and ended with character and not '\0'
	UnsignedDecimalToString(messageInformation->MyAddress,createToMessage->MyAddress);
	
	// Default Separator
	StrCpyFixLength(createToMessage->Seperator1,HOMEAUTMESSAGE_DefaultSeperator, HOMEAUTMESSAGE_DefaultSeparator_Length);

	// TargetAddress
	// TODO: Error with it! Fix length, and ended with character and not '\0'
	UnsignedDecimalToString(messageInformation->TargetAddress,createToMessage->TargetAddress);
	
	// Default Separator
	StrCpyFixLength(createToMessage->Seperator2,HOMEAUTMESSAGE_DefaultSeperator, HOMEAUTMESSAGE_DefaultSeparator_Length);

	
	// Function
	for (int i = 0; FunctionTypeParity_List[i].function != Function_End; i++)
	{
		if (FunctionTypeParity_List[i].function == messageInformation->Function )
		{
			StrCpyFixLength(createToMessage->Function,FunctionTypeParity_List[i].name,HOMEAUTMESSAGE_DefaultFunction_Length);
			break;
		}
	}
	
	// Default Separator
	StrCpyFixLength(createToMessage->Seperator3,HOMEAUTMESSAGE_DefaultSeperator, HOMEAUTMESSAGE_DefaultSeparator_Length);
	
	
	// DataType
	for (int i = 0; DataTypeParity_List[i].type != DataType_End; i++)
	{
		if (DataTypeParity_List[i].type == messageInformation->DataType )
		{
			StrCpyFixLength(createToMessage->DataType,DataTypeParity_List[i].name,HOMEAUTMESSAGE_DefaultData_Length);
			break;
		}
	}
	
	// Data
	DecimalToHexaString(messageInformation->Data, createToMessage->Data, 4);
	
	
	// Default Separator
	StrCpyFixLength(createToMessage->Seperator4,HOMEAUTMESSAGE_DefaultSeperator, HOMEAUTMESSAGE_DefaultSeparator_Length);
	
	
	return Return_Ok;
}





/*
uint8_t MyAddress;
uint8_t TargetAddress;
HOMEAUTMESSAGE_FunctionType Function;
HOMEAUTMESSAGE_DataType DataType;
uint32_t Data;
uint8_t isValid;
*/
ReturnType HOMEAUTMESSAGE_CreateAndSendHomeAutMessage
	( uint8_t myIp, uint8_t destIp, HOMEAUTMESSAGE_FunctionType function,
	HOMEAUTMESSAGE_DataType dataType, uint32_t data, uint8_t isValid)
{
	
	HOMEAUTMESSAGE_InformationType messageInformation;
	HOMEAUTMESSAGE_MessageType message;
	

	messageInformation.MyAddress = myIp;	
	messageInformation.TargetAddress = destIp;
	
	messageInformation.Function = function;
	
	messageInformation.DataType = dataType;
	
	messageInformation.Data = data;

	messageInformation.isValid = isValid;
	
	// Create message ...
	if ( HOMEAUTMESSAGE_CreateMessage(&message,&messageInformation) == Return_Ok )
	{
		
		// TODO: queue-ba kĂ©ne kĂĽldeni ĂĽzenetet
		// VigyĂˇzat: Ă­gy a NodeSmall-nak mĂˇsmilyen kĂłd kell majd...
		/*
		if ( ESP8266_TcpConnectionStatus == ESP8266_TcpConnectionStatus_Connected )
		{
			//ESP8266_SendTcpMessage((uint8_t *)&message);	// direct sending
			
		}
		*/
#ifdef CONFIG_USE_FREERTOS
		ESP8266_SendMessageToQueue((uint8_t *)&message);
#endif


	}	

	return Return_Ok;	// TODO: VisszatĂ©rĂ©seket jĂłl megcsinĂˇlni
	
}



