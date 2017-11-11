/*
 *		GlobalVarHandler.c
 *		Created on:		2016-09-05
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Global Variable Handler: Handle set-get variable from uart or other
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
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



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


/*
GlobVarH_Type_Unknown = 0,
GlobVarH_Type_Bool,
GlobVarH_Type_Uint8,
GlobVarH_Type_Int8,
GlobVarH_Type_Uint16,
GlobVarH_Type_Int16,
GlobVarH_Type_Uint32,
GlobVarH_Type_Int32,
GlobVarH_Type_Float,
GlobVarH_Type_String,
GlobVarH_Type_Enumerator
*/
static const char * const GlobVarH_TypesNames[] =
{
	// NOTE: Important!! Must be in the same order with GlobVarH_Type_t
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
uint32_t GlobVarH_TemporaryValue = 0;							///< used when the global variable has its own getter function


#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
/**<
 * Used as bit field to enable/disable variable tracing.
 * e.g.: bit 1. corresponds to the 1st item in GlobalVarList[] -> NOTE 32th variable can not be traced!
 */
static uint32_t GlobVarH_TraceVarEnabled = 0;

	#ifdef CONFIG_GLOBALVARHANDLER_TRACE_RAM_BUFFER
		#define GLOBALVARHANDLER_TRACE_BUFFER_SIZE 		(100U)	///< size of GlobVarH_TraceRamBuffer[]
/**<
 * Array to store the traced values. Used as ring buffer: when the buffer is full, value storage starts from item 0 again, old values
 * will be overwritten.
 */
static GlobVarH_TraceLogRecord_t GlobVarH_TraceRamBuffer[GLOBALVARHANDLER_TRACE_BUFFER_SIZE] = { 0 };
static uint8_t GlobVarH_TraceRam_BufferCnt = 0;					///< current position in GlobVarH_TraceRamBuffer[]
	#endif
#endif



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static bool GlobVarH_SearchVariableName(const char *commandName, GlobVarH_ID_t *globVarID);
static GlobVarH_ProcessResult_t GlobVarH_SetVariable(const GlobVarH_ID_t globVarID, const char *param);
static GlobVarH_ProcessResult_t GlobVarH_CheckValue(GlobVarH_ID_t globVarID, uint32_t num);
static void GlobVarH_PrintVariableDescriptions (GlobVarH_ID_t globVarID);

static GlobVarH_ProcessResult_t GlobVarH_GetVariable(GlobVarH_VarRecord_t * varRecord);
static void GlobVarH_GetInteger(GlobVarH_VarRecord_t * varRecord);
static void GlobVarH_GetBits(GlobVarH_VarRecord_t * varRecord);
static void GlobVarH_GetEnumerator(GlobVarH_VarRecord_t * varRecord);
static void GlobVarH_GetFunctionValue(GlobVarH_VarRecord_t * varRecord);


static GlobVarH_ProcessResult_t GlobVarH_SetBool(GlobVarH_ID_t globVarID, const char *param);
static GlobVarH_ProcessResult_t GlobVarH_SetInteger(GlobVarH_ID_t globVarID, const char *param);
static GlobVarH_ProcessResult_t GlobVarH_SetFloat(GlobVarH_ID_t globVarID, const char *param);
static GlobVarH_ProcessResult_t GlobVarH_SetBits(GlobVarH_ID_t globVarID, const char *param);
static GlobVarH_ProcessResult_t GlobVarH_SetString(GlobVarH_ID_t globVarID, const char *param);
static GlobVarH_ProcessResult_t GlobVarH_SetEnumerator(GlobVarH_ID_t globVarID, const char *param);

#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
static void GlobVarH_SetTrace(GlobVarH_ID_t commandID, const char * param);
#endif

#ifdef MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE
static void GlobVarH_UT_Clear(void);
#endif


/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


#ifdef CONFIG_GLOBALVARHANDLER_CHECK_ENABLE
/**
 * \brief	Check the GlobalVarList[], are settings valid?
 */
void GlobVarH_CheckGlobalVarArray(void)
{
	//#error "Synhcronize 'GlobVarH_Type_Count' with 'GlobalVarTypesNames'"
	BUILD_BUG_ON(NUM_OF(GlobVarH_TypesNames) != GlobVarH_Type_Count);

	GlobVarH_ID_t i;

	for (i = 0; i < GlobalVar_MaxCommandNum; i++)
	{
		// TODO: Put some comments for reason / checks

		ASSERT(GlobalVarList[i].varPointer != NULL);
		ASSERT(GlobalVarList[i].type != GlobVarH_Type_Unknown);
		ASSERT(GlobalVarList[i].name != NULL);
		ASSERT(StringLength(GlobalVarList[i].name) < GLOBVARH_NAME_MAX_LENGTH);

		if (GlobalVarList[i].isFunction)
		{
			ASSERT(GlobalVarList[i].type != GlobVarH_Type_String);		// callback functions can not be used for string variable!
			ASSERT(GlobalVarList[i].getFunctionPointer != NULL);
			ASSERT(GlobalVarList[i].varPointer == &GlobVarH_TemporaryValue);

			if (!GlobalVarList[i].isReadOnly)
			{
				ASSERT(GlobalVarList[i].setFunctionPointer != NULL);
			}
		}

		if (GlobalVarList[i].type == GlobVarH_Type_Enumerator)
		{
			ASSERT(GlobalVarList[i].enumList != NULL);
		}

		if (GlobalVarList[i].type == GlobVarH_Type_String && !GlobalVarList[i].isReadOnly)
		{
			ASSERT(GlobalVarList[i].maxValue != 0);
		}

		if (GlobalVarList[i].type == GlobVarH_Type_Bits)
		{
			ASSERT(GlobalVarList[i].maxValue <= 31);
			ASSERT(GlobalVarList[i].minValue < 31);
			ASSERT(GlobalVarList[i].minValue <= GlobalVarList[i].maxValue);
		}
	}
}
#endif



/**
 * \brief	Process received command
 * \param	*varName			name of the variable to be looked for (string)
 * \param	*param				received parameters after command
 * \param	setGetType			Set or get command
 * \param	source				Command source (e.g. USART)
 * \return	result				Command process result
 */
GlobVarH_ProcessResult_t GlobVarH_ProcessVariableCommand(const char *varName, const char *param, GlobVarH_SetGetType_t setGetType, CommProtocol_t source)
{
	 GlobVarH_ProcessResult_t result = GlobVarH_Process_Unknown;

	// Search command
	GlobVarH_ID_t globVarID = 0;

	if (GlobVarH_SearchVariableName(varName, &globVarID))
	{
		// Found, Check the source
		if (((1 << source) & GlobalVarList[globVarID].sourceEnable) ||
				(GlobalVarList[globVarID].sourceEnable == CommProtBit_Unknown))		// TODO: to access a variable is always enabled!
		{
			// Source is enabled
			if (setGetType == GlobVarH_SetGet_Get)
			{
				// Get
				result = GlobVarH_GetVariable((GlobVarH_VarRecord_t *)&GlobalVarList[globVarID]);
			}
			else if (setGetType == GlobVarH_SetGet_Set)
			{
				// Can set? (not const?)
				if (!GlobalVarList[globVarID].isReadOnly)
				{
					result = GlobVarH_SetVariable(globVarID, param);	// It not const, can set
				}
				else
				{
					result = GlobVarH_Process_IsReadOnly;			// Cannot set, it is read only
				}
			}
			else if (setGetType == GlobVarH_SetGet_Help)
			{
				GlobVarH_PrintVariableDescriptions(globVarID);
				result = GlobVarH_Process_Ok_Answered;
			}
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
			else if (setGetType == GlobVarH_SetGet_Trace)
			{
				GlobVarH_SetTrace(globVarID, param);
				result = GlobVarH_Process_Ok_Answered;
			}
#endif
			else
			{
				result = GlobVarH_Process_Unknown;
			}
		}
		else
		{
			result = GlobVarH_Process_SourceNotEnabled;				// Source not enabled
		}
	}
	else
	{
		result = GlobVarH_Process_GlobalVariableNameNotFind;			// Not found
	}

	return result;
}



/**
 * \brief	Search global var name in GlobalVarList
 * \retval	true, if found, it is commandID
 * 			false, if not found
 */
static bool GlobVarH_SearchVariableName(const char *commandName, GlobVarH_ID_t *globVarID)
{
	GlobVarH_ID_t i;

	// Search
	for (i = 0; i < GlobalVar_MaxCommandNum; i++)
	{
		if (!StrCmp(GlobalVarList[i].name, commandName))
		{
		// Found
		*globVarID = i;

		return true;
		}
	}

	// Not found
	return false;
}



/**
 * \brief	Get value of a global variable. Response is written to buffer pointed by CmdH_ResponseBuffer.
 */
static GlobVarH_ProcessResult_t GlobVarH_GetVariable(GlobVarH_VarRecord_t * varRecord)
{
	// Check type
	if (varRecord->isFunction)		// callback of the variable must be used for getting its value!
	{
		// Get function variable to .*varPointer
		// .varPointer struct member must point to GlobVarH_TemporaryValue if the variable has callback function for getting its value!
		GlobVarH_GetFunctionValue(varRecord);
	}

	// Get Variable
	switch (varRecord->type)
	{
		case GlobVarH_Type_Bool:
		{
			bool *boolPointer = (bool *)varRecord->varPointer;
			if (*boolPointer)
			{
				CmdH_SendMessage("1 / TRUE");
			}
			else
			{
				CmdH_SendMessage("0 / FALSE");
			}
		}
			break;

		case GlobVarH_Type_Uint8:
		case GlobVarH_Type_Uint16:
		case GlobVarH_Type_Uint32:
		case GlobVarH_Type_Int8:
		case GlobVarH_Type_Int16:
		case GlobVarH_Type_Int32:
			GlobVarH_GetInteger(varRecord);
			break;

		case GlobVarH_Type_Float:
		{
			float *floatPointer = (float *)varRecord->varPointer;
			float value = *floatPointer;
			CmdH_Printf("%0.2f", value);
		}
			break;

		case GlobVarH_Type_Bits:
			GlobVarH_GetBits(varRecord);
			break;

		case GlobVarH_Type_String:
		{
			const char *string = varRecord->varPointer;
			CmdH_SendMessage(string);
		}
			break;

		case GlobVarH_Type_Enumerator:
			GlobVarH_GetEnumerator(varRecord);
			break;

		// Wrong types
		case GlobVarH_Type_Unknown:
		case GlobVarH_Type_Count:
		default:
			return GlobVarH_Process_FailParam;
			break;
	}

#ifdef GLOBVARH_UNIT_ENABLE
	// Append unit, if need
	if (varRecord->unit)
	{
		// Print unit
		CmdH_Printf(" %s", varRecord->unit);
	}
#endif

	return GlobVarH_Process_Ok_Answered;
}



/**
 * \brief	Get integer value. Response is written to buffer pointed by CmdH_ResponseBuffer.
 */
static void GlobVarH_GetInteger(GlobVarH_VarRecord_t * varRecord)
{
	GlobVarH_Type_t type = varRecord->type;

	if (varRecord->isHex)
	{
		// Get in Hex format

		uint8_t octetNum = 0;
		switch (type)
		{
			case GlobVarH_Type_Uint8:
				octetNum = 2;
				break;

			case GlobVarH_Type_Uint16:
				octetNum = 4;
				break;

			case GlobVarH_Type_Uint32:
				octetNum = 8;
				break;

			// Wrong types
			case GlobVarH_Type_Int8:
			case GlobVarH_Type_Int16:
			case GlobVarH_Type_Int32:
			case GlobVarH_Type_Bool:
			case GlobVarH_Type_Enumerator:
			case GlobVarH_Type_Float:
			case GlobVarH_Type_Bits:
			case GlobVarH_Type_String:
			case GlobVarH_Type_Unknown:
			case GlobVarH_Type_Count:
			default:
				// Error...
				// TODO: Error handling, if wrong types... (unexpected types)
				octetNum = 0;
				break;
		}

		if (octetNum != 0)
		{
			// Octet num is ok
			uint32_t *numPointer = (uint32_t *)varRecord->varPointer;
			uint32_t num = *numPointer;
			char format[10];

			usprintf(format, "0x%%%dX", octetNum);
			// Example: "0x%2X"
			CmdH_Printf(format, num);
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
			// TODO: Optimize these
			case GlobVarH_Type_Uint8:
			{
				uint8_t *numPointer = (uint8_t *)varRecord->varPointer;
				uint8_t num = *numPointer;
				CmdH_Printf("%u", num);
			}
				break;

			case GlobVarH_Type_Uint16:
			{
				uint16_t *numPointer = (uint16_t *)varRecord->varPointer;
				uint16_t num = *numPointer;
				CmdH_Printf("%u", num);
			}
				break;

			case GlobVarH_Type_Uint32:
			{
				uint32_t *numPointer = (uint32_t *)varRecord->varPointer;
				uint32_t num = *numPointer;
				CmdH_Printf("%u", num);
			}
				break;

			case GlobVarH_Type_Int8:
			{
				int8_t *numPointer = (int8_t *)varRecord->varPointer;
				int8_t num = *numPointer;
				CmdH_Printf("%d", num);
			}
				break;

			case GlobVarH_Type_Int16:
			{
				int16_t *numPointer = (int16_t *)varRecord->varPointer;
				int16_t num = *numPointer;
				CmdH_Printf("%d", num);
			}
				break;

			case GlobVarH_Type_Int32:
			{
				int32_t *numPointer = (int32_t *)varRecord->varPointer;
				int32_t num = *numPointer;
				CmdH_Printf("%d", num);
			}
				break;

			// Wrong types
			case GlobVarH_Type_Bool:
			case GlobVarH_Type_Enumerator:
			case GlobVarH_Type_Float:
			case GlobVarH_Type_Bits:
			case GlobVarH_Type_String:
			case GlobVarH_Type_Unknown:
			case GlobVarH_Type_Count:
			default:
				// Wrong case
				break;
		}

	}
}



/**
 * \brief	Get bits / get binary values. Response is written to buffer pointed by CmdH_ResponseBuffer.
 */
static void GlobVarH_GetBits(GlobVarH_VarRecord_t * varRecord)
{
	uint32_t *numPointer = (uint32_t *)varRecord->varPointer;
	uint32_t num = *numPointer;

	// xxxx11111yyyyyy
	//     mask  shift
	uint8_t shift = varRecord->minValue;
	uint8_t bitLength = varRecord->maxValue - shift;
	// TODO: Check minValue and maxValue

	// Shift to right, and mask to make our important bits
	num = (num >> shift) & (power(2, bitLength)-1);

	CmdH_Printf("0b%b", num);

	return;
}



/**
 * \brief	Get enumerators. Response is written to buffer pointed by CmdH_ResponseBuffer.
 */
static void GlobVarH_GetEnumerator(GlobVarH_VarRecord_t * varRecord)
{
	uint8_t *enumPointer = (uint8_t *)varRecord->varPointer;
	uint8_t enumValue = *enumPointer;
	char *enumString;

	// Print enum value
	CmdH_Printf("%d ", enumValue);

	// Check "enumList" pointer
	if (varRecord->enumList == NULL)
	{
		// TODO: Add new result type?
		CmdH_SendMessage("ERROR - There is not set \"enumList\" pointer");
		return;
	}

	// If has good enumList
	// Check value (It is not too high?)
	if (enumValue > varRecord->maxValue)
	{
		// TODO: Add new result type?
		CmdH_SendMessage("ERROR - Enum has too high value");
		return;
	}

	// Good value
	// Print enum string
	enumString = (char *)varRecord->enumList[enumValue];	// string pointer
	CmdH_SendMessage(enumString);
}



/**
 * \brief	Get function value into the GlobVarH_TemporaryValue variable
 */
static void GlobVarH_GetFunctionValue(GlobVarH_VarRecord_t * varRecord)
{
	// Variable type checked at GlobVarH_CheckGlobalVarArray()

	// Get Function pointer
	GetFunctionPointer getFunction = (GetFunctionPointer)varRecord->getFunctionPointer;
	// TODO: Make to other integer and float and bool

	GlobVarH_TemporaryValue = 0;
	GlobVarH_TemporaryValue = getFunction();
}



/**
 * \brief	Set variable
 */
static GlobVarH_ProcessResult_t GlobVarH_SetVariable(const GlobVarH_ID_t globVarID, const char *param)
{
	GlobVarH_ProcessResult_t result = GlobVarH_Process_Unknown;

	// Variable type
	switch (GlobalVarList[globVarID].type)
	{
		case GlobVarH_Type_Bool:
			result = GlobVarH_SetBool(globVarID, param);
			break;

		case GlobVarH_Type_Uint8:
		case GlobVarH_Type_Uint16:
		case GlobVarH_Type_Uint32:
		case GlobVarH_Type_Int8:
		case GlobVarH_Type_Int16:
		case GlobVarH_Type_Int32:
			result = GlobVarH_SetInteger(globVarID, param);
			break;

		case GlobVarH_Type_Float:
			result = GlobVarH_SetFloat(globVarID, param);
			break;

		case GlobVarH_Type_Bits:
			result = GlobVarH_SetBits(globVarID, param);
			break;

		case GlobVarH_Type_String:
			result = GlobVarH_SetString(globVarID, param);
			break;

		case GlobVarH_Type_Enumerator:
			result = GlobVarH_SetEnumerator(globVarID, param);
			break;

		// Wrong types
		case GlobVarH_Type_Unknown:
		case GlobVarH_Type_Count:
		default:
			result = GlobVarH_Process_FailType;
			break;
	}

	if (GlobalVarList[globVarID].isFunction)
	{
		// Function
		// Set value

		// Set Function pointer
		SetFunctionPointer setFunction = (SetFunctionPointer)GlobalVarList[globVarID].setFunctionPointer;

		// Set value with parameter
		if (setFunction(GlobVarH_TemporaryValue))
		{
			// Successful set
			result = GlobVarH_Process_Ok_SetSuccessful_SendOk;
		}
		else
		{
			// Failed set
			// TODO: Process_FailSet
			result = GlobVarH_Process_Unknown;
		}
	}

	// Return with result
	return result;
}



/**
 * \brief	Set bool variable
 */
static GlobVarH_ProcessResult_t GlobVarH_SetBool(GlobVarH_ID_t globVarID, const char *param)
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
			return GlobVarH_Process_InvalidValue_NotBool;			// Wrong num (not 0, and not 1)
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
			return GlobVarH_Process_InvalidValue_NotBool;
		}
	}
	// If reach here, boolVal is contain a valid value
	bool *bPointer = (bool *)GlobalVarList[globVarID].varPointer;
	*bPointer = boolVal;

	return GlobVarH_Process_Ok_SetSuccessful_SendOk;
}



/**
 * \brief	Set integer global variable
 */
static GlobVarH_ProcessResult_t GlobVarH_SetInteger(GlobVarH_ID_t globVarID, const char *param)
{
	GlobVarH_Type_t varType = GlobalVarList[globVarID].type;
	uint32_t num;

	// If hex
	if (GlobalVarList[globVarID].isHex)
	{
		if (!(param[0] == '0' && param[1] == 'x'))
		{
			// Need set with "0x"
			return GlobVarH_Process_FailParamIsNotHexStart;
		}

		// Convert HexString to Num
		if (StringHexToNum(&param[2], &num))
		{
			// Is good num?
			GlobVarH_ProcessResult_t result = GlobVarH_CheckValue(globVarID, num);
			if (result == GlobVarH_Process_Ok_SetSuccessful_SendOk)
			{
				switch (varType)
				{
					case GlobVarH_Type_Uint8:
					{
						uint8_t *wPointer = (uint8_t *)GlobalVarList[globVarID].varPointer;
						*wPointer = num;
					}
						break;
					case GlobVarH_Type_Uint16:
					{
						uint16_t *wPointer = (uint16_t *)GlobalVarList[globVarID].varPointer;
						*wPointer = num;
					}
						break;
					case GlobVarH_Type_Uint32:
					{
						uint32_t *wPointer = (uint32_t *)GlobalVarList[globVarID].varPointer;
						*wPointer = num;
					}
						break;

					// Wrong types
					case GlobVarH_Type_Int8:
					case GlobVarH_Type_Int16:
					case GlobVarH_Type_Int32:
					case GlobVarH_Type_Bool:
					case GlobVarH_Type_Float:
					case GlobVarH_Type_Bits:
					case GlobVarH_Type_Enumerator:
					case GlobVarH_Type_String:
					case GlobVarH_Type_Unknown:
					case GlobVarH_Type_Count:
					default:
						return GlobVarH_Process_Unknown;
						break;
				}

				return GlobVarH_Process_Ok_SetSuccessful_SendOk;
			}
			else
			{
				return result;						// Too much or small
			}
		}
		else
		{
			return GlobVarH_Process_FailParamIsNotHexNumber;	// Wrong hex converting
		}
	} // End of "isHex"
	else
	{
	// If it is not hex
	// It is unsigned integers?
		if (varType == GlobVarH_Type_Uint8 || varType == GlobVarH_Type_Uint16 || varType == GlobVarH_Type_Uint32)
		{
			// Unsigned types
			if (StringToUnsignedDecimalNum(param, &num))
			{
				GlobVarH_ProcessResult_t result;
				result = GlobVarH_CheckValue(globVarID, num);

				if (result == GlobVarH_Process_Ok_SetSuccessful_SendOk)
				{
					// Good
					if (varType == GlobVarH_Type_Uint8)
					{
						uint8_t *numPointer = (uint8_t *)GlobalVarList[globVarID].varPointer;
						*numPointer = num;
					}
					else if (varType == GlobVarH_Type_Uint16)
					{
						uint16_t *numPointer = (uint16_t *)GlobalVarList[globVarID].varPointer;
						*numPointer = num;
					}
					else if (varType == GlobVarH_Type_Uint32)
					{
						uint32_t *numPointer = (uint32_t *)GlobalVarList[globVarID].varPointer;
						*numPointer = num;
					}

					return GlobVarH_Process_Ok_SetSuccessful_SendOk;		//else - not reaching
				}
				else
				{
					return result;								// Wrong
				}

				// NOTE: Do not return from here
				//return GlobVarH_Process_Ok_SetSuccessful_SendOk;
			}
			else
			{
				return GlobVarH_Process_FailParamIsNotNumber;
			}
		}

		// It is signed integers?
		if (varType == GlobVarH_Type_Int8 || varType == GlobVarH_Type_Int16 || varType == GlobVarH_Type_Int32)
		{
			// Signed types

			int32_t num = 0;

			if (StringToSignedDecimalNum(param, &num))
			{
				GlobVarH_ProcessResult_t result;
				result = GlobVarH_CheckValue(globVarID, num);

				if (result == GlobVarH_Process_Ok_SetSuccessful_SendOk)
				{
					// Good
					if (varType == GlobVarH_Type_Int8)
					{
						uint8_t *numPointer = (uint8_t *)GlobalVarList[globVarID].varPointer;
						*numPointer = num;
					}
					else if (varType == GlobVarH_Type_Int16)
					{
						uint16_t *numPointer = (uint16_t *)GlobalVarList[globVarID].varPointer;
						*numPointer = num;
					}
					else if (varType == GlobVarH_Type_Int32)
					{
						uint32_t *numPointer = (uint32_t *)GlobalVarList[globVarID].varPointer;
						*numPointer = num;
					}
					//else - not reaching
					return GlobVarH_Process_Ok_SetSuccessful_SendOk;
				}
				else
				{
					return result;		// Wrong
				}

				// NOTE: Do not return from here
				//return GlobVarH_Process_Ok_SetSuccessful_SendOk;
			}
			else
			{
				return GlobVarH_Process_FailParamIsNotNumber;
			}
		}
	}

	return GlobVarH_Process_Unknown;
}



/**
 * \brief	Set float type GlobalVar
 */
static GlobVarH_ProcessResult_t GlobVarH_SetFloat(GlobVarH_ID_t globVarID, const char *param)
{
	GlobVarH_ProcessResult_t result = GlobVarH_Process_Unknown;

	float floatValue = 0.0f;
	if (StringToFloat(param, &floatValue))
	{
		// Successful convert
		result = GlobVarH_CheckValue(globVarID,(uint32_t)(int32_t)floatValue);
		if (result == GlobVarH_Process_Ok_SetSuccessful_SendOk)
		{
			// Value is OK
			float *floatPointer = (float *)GlobalVarList[globVarID].varPointer;
			*floatPointer = floatValue;
		}
		else
		{
			// Wrong value (too high or too less)
			// Do nothing
			// TODO: Need process type like this:
			//result = Process_InvalidValue
		}
	}
	else
	{
		result = GlobVarH_Process_FailParamIsNotNumber;
	}

	return result;
}



/**
 * \brief	Set bits type GlobalVar
 */
static GlobVarH_ProcessResult_t GlobVarH_SetBits(GlobVarH_ID_t globVarID, const char *param)
{
	uint32_t num = 0;
	bool isOk = true;
	GlobVarH_ProcessResult_t result = GlobVarH_Process_Unknown;
	uint8_t maxLength = GlobalVarList[globVarID].maxValue - GlobalVarList[globVarID].minValue + 1;

	// Check prefix, need '0b'
	if ((StringLength(param) <= 2) || (param[0] != '0') || (param[1] != 'b'))
	{
		// Too short or there is no prefix
		result = GlobVarH_Process_FailParamIsNotBinary;
		isOk = false;
	}

	if (isOk)
	{
		// Check length: max = 0bxxx --> xxx from max-minValue, '0b' length is 2
		if (StringLength(param) > (size_t)(maxLength + 2))
		{
			result = GlobVarH_Process_FailParamTooLongBinary;
			isOk = false;
		}
	}

	if (isOk)
	{
		// Convert binary string to num
		if (StringBinaryToNum(&param[2], &num))
		{
			num <<= GlobalVarList[globVarID].minValue;					// Convert is ok, make correct value
			result = GlobVarH_CheckValue(globVarID, num);		// Check num value

			if (result == GlobVarH_Process_Ok_SetSuccessful_SendOk)
			{
				// Value is ok
				uint32_t *valueNeedSet = (uint32_t *)GlobalVarList[globVarID].varPointer;
				// Clear bits
				uint8_t i;
				for (i = (uint8_t)GlobalVarList[globVarID].minValue;
					 i < (uint8_t)GlobalVarList[globVarID].maxValue;
					 i++)
				{
					*valueNeedSet &= (uint32_t)~(1 << i);	// Clear bit
				}
				*valueNeedSet |= num;						// Set new value
			}
		}
		else
		{
			result = GlobVarH_Process_FailParamIsNotBinary;			// Convert failed
		}
	}

	return result;
}



/**
 * \brief	Set string type GlobalVar
 */
static GlobVarH_ProcessResult_t GlobVarH_SetString(GlobVarH_ID_t globVarID, const char *param)
{
	GlobVarH_ProcessResult_t result = GlobVarH_Process_Unknown;

	// Check length
	uint8_t stringLength = StringLength(param);
	if (stringLength >= GlobalVarList[globVarID].maxValue)
	{
		result = GlobVarH_Process_FailParamTooLongString;		// Too long
	}
	else
	{
		// Correct length
		char *string = (char *)GlobalVarList[globVarID].varPointer;

		StrCpyMax(string, param, stringLength);			// Copy parameter

		result = GlobVarH_Process_Ok_SetSuccessful_SendOk;
	}

	return result;
}



/**
 * \brief	Set enumerator
 */
static GlobVarH_ProcessResult_t GlobVarH_SetEnumerator(GlobVarH_ID_t globVarID, const char *param)
{
	GlobVarH_ProcessResult_t result = GlobVarH_Process_Unknown;
	uint32_t enumValue = 0;
	uint8_t *enumPointer = (uint8_t *)GlobalVarList[globVarID].varPointer;
	uint8_t i;

	// Check enumList pointer
	if (GlobalVarList[globVarID].enumList == NULL)
	{
		result = GlobVarH_Process_Settings_EmptyEnumList;
	}
	else
	{
		// Check, it is number?
		if (StringIsUnsignedDecimalString(param))
		{
			// It is number
			if (StringToUnsignedDecimalNum(param, &enumValue))
			{
				if (GlobVarH_CheckValue(globVarID,enumValue) == GlobVarH_Process_Ok_SetSuccessful_SendOk)
				{
					// It is Ok
					*enumPointer = (uint8_t)enumValue;
					result = GlobVarH_Process_Ok_SetSuccessful_SendOk;
				}
				else
				{
					result = GlobVarH_Process_InvalidValue_TooMuch;
				}
			}
		}
		else
		{
			// Not number, check string
			bool isGoodEnum = false;

			for (i = 0; i < GlobalVarList[globVarID].maxValue; i++)
			{
				// It is equal string?
				if (StrCmp(param, GlobalVarList[globVarID].enumList[i]) == 0)
				{
					// Equal
					// Set value
					*enumPointer = i;
					isGoodEnum = true;
					result = GlobVarH_Process_Ok_SetSuccessful_SendOk;
					break;
				}
			}

			if (!isGoodEnum)	// Not found, it is invalid string
			{
				result = GlobVarH_Process_InvalidValue_NotEnumString;
			}
		}
	}

	if (result != GlobVarH_Process_Ok_SetSuccessful_SendOk && result != GlobVarH_Process_Settings_EmptyEnumList)
	{
		// TODO: Put to in a new function
		// Wrong, send enumerator strings
		CmdH_Printf("Invalid enum value, \"%s\" command has this enumerator strings:\r\n",
				GlobalVarList[globVarID].name);
		for (i = 0; i < GlobalVarList[globVarID].maxValue; i++)
		{
			CmdH_Printf("- %s\r\n", GlobalVarList[globVarID].enumList[i]);
		}
	}

	return result;
}



/**
 * \brief	Check values
 * 			- Check type value (integer)
 * 			- Check min-max
 * \return	GlobVarH_Process_Ok_SetSuccessful_SendOk, if ok
 */
static GlobVarH_ProcessResult_t GlobVarH_CheckValue(GlobVarH_ID_t globVarID, uint32_t num)
{
	GlobVarH_ProcessResult_t result = GlobVarH_Process_Ok_SetSuccessful_SendOk;

	// First, check the type value
	switch (GlobalVarList[globVarID].type)
	{
		case GlobVarH_Type_Bool:
			if (num > BOOL_MAX)
			{
				result = GlobVarH_Process_InvalidValue_TooMuch;
			}
			break;

		case GlobVarH_Type_Uint8:
			if (num > UINT8_MAX)
			{
				result = GlobVarH_Process_InvalidValue_TooMuch;
			}
			break;

		case GlobVarH_Type_Uint16:
			if (num > UINT16_MAX)
			{
				result = GlobVarH_Process_InvalidValue_TooMuch;
			}
			break;

		case GlobVarH_Type_Uint32:
			// TODO: Always good, do better code
			if (num > UINT32_MAX)
			{
				result = GlobVarH_Process_InvalidValue_TooMuch;
			}
			break;

		case GlobVarH_Type_Int8:
			if ((int32_t)num > INT8_MAX)
			{
				result = GlobVarH_Process_InvalidValue_TooMuch;
			}
			else if ((int32_t)num < INT8_MIN)
			{
				result = GlobVarH_Process_InvalidValue_TooSmall;
			}
			break;

		case GlobVarH_Type_Int16:
			if ((int32_t)num > INT16_MAX)
			{
				result = GlobVarH_Process_InvalidValue_TooMuch;
			}
			else if ((int32_t)num < INT16_MIN)
			{
				result = GlobVarH_Process_InvalidValue_TooSmall;
			}
			break;

		case GlobVarH_Type_Int32:
			// TODO: Always good, do better code
			if ((int32_t)num > INT32_MAX)
			{
				result = GlobVarH_Process_InvalidValue_TooMuch;
			}
			if ((int32_t)num < INT32_MIN)
			{
				result = GlobVarH_Process_InvalidValue_TooSmall;
			}
			break;

		case GlobVarH_Type_Float:
			// TODO: How to check float type value?
			break;

		case GlobVarH_Type_Bits:
			if (num > (uint32_t)(power(2,GlobalVarList[globVarID].maxValue+1)-1))
			{
				result = GlobVarH_Process_InvalidValue_TooMuch;
			}
			break;

		case GlobVarH_Type_String:
			if (num >= GlobalVarList[globVarID].maxValue)
			{
				result = GlobVarH_Process_FailParamTooLongString;
			}
			break;

		case GlobVarH_Type_Enumerator:
			if (num >= GlobalVarList[globVarID].maxValue)
			{
				result = GlobVarH_Process_InvalidValue_TooMuch;
			}
			break;

		// Wrong types
		case GlobVarH_Type_Unknown:
		case GlobVarH_Type_Count:
		default:
			result = GlobVarH_Process_FailType;
			break;
	}

	if (result == GlobVarH_Process_Ok_SetSuccessful_SendOk)
	{
		// Check maxValue
		// maxValue is set?
		if (GlobalVarList[globVarID].maxValue != GlobalVarList[globVarID].minValue && GlobalVarList[globVarID].type != GlobVarH_Type_Bits)
		{
			// There is setted maxValue, because max not equal than min
			// Check, it is too large or too small?

			if (num > GlobalVarList[globVarID].maxValue)
			{
				result = GlobVarH_Process_InvalidValue_TooMuch;			// Too large
			}
			else if (num < GlobalVarList[globVarID].minValue)
			{
				result = GlobVarH_Process_InvalidValue_TooSmall;			// Too small
			}
		}
	}

	return result;
}



/**
 * \brief	Write process result
 */
void GlobVarH_WriteResults(GlobVarH_ProcessResult_t result)
{
	const char *pMessage = NULL;

	switch (result)
	{
		case GlobVarH_Process_Ok_Answered:
			// Do nothing
			break;

		case GlobVarH_Process_Ok_SetSuccessful_SendOk:
			pMessage = "Set successful";
			break;

		case GlobVarH_Process_GlobalVariableNameNotFind:
			pMessage = "Global variable not find";
			break;

		case GlobVarH_Process_FailParam:
			pMessage = "Fail parameter";
			break;

		case GlobVarH_Process_FailType:
			pMessage = "Fail type";
			break;

		case GlobVarH_Process_FailParamIsNotNumber:
			pMessage = "Not number";
			break;

		case GlobVarH_Process_FailParamIsNotHexNumber:
			pMessage = "Not hex number";
			break;

		case GlobVarH_Process_FailParamIsNotHexStart:
			pMessage = "Not hex, missed \"0x\"";
			break;

		case GlobVarH_Process_FailParamIsNotBinary:
			pMessage = "Not binary number. Syntax: \"0b1010\"";
			break;

		case GlobVarH_Process_FailParamTooLongBinary:
			pMessage = "Too long binary value";
			break;

		case GlobVarH_Process_InvalidValue_TooSmall:
			pMessage = "Invalid value, too small";
			break;

		case GlobVarH_Process_InvalidValue_TooMuch:
			pMessage = "Invalid value, too much";
			break;

		case GlobVarH_Process_InvalidValue_NotBool:
			pMessage = "Invalid value, not bool";
			break;

		case GlobVarH_Process_InvalidValue_NotEnumString:
			pMessage = "Invalid enum string";
			break;

		case GlobVarH_Process_Settings_EmptyEnumList:
			pMessage = "EnumList settings error";
			break;

		case GlobVarH_Process_IsReadOnly:
			pMessage = "Cannot set, it is constant";
			break;

		case GlobVarH_Process_SourceNotEnabled:
			pMessage = "Cannot process this command from this source";
			break;

		case GlobVarH_Process_FailParamTooLongString:
			pMessage = "Too long string";
			break;

		case GlobVarH_Process_Unknown:
			pMessage = "Unknown error";
			break;

		default:
			pMessage = "Fatal error";
			break;
	}

	CmdH_SendMessage(pMessage);
}



/*
 * \brief	Send header (for List all variables)
 */
static void GlobalVarHandler_ListAllVariable_SendHeader(void)
{
	CmdH_Printf("+-%2c-+-%20c-+-%10c-+-%5c-+-%5c-+-%4c-+-%20c-+\r\n", '-', '-', '-', '-', '-', '-', '-');
}



/**
 * \brief	List all variables
 */
void GlobVarH_ListAllVariableParameters(void)
{
	GlobVarH_ID_t i;

	// TODO: Enumokat is kiírni, ha van?

	// Send header
	GlobalVarHandler_ListAllVariable_SendHeader();
	CmdH_Printf("| %2s | %20s | %10s | %5s | %5s | %4s | %20s |\r\n",
			"ID", "Name", "Type", "Min", "Max", "Unit", "Description");
	GlobalVarHandler_ListAllVariable_SendHeader();

	// Rows (commands)
	for (i = 0; i < GlobalVar_MaxCommandNum; i++)
	{
		// Print one command / line:
		CmdH_Printf(
				"| %2d | %20s | %10s | %5d | %5d | %4s | %20s |\r\n",
				i,
				GlobalVarList[i].name,
				GlobVarH_TypesNames[GlobalVarList[i].type],
				GlobalVarList[i].minValue,
				GlobalVarList[i].maxValue,
				GlobalVarList[i].unit,
				GlobalVarList[i].description
				);
	}

	// After commands (end)
	GlobalVarHandler_ListAllVariable_SendHeader();
}



/**
 * \brief	Print all variable values
 */
void GlobVarH_PrintAllVariableValues(void)
{
	GlobVarH_ID_t i;

	CmdH_Printf("Global variables:\r\n"
			" %20s %20s\r\n",
			"<Name>",
			"<Value>");

	// Print all variables
	for (i = 0; i < GlobalVar_MaxCommandNum; i++)
	{
		// Print a variable name and value
		//CmdH_Printf(" %20s %20s\r\n", GlobalVarList[i].name);
		CmdH_Printf(" %20s ", GlobalVarList[i].name);
		GlobVarH_GetVariable((GlobVarH_VarRecord_t *)&GlobalVarList[i]);
		CmdH_SendLine(NULL);
	}

	CmdH_SendLine("End of global variables");
}



/**
 * \brief	Print global variable descriptions
 */
static void GlobVarH_PrintVariableDescriptions(GlobVarH_ID_t globVarID)
{
	CmdH_Printf(
			"Command help: %s\r\n"
			"type: %s\r\n"
			"min:  %d\r\n"
			"max:  %d\r\n"
			"desc: %s\r\n",
			GlobalVarList[globVarID].name,
			GlobVarH_TypesNames[GlobalVarList[globVarID].type],
			GlobalVarList[globVarID].minValue,
			GlobalVarList[globVarID].maxValue,
			GlobalVarList[globVarID].description
			);
}



#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
/**
 * \brief	Set trace with string parameter
 */
static void GlobVarH_SetTrace(GlobVarH_ID_t commandID, const char * param)
{
	if (!StrCmp("enable", param))
	{
		GlobVarH_EnableTrace(commandID, true);
	}
	else
	{
		// TODO: Check "disable" or not?
		GlobVarH_EnableTrace(commandID, false);
	}
}



/**
 * \brief	Enable / Disable trace
 */
void GlobVarH_EnableTrace(GlobVarH_ID_t id, bool isEnable)
{
	if (id < GlobalVar_MaxCommandNum)
	{
		if (isEnable)
		{
			GlobVarH_TraceVarEnabled |= (1 << id);
		}
		else
		{
			GlobVarH_TraceVarEnabled &= ~(1 << id);
		}
	}
}



/**
 * \brief	Trace GlobalVar
 */
void GlobVarH_RunTrace(void)
{
	// TODO: Only use for max 32bits (4byte) variable
	if (GlobVarH_TraceVarEnabled)
	{
		GlobVarH_ID_t i;
		for (i = 0; i < GlobalVar_MaxCommandNum; i++)
		{
			// Step on GlobalVar list
			if (GlobVarH_TraceVarEnabled & (1 << i))
			{
				// Need trace
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_RAM_BUFFER
				// TODO: Unknown size... Now use uint32_t
				// TODO: Skip "string" variables
				// TODO: Only function values saved to Temporary variable...


				// TODO: Problem: GetVariable() is printing..
				/*
				GlobalVarHandler_GetVariable(&GlobalVarList[i]);
				*/
				if (GlobalVarList[i].isFunction)
				{
					GlobVarH_GetFunctionValue((GlobVarH_VarRecord_t *)&GlobalVarList[i]);				// Get with function
				}
				else
				{
					GlobVarH_TemporaryValue = (uint32_t)*(uint32_t *)GlobalVarList[i].varPointer;	// Get variable
				}

				// Save value and trace data
				GlobVarH_TraceRamBuffer[GlobVarH_TraceRam_BufferCnt].tick = HAL_GetTick();
				GlobVarH_TraceRamBuffer[GlobVarH_TraceRam_BufferCnt].data = GlobVarH_TemporaryValue;
				GlobVarH_TraceRamBuffer[GlobVarH_TraceRam_BufferCnt].varID = i;

				GlobVarH_TraceRam_BufferCnt++;

				// Check buffer is full?
				if (GlobVarH_TraceRam_BufferCnt >= GLOBALVARHANDLER_TRACE_BUFFER_SIZE)
				{
					GlobVarH_TraceRam_BufferCnt = 0;
					// TODO: Can we print, when buffer is full?
					GlobVarH_PrintTraceBuffer();
				}
#else
				// Trace (print) to debug port
				CmdH_Printf("Trace: %d - ", HAL_GetTick());
				GlobVarH_GetVariable((GlobVarH_VarRecord_t *)&GlobalVarList[i]);
				CmdH_SendLine("");
#endif
			}
		}
	}
}



#ifdef CONFIG_GLOBALVARHANDLER_TRACE_RAM_BUFFER
/**
 * \brief	Print Trace buffer content
 */
void GlobVarH_PrintTraceBuffer(void)
{
	uint8_t i;
	for (i = 0; i < GLOBALVARHANDLER_TRACE_BUFFER_SIZE; i++)
	{
		// Trace record has: tick, data, varID

		GlobVarH_ID_t index = GlobVarH_TraceRamBuffer[i].varID;

		// Create a GlobalVar struct, which has been printed out
		GlobVarH_VarRecord_t command =
		{
			// XXX: Sync with GlobVarH_VarRecord_t !!!
			.name = GlobalVarList[index].name,
			.type = GlobalVarList[index].type,

			// Extreme !!
			.varPointer = (void *)&GlobVarH_TraceRamBuffer[i].data,
			.isReadOnly = GlobalVarList[index].isReadOnly,

			// Extreme !!
			.isFunction = false,
			.getFunctionPointer = NULL,
			.setFunctionPointer = NULL,

			.maxValue = GlobalVarList[index].maxValue,
			.minValue = GlobalVarList[index].minValue,

			.sourceEnable = GlobalVarList[index].sourceEnable,

			// TODO: Optimize these: isHex (bool), isReadOnly (bool), isFunction (bool)
			.isHex = GlobalVarList[index].isHex,

			.enumList = GlobalVarList[index].enumList,

	#ifdef GLOBVARH_UNIT_ENABLE
			.unit = GlobalVarList[index].unit,
	#endif
	#ifdef GLOBVARH_DESCRIPTION_ENABLE
			.description = GlobalVarList[index].description,
	#endif
		};

		CmdH_Printf("%3d.\t%8u\t", i, GlobVarH_TraceRamBuffer[i].tick);
		GlobVarH_GetVariable(&command);
		CmdH_SendLine("");
	}
}
#endif

#endif



#ifdef MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE
/**
 * \brief	Clear Global Var UnitTest buffer
 */
static void GlobVarH_UT_Clear(void)
{
	COMMUNICATION_ClearProtocolBuffer();
	CmdH_SetResponse(CommProt_Buffer, Communication_Buffer, COMMUNICATION_PROTOCOL_BUFFER_SIZE);
}



/**
 * \brief	GlobalVarHandler unit test
 */
void GlobVarH_UnitTest(void)
{
// TODO: Put at header or merge with WriteResult function...
#define GLOBALVARHANDLER_MSG_SET_SUCCESSFUL				("Set successful")
#define GLOBALVARHANDLER_MSG_SET_FAILED_TOO_MUCH		("Invalid value, too much")
#define GLOBALVARHANDLER_MSG_SET_FAILED_NOT_BOOL		("Invalid value, not bool")
#define GLOBALVARHANDLER_MSG_SET_FAILED_NOT_NUMBER		("Not number")
#define GLOBALVARHANDLER_MSG_SET_FAILED_WRONG_SOURCE	("Cannot process this command from this source")
#define GLOBALVARHANDLER_MSG_SET_FAILED_NOT_HEX			("Not hex, missed \"0x\"")
#define GLOBALVARHANDLER_MSG_SET_FAILED_NOT_BINARY		("Not binary number. Syntax: \"0b1010\"")
#define GLOBALVARHANDLER_MSG_SET_FAILED_WRONG_ENUM		("Invalid enum value")

	GlobVarH_ProcessResult_t result;

	// Start GlobalVarHandler UnitTest
	UnitTest_Start("GlobalVarHandler", __FILE__);

	// Initialize
	CmdH_SetResponse(CommProt_Buffer, Communication_Buffer, COMMUNICATION_PROTOCOL_BUFFER_SIZE);


	// Check GlobalVarHandler structures
#ifndef CONFIG_GLOBALVARHANDLER_CHECK_ENABLE
	#define CONFIG_GLOBALVARHANDLER_CHECK_ENABLE
	#warning "Need enable GlobalVarChecker"
#endif
	GlobVarH_CheckGlobalVarArray();
	//UNITTEST_ASSERT(isOk, "GlobalVarHandler structs are wrong");


	// Test "testuint8" variable
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint8", "8", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testuint8 set error");

	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint8", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testuint8 get error");
	UNITTEST_ASSERT(!StrCmp("8 cm", Communication_Buffer), "testuint8 get error");

	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint8", "255", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testuint8 set error");

	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint8", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testuint8 get error");
	UNITTEST_ASSERT(!StrCmp("255 cm", Communication_Buffer), "testuint8 get error");

	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint8", "256", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_InvalidValue_TooMuch, "testuint8 get error");

	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint8", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testuint8 get error");
	UNITTEST_ASSERT(!StrCmp("255 cm", Communication_Buffer), "testuint8 set-get error");


	// test "testbool" variable
	// Set valid value: 1/true
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testbool", "true", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testbool set error");

	// Get
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testbool", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testbool get error");
	UNITTEST_ASSERT(!StrCmp("1 / TRUE", Communication_Buffer), "testbool get error");

	// Set valid value: 0/false
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testbool", "0", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testbool get error");

	// Get
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testbool", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testbool get error");
	UNITTEST_ASSERT(!StrCmp("0 / FALSE", Communication_Buffer), "testbool get error");

	// Set invalid value
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testbool", "2", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_InvalidValue_NotBool, "testbool get error");

	// Check unchange
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testbool", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testbool get error");
	UNITTEST_ASSERT(!StrCmp("0 / FALSE", Communication_Buffer), "testbool set-get error");


	// Test "testint16" variable
	// Set valid value: 8
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint16", "8", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testint16 get error");

	// Get
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint16", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testint16 get error");
	UNITTEST_ASSERT(!StrCmp("8", Communication_Buffer), "testint16 get error");

	// Set valid value: 255
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint16", "255", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testint16 get error");

	// Get
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint16", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testint16 get error");
	UNITTEST_ASSERT(!StrCmp("255", Communication_Buffer), "testint16 get error");

	// Set valid value: -255
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint16", "-255", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testint16 get error");

	// Get
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint16", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testint16 get error");
	UNITTEST_ASSERT(!StrCmp("-255", Communication_Buffer), "testint16 get error");

	// Set invalid value:
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint16", "65536", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_InvalidValue_TooMuch, "testint16 value validation error");

	// Check unchange
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint16", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testint16 get error");
	UNITTEST_ASSERT(!StrCmp("-255", Communication_Buffer), "testint16 set-get error");


	// Test "testint32" variable
	// Set valid value: 8
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint32", "8", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testint32 set error");

	// Get
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint32", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testint32 get error");
	UNITTEST_ASSERT(!StrCmp("8", Communication_Buffer), "testint32 get error");

	// Set valid value: 65535
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint32", "65535", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testint32 set error");

	// Get
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint32", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testint32 get error");
	UNITTEST_ASSERT(!StrCmp("65535", Communication_Buffer), "testint32 get error");

	// Set valid value: -255
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint32", "-255", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testint32 set error");

	// Get
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint32", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testint32 get error");
	UNITTEST_ASSERT(!StrCmp("-255", Communication_Buffer), "testint32 get error");

	// Set invalid value:
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint32", "12.34", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_FailParamIsNotNumber, "testint32 value validation error");

	// Check unchange
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testint32", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testint32 set-get error");
	UNITTEST_ASSERT(!StrCmp("-255", Communication_Buffer), "testint32 set-get error");


	// Test "testuint32" variable
	// Set valid value: 8
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint32", "0x08", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testuint32 set error");

	// Get
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint32", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testuint32 get error");
	UNITTEST_ASSERT(!StrCmp("0x00000008", Communication_Buffer), "testuint32 get error");

	// Set valid value: 65535
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint32", "0x65535", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testuint32 set error");

	// Get
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint32", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testuint32 get error");
	UNITTEST_ASSERT(!StrCmp("0x00065535", Communication_Buffer), "testuint32 get error");

	// Set invalid value: -255
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint32", "-255", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_FailParamIsNotHexStart, "testuint32 set error");

	// Get
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint32", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testuint32 get error");
	UNITTEST_ASSERT(!StrCmp("0x00065535", Communication_Buffer), "testuint32 get error");

	// Set invalid value:
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint32", "12.34", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_FailParamIsNotHexStart, "testuint32 value validation error");

	// Check unchange
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testuint32", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testuint32 get error");
	UNITTEST_ASSERT(!StrCmp("0x00065535", Communication_Buffer), "testuint32 set-get error");


	// Test "cannotaccess" variable
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testcannotaccess", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_SourceNotEnabled, "Variable source error");


	// Test "testfloat" variable (float)
	// Set float value:
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testfloat", "12.34", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "Float value setting error");

	// Check value/range
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testfloat", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	float testNumber = 0.0f;
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "Float type error");
	UNITTEST_ASSERT(StringToFloat(Communication_Buffer, &testNumber), "float type error");
	UNITTEST_ASSERT(((testNumber>12.33)&&(testNumber<12.35)), "float set-get error");

	// TODO: Extend with invalid float and etc.


	// Test "testenum" (enum)
	// Set enum
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testenum", "1", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "enum value set error");

	// Get enum
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testenum", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "enum value get error");
	UNITTEST_ASSERT(!StrCmp("1 exampleenumstring1", Communication_Buffer), "enum value get error");

	// Set enum - fail
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testenum", "4", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_InvalidValue_TooMuch, "enum value set error");

	// Get enum
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testenum", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "enum value get error");
	UNITTEST_ASSERT(!StrCmp("1 exampleenumstring1", Communication_Buffer), "enum value get error");

	// TODO: Extend enum tests
	// E.g. GlobVarH_Process_InvalidValue_NotEnumString


	// Test "testbit" (bits)
	// Set bit
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testbit", "0b0", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testbit value set error");

	// Get bit
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testbit", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testbit value get error");
	UNITTEST_ASSERT(!StrCmp("0b0", Communication_Buffer), "testbit value get error");

	// Set bit
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testbit", "0b111", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_SetSuccessful_SendOk, "testbit value set error");

	// Get bit
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testbit", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testbit value get error");
	UNITTEST_ASSERT(!StrCmp("0b111", Communication_Buffer), "testbit value get error");

	// Set bit - fail
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testbit", "A", GlobVarH_SetGet_Set, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_FailParamIsNotBinary, "testbit value set error");

	// Get bit
	GlobVarH_UT_Clear();
	result = GlobVarH_ProcessVariableCommand("testbit", "", GlobVarH_SetGet_Get, CommProt_Buffer);
	UNITTEST_ASSERT(result == GlobVarH_Process_Ok_Answered, "testbit value get error");
	UNITTEST_ASSERT(!StrCmp("0b111", Communication_Buffer), "testbit value get error");

	// TODO: Extend bit tests


	// End of UnitTest
	//CmdH_CommandSource = CommProt_Unknown;

	UnitTest_End();
}
#endif	// #ifdef MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE



#endif	// #ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
