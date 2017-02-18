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


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define BOOL_MAX	(1)

/*
#define UINT8_MAX	(0xFF)
#define INT8_MAX	(0x7F)
#define UINT16_MAX	(0xFFFF)
#define INT16_MAX	(0x7FFF)
#define UINT32_MAX	(0xFFFFFFFF)
#define INT32_MAX	(0x7FFFFFFF)
*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


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
// TODO: Lehet, hogy lekérdező függvénybe kéne rakni?
static const char const *GlobalVarTypesNames[] =
{
	// NOTE: Important!! Must be in the same order with VarType_t
	"unknown",
	"bool",
	"uint8_t",
	"int8_t",
	"uint16_t",
	"int16_t",
	"uint32_t",
	"int32_t",
	"float",
	"bits",
	"string",
	"enumerator",
	NULL
};

// TODO: Macro for checking
/*
#if ((sizeof(GlobalVarTypesNames)/sizeof(GlobalVarTypesNames[0])) != Type_Count)
#error "Syncronize 'Type_Count' with 'GlobalVarTypesNames'"
#endif
*/


static CommProtocol_t GlobalVarHandler_Source = CommProt_Unknown;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static ProcessResult_t GlobalVarHandler_GetCommand(VarID_t commandID);
static bool GlobalVarHandler_SearchVariableName(const char *commandName, VarID_t *commandID);
static ProcessResult_t GlobalVarHandler_SetCommand(const VarID_t commandID, const char *param);
static void GlobalVarHandler_WriteResults(ProcessResult_t result);
static ProcessResult_t GlobalVarHandler_CheckValue(VarID_t commandID, uint32_t num);
static void GlobalVarHandler_PrintVariableDescriptions (VarID_t commandID);

static void GlobalVarHandler_GetIntegerVariable(VarID_t commandID);
static void GlobalVarHandler_GetBits(const VarID_t commandID);
static void GlobalVarHandler_GetEnumerator(const VarID_t commandID);

static ProcessResult_t GlobalVarHandler_SetBool(VarID_t commandID, const char *param);
static ProcessResult_t GlobalVarHandler_SetInteger(VarID_t commandID, const char *param);
static ProcessResult_t GlobalVarHandler_SetFloat(VarID_t commandID, const char *param);
static ProcessResult_t GlobalVarHandler_SetBits(VarID_t commandID, const char *param);
static ProcessResult_t GlobalVarHandler_SetString(VarID_t commandID, const char *param);
static ProcessResult_t GlobalVarHandler_SetEnumerator(VarID_t commandID, const char *param);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Check the GlobalVarList[], are set valid?
 * \retval	true	 ok
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

		if (StringLength(GlobalVarList[i].name) >= GLOBALVARHANDLER_NAME_MAX_LENGTH)
		{
			hasError = true;
			break;
		}

		if (GlobalVarList[i].varPointer == NULL)
		{
			hasError = true;
			break;
		}

		if (GlobalVarList[i].type == Type_Unknown)
		{
			hasError = true;
			break;
		}

		if ((GlobalVarList[i].type == Type_Enumerator) && (GlobalVarList[i].enumList == NULL))
		{
			hasError = true;
			break;
		}

		if ((GlobalVarList[i].type == Type_String) && (GlobalVarList[i].maxValue == 0) && (!GlobalVarList[i].isReadOnly))
		{
			hasError = true;
			break;
		}

		if (GlobalVarList[i].type == Type_Bits)
		{
			if (GlobalVarList[i].maxValue > 31)
			{
				hasError = true;
				break;
			}
			if (GlobalVarList[i].minValue > 31)
			{
				hasError = true;
				break;
			}
			if (GlobalVarList[i].minValue > GlobalVarList[i].maxValue)
			{
				hasError = true;
				break;
			}
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
 */
void GlobalVarHandler_ProcessCommand(
		const char *commandName, const char *param,
		SetGetType_t setGetType, CommProtocol_t source)
{
	 ProcessResult_t result = Process_Unknown;

	// Search command
	VarID_t commandID = 0;

	if (GlobalVarHandler_SearchVariableName(commandName, &commandID))
	{
		// Found, Check the source
		if ((source & GlobalVarList[commandID].sourceEnable) || (GlobalVarList[commandID].sourceEnable == CommProt_Unknown))
		{
			// Source is enabled
			if (setGetType == SetGet_Get)
			{
				// Get
				result = GlobalVarHandler_GetCommand(commandID);
			}
			else if (setGetType == SetGet_Set)
			{
				// Can set? (not const?)
				if (!GlobalVarList[commandID].isReadOnly)
				{
					// It not const, can set
					result = GlobalVarHandler_SetCommand(commandID, param);
				}
				else
				{
					// Cannot set, it is read only
					result = Process_IsReadOnly;
				}
			}
			else if (setGetType == SetGet_Help)
			{
				GlobalVarHandler_PrintVariableDescriptions(commandID);
				result = Process_Ok_Answered;
			}
			else
			{
				result = Process_Unknown;
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
		result = Process_GlobalVariableNameNotFind;
	}

	GlobalVarHandler_WriteResults(result);
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
static ProcessResult_t GlobalVarHandler_GetCommand(VarID_t commandID)
{
	// Check type
	switch (GlobalVarList[commandID].type)
	{
		case Type_Bool:
		{
			bool *boolPointer = (bool *)GlobalVarList[commandID].varPointer;
			if (*boolPointer)
			{
				COMMUNICATION_SendMessage(GlobalVarHandler_Source, "1 / TRUE");
			}
			else
			{
				COMMUNICATION_SendMessage(GlobalVarHandler_Source, "0 / FALSE");
			}
		}
			break;

		case Type_Uint8:
		case Type_Uint16:
		case Type_Uint32:
		case Type_Int8:
		case Type_Int16:
		case Type_Int32:
			GlobalVarHandler_GetIntegerVariable(commandID);
			break;

		case Type_Float:
		{
			float *floatPointer = (float *)GlobalVarList[commandID].varPointer;
			float value = *floatPointer;
			COMMUNICATION_Printf(GlobalVarHandler_Source, "%8.2f", value);
		}
			break;

		case Type_Bits:
			GlobalVarHandler_GetBits(commandID);
			break;

		case Type_String:
		{
			const char *string = GlobalVarList[commandID].varPointer;
			COMMUNICATION_SendMessage(GlobalVarHandler_Source, string);
		}
			break;

		case Type_Enumerator:
			GlobalVarHandler_GetEnumerator(commandID);
			break;

		// Wrong types
		case Type_Unknown:
		case Type_Count:
		default:
			return Process_FailParam;
			break;
	}


#ifdef GLOBALVARHANDLER_UNIT_ENABLE
	// Append unit, if need
	if (GlobalVarList[commandID].unit)
	{
		// Print unit
		COMMUNICATION_Printf(GlobalVarHandler_Source, " %s", GlobalVarList[commandID].unit);
	}
#endif

	return Process_Ok_Answered;
}



/**
 * \brief	Get integer value
 * 			NOTE: Be careful, commandID not checked
 */
static void GlobalVarHandler_GetIntegerVariable(VarID_t commandID)
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

			// Wrong types
			case Type_Int8:
			case Type_Int16:
			case Type_Int32:
			case Type_Bool:
			case Type_Enumerator:
			case Type_Float:
			case Type_Bits:
			case Type_String:
			case Type_Unknown:
			case Type_Count:
			default:
				octetNum = 0;
				break;
		}

		if (octetNum != 0)
		{
			// Octet num is ok
			uint32_t *numPointer = (uint32_t *)GlobalVarList[commandID].varPointer;
			uint32_t num = *numPointer;
			char format[10];
			usprintf(format, "0x%dX", octetNum);
			COMMUNICATION_Printf(GlobalVarHandler_Source, format, num);
		}
		else
		{
			// Error, octet = 0
		}
	}
	else
	{
		// If not hex
		switch (type)
		{
			case Type_Uint8:
			{
				uint8_t *numPointer =
						(uint8_t *)GlobalVarList[commandID].varPointer;
				uint8_t num = *numPointer;
				COMMUNICATION_Printf(GlobalVarHandler_Source, "%u", num);
			}
				break;

			case Type_Uint16:
			{
				uint16_t *numPointer =
						(uint16_t *)GlobalVarList[commandID].varPointer;
				uint16_t num = *numPointer;
				COMMUNICATION_Printf(GlobalVarHandler_Source, "%u", num);
			}
				break;

			case Type_Uint32:
			{
				uint32_t *numPointer =
						(uint32_t *)GlobalVarList[commandID].varPointer;
				uint32_t num = *numPointer;
				COMMUNICATION_Printf(GlobalVarHandler_Source, "%u", num);
			}
				break;

			case Type_Int8:
			{
				int8_t *numPointer =
						(int8_t *)GlobalVarList[commandID].varPointer;
				int8_t num = *numPointer;
				COMMUNICATION_Printf(GlobalVarHandler_Source, "%d", num);
			}
				break;

			case Type_Int16:
			{
				int16_t *numPointer =
						(int16_t *)GlobalVarList[commandID].varPointer;
				int16_t num = *numPointer;
				COMMUNICATION_Printf(GlobalVarHandler_Source, "%d", num);
			}
				break;

			case Type_Int32:
			{
				int32_t *numPointer =
						(int32_t *)GlobalVarList[commandID].varPointer;
				int32_t num = *numPointer;
				COMMUNICATION_Printf(GlobalVarHandler_Source, "%d", num);
			}
				break;

			// Wrong types
			case Type_Bool:
			case Type_Enumerator:
			case Type_Float:
			case Type_Bits:
			case Type_String:
			case Type_Unknown:
			case Type_Count:
			default:
				// Wrong case
				break;
		}

	}

	return;
}



/**
 * \brief
 */
static void GlobalVarHandler_GetBits(const VarID_t commandID)
{
	uint32_t *numPointer = (uint32_t *)GlobalVarList[commandID].varPointer;
	uint32_t num = *numPointer;

	// xxxx11111yyyyyy
	//     mask  shift
	uint8_t shift = GlobalVarList[commandID].minValue;
	uint8_t bitLength = GlobalVarList[commandID].maxValue - shift;
	// TODO: Check minValue and maxValue

	// Shift to right, and mask to make our important bits
	num = (num >> shift) & (power(2,bitLength)-1);

	COMMUNICATION_Printf(GlobalVarHandler_Source, "0b%b", num);

	return;
}



/**
 * \brief	Get enumerators
 */
static void GlobalVarHandler_GetEnumerator(const VarID_t commandID)
{
	uint8_t *enumPointer = (uint8_t *)GlobalVarList[commandID].varPointer;
	uint8_t enumValue = *enumPointer;
	char *enumString;

	// Print enum value
	COMMUNICATION_Printf(GlobalVarHandler_Source, "%d ", enumValue);

	// Check "enumList" pointer
	if (GlobalVarList[commandID].enumList == NULL)
	{
		// TODO: Add new result type?
		COMMUNICATION_SendMessage(GlobalVarHandler_Source, "ERROR - There is not set \"enumList\" pointer");
		return;
	}

	// If has good enumList
	// Check value (It is not too high?)
	if (enumValue > GlobalVarList[commandID].maxValue)
	{
		// TODO: Add new result type?
		COMMUNICATION_SendMessage(GlobalVarHandler_Source, "ERROR - Enum has too high value");
		return;
	}

	// Good value
	// Print enum string
	enumString = (char *)GlobalVarList[commandID].enumList[enumValue];	// string pointer
	COMMUNICATION_SendMessage(GlobalVarHandler_Source, enumString);

	// Return
	return;

}



/**
 * \brief	Set command
 */
static ProcessResult_t GlobalVarHandler_SetCommand(const VarID_t commandID, const char *param)
{

	ProcessResult_t result = Process_Unknown;

	switch (GlobalVarList[commandID].type)
	{
		case Type_Bool:
			result = GlobalVarHandler_SetBool(commandID, param);
			break;

		case Type_Uint8:
		case Type_Uint16:
		case Type_Uint32:
		case Type_Int8:
		case Type_Int16:
		case Type_Int32:
			result = GlobalVarHandler_SetInteger(commandID, param);
			break;

		case Type_Float:
			result = GlobalVarHandler_SetFloat(commandID, param);
			break;

		case Type_Bits:
			result = GlobalVarHandler_SetBits(commandID, param);
			break;

		case Type_String:
			result = GlobalVarHandler_SetString(commandID, param);
			break;

		case Type_Enumerator:
			result = GlobalVarHandler_SetEnumerator(commandID, param);
			break;

		// Wrong types
		case Type_Unknown:
		case Type_Count:
		default:
			result = Process_FailType;
			break;
	}

	// Return with result
	return result;
}



/**
 * \brief	Set bool variable
 */
static ProcessResult_t GlobalVarHandler_SetBool(VarID_t commandID, const char *param)
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
		if (!StrCmp((const char*)param, "true"))
		{
			boolVal = true;
		}
		else if (!StrCmp((const char*)param, "false"))
		{
			boolVal = false;
		}
		// Check it is "on" / "off"?
		else if (!StrCmp((const char*)param, "on"))
		{
			boolVal = true;
		}
		else if (!StrCmp((const char*)param, "off"))
		{
			boolVal = false;
		}
		// Check it is "enable" / "disable"
		else if (!StrCmp((const char*)param, "enable"))
		{
			boolVal = true;
		}
		else if (!StrCmp((const char*)param, "disable"))
		{
			boolVal = false;
		}
		// Check it is "set" / "reset"
		else if (!StrCmp((const char*)param, "set"))
		{
			boolVal = true;
		}
		else if (!StrCmp((const char*)param, "reset"))
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
static ProcessResult_t GlobalVarHandler_SetInteger(VarID_t commandID, const char *param)
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
			if (result == Process_Ok_SetSuccessful_SendOk)
			{
				switch (varType)
				{
					case Type_Uint8:
					{
						uint8_t *wPointer =
								(uint8_t *)GlobalVarList[commandID].varPointer;
						*wPointer = num;
					}
						break;
					case Type_Uint16:
					{
						uint16_t *wPointer =
								(uint16_t *)GlobalVarList[commandID].varPointer;
						*wPointer = num;
					}
						break;
					case Type_Uint32:
					{
						uint32_t *wPointer =
								(uint32_t *)GlobalVarList[commandID].varPointer;
						*wPointer = num;
					}
						break;

					// Wrong types
					case Type_Int8:
					case Type_Int16:
					case Type_Int32:
					case Type_Bool:
					case Type_Float:
					case Type_Bits:
					case Type_Enumerator:
					case Type_String:
					case Type_Unknown:
					case Type_Count:
					default:
						return Process_Unknown;
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

	// TODO: else if legyen
	// If it is not hex
	// It is unsigned integers?
	if (varType == Type_Uint8 || varType == Type_Uint16 || varType == Type_Uint32)
	{
		// Unsigned types

		uint32_t num = 0;
		if (StringToUnsignedDecimalNum(param, &num))
		{
			ProcessResult_t result;
			result = GlobalVarHandler_CheckValue(commandID, num);

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
			result = GlobalVarHandler_CheckValue(commandID, num);

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

	return Process_Unknown;
}



/**
 * \brief	Set float type GlobalVar
 */
static ProcessResult_t GlobalVarHandler_SetFloat(VarID_t commandID, const char *param)
{
	ProcessResult_t result = Process_Unknown;

	float floatValue = 0.0f;
	if (StringToFloat(param, &floatValue))
	{
		// Successful convert
		result = GlobalVarHandler_CheckValue(commandID,(uint32_t)(int32_t)floatValue);
		if (result == Process_Ok_SetSuccessful_SendOk)
		{
			// Value is OK
			float *floatPointer = (float *)GlobalVarList[commandID].varPointer;
			*floatPointer = floatValue;
		}
		/*else
		{
			// Wrong value (too high or too less)
			// Do nothing
		}*/
	}
	else
	{
		result = Process_FailParamIsNotNumber;
	}

	return result;
}



/**
 * \brief	Set bits type GlobalVar
 */
static ProcessResult_t GlobalVarHandler_SetBits(VarID_t commandID, const char *param)
{
	uint32_t num = 0;
	bool isOk = true;
	ProcessResult_t result = Process_Unknown;
	uint8_t maxLength = GlobalVarList[commandID].maxValue - GlobalVarList[commandID].minValue + 1;

	// Check prefix, need '0b'
	if ((StringLength(param) <= 2) || (param[0] != '0') || (param[1] != 'b'))
	{
		// Too short or there is no prefix
		result = Process_FailParamIsNotBinary;
		isOk = false;
	}

	if (isOk)
	{
		// Check length
		if (StringLength(param) > maxLength + 2)
		{
			result = Process_FailParamTooLongBinary;
			isOk = false;
		}
	}

	if (isOk)
	{
		// Convert binary string to num
		if (StringBinaryToNum(&param[2], &num))
		{
			// Convert is ok, make correct value
			num <<= GlobalVarList[commandID].minValue;

			// Check num value
			result = GlobalVarHandler_CheckValue(commandID, num);

			if (result == Process_Ok_SetSuccessful_SendOk)
			{
				// Value is ok
				uint32_t *valueNeedSet = (uint32_t *)GlobalVarList[commandID].varPointer;
				// Clear bits
				uint8_t i;
				for (i = GlobalVarList[commandID].minValue;
					 i < GlobalVarList[commandID].maxValue;
					 i++)
				{
					// Clear bit
					*valueNeedSet &= ~(1 << i);
				}
				// Set new value
				*valueNeedSet |= num;
			}
		}
		else
		{
			// Convert failed
			result = Process_FailParamIsNotBinary;
		}
	}

	return result;

}



/**
 * \brief	Set string type GlobalVar
 */
static ProcessResult_t GlobalVarHandler_SetString(VarID_t commandID, const char *param)
{
	ProcessResult_t result = Process_Unknown;

	// Check length
	uint8_t stringLength = StringLength(param);
	if (stringLength >= GlobalVarList[commandID].maxValue)
	{
		// Too long
		result = Process_FailParamTooLongString;
	}
	else
	{
		// Correct length
		char *string = (char *)GlobalVarList[commandID].varPointer;
		// Copy parameter
		StrCpyMax(string, param, stringLength);

		result = Process_Ok_SetSuccessful_SendOk;
	}

	return result;
}



/**
 * \brief	Set enumerator
 */
static ProcessResult_t GlobalVarHandler_SetEnumerator(VarID_t commandID, const char *param)
{
	ProcessResult_t result = Process_Unknown;
	uint32_t enumValue = 0;
	uint8_t *enumPointer = (uint8_t *)GlobalVarList[commandID].varPointer;
	uint8_t i;

	// Check enumList pointer
	if (GlobalVarList[commandID].enumList == NULL)
	{
		result = Process_Settings_EmptyEnumList;
	}
	else
	{
		// Check, it is number?
		if (StringIsUnsignedDecimalString(param))
		{
			// It is number
			if (StringToUnsignedDecimalNum(param, &enumValue))
			{
				if (GlobalVarHandler_CheckValue(commandID,enumValue) == Process_Ok_SetSuccessful_SendOk)
				{
					// It is Ok
					*enumPointer = (uint8_t)enumValue;
					result = Process_Ok_SetSuccessful_SendOk;
				}
				else
				{
					result = Process_InvalidValue_TooMuch;
				}
			}
		}
		else
		{
			// Not number, check string
			for (i = 0; i < GlobalVarList[commandID].maxValue; i++)
			{
				// It is equal string?
				if (StrCmp(param, GlobalVarList[commandID].enumList[i]) == 0)
				{
					// Equal
					// Set value
					*enumPointer = i;
					result = Process_Ok_SetSuccessful_SendOk;
				}
			}

			// Not found, it is invalid string
			result = Process_InvalidValue_NotEnumString;
		}
	}

	if (result != Process_Ok_SetSuccessful_SendOk && result != Process_Settings_EmptyEnumList)
	{
		// Wrong, send enumerator strings
		CommandHandler_Printf("Invalid enum value, \"%s\" command has this enumerator strings:\r\n",
				GlobalVarList[commandID].name);
		for (i = 0; i < GlobalVarList[commandID].maxValue; i++)
		{
			CommandHandler_Printf("- %s\r\n",GlobalVarList[commandID].enumList[i]);
		}
	}

	return result;
}



/**
 * \brief	Check values
 * 			- Check type value (integer)
 * 			- Check min-max
 * \return	Process_Ok_SetSuccessful_SendOk, if ok
 */
static ProcessResult_t GlobalVarHandler_CheckValue(VarID_t commandID, uint32_t num)
{
	ProcessResult_t result = Process_Ok_SetSuccessful_SendOk;

	// First, check the type value
	switch (GlobalVarList[commandID].type)
	{
		case Type_Bool:
			if (num > BOOL_MAX)
				result = Process_InvalidValue_TooMuch;
			break;

		case Type_Uint8:
			if (num > UINT8_MAX)
				result = Process_InvalidValue_TooMuch;
			break;

		case Type_Uint16:
			if (num > UINT16_MAX)
				result = Process_InvalidValue_TooMuch;
			break;

		case Type_Uint32:
			// TODO: Always good, do better code
			if (num > UINT32_MAX)
				result = Process_InvalidValue_TooMuch;
			break;

		case Type_Int8:
			if ((int32_t)num > INT8_MAX)
				result = Process_InvalidValue_TooMuch;
			else if ((int32_t)num < INT8_MIN)
				result = Process_InvalidValue_TooSmall;
			break;

		case Type_Int16:
			if ((int32_t)num > INT16_MAX)
				result = Process_InvalidValue_TooMuch;
			else if ((int32_t)num < INT16_MIN)
				result = Process_InvalidValue_TooSmall;
			break;

		case Type_Int32:
			// TODO: Always good, do better code
			if ((int32_t)num > INT32_MAX)
				result = Process_InvalidValue_TooMuch;
			if ((int32_t)num < INT32_MIN)
				result = Process_InvalidValue_TooSmall;
			break;

		case Type_Float:
			// TODO: Not a good compare in float type
			if ((int32_t)num > INT32_MAX)
				result = Process_InvalidValue_TooMuch;
			else if ((int32_t)num < INT32_MIN)
				result = Process_InvalidValue_TooSmall;
			break;

		case Type_Bits:
			if (num > (uint32_t)(power(2,GlobalVarList[commandID].maxValue+1)-1))
				result = Process_InvalidValue_TooMuch;
			break;

		case Type_String:
			if (num >= GlobalVarList[commandID].maxValue)
				result = Process_FailParamTooLongString;
			break;

		case Type_Enumerator:
			if (num >= GlobalVarList[commandID].maxValue)
				result = Process_InvalidValue_TooMuch;
			break;

		// Wrong types
		case Type_Unknown:
		case Type_Count:
		default:
			result = Process_FailType;
			break;
	}

	if (result == Process_Ok_SetSuccessful_SendOk)
	{

		// Check maxValue
		// maxValue is set?
		if (GlobalVarList[commandID].maxValue != GlobalVarList[commandID].minValue
				&& GlobalVarList[commandID].type != Type_Bits)
		{
			// There is setted maxValue, because max not equal than min
			// Check, it is too large or too small?

			if (num > GlobalVarList[commandID].maxValue)
			{
				// Too large
				result = Process_InvalidValue_TooMuch;
			}
			else if (num < GlobalVarList[commandID].minValue)
			{
				// Too small
				result = Process_InvalidValue_TooSmall;
			}
		}
	}

	return result;
}



/**
 * \brief	Write process result
 */
static void GlobalVarHandler_WriteResults(ProcessResult_t result)
{
	const char *pMessage = NULL;

	switch (result)
	{
		case Process_Ok_Answered:
			// Do nothing
			break;

		case Process_Ok_SetSuccessful_SendOk:
			pMessage = "Set successful";
			break;

		case Process_GlobalVariableNameNotFind:
			pMessage = "Global variable not find";
			break;

		case Process_FailParam:
			pMessage = "Fail parameter";
			break;

		case Process_FailType:
			pMessage = "Fail type";
			break;

		case Process_FailParamIsNotNumber:
			pMessage = "Not number";
			break;

		case Process_FailParamIsNotHexNumber:
			pMessage = "Not hex number";
			break;

		case Process_FailParamIsNotHexStart:
			pMessage = "Not hex, missed \"0x\"";
			break;

		case Process_FailParamIsNotBinary:
			pMessage = "Not binary number. Syntax: \"0b1010\"";
			break;

		case Process_FailParamTooLongBinary:
			pMessage = "Too long binary value";
			break;

		case Process_InvalidValue_TooSmall:
			pMessage = "Invalid value, too small";
			break;

		case Process_InvalidValue_TooMuch:
			pMessage = "Invalid value, too much";
			break;

		case Process_InvalidValue_NotBool:
			pMessage = "Invalid value, not bool";
			break;

		case Process_InvalidValue_NotEnumString:
			pMessage = "Invalid enum string";
			break;

		case Process_Settings_EmptyEnumList:
			pMessage = "EnumList settings error";
			break;

		case Process_IsReadOnly:
			pMessage = "Cannot set, it is constant";
			break;

		case Process_SourceNotEnabled:
			pMessage = "Cannot process this command from this source";
			break;

		case Process_FailParamTooLongString:
			pMessage = "Too long string";
			break;

		case Process_Unknown:
			pMessage = "Unknown error";
			break;

		default:
			pMessage = "Fatal error";
			break;
	}

	COMMUNICATION_SendMessage(GlobalVarHandler_Source, pMessage);

}



/**
 * \brief	List all variables
 */
void GlobalVarHandler_ListAllVariableParameters(void)
{
	VarID_t i;

	// TODO: Enumokat is kiírni, ha van?
	const char *header = "+-ID-+-------Name-----------+----Type----+--min--+--max--+-unit-+-----Description------+";

	// Send header
	COMMUNICATION_SendMessage(GlobalVarHandler_Source, header);

	// Rows (commands)
	for (i=0; i<GlobalVarMaxCommandNum; i++)
	{

		// Print one command / line:
		COMMUNICATION_Printf(GlobalVarHandler_Source,
				"| %2d | %20s | %10s | %5d | %5d | %4s | %20s |\r\n",
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
	COMMUNICATION_SendMessage(GlobalVarHandler_Source, header);

}



/**
 * \brief	Print all variable values
 */
void GlobalVarHandler_PrintAllVariableValues(void)
{
	uint8_t i;

	COMMUNICATION_Printf(GlobalVarHandler_Source, "Global variables:\r\n"
			" %20s %20s\r\n",
			"<Name>",
			"<Value>");

	// Print all variables
	for (i = 0; i < GlobalVarMaxCommandNum; i++)
	{
		// Print a variable name and value
		// TODO: Itt elrontotta a lehetőségeinket... ?
		//COMMUNICATION_Printf(GlobalVarHandler_Source, " %20s %20s\r\n", GlobalVarList[i].name);
		COMMUNICATION_Printf(GlobalVarHandler_Source, " %20s ", GlobalVarList[i].name);
		GlobalVarHandler_GetCommand(i);
	}

	COMMUNICATION_SendMessage(GlobalVarHandler_Source, "End of global variables\r\n");
}



/**
 * \brief	Print global variable descriptions
 */
static void GlobalVarHandler_PrintVariableDescriptions(VarID_t commandID)
{

	COMMUNICATION_Printf(GlobalVarHandler_Source,
			"Command help: %s, type:%s, min:%d, max:%d, desc:%s\r\n",
			GlobalVarList[commandID].name,
			GlobalVarTypesNames[GlobalVarList[commandID].type],
			GlobalVarList[commandID].minValue,
			GlobalVarList[commandID].maxValue,
			GlobalVarList[commandID].description
			);
}



/**
 * \brief	GlobalVarHandler unit test
 */
// TODO:
void GlobalVarHandler_UnitTest(void);
