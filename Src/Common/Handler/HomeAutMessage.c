/*
 *        HomeAutMessage.c
 *        Created on:        2016.01.01
 *        Author:         Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        HomeAutMessage message handling
 *        Target:            STM32Fx
 *        Version:        v4
 *        Last modified:    2016.09.28
 */


#include "options.h"
#include "compiler.h"
#include "StringHelper.h"
#include "HomeAutMessage.h"
#include "DebugUart.h"

#ifdef MODULE_HOMEAUTMESSAGE_UNITTEST_ENABLE
#include "unittest.h"
#endif


#ifdef CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

// Constant global variables

///< HomeAut Header
const char HOMEAUTMESSAGE_DefaultHeader[] = "HomeAut";


///< HomeAut Function parity list
const FunctionTypeParity FunctionTypeParity_List[] =
{
    {
        .name = "INVALID",
        .function = Function_Invalid
    },
    {
        .name = "LOGIN",
        .function = Function_Login
    },
    {
        .name = "STATE",
        .function = Function_State
    },
    {
        .name = "ALARM",
        .function = Function_Alarm
    },
    {
        .name = "COMMAND",
        .function = Function_Command
    },
    {
        .name = "MODE",
        .function = Function_Mode
    },
    {
        .name = "CONFIG",
        .function = Function_Config
    },

    // XXX: Add here new FunctionType

    {
        .name = NULL,
        .function = Function_End
    }
};


///< HomeAut DataType parity list
const DataTypeParity DataTypeParity_List[] =
{
    // 6 char length

    // LOGIN
    {
        .name = "NSMALL",
        .type = Login_ImLoginImNodeSmall
    },
    {
        .name = "NMEDIUM",
        .type = Login_ImLoginImNodeMedium
    },
    {
        .name = "CENTERPANEL",
        .type = Login_ImLoginImCenterPanel
    },
    {
        .name = "DISCOVERY",
        .type = Login_ImLoginImDiscovery
    },
    {
        .name = "WELCOME",
        .type = Login_Welcome
    },
    {
        .name = "SYNC",
        .type = Login_Sync
    },


    // STATE
    {
        .name = "TEMP",
        .type = State_Temperature
    },
    {
        .name = "LIGHT",
        .type = State_Brightness
    },
    {
        .name = "SOUND",
        .type = State_Sound
    },
    {
        .name = "BUTTON",
        .type = State_Button
    },
    {
        .name = "BATT",
        .type = State_Battery
    },
    {
        .name = "OUTPUT",
        .type = State_Output
    },

    // ALARM
    {
        .name = "PRESSEDBUTTON",
        .type = Alarm_PressedButton
    },
    {
        .name = "LOWBATTERY",
        .type = Alarm_LowBattery
    },
    {
        .name = "ONCHGARGE",
        .type = Alarm_OnCharge
    },
    {
        .name = "TOOHOT",
        .type = Alarm_TooHot
    },
    {
        .name = "TOOCOLD",
        .type = Alarm_TooCold
    },
    {
        .name = "INTHOT",
        .type = Alarm_InternalTemperature_TooHot
    },
    {
        .name = "INTCOLD",
        .type = Alarm_InternalTemperature_TooCold
    },
    {
        .name = "BRIGHT",
        .type = Alarm_TooBright
    },
    {
        .name = "TOODARK",
        .type = Alarm_TooDark
    },
    {
        .name = "MOVING",
        .type = Alarm_Moving
    },
    {
        .name = "SOUND",
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

    // XXX: Add here new DataType

    {
        .name = NULL,
        .type = DataType_End
    }
};


///< HomeAut message information
HomeAut_InformationType HomeAutMessage_MessageInformation;



/*------------------------------------------------------------------------------
 *                                    Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief    Check HomeAutMessage (Convert string to information)
 */
bool HomeAutMessage_CheckAndProcessMessage(const char *messageString,
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
        // OK, copy message to modify buffer
        StrCpy(message, messageString);
    }
    else
    {
        isOk = false;
    }


    // Check first character
    if (isOk)
    {
        isOk = (message[0] == HOMEAUTMESSAGE_SEPARATOR_CHARACTER);
    }


    // Split
    if (isOk)
    {
        isOk = (STRING_Splitter(&message[1], HOMEAUTMESSAGE_SEPARATOR_STRING,
                split, HOMEAUTMESSAGE_SPLIT_NUM) == HOMEAUTMESSAGE_SPLIT_NUM);
    }


    // Check header
    if (isOk)
    {
        isOk = !StrCmp(split[HOMEAUTMESSAGE_MESSAGE_STRUCT_HEADER_COUNT],
                HOMEAUTMESSAGE_DefaultHeader);
    }


    // Check source address
    if (isOk)
    {
        isOk = Network_ConvertIpAddressStringToIP(
                split[HOMEAUTMESSAGE_MESSAGE_STRUCT_SOURCEADDRESS_COUNT],
                &information.SourceAddress);
    }


    // Check target address
    if (isOk)
    {
        isOk = Network_ConvertIpAddressStringToIP(
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
 * \brief    Create an HomeAutMessage
 */
uint8_t HomeAutMessage_CreateMessage(HomeAut_InformationType *messageInformation, char *createToMessage)
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
    length += Network_PrintIp(&createToMessage[length], &messageInformation->SourceAddress);

    // Separator
    length += StrCpy(&createToMessage[length], HOMEAUTMESSAGE_SEPARATOR_STRING);

    // Target Address
    length += Network_PrintIp(&createToMessage[length], &messageInformation->TargetAddress);

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
 * \brief    Create and send message the sending queue
 * \param    SourceAddress;
 * \param    TargetAddress;
 * \param    dateTime
 * \param    HomeAut_FunctionType Function;
 * \param    HomeAut_DataType DataType;
 * \param    uint32_t Data;
 */
bool HomeAutMessage_CreateAndSendHomeAutMessage(
    Network_IP_t *myIp,
    Network_IP_t *destIp,
    DateTime_t *dateTime,
    HomeAut_FunctionType function,
    HomeAut_DataType dataType,
    uint32_t data)
{
    
    HomeAut_InformationType messageInformation;
    char message[HOMEAUTMESSAGE_MESSAGE_MAX_LENGTH];
    
    memcpy(&messageInformation.SourceAddress, myIp, sizeof(Network_IP_t));

    memcpy(&messageInformation.TargetAddress, destIp, sizeof(Network_IP_t));
    
    memcpy(&messageInformation.DateTime, dateTime, sizeof(DateTime_t));

    messageInformation.Function = function;
    
    messageInformation.DataType = dataType;
    
    messageInformation.Data = data;

    messageInformation.isValid = true;
    
    // Create message ...
    if (HomeAutMessage_CreateMessage(&messageInformation, message))
    {
        // Send message
#ifdef CONFIG_USE_FREERTOS
        return ESP8266_SendMessageToQueue(message);
#elif defined(CONFIG_MODULE_ESP8266_ENABLE)
        if (ESP8266_TcpConnectionStatus == ESP8266_TcpConnectionStatus_Connected)
        {
            return ESP8266_SendTcpMessage((const char *)&message);    // direct sending
        }
        else
        {
            return false;
        }
#elif defined(CONFIG_MODULE_DEBUGUART_ENABLE)
        DebugUart_SendMessage(message);
        return true;
#elif defined(CONFIG_USE_PANEL_PC)
        uprintf("[HomeAutMessage] %s", message);
        return true;
#else
        #warning "HomeAutMessage module cannot send message to anything"
        return true;
#endif
    }    
    else
    {
        return false;
    }
}



/**
 * \brief    Send HomeAutMessage immediately on communication port
 */
void HomeAutMessage_SendMessage(uint8_t myAddress,
        HomeAut_FunctionType functionType, HomeAut_DataType dataType, float data)
{
    //myAddress    // ok
    char function = 0;
    char type = 0;

    switch (functionType)
    {
        case Function_Alarm:
            function = 'a';
            break;
        case Function_Command:
            function = 'c';
            break;
        case Function_Config:
            function = 'c';
            break;
        case Function_Login:
            function = 'l';
            break;
        case Function_Mode:
            function = 'm';
            break;
        case Function_State:
            function = 's';
            break;

        case Function_End:
        case Function_Invalid:
        default:
            function = '-';
            break;
    }

    switch (dataType)
    {
        case State_Temperature:
            type = 't';
            break;
        case State_Brightness:
            type = 'b';
            break;
        case State_Sound:
            type = 's';
            break;
        case State_Battery:
            type = 'v';        // Status ~Voltage
            break;
        case State_Input:
            type = 'i';
            break;
        case State_Output:
            type = 'o';
            break;
        case Alarm_PressedButton:
            type = 'b';
            break;
        case Alarm_Moving:
            type = 'm';
            break;
        case Alarm_SoundImpacted:
            type = 's';
            break;
        case Alarm_DoorOpened:
            type = 'd';
            break;

        case DataType_Unknown:
        case DataType_End:
        case Login_ImLoginImNodeSmall:
        case Login_ImLoginImNodeMedium:
        case Login_ImLoginImCenterPanel:
        case Login_ImLoginImDiscovery:
        case Login_Welcome:
        case Login_Sync:
        case State_Button:
        case State_Vin:
        case State_Accelerometer:
        case Alarm_Accelerometer:
        case Alarm_LowBattery:
        case Alarm_OffCharge:
        case Alarm_OnCharge:
        case Alarm_TooHot:
        case Alarm_TooCold:
        case Alarm_TooBright:
        case Alarm_TooDark:
        case Alarm_InputAllState:
        case Alarm_InputFalling:
        case Alarm_InputRise:
        case Alarm_InternalTemperature_TooCold:
        case Alarm_InternalTemperature_TooHot:
        case Command_SetOutput:
        case Command_AllOutput:
        case Command_ResetOutput:
        case Command_SetLed:
        case Command_AllLed:
        case Command_ResetLed:
        case Command_Remote:
        case Mode_RemoteControl:
        case Mode_Spectator:
        case Mode_Sleep:
        case Config_TODO:
        case Config_AnythingLimit:
        default:
            type = '-';
            break;
    }


    // Sending:
    uprintf("#%d_%c%c_%0.2f\r\n", myAddress, function, type, data);
}



#ifdef MODULE_HOMEAUTMESSAGE_UNITTEST_ENABLE
/**
 * \brief    Test an HomeAutMessage
 */
uint32_t HomeAutMessage_UnitTest(void)
{

    UnitTest_Start("HomeAutMessage", __FILE__);


    // |HomeAut|<SourceAddress>|<TargetAddress>|<DateTime>|<Function>|<DataType>|<Data>|
    const char TestMessage[] = "|HomeAut|192.168.100.100|192.168.100.14|2017-01-10 18:49:50|COMMAND|REMOTE|01234567|";
    HomeAut_InformationType testInformation = { 0 };
    char exampleStringMessage[HOMEAUTMESSAGE_MESSAGE_MAX_LENGTH];
    uint8_t length;


    // It is valid message?
    UNITTEST_ASSERT(HomeAutMessage_CheckAndProcessMessage(
            TestMessage, &testInformation),
            "HomeAutMessage Check function has error");


    // Check results (processed values)
    UNITTEST_ASSERT(
            ((testInformation.isValid == true)
            // TODO: Extend SourceAddress & TargetAddress checks
            && (testInformation.SourceAddress.IP[0] == 192)
            && (testInformation.TargetAddress.IP[0] == 192)
            // TODO: Check DateTime with more culture
            && (testInformation.DateTime.date.year == 17)
            && (testInformation.Function == Function_Command)
            && (testInformation.DataType == Command_Remote)
            && (testInformation.Data == 0x01234567)),
            "HomeAutMessage processed value(s) error(s)");


    // Check message creating
    length = HomeAutMessage_CreateMessage(&testInformation, exampleStringMessage);
    UNITTEST_ASSERT(length == StringLength(TestMessage),
            "HomeAutMessage CreateMessage error");


    // Check created message
    UNITTEST_ASSERT(!StrCmp(TestMessage, exampleStringMessage),
            "HomeAutMessage created message error");


    // Finish
    return UnitTest_End();
}
#endif    // #ifdef MODULE_HOMEAUTMESSAGE_UNITTEST_ENABLE



#endif    // CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE
