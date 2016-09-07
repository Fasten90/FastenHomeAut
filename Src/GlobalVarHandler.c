/*
 * GlobalVarHandler.c
 *
 *  Created on: Sep 5, 2016
 *      Author: Vizi Gábor
 */


#include "include.h"
#include "GlobalVarHandler.h"
#include "GlobalVariables.h"



/// GLOBAL VARIABLES
/*
Type_Unknown = 0,
Type_Bool,
Type_Uint8,
Type_Int8,
Type_Uint16,
Type_Int16,
Type_Uint32,
Type_Int32,
Type_Float,
Type_String,
Type_Enumerator
*/
static const char const*GlobalVarTypesNames[] =
{
		// NOTE: Important!! Must be in the same order with VarType_t
		"Unknown",
		"bool",
		"uint8_t",
		"int8_t",
		"uint16_t",
		"int16_t",
		"uint32_t",
		"int32_t",
		"float",
		"string",
		"enumerator"
};




/// FUNCTION PROTOTYPES

static ProcessResult_t GlobalVarHandler_GetCommand(uint8_t commandID, char *resultBuffer, uint8_t *resultBufferLength);
static int16_t GlobalVarHandler_SearchVariableName(const char *commandName);
static ProcessResult_t GlobalVarHandler_SetCommand(uint8_t commandID, const char *param, char *resultBuffer, uint8_t *resultBufferLength);
static void GlobalVarHandler_WriteResults(ProcessResult_t result, char *resultBuffer, uint8_t resultBufferLength);
static ProcessResult_t GlobalVarHandler_CheckValue(uint32_t num, uint8_t commandID);



/// FUNCTIONS


bool GlobalVarHandler_CheckCommandStructAreValid(void)
{
	// TODO: vagy itt futásidejû teszt, vagy makrókkal fordításidejû teszt?
	return false;
}



/**
 * \brief	Process received command
 * \param	*commandName		received command name (string)
 * \param	*param				received parameters after command
 * \param	setGetType			Set or get command
 * \param	source				Command source (e.g. USART)
 * \param	*resultBuffer		Buffer, which writed with result
 * \param	resultBufferLength	Buffer length
 */
void GlobalVarHandler_ProcessCommand(
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
	for (i=0; i<GlobalVarMaxCommandNum; i++)
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
		length += UnsignedDecimalToString(num, resultBuffer);
		}
		break;
	case Type_String:
		{
		const char *string = GlobalVarList[commandID].varPointer;
		length += StrCpyMax(resultBuffer,string, *resultBufferLength-length);
		}
		break;

	default:
		return Process_FailParam;
		break;
	}


#ifdef GLOBALVARHANDLER_UNIT_ENABLE
	// Append unit, if need
	if (GlobalVarList[commandID].unit)
	{
		// Space
		length += StrCpyMax(&resultBuffer[length], " ", *resultBufferLength-length);
		// Copy *unit
		length += StrCpyMax(&resultBuffer[length],GlobalVarList[commandID].unit, *resultBufferLength-length);
	}
#endif


	// Check length
	// TODO:
	if (length <= *resultBufferLength)
	{
		*resultBufferLength -= length;
	}
	else
	{
		*resultBufferLength = 0;
		// TODO: Buffer Overflow
	}


	return Process_Ok_Answered;
}



/**
 * \brief	Set command
 */
static ProcessResult_t GlobalVarHandler_SetCommand(uint8_t commandID, const char *param, char *resultBuffer, uint8_t *resultBufferLength)
{

	uint8_t length = 0;

	// TODO: isHex?

	// TODO: külön függvénybe az értékkonvertálást?
	switch(GlobalVarList[commandID].type)
	{
	case Type_Uint8:
		{
		uint8_t *numPointer = (uint8_t *)GlobalVarList[commandID].varPointer;
		uint32_t num = 0;
		if(UnsignedDecimalStringToNum(param, &num))
		{
			// TODO: Check value
			ProcessResult_t result;
			result = GlobalVarHandler_CheckValue(num,commandID);

			if (result == Process_Ok_SetSuccessful_SendOk)
			{
				// Good
				*numPointer = num;
			}
			else
			{
				// Wrong
				return result;
			}

			// NOTE: Do not return from here
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
		if ( stringLength >= GlobalVarList[commandID].maxValue )
		{
			stringLength =  GlobalVarList[commandID].maxValue;
		}
		if (stringLength > *resultBufferLength)
		{
			stringLength = *resultBufferLength;
		}

		char *string = (char *)GlobalVarList[commandID].varPointer;

		length += StrCpyMax(string,param,stringLength);
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
 * \brief	Check values
 */
static ProcessResult_t GlobalVarHandler_CheckValue(uint32_t num, uint8_t commandID)
{
	switch(GlobalVarList[commandID].type)
	{
	case Type_Bool:
		if (num > 1) return Process_InvalidValue_TooMuch;
		break;
	case Type_Uint8:
		if (num > 255) return Process_InvalidValue_TooMuch;
		break;
	case Type_Uint16:
		if (num > 65535) return Process_InvalidValue_TooMuch;
		break;
	case Type_String:
		if (num > GlobalVarList[commandID].maxValue) return Process_TooLongString;
		break;
	case Type_Enumerator:
		if (num > GlobalVarList[commandID].maxValue) return Process_InvalidValue_TooMuch;
		break;
		// TODO: Lekezelni a többi ágat is
	default:
		return Process_FailType;
		break;
	}

	// check maxValue
	if (num > GlobalVarList[commandID].maxValue) return Process_InvalidValue_TooMuch;

	if (num < GlobalVarList[commandID].minValue) return Process_InvalidValue_TooSmall;

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
		//resultBuffer[0] = '\0';
		break;
	case Process_Ok_SetSuccessful_SendOk:
		StrCpyMax(resultBuffer,"Set successful!",resultBufferLength);
		break;
	case Process_CommandNotFound:
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



/**
 * \brief	List all variables
 */
void GlobalVarHandler_ListAllVariables(void)
{
	uint8_t i;
	uint8_t length;

	// TODO: Megcsinálni szebbre?
	// TODO: min/max-okat kiírni?
	// TODO: Enumokat is kiírni, ha van?
	USART_SendLine("|------Name--------|--Type---|unit|----Description-------|");
	for (i=0; i<GlobalVarMaxCommandNum; i++)
	{
		// Print one command / line:
		// name \t type \t description
		/*
		uprintf("%s\t%s\t%s\t[%s]\r\n",
				GlobalVarList[i].name,	// name
				GlobalVarTypesNames[GlobalVarList[i].type],	// type
				GlobalVarList[i].description,	// description
				GlobalVarList[i].unit			// unit
				);
		*/
		length = 0;
		length += USART_SendString("| ");
		// Send name
		length += USART_SendString(GlobalVarList[i].name);
		while(length < 20)
		{
			USART_SendChar(' ');
			length++;
		}
		// Send type
		length += USART_SendString(GlobalVarTypesNames[GlobalVarList[i].type]);
		while(length < 30)
		{
			USART_SendChar(' ');
			length++;
		}
		// Send unit
		if(GlobalVarList[i].unit)
		{
			// If has unit
			length += USART_SendString("[");
			length += USART_SendString(GlobalVarList[i].unit);
			length += USART_SendString("]");
		}
		while(length < 35)
		{
			USART_SendChar(' ');
			length++;
		}
		// Send description
		length += USART_SendString(GlobalVarList[i].description);
		length += USART_SendLine("");
	}
	USART_SendLine("--------------------------------");


}
