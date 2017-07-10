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


#include "options.h"

#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE

#include "String.h"
#include "DebugUart.h"
#include "GlobalVarHandler.h"
#include "GlobalVariables.h"
#include "CommandHandler.h"
#include "Calc.h"

#ifdef MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE
#include "UnitTest.h"
#endif



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
static const char * const GlobalVarTypesNames[] =
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
	"enum"
};


// TODO: Put guard value around this value?
uint32_t GlobarVarHandler_TemporaryValue = 0;


#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
static uint32_t GlobalVarHandler_TraceVarEnabled = 0;

#ifdef CONFIG_GLOBALVARHANDLER_TRACE_RAM_BUFFER
#define GLOBALVARHANDLER_TRACE_BUFFER_SIZE 		(100U)
static uint32_t GlobalVarHandler_TraceRamBuffer[GLOBALVARHANDLER_TRACE_BUFFER_SIZE] = { 0 };
static uint8_t GlobalVarHandler_TraceRam_BufferCnt = 0;
#endif
#endif


/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static ProcessResult_t GlobalVarHandler_GetCommand(VarID_t commandID);
static bool GlobalVarHandler_SearchVariableName(const char *commandName, VarID_t *commandID);
static ProcessResult_t GlobalVarHandler_SetCommand(const VarID_t commandID, const char *param);
static void GlobalVarHandler_WriteResults(ProcessResult_t result);
static ProcessResult_t GlobalVarHandler_CheckValue(VarID_t commandID, uint32_t num);
static void GlobalVarHandler_PrintVariableDescriptions (VarID_t commandID);

static void GlobalVarHandler_GetInteger(VarID_t commandID);
static void GlobalVarHandler_GetBits(const VarID_t commandID);
static void GlobalVarHandler_GetEnumerator(const VarID_t commandID);
static void GlobalVarHandler_GetFunctionValue(const VarID_t commandID);

static ProcessResult_t GlobalVarHandler_SetBool(VarID_t commandID, const char *param);
static ProcessResult_t GlobalVarHandler_SetInteger(VarID_t commandID, const char *param);
static ProcessResult_t GlobalVarHandler_SetFloat(VarID_t commandID, const char *param);
static ProcessResult_t GlobalVarHandler_SetBits(VarID_t commandID, const char *param);
static ProcessResult_t GlobalVarHandler_SetString(VarID_t commandID, const char *param);
static ProcessResult_t GlobalVarHandler_SetEnumerator(VarID_t commandID, const char *param);

#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
static void GlobalVarHandler_SetTrace(VarID_t commandID, const char * param);
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Check the GlobalVarList[], are set valid?
 * \retval	true	 ok
 */
bool GlobalVarHandler_CheckCommandStructAreValid(void)
{
	// TODO: Macro for checking

	//#error "Syncronize 'Type_Count' with 'GlobalVarTypesNames'"
	BUILD_BUG_ON((sizeof(GlobalVarTypesNames)/sizeof(GlobalVarTypesNames[0])) != Type_Count);


	VarID_t i;
	bool hasError = false;

	for (i = 0; i < GlobalVar_MaxCommandNum; i++)
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

		if (GlobalVarList[i].varPointer == NULL && !GlobalVarList[i].isFunction)
		{
			hasError = true;
			break;
		}

		if (GlobalVarList[i].isFunction && GlobalVarList[i].type == Type_String)
		{
			hasError = true;
			break;
		}

		if (GlobalVarList[i].isFunction && GlobalVarList[i].varPointer == NULL)
		{
			hasError = true;
			break;
		}

		if (GlobalVarList[i].getFunctionPointer == NULL && GlobalVarList[i].isFunction)
		{
			hasError = true;
			break;
		}

		if (GlobalVarList[i].setFunctionPointer == NULL && GlobalVarList[i].isFunction
				&& !GlobalVarList[i].isReadOnly)
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
		CommandHandler_Printf("Error in %d. (%s) command\r\n", i, GlobalVarList[i].name);
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
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
			else if (setGetType == SetGet_Trace)
			{
				GlobalVarHandler_SetTrace(commandID, param);
				result = Process_Ok_Answered;
			}
#endif
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
	for (i = 0; i < GlobalVar_MaxCommandNum; i++)
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
	if (GlobalVarList[commandID].isFunction)
	{
		// Get function variable to *varPointer
		GlobalVarHandler_GetFunctionValue(commandID);
	}

	// Get Variable
	switch (GlobalVarList[commandID].type)
	{
		case Type_Bool:
		{
			bool *boolPointer = (bool *)GlobalVarList[commandID].varPointer;
			if (*boolPointer)
			{
				CommandHandler_SendMessage("1 / TRUE");
			}
			else
			{
				CommandHandler_SendMessage("0 / FALSE");
			}
		}
			break;

		case Type_Uint8:
		case Type_Uint16:
		case Type_Uint32:
		case Type_Int8:
		case Type_Int16:
		case Type_Int32:
			GlobalVarHandler_GetInteger(commandID);
			break;

		case Type_Float:
		{
			float *floatPointer = (float *)GlobalVarList[commandID].varPointer;
			float value = *floatPointer;
			CommandHandler_Printf("%0.2f", value);
		}
			break;

		case Type_Bits:
			GlobalVarHandler_GetBits(commandID);
			break;

		case Type_String:
		{
			const char *string = GlobalVarList[commandID].varPointer;
			CommandHandler_SendMessage(string);
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
		CommandHandler_Printf(" %s", GlobalVarList[commandID].unit);
	}
#endif

	return Process_Ok_Answered;
}



/**
 * \brief	Get integer value
 * 			NOTE: Be careful, commandID not checked
 */
static void GlobalVarHandler_GetInteger(VarID_t commandID)
{
	VarType_t type = GlobalVarList[commandID].type;

	if (GlobalVarList[commandID].isHex)
	{
		// Get in Hex format

		uint8_t octetNum = 0;
		switch (type)
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
			usprintf(format, "0x%%%dX", octetNum);
			// Example: "0x%2X"
			CommandHandler_Printf(format, num);
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
				CommandHandler_Printf("%u", num);
			}
				break;

			case Type_Uint16:
			{
				uint16_t *numPointer =
						(uint16_t *)GlobalVarList[commandID].varPointer;
				uint16_t num = *numPointer;
				CommandHandler_Printf("%u", num);
			}
				break;

			case Type_Uint32:
			{
				uint32_t *numPointer =
					(uint32_t *)GlobalVarList[commandID].varPointer;
				uint32_t num = *numPointer;
				CommandHandler_Printf("%u", num);
			}
				break;

			case Type_Int8:
			{
				int8_t *numPointer =
						(int8_t *)GlobalVarList[commandID].varPointer;
				int8_t num = *numPointer;
				CommandHandler_Printf("%d", num);
			}
				break;

			case Type_Int16:
			{
				int16_t *numPointer =
						(int16_t *)GlobalVarList[commandID].varPointer;
				int16_t num = *numPointer;
				CommandHandler_Printf("%d", num);
			}
				break;

			case Type_Int32:
			{
				int32_t *numPointer =
						(int32_t *)GlobalVarList[commandID].varPointer;
				int32_t num = *numPointer;
				CommandHandler_Printf("%d", num);
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
 * \brief	Get bits / get binary values
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
	num = (num >> shift) & (power(2, bitLength)-1);

	CommandHandler_Printf("0b%b", num);

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
	CommandHandler_Printf("%d ", enumValue);

	// Check "enumList" pointer
	if (GlobalVarList[commandID].enumList == NULL)
	{
		// TODO: Add new result type?
		CommandHandler_SendMessage("ERROR - There is not set \"enumList\" pointer");
		return;
	}

	// If has good enumList
	// Check value (It is not too high?)
	if (enumValue > GlobalVarList[commandID].maxValue)
	{
		// TODO: Add new result type?
		CommandHandler_SendMessage("ERROR - Enum has too high value");
		return;
	}

	// Good value
	// Print enum string
	enumString = (char *)GlobalVarList[commandID].enumList[enumValue];	// string pointer
	CommandHandler_SendMessage(enumString);

	// Return
	return;

}



/**
 * \brief	Get function value
 */
static void GlobalVarHandler_GetFunctionValue(const VarID_t commandID)
{
	// Variable type checked at GlobalVarHandler_CheckCommandStructAreValid()

	// Get Function pointer
	GetFunctionPointer getFunction = (GetFunctionPointer)GlobalVarList[commandID].getFunctionPointer;
	// TODO: Make to other integer and float and bool

	GlobarVarHandler_TemporaryValue = 0;
	GlobarVarHandler_TemporaryValue = getFunction();
}



/**
 * \brief	Set command
 */
static ProcessResult_t GlobalVarHandler_SetCommand(const VarID_t commandID, const char *param)
{

	ProcessResult_t result = Process_Unknown;


	// Variable type
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

	if (GlobalVarList[commandID].isFunction)
	{
		// Function
		// Set value

		// Set Function pointer
		SetFunctionPointer setFunction = (SetFunctionPointer)GlobalVarList[commandID].setFunctionPointer;

		// Set value with parameter
		if (setFunction(GlobarVarHandler_TemporaryValue))
		{
			// Successful set
			result = Process_Ok_SetSuccessful_SendOk;
		}
		else
		{
			// Failed set
			// TODO: Process_FailSet
			result = Process_Unknown;
		}
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
		if (!(param[0] == '0' && param[1] == 'x'))
		{
			// Need set with "0x"
			return Process_FailParamIsNotHexStart;
		}

		// Convert HexString to Num
		if (StringHexToNum(&param[2], &num))
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

	CommandHandler_SendMessage(pMessage);

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
	CommandHandler_SendLine(header);

	// Rows (commands)
	for (i = 0; i < GlobalVar_MaxCommandNum; i++)
	{

		// Print one command / line:
		CommandHandler_Printf(
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
	CommandHandler_SendLine(header);

}



/**
 * \brief	Print all variable values
 */
void GlobalVarHandler_PrintAllVariableValues(void)
{
	VarID_t i;

	CommandHandler_Printf("Global variables:\r\n"
			" %20s %20s\r\n",
			"<Name>",
			"<Value>");

	// Print all variables
	for (i = 0; i < GlobalVar_MaxCommandNum; i++)
	{
		// Print a variable name and value
		//CommandHandler_Printf(" %20s %20s\r\n", GlobalVarList[i].name);
		CommandHandler_Printf(" %20s ", GlobalVarList[i].name);
		GlobalVarHandler_GetCommand(i);
		CommandHandler_SendLine(NULL);
	}

	CommandHandler_SendLine("End of global variables");
}



/**
 * \brief	Print global variable descriptions
 */
static void GlobalVarHandler_PrintVariableDescriptions(VarID_t commandID)
{

	CommandHandler_Printf(
			"Command help: %s\r\n"
			"type:%s\r\n"
			"min:%d\r\n"
			"max:%d\r\n"
			"desc:%s\r\n",
			GlobalVarList[commandID].name,
			GlobalVarTypesNames[GlobalVarList[commandID].type],
			GlobalVarList[commandID].minValue,
			GlobalVarList[commandID].maxValue,
			GlobalVarList[commandID].description
			);
}



#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
/**
 * \brief	Set trace with string parameter
 */
static void GlobalVarHandler_SetTrace(VarID_t commandID, const char * param)
{
	if (!StrCmp("enable", param))
	{
		GlobalVarHandler_EnableTrace(commandID, true);
	}
	else
	{
		// TODO: Check "disable" or not?
		GlobalVarHandler_EnableTrace(commandID, false);
	}
}



/**
 * \brief	Enable / Disable trace
 */
void GlobalVarHandler_EnableTrace(VarID_t id, bool isEnable)
{
	if (id < GlobalVar_MaxCommandNum)
	{
		if (isEnable)
			GlobalVarHandler_TraceVarEnabled |= (1 << id);
		else
			GlobalVarHandler_TraceVarEnabled &= ~(1 << id);
	}
}



/**
 * \brief	Trace GlobalVar
 */
void GlobalVarHandler_RunTrace(void)
{
	// TODO: Only use for max 32bits (4byte) variable
	if (GlobalVarHandler_TraceVarEnabled)
	{
		VarID_t i;
		for (i = 0; i < GlobalVar_MaxCommandNum; i++)
		{
			// Step on GlobalVar list
			if (GlobalVarHandler_TraceVarEnabled & (1 << i))
			{
				// Need trace
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_RAM_BUFFER
				// TODO: Unknown size... Now use uint32_t
				// TODO: Only function values saved to Temporary variable...
				// TODO: Save tick?
				// TODO: Save VarID (for known type)?

				GlobalVarHandler_GetCommand(i);

				GlobalVarHandler_TraceRamBuffer[GlobalVarHandler_TraceRam_BufferCnt] = GlobarVarHandler_TemporaryValue;
				GlobalVarHandler_TraceRam_BufferCnt++;

				// Check buffer is full?
				if (GlobalVarHandler_TraceRam_BufferCnt >= GLOBALVARHANDLER_TRACE_BUFFER_SIZE)
				{
					GlobalVarHandler_TraceRam_BufferCnt = 0;
					// TODO: Can we print, when buffer is full?
					GlobalVarHandler_PrintTraceBuffer();
				}
#else
				// Trace (print) to debug port
				CommandHandler_Printf("Trace: %d - ", HAL_GetTick());
				GlobalVarHandler_GetCommand(i);
				CommandHandler_SendLine("");
#endif
			}
		}
	}
}



#ifdef CONFIG_GLOBALVARHANDLER_TRACE_RAM_BUFFER
/**
 * \brief	Print Trace buffer content
 */
void GlobalVarHandler_PrintTraceBuffer(void)
{
	uint8_t i;
	for (i = 0; i < GLOBALVARHANDLER_TRACE_BUFFER_SIZE; i++)
	{
		// TODO: Unknown type... uint32_t, int32_t, float, bool, enum
		CommandHandler_Printf("Value: %d = %u\r\n", i, GlobalVarHandler_TraceRamBuffer[i]);
	}
}
#endif

#endif



#ifdef MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE
/**
 * \brief	GlobalVarHandler unit test
 */
void GlobalVarHandler_UnitTest(void)
{
	bool isOk;

// TODO: Put at header or merge with WriteResult function...
#define GLOBALVARHANDLER_MSG_SET_SUCCESSFUL			("Set successful")
#define GLOBALVARHANDLER_MSG_SET_FAILED_TOO_MUCH	("Invalid value, too much")
#define GLOBALVARHANDLER_MSG_SET_FAILED_NOT_BOOL	("Invalid value, not bool")
#define GLOBALVARHANDLER_MSG_SET_FAILED_NOT_NUMBER	("Not number")
#define GLOBALVARHANDLER_MSG_SET_FAILED_WRONG_SOURCE	("Cannot process this command from this source")
#define GLOBALVARHANDLER_MSG_SET_FAILED_NOT_HEX		("Not hex, missed \"0x\"")
#define GLOBALVARHANDLER_MSG_SET_FAILED_NOT_BINARY	("Not binary number. Syntax: \"0b1010\"")
#define GLOBALVARHANDLER_MSG_SET_FAILED_WRONG_ENUM	("Invalid enum value")

	// Start GlobalVarHandler UnitTest
	UnitTest_Start("GlobalVarHandler", __FILE__);
	// Initialize
	CommandHandler_CommandSource = CommProt_Buffer;


	// Check GlobalVarHandler structures
	isOk = GlobalVarHandler_CheckCommandStructAreValid();
	UNITTEST_ASSERT(isOk, "GlobalVarHandler structs are wrong");


	// Test "testuint8" variable
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint8", "8", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testuint8 set error");

	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint8", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("8 cm", Communication_Buffer), "testuint8 get error");

	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint8", "255", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testuint8 set error");

	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint8", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("255 cm", Communication_Buffer), "testuint8 get error");

	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint8", "256", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_FAILED_TOO_MUCH, Communication_Buffer), "testuint8 value validation error");

	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint8", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("255 cm", Communication_Buffer), "testuint8 set-get error");


	// test "testbool" variable
	// Set valid value: 1/true
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testbool", "true", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testbool set error");

	// Get
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testbool", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("1 / TRUE", Communication_Buffer), "testbool get error");

	// Set valid value: 0/false
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testbool", "0", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testbool set error");

	// Get
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testbool", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("0 / FALSE", Communication_Buffer), "testbool get error");

	// Set invalid value
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testbool", "2", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_FAILED_NOT_BOOL, Communication_Buffer), "testbool value validation error");

	// Check unchange
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testbool", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("0 / FALSE", Communication_Buffer), "testbool set-get error");


	// Test "testint16" variable
	// Set valid value: 8
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint16", "8", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testint16 set error");

	// Get
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint16", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("8", Communication_Buffer), "testint16 get error");

	// Set valid value: 255
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint16", "255", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testint16 set error");

	// Get
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint16", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("255", Communication_Buffer), "testint16 get error");

	// Set valid value: -255
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint16", "-255", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testint16 set error");

	// Get
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint16", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("-255", Communication_Buffer), "testint16 get error");

	// Set invalid value:
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint16", "65536", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_FAILED_TOO_MUCH, Communication_Buffer), "testint16 value validation error");

	// Check unchange
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint16", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("-255", Communication_Buffer), "testint16 set-get error");


	// Test "testint32" variable
	// Set valid value: 8
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint32", "8", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testint32 set error");

	// Get
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint32", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("8", Communication_Buffer), "testint32 get error");

	// Set valid value: 65535
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint32", "65535", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testint32 set error");

	// Get
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint32", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("65535", Communication_Buffer), "testint32 get error");

	// Set valid value: -255
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint32", "-255", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testint32 set error");

	// Get
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint32", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("-255", Communication_Buffer), "testint32 get error");

	// Set invalid value:
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint32", "12.34", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_FAILED_NOT_NUMBER, Communication_Buffer), "testint32 value validation error");

	// Check unchange
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testint32", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("-255", Communication_Buffer), "testint32 set-get error");


	// Test "testuint32" variable
	// Set valid value: 8
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint32", "0x08", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testuint32 set error");

	// Get
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint32", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("0x00000008", Communication_Buffer), "testuint32 get error");

	// Set valid value: 65535
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint32", "0x65535", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testuint32 set error");

	// Get
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint32", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("0x00065535", Communication_Buffer), "testuint32 get error");

	// Set valid value: -255
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint32", "-255", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_FAILED_NOT_HEX, Communication_Buffer), "testuint32 set error");

	// Get
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint32", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("0x00065535", Communication_Buffer), "testuint32 get error");

	// Set invalid value:
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint32", "12.34", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_FAILED_NOT_HEX, Communication_Buffer), "testuint32 value validation error");

	// Check unchange
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testuint32", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("0x00065535", Communication_Buffer), "testuint32 set-get error");


	// Test "cannotaccess" variable
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testcannotaccess", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_FAILED_WRONG_SOURCE, Communication_Buffer), "Variable source setting error");


	// Test "testfloat" variable (float)
	// Set float value:
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testfloat", "12.34", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "Float value setting error");

	// Check value/range
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testfloat", "", SetGet_Get, CommProt_Buffer);
	float testNumber = 0.0f;
	UNITTEST_ASSERT(StringToFloat(Communication_Buffer, &testNumber), "float type error");
	UNITTEST_ASSERT(((testNumber>12.33)&&(testNumber<12.35)), "float set-get error");


	// Test "testenum" (enum)
	// Set enum
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testenum", "1", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "enum value set error");

	// Get enum
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testenum", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("1 exampleenumstring1", Communication_Buffer), "enum value get error");

	// Set enum - fail
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testenum", "4", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmpWithLength(GLOBALVARHANDLER_MSG_SET_FAILED_WRONG_ENUM, Communication_Buffer, strlen(GLOBALVARHANDLER_MSG_SET_FAILED_WRONG_ENUM)),
			"enum value set error");

	// Get enum
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testenum", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("1 exampleenumstring1", Communication_Buffer), "enum value get error");
	// TODO: Extend...


	// Test "testbit" (bits)
	// Set bit
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testbit", "0b0", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testbit value set error");

	// Get bit
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testbit", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("0b0", Communication_Buffer), "testbit value get error");

	// Set bit
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testbit", "0b111", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_SUCCESSFUL, Communication_Buffer), "testbit value set error");

	// Get bit
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testbit", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("0b111", Communication_Buffer), "testbit value get error");

	// Set bit - fail
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testbit", "A", SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp(GLOBALVARHANDLER_MSG_SET_FAILED_NOT_BINARY, Communication_Buffer), "testbit value set error");

	// Get bit
	COMMUNICATION_ClearProtocolBuffer();
	GlobalVarHandler_ProcessCommand("testbit", "", SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(!StrCmp("0b111", Communication_Buffer), "testbit value get error");
	// TODO: Extend...


	// End of UnitTest
	CommandHandler_CommandSource = CommProt_Unknown;

	UnitTest_End();
}
#endif	// #ifdef MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE



#endif	// #ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
