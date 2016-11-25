/*
 *		GlobalVarHandler.c
 *
 *		Created on:		2016.09.05
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Global Variable Handler: Handle set-get variable from uart or other
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */


#include "include.h"
#include "GlobalVarHandler.h"
#include "GlobalVariables.h"


#define BOOL_MAX	(1)

/*
#define UINT8_MAX	(0xFF)
#define INT8_MAX	(0x7F)
#define UINT16_MAX	(0xFFFF)
#define INT16_MAX	(0x7FFF)
#define UINT32_MAX	(0xFFFFFFFF)
#define INT32_MAX	(0x7FFFFFFF)
*/

/// GLOBAL VARIABLES

/*
Type_Error = 0,
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
// TODO: Lehet, hogy lekérdező függvénybe kéne rakni?
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

static ProcessResult_t GlobalVarHandler_GetCommand(VarID_t commandID, char *resultBuffer, uint8_t *resultBufferLength);
static bool GlobalVarHandler_SearchVariableName(const char *commandName, VarID_t *commandID);
static ProcessResult_t GlobalVarHandler_SetCommand(const VarID_t commandID, const char *param, char *resultBuffer, uint8_t *resultBufferLength);
static void GlobalVarHandler_WriteResults(ProcessResult_t result, char *resultBuffer, uint8_t resultBufferLength);
static ProcessResult_t GlobalVarHandler_CheckValue(VarID_t commandID, uint32_t num);
static void GlobalVarHandler_PrintVariableDescriptions (VarID_t commandID, char *resultBuffer);
static ProcessResult_t GlobalVarHandler_GetIntegerVariable(VarID_t commandID, char *resultBuffer, uint8_t *resultBufferLength);
static uint8_t GlobalVarHandler_GetEnumerator(const VarID_t commandID, char *resultBuffer, uint8_t *resultBufferLength);
static ProcessResult_t GlobalVarHandler_SetBool(VarID_t commandID, const char *param, char *resultBuffer, uint8_t *resultBufferLength);
static ProcessResult_t GlobalVarHandler_SetInteger(VarID_t commandID, const char *param, char *resultBuffer, uint8_t *resultBufferLength);
static ProcessResult_t GLobalVarHandler_SetEnumerator(VarID_t commandID, const char *param, char *resultBuffer, uint8_t *resultBufferLength);


/// FUNCTIONS

/**
 * \brief	Check the GlobalVarList[], are set valid?
 * \return	true, if ok
 */
bool GlobalVarHandler_CheckCommandStructAreValid(void)
{
	// TODO: vagy itt futásidejűteszt, vagy makrókkal fordításidejű teszt?

	VarID_t i;
	bool hasError = false;

	for (i=0; i < GlobalVarMaxCommandNum; i++)
	{
		if (GlobalVarList[i].name == NULL)
		{
			hasError = true;
			break;
		}

		if (GlobalVarList[i].varPointer == NULL)
		{
			hasError = true;
			break;
		}

		if (GlobalVarList[i].type == Type_Error)
		{
			hasError = true;
			break;
		}

		if ( (GlobalVarList[i].type == Type_Enumerator) && (GlobalVarList[i].enumList == NULL) )
		{
			hasError = true;
			break;
		}

		if ( (GlobalVarList[i].type == Type_String) && (GlobalVarList[i].maxValue == 0) && (!GlobalVarList[i].isReadOnly) )
		{
			hasError = true;
			break;
		}

	}

	if (hasError)
	{
		// Error
		uprintf("Error in %d. (%s) command\r\n", i, GlobalVarList[i].name);
#ifdef CONFIG_DEBUG_MODE
		// Stop debugger
		__asm("BKPT #0\n");		// ASM: Break debugger
#endif
	}

	return !hasError;
}



/**
 * \brief	Process received command
 * \param	*commandName		received command name (string)
 * \param	*param				received parameters after command
 * \param	setGetType			Set or get command
 * \param	source				Command source (e.g. USART)
 * \param	*resultBuffer		Buffer, which writing with result
 * \param	resultBufferLength	Buffer length
 */
void GlobalVarHandler_ProcessCommand(
		const char *commandName, const char *param,
		SetGetType_t setGetType, CommandSource_t source,
		char *resultBuffer, uint8_t resultBufferLength)
{
	 ProcessResult_t result = Process_UnknownError;

	// Search command
	VarID_t commandID = 0;

	if (GlobalVarHandler_SearchVariableName(commandName,&commandID))
	{
		// Found, Check the source
		if ((source & GlobalVarList[commandID].sourceEnable) || (GlobalVarList[commandID].sourceEnable == Source_All))
		{
			// Source is enabled
			if (setGetType == SetGet_Get)
			{
				// Get
				result = GlobalVarHandler_GetCommand(commandID,resultBuffer,&resultBufferLength);
			}
			else if (setGetType == SetGet_Set)
			{
				// Can set? (not const?)
				if (!GlobalVarList[commandID].isReadOnly)
				{
					// It not const, can set
					result = GlobalVarHandler_SetCommand(commandID,param,resultBuffer,&resultBufferLength);
				}
				else
				{
					// Cannot set, it is read only
					result = Process_IsReadOnly;
				}
			}
			else if (setGetType == SetGet_Help)
			{
				GlobalVarHandler_PrintVariableDescriptions(commandID,resultBuffer);
				result = Process_Ok_Answered;
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
static bool GlobalVarHandler_SearchVariableName(const char *commandName, VarID_t *commandID)
{
	VarID_t i;

	// Search
	for (i=0; i<GlobalVarMaxCommandNum; i++)
	{
		if (!StrCmp(GlobalVarList[i].name, commandName))
		{
		// Found
		*commandID = i;
		return true;
		}
	}

	// Not found
	return false;

}



/**
 * \brief	Get command
 */
static ProcessResult_t GlobalVarHandler_GetCommand(VarID_t commandID, char *resultBuffer, uint8_t *resultBufferLength)
{
	uint8_t length = 0;

	// Check type
	switch(GlobalVarList[commandID].type)
	{
	case Type_Bool:
		{
			bool *boolPointer = (bool *)GlobalVarList[commandID].varPointer;
			if (*boolPointer)
			{
				length += StrCpyMax(resultBuffer,"1 / TRUE",*resultBufferLength);
			}
			else
			{
				length += StrCpyMax(resultBuffer,"0 / FALSE",*resultBufferLength);
			}
		}
		break;

	case Type_Uint8:
	case Type_Uint16:
	case Type_Uint32:
	case Type_Int8:
	case Type_Int16:
	case Type_Int32:
		GlobalVarHandler_GetIntegerVariable(commandID,resultBuffer,resultBufferLength);
		break;

	case Type_Float:
		{
			float *floatPointer = (float *)GlobalVarList[commandID].varPointer;
			float value = *floatPointer;
			FloatToString(value,resultBuffer,0,2);
		}
		break;

	case Type_String:
		{
			const char *string = GlobalVarList[commandID].varPointer;
			length += StrCpyMax(resultBuffer,string, *resultBufferLength-length);
		}
		break;

	case Type_Enumerator:
		length += GlobalVarHandler_GetEnumerator(commandID, resultBuffer, resultBufferLength);
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
	// TODO: Check length
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
 * \brief Get integer value
 */
static ProcessResult_t GlobalVarHandler_GetIntegerVariable(VarID_t commandID, char *resultBuffer, uint8_t *resultBufferLength)
{

	VarType_t type = GlobalVarList[commandID].type;

	if (GlobalVarList[commandID].isHex)
	{
		// Get in Hex format

		uint8_t octetNum = 0;
		switch(type)
		{
			case Type_Uint8:
				octetNum = 2;
				break;
			case Type_Uint16:
				octetNum = 4;
				break;
			case Type_Uint32:
				octetNum = 8;
				break;
			default:
				octetNum = 0;
				break;
		}

		if (octetNum != 0)
		{
			uint32_t *numPointer = (uint32_t *)GlobalVarList[commandID].varPointer;
			uint32_t num = *numPointer;
			// TODO: Buffer túlírás ellenőrzés
			uint8_t length = 0;
			length += StrCpyMax(resultBuffer,"0x",3);
			length += DecimalToHexaString(num, &resultBuffer[length], octetNum);
			*resultBufferLength -= length;
			return Process_Ok_Answered;
		}
		else
		{
			return Process_UnknownError;
		}
	}


	// If not hex
	// TODO: Szépíteni
	switch(type)
	{
	case Type_Uint8:
		{
			uint8_t *numPointer = (uint8_t *)GlobalVarList[commandID].varPointer;
			uint8_t num = *numPointer;
			// TODO: Buffer túlírás ellenőrzés
			*resultBufferLength -= UnsignedDecimalToString(num, resultBuffer);
		}
		break;

	case Type_Uint16:
		{
			uint16_t *numPointer = (uint16_t *)GlobalVarList[commandID].varPointer;
			uint16_t num = *numPointer;
			// TODO: Buffer túlírás ellenőrzés
			*resultBufferLength -= UnsignedDecimalToString(num, resultBuffer);
		}
		break;

	case Type_Uint32:
		{
			uint32_t *numPointer = (uint32_t *)GlobalVarList[commandID].varPointer;
			uint32_t num = *numPointer;
			// TODO: Buffer túlírás ellenőrzés
			*resultBufferLength -= UnsignedDecimalToString(num, resultBuffer);
		}
		break;

	case Type_Int8:
		{
			int8_t *numPointer = (int8_t *)GlobalVarList[commandID].varPointer;
			int8_t num = *numPointer;
			// TODO: Buffer túlírás ellenőrzés
			*resultBufferLength -= SignedDecimalToString(num, resultBuffer);
		}
		break;

	case Type_Int16:
		{
			int16_t *numPointer = (int16_t *)GlobalVarList[commandID].varPointer;
			int16_t num = *numPointer;
			// TODO: Buffer túlírás ellenőrzés
			*resultBufferLength -= SignedDecimalToString(num, resultBuffer);
		}
		break;

	case Type_Int32:
		{
			int32_t *numPointer = (int32_t *)GlobalVarList[commandID].varPointer;
			int32_t num = *numPointer;
			// TODO: Buffer túlírás ellenőrzés
			*resultBufferLength -= SignedDecimalToString(num, resultBuffer);
		}
		break;

	default:
		// Wrong case
		return Process_UnknownError;
		break;
	}

	return Process_Ok_Answered;
}



/**
 * \brief	Get enumerators
 */
static uint8_t GlobalVarHandler_GetEnumerator(const VarID_t commandID, char *resultBuffer, uint8_t *resultBufferLength)
{
	uint8_t *enumPointer = (uint8_t *)GlobalVarList[commandID].varPointer;
	uint8_t enumValue = *enumPointer;
	uint8_t length = 0;		// TODO: Lehet, hogy nem így érdemes a hosszt figyelni
	char *enumString;

	// Print enum value
	length += UnsignedDecimalToString(enumValue, resultBuffer);
	length += StrCpy(&resultBuffer[length], " ");
	// TODO: Check overflow

	// Check "enumList" pointer
	if (GlobalVarList[commandID].enumList == NULL)
	{
		length += StrCpyMax(&resultBuffer[length],"ERROR - There is not set \"enumList\" pointer", *resultBufferLength-length);
		return length;
	}

	// If has good enumList
	// Check value (It is not too high?)
	if (enumValue > GlobalVarList[commandID].maxValue)
	{
		length += StrCpyMax(&resultBuffer[length],"ERROR - Enum has too high value", *resultBufferLength-length);
		return length;
	}

	// Good value
	// Print enum string
	enumString = (char *)GlobalVarList[commandID].enumList[enumValue];	// string pointer
	length += StrCpyMax(&resultBuffer[length],enumString, *resultBufferLength-length);

	// Return length
	return length;

}



/**
 * \brief	Set command
 */
static ProcessResult_t GlobalVarHandler_SetCommand(const VarID_t commandID, const char *param, char *resultBuffer, uint8_t *resultBufferLength)
{

	uint8_t length = 0;
	ProcessResult_t result = Process_UnknownError;

	switch(GlobalVarList[commandID].type)
	{
	case Type_Bool:
		result = GlobalVarHandler_SetBool(commandID, param, resultBuffer, resultBufferLength);
		break;

	case Type_Uint8:
	case Type_Uint16:
	case Type_Uint32:
	case Type_Int8:
	case Type_Int16:
	case Type_Int32:
		result = GlobalVarHandler_SetInteger(commandID, param, resultBuffer, resultBufferLength);
		break;

	case Type_Float:
		{
			float floatValue;
			if (StringToFloat(param,&floatValue))
			{
				// Successful convert
				result = GlobalVarHandler_CheckValue(commandID,(uint32_t)(int32_t)floatValue);
				if (result == Process_Ok_SetSuccessful_SendOk)
				{
					// Value is OK
					float *floatPointer = (float *)GlobalVarList[commandID].varPointer;
					*floatPointer = floatValue;
					return Process_Ok_SetSuccessful_SendOk;
				}
				else
				{
					// Wrong value (too high or too less)
					return result;
				}
			}
			else
			{
				return Process_FailParamIsNotNumber;
			}
			return Process_FailParam;
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

			// TODO: Nincs hossz ellenőrzés és arra riasztás, levágja

			char *string = (char *)GlobalVarList[commandID].varPointer;

			length += StrCpyMax(string,param,stringLength);

			result = Process_Ok_SetSuccessful_SendOk;
		}
		break;

	case Type_Enumerator:
		result = GLobalVarHandler_SetEnumerator(commandID, param, resultBuffer, resultBufferLength);
		break;

	default:
		return Process_FailParam;
		break;
	}

	if (result != Process_Ok_SetSuccessful_SendOk)
	{
		return result;
	}

	// Check length
	// TODO: Check length with correct
	if (length <= *resultBufferLength)
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
 * \brief	Set bool variable
 */
static ProcessResult_t GlobalVarHandler_SetBool(VarID_t commandID, const char *param, char *resultBuffer, uint8_t *resultBufferLength)
{

	uint32_t num;
	bool boolVal;
	// Check it is decimal?
	if (StringToUnsignedDecimalNum(param, &num))
	{
		if (num == 1)
		{
			boolVal = true;
		}
		else if (num == 0)
		{
			boolVal = false;
		}
		else
		{
			// Wrong num (not 0, and not 1)
			return Process_InvalidValue_NotBool;
		}
	}
	else
	{
		// Not number

		// Check it is "true" / "false" ?
		if (!StrCmp((const char*)param,"true"))
		{
			boolVal = true;
		}
		else if (!StrCmp((const char*)param,"false"))
		{
			boolVal = false;
		}
		// Check it is "on" / "off"?
		else if (!StrCmp((const char*)param,"on"))
		{
			boolVal = true;
		}
		else if (!StrCmp((const char*)param,"off"))
		{
			boolVal = false;
		}
		// Check it is "enable" / "disable"
		else if (!StrCmp((const char*)param,"enable"))
		{
			boolVal = true;
		}
		else if (!StrCmp((const char*)param,"disable"))
		{
			boolVal = false;
		}
		else
		{
			// Not good "string"
			return Process_InvalidValue_NotBool;
		}
	}
	// If reach here, boolVal is contain a valid value
	bool *bPointer = (bool *)GlobalVarList[commandID].varPointer;
	*bPointer = boolVal;

	return Process_Ok_SetSuccessful_SendOk;
}



/**
 * \brief	Set integer global variable
 */
static ProcessResult_t GlobalVarHandler_SetInteger(VarID_t commandID, const char *param, char *resultBuffer, uint8_t *resultBufferLength)
{
	VarType_t varType = GlobalVarList[commandID].type;
	uint32_t num;

	// If hex
	if (GlobalVarList[commandID].isHex)
	{
		uint8_t length = 0;
		if (param[0] == '0' && param[1] == 'x')
		{
			length = 2;
		}
		else
		{
			// Need set with "0x"
			return Process_FailParamIsNotHexStart;
		}

		// Convert HexString to Num
		if (StringHexToNum(&param[length],&num))
		{
			// Is good num?
			ProcessResult_t result = GlobalVarHandler_CheckValue(commandID, num);
			if ( result == Process_Ok_SetSuccessful_SendOk)
			{
				switch(varType)
				{
				case Type_Uint8:
					{
						uint8_t *wPointer = (uint8_t *)GlobalVarList[commandID].varPointer;
						*wPointer = num;
					}
					break;
				case Type_Uint16:
					{
						uint16_t *wPointer = (uint16_t *)GlobalVarList[commandID].varPointer;
						*wPointer = num;
					}
					break;
				case Type_Uint32:
					{
						uint32_t *wPointer = (uint32_t *)GlobalVarList[commandID].varPointer;
						*wPointer = num;
					}
					break;
				default:
					return Process_UnknownError;
					break;
				}

				return Process_Ok_SetSuccessful_SendOk;
			}
			else
			{
				// Too much or small
				return result;
			}
		}
		else
		{
			// Wrong hex converting
			return Process_FailParamIsNotHexNumber;
		}
	} // End of "isHex"

	// If it is not hex
	// It is unsigned integers?
	if (varType == Type_Uint8 || varType == Type_Uint16 || varType == Type_Uint32)
	{
		// Unsigned types

		uint32_t num = 0;
		if (StringToUnsignedDecimalNum(param, &num))
		{
			ProcessResult_t result;
			result = GlobalVarHandler_CheckValue(num,commandID);

			if (result == Process_Ok_SetSuccessful_SendOk)
			{
				// Good
				if (varType == Type_Uint8)
				{
					uint8_t *numPointer = (uint8_t *)GlobalVarList[commandID].varPointer;
					*numPointer = num;
				}
				else if (varType == Type_Uint16)
				{
					uint16_t *numPointer = (uint16_t *)GlobalVarList[commandID].varPointer;
					*numPointer = num;
				}
				else if (varType == Type_Uint32)
				{
					uint32_t *numPointer = (uint32_t *)GlobalVarList[commandID].varPointer;
					*numPointer = num;
				}
				//else - not reaching
				return Process_Ok_SetSuccessful_SendOk;
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

	// It is signed integers?
	if (varType == Type_Int8 || varType == Type_Int16 || varType == Type_Int32)
	{
		// Signed types

		int32_t num = 0;
		if (StringToSignedDecimalNum(param, &num))
		{
			ProcessResult_t result;
			result = GlobalVarHandler_CheckValue(num,commandID);

			if (result == Process_Ok_SetSuccessful_SendOk)
			{
				// Good
				if (varType == Type_Int8)
				{
					uint8_t *numPointer = (uint8_t *)GlobalVarList[commandID].varPointer;
					*numPointer = num;
				}
				else if (varType == Type_Int16)
				{
					uint16_t *numPointer = (uint16_t *)GlobalVarList[commandID].varPointer;
					*numPointer = num;
				}
				else if (varType == Type_Int32)
				{
					uint32_t *numPointer = (uint32_t *)GlobalVarList[commandID].varPointer;
					*numPointer = num;
				}
				//else - not reaching
				return Process_Ok_SetSuccessful_SendOk;
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

	return Process_UnknownError;
}



/**
 * \brief	Set enumerator
 */
static ProcessResult_t GLobalVarHandler_SetEnumerator(VarID_t commandID, const char *param, char *resultBuffer, uint8_t *resultBufferLength)
{
	uint32_t enumValue = 0;
	uint8_t *enumPointer = (uint8_t *)GlobalVarList[commandID].varPointer;
	uint8_t i;

	// Check enumList pointer
	if (GlobalVarList[commandID].enumList == NULL)
	{
		return Process_Settings_EmptyEnumList;
	}

	// It is number
	if (StringIsUnsignedDecimalString(param))
	{
		// It is number
		if (StringToUnsignedDecimalNum(param, &enumValue))
		{
			if (GlobalVarHandler_CheckValue(commandID,enumValue) == Process_Ok_SetSuccessful_SendOk)
			{
				// It is Ok
				*enumPointer = (uint8_t)enumValue;
				return Process_Ok_SetSuccessful_SendOk;
			}
			else
			{
				return Process_InvalidValue_TooMuch;
			}
		}
	}
	else
	{
		// Not number, check string
		for (i=0; i<GlobalVarList[commandID].maxValue; i++)
		{
			// It is equal string?
			if (StrCmp(param, GlobalVarList[commandID].enumList[i])==0)
			{
				// Equal
				// Set value
				*enumPointer = i;
				return Process_Ok_SetSuccessful_SendOk;
			}
		}

		// Not found, it is invalid string
		return Process_InvalidValue_NotEnumString;
	}

	return Process_UnknownError;
}



/**
 * \brief	Check values
 * \return	Process_Ok_SetSuccessful_SendOk, if ok
 */
static ProcessResult_t GlobalVarHandler_CheckValue(VarID_t commandID, uint32_t num)
{

	// First, check the type value
	switch (GlobalVarList[commandID].type)
	{
		case Type_Bool:
			if (num > BOOL_MAX)
				return Process_InvalidValue_TooMuch;
			break;
		case Type_Uint8:
			if (num > UINT8_MAX)
				return Process_InvalidValue_TooMuch;
			break;
		case Type_Uint16:
			if (num > UINT16_MAX)
				return Process_InvalidValue_TooMuch;
			break;
		case Type_Uint32:
			// TODO: Always good, do better code
			if (num > UINT32_MAX)
				return Process_InvalidValue_TooMuch;
			break;
		case Type_Int8:
			if ((int32_t)num > INT8_MAX)
				return Process_InvalidValue_TooMuch;
			if ((int32_t)num < INT8_MIN)
				return Process_InvalidValue_TooSmall;
			break;
		case Type_Int16:
			if ((int32_t)num > INT16_MAX)
				return Process_InvalidValue_TooMuch;
			if ((int32_t)num < INT16_MIN)
				return Process_InvalidValue_TooSmall;
			break;
		case Type_Int32:
			// TODO: Always good, do better code
			if ((int32_t)num > INT32_MAX)
				return Process_InvalidValue_TooMuch;
			if ((int32_t)num < INT32_MIN)
				return Process_InvalidValue_TooSmall;
			break;
		case Type_Float:
			// TODO: Not a good compare in float type
			if ((int32_t)num > INT32_MAX)
				return Process_InvalidValue_TooMuch;
			if ((int32_t)num < INT32_MIN)
				return Process_InvalidValue_TooSmall;
			break;
		case Type_String:
			if (num > GlobalVarList[commandID].maxValue)
				return Process_TooLongString;
			break;
		case Type_Enumerator:
			if (num >= GlobalVarList[commandID].maxValue)
				return Process_InvalidValue_TooMuch;
			break;
		default:
			return Process_FailType;
			break;
	}


	// Check maxValue
	// Maxvalue is set?
	if (GlobalVarList[commandID].maxValue == GlobalVarList[commandID].minValue)
	{
		// Is not set
		return Process_Ok_SetSuccessful_SendOk;
	}
	else
	{
		// is set, because max not equal than min

		if (num > GlobalVarList[commandID].maxValue) return Process_InvalidValue_TooMuch;

		if (num < GlobalVarList[commandID].minValue) return Process_InvalidValue_TooSmall;

	}

	return Process_Ok_SetSuccessful_SendOk;
}



/**
 * \brief	Write process result
 */
static void GlobalVarHandler_WriteResults(ProcessResult_t result, char *resultBuffer, uint8_t resultBufferLength)
{

	switch (result)
	{

		case Process_Ok_Answered:
			// Do nothing
			break;

		case Process_Ok_SetSuccessful_SendOk:
			StrCpyMax(resultBuffer, "Set successful!", resultBufferLength);
			break;

		case Process_CommandNotFound:
			StrCpyMax(resultBuffer, "Command not find!", resultBufferLength);
			break;

		case Process_FailParam:
			StrCpyMax(resultBuffer, "Fail parameter", resultBufferLength);
			break;

		case Process_FailType:
			StrCpyMax(resultBuffer, "Fail type", resultBufferLength);
			break;

		case Process_FailParamIsNotNumber:
			StrCpyMax(resultBuffer, "Not number", resultBufferLength);
			break;

		case Process_FailParamIsNotHexNumber:
			StrCpyMax(resultBuffer, "Not hex number", resultBufferLength);
			break;

		case Process_FailParamIsNotHexStart:
			StrCpyMax(resultBuffer, "Not hex, missed \"0x\"",
					resultBufferLength);
			break;

		case Process_InvalidValue_TooSmall:
			StrCpyMax(resultBuffer, "Invalid value, too small",
					resultBufferLength);
			break;

		case Process_InvalidValue_TooMuch:
			StrCpyMax(resultBuffer, "Invalid value, too much",
					resultBufferLength);
			break;

		case Process_InvalidValue_NotBool:
			StrCpyMax(resultBuffer, "Invalid value, not bool",
					resultBufferLength);
			break;

		case Process_InvalidValue_NotEnumString:
			StrCpyMax(resultBuffer, "Invalid enum string", resultBufferLength);
			break;

		case Process_Settings_EmptyEnumList:
			StrCpyMax(resultBuffer, "EnumList settings error",
					resultBufferLength);
			break;

		case Process_IsReadOnly:
			StrCpyMax(resultBuffer, "Cannot set, it is constant",
					resultBufferLength);
			break;

		case Process_SourceNotEnabled:
			StrCpyMax(resultBuffer,
					"Cannot process this command from this source",
					resultBufferLength);
			break;

		case Process_TooLongString:
			StrCpyMax(resultBuffer, "Too long string", resultBufferLength);
			break;

		case Process_UnknownError:
			StrCpyMax(resultBuffer, "Unknown error", resultBufferLength);
			break;

		default:
			StrCpyMax(resultBuffer, "Fatal error", resultBufferLength);
			break;
	}
}



/**
 * \brief	List all variables
 */
void GlobalVarHandler_ListAllVariableParameters(void)
{
	uint8_t i;

	// TODO: Enumokat is kiírni, ha van?
	const char *header = "+-ID-+-------Name-----------+----Type----+--min--+--max--+-unit-+-----Description------+";

	// Send header
	USART_SendLine(header);

	// Rows (commands)
	for (i=0; i<GlobalVarMaxCommandNum; i++)
	{

		// Print one command / line:
		uprintf("| %2d | %20s | %10s | %5d | %5d | %4s | %20s |\r\n",
				i,
				GlobalVarList[i].name,
				GlobalVarTypesNames[GlobalVarList[i].type],
				GlobalVarList[i].minValue,
				GlobalVarList[i].maxValue,
				GlobalVarList[i].unit,
				GlobalVarList[i].description
				);
	}

	// After commands (end)
	USART_SendLine(header);

}



/**
 * \brief	Print all variable values
 */
void GlobalVarHandler_PrintAllVariableValues (void)
{
	uint8_t i;
	char resultBuffer[20];
	uint8_t resultBufferLength;

	uprintf("Global variables:\r\n"
			" <Name>               | <Value>\r\n");

	// Print all variables
	for (i=0; i<GlobalVarMaxCommandNum; i++)
	{
		// Print a variable name and value
		resultBufferLength = 20;
		GlobalVarHandler_GetCommand(i,resultBuffer,&resultBufferLength);
		uprintf(" %20s %20s\r\n", GlobalVarList[i].name, resultBuffer);
	}

	uprintf("End of global variables\r\n");
}



/**
 * \brief	Print global variable descriptions
 */
static void GlobalVarHandler_PrintVariableDescriptions (VarID_t commandID, char *resultBuffer)
{

	usprintf(resultBuffer, "Command help: %s, type:%s, min:%d, max:%d, desc:%s\r\n",
			GlobalVarList[commandID].name,
			GlobalVarTypesNames[GlobalVarList[commandID].type],
			GlobalVarList[commandID].minValue,
			GlobalVarList[commandID].maxValue,
			GlobalVarList[commandID].description
			);

}
