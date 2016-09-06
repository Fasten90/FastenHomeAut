/*
 * GlobalVarHandler.c
 *
 *  Created on: Sep 5, 2016
 *      Author: Vizi Gábor
 */


#include "include.h"
#include "GlobalVarHandler.h"


#define NUM_OF(x) (sizeof(x)/sizeof(x[0]))

#define GLOBAL_VAR_MAX_COMMAND_NUM				(uint8_t)( NUM_OF(GlobalVarList))


// Global variables

// TODO: Add to new file

uint8_t testVar = 17;
char deviceName[25] = "deviceName: Discovery";


GlobalVarCommand_t GlobalVarList[] =
{
		{
			.name = "testvar",
			.varPointer = &testVar,
			.type = Type_Uint8
		},
		{
			.name = "teststring",
			.varPointer = deviceName,
			.type = Type_String,
			.stringMaxLength = 19
		}

		// ADD new global variable here
		/*
			const VarType_t type;

			void const *varPointer;
			const bool isReadOnly;

			const void const *maxPointer;
			const void const *minPointer;
			const char const *unit;

			const uint8_t stringMaxLength;

			const CommandSource_t sourceEnable;
		 */
};


/// FUNCTION PROTOTYPES

static ProcessResult_t GlobalVarHandler_GetCommand(uint8_t commandID, char *resultBuffer, uint8_t *resultBufferLength);
static int16_t GlobalVarHandler_SearchVariableName(const char *commandName);
static ProcessResult_t GlobalVarHandler_SetCommand(uint8_t commandID, const char *param, char *resultBuffer, uint8_t *resultBufferLength);
static void GlobalVarHandler_WriteResults(ProcessResult_t result, char *resultBuffer, uint8_t resultBufferLength);


/// FUNCTIONS


bool GlobalVarHandler_CheckCommandStructAreValid(void)
{
	// TODO: vagy itt futásidejû teszt, vagy makrókkal fordításidejû teszt?
	return false;
}



/**
 * \brief	Check received command
 * TODO: params
 */
void GlobalVarHandler_CheckReceivedCommand(
		const char *commandName, const char *param,
		SetGetType_t setGetType, CommandSource_t source,
		char *resultBuffer, uint8_t resultBufferLength)
{
	 ProcessResult_t result = Process_UnknownError;

	// Search command
	int16_t commandID = GlobalVarHandler_SearchVariableName(commandName);
	if(commandID > -1)
	{
		// Found, Check the source
		if ((source & GlobalVarList[commandID].sourceEnable) || (GlobalVarList[commandID].sourceEnable == Source_All))
		{
			// Source is enabled
			if(setGetType == SetGet_Get)
			{
				// Get
				result = GlobalVarHandler_GetCommand(commandID,resultBuffer,&resultBufferLength);
			}
			else if(setGetType == SetGet_Set)
			{
				// Can set? (not const?)
				if(!GlobalVarList[commandID].isReadOnly)
				{
					// It not const, set
					result = GlobalVarHandler_SetCommand(commandID,param,resultBuffer,&resultBufferLength);
				}
				else
				{
					// Cannot set, it is read only
					result = Process_IsReadOnly;
				}
			}
			else
			{
				result = Process_UnknownError;
			}
		}
		else
		{
			// Source not enabled
			result = Process_SourceNotEnabled;
		}
	}
	else
	{
		// Not found
		result = Process_CommandNotFound;
	}


	GlobalVarHandler_WriteResults(result,resultBuffer,resultBufferLength);

}



/**
 * \brief	Search global var name
 * \return	0..., if found, it is commandID
 * 			-1, if no founded
 */
static int16_t GlobalVarHandler_SearchVariableName(const char *commandName)
{
	uint8_t i;

	// Search
	for (i=0; i<GLOBAL_VAR_MAX_COMMAND_NUM; i++)
	{
		if(!StrCmp(GlobalVarList[i].name, commandName))
		{
		// Found
		return i;
		}
	}

	// Not found
	return -1;

}



/**
 * \brief	Get command
 */
static ProcessResult_t GlobalVarHandler_GetCommand(uint8_t commandID, char *resultBuffer, uint8_t *resultBufferLength)
{
	uint8_t length = 0;

	// TODO: isHex?

	switch(GlobalVarList[commandID].type)
	{
	case Type_Uint8:
		{
		uint8_t *numPointer = (uint8_t *)GlobalVarList[commandID].varPointer;
		uint8_t num = *numPointer;
		// TODO: Buffer túlírás ellenõrzés
		length = UnsignedDecimalToString(num, resultBuffer);

		}
		break;
	case Type_String:
		{
		const char *string = GlobalVarList[commandID].varPointer;
		length = StrCpy(resultBuffer,string);
		}
		break;

	default:
		return Process_FailParam;
		break;
	}

	// Append unit, if need
	// TODO:

	// Check length
	// TODO:
	if(length <= *resultBufferLength)
	{
		*resultBufferLength -= length;
	}
	else
	{
		*resultBufferLength = 0;
	}

	return Process_Ok_Answered;
}



/**
 * \brief	Set command
 */
static ProcessResult_t GlobalVarHandler_SetCommand(uint8_t commandID, const char *param, char *resultBuffer, uint8_t *resultBufferLength)
{
	// TODO: Need implement

	uint8_t length = 0;

	// TODO: isHex?

	switch(GlobalVarList[commandID].type)
	{
	case Type_Uint8:
		{
		uint8_t *numPointer = (uint8_t *)GlobalVarList[commandID].varPointer;
		uint32_t num = 0;
		if(UnsignedDecimalStringToNum(param, &num))
		{
			// TODO: Check value
			*numPointer = num;
			//return Process_Ok_SetSuccessful_SendOk;
		}
		else
		{
			return Process_FailParamIsNotNumber;
		}

		}
		break;
	case Type_String:
		{
		uint8_t stringLength = StringLength(param);
		if ( stringLength >= GlobalVarList[commandID].stringMaxLength )
		{
			stringLength =  GlobalVarList[commandID].stringMaxLength;
		}

		char *string = (char *)GlobalVarList[commandID].varPointer;

		length = StrCpyMax(string,param,stringLength);
		}
		break;

	default:
		return Process_FailParam;
		break;
	}

	// Check length
	// TODO: Check length with correct
	if(length <= *resultBufferLength)
	{
		*resultBufferLength -= length;
	}
	else
	{
		*resultBufferLength = 0;
	}

	return Process_Ok_SetSuccessful_SendOk;
}



/**
 * \brief	Write process result
 */
static void GlobalVarHandler_WriteResults(ProcessResult_t result, char *resultBuffer, uint8_t resultBufferLength)
{

	switch(result)
	{

	case Process_Ok_Answered:
		// Do nothing
		resultBuffer[0] = '\0';
		break;
	case Process_Ok_SetSuccessful_SendOk:
		StrCpyMax(resultBuffer,"Set successful!",resultBufferLength);
		break;
	case Process_CommandNotFound:
		// TODO: StrCpyMax()
		StrCpyMax(resultBuffer,"Command not find!",resultBufferLength);
		break;
	case Process_FailParam:
		StrCpyMax(resultBuffer,"Fail parameter",resultBufferLength);
		break;
	case Process_FailParamIsNotNumber:
		StrCpyMax(resultBuffer,"Not number",resultBufferLength);
		break;
	case Process_InvalidValue_TooSmall:
		StrCpyMax(resultBuffer,"Invalid value, too small",resultBufferLength);
		break;
	case Process_InvalidValue_TooMuch:
		StrCpyMax(resultBuffer,"Invalid value, too much",resultBufferLength);
		break;
	case Process_IsReadOnly:
		StrCpyMax(resultBuffer,"Cannot set, it is constant",resultBufferLength);
		break;
	case Process_SourceNotEnabled:
		StrCpyMax(resultBuffer,"Cannot process this command from this source",resultBufferLength);
		break;
	case Process_TooLongString:
		StrCpyMax(resultBuffer,"Too long string",resultBufferLength);
		break;
	case Process_UnknownError:
		StrCpyMax(resultBuffer,"Unknown error",resultBufferLength);
		break;
	default:
		StrCpyMax(resultBuffer,"Fatal error",resultBufferLength);
		break;
	}
}

