/*
 *		GlobalVarHandler.h
 *		Created on:		2016-09-05
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Global Variable Handler: Handle set-get variable from uart or other
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
 */

#ifndef GLOBALVARHANDLER_H_
#define GLOBALVARHANDLER_H_


#include "options.h"
#include "GenericTypeDefs.h"
#include "Communication.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

// Set default values for module options:
#if !defined(GLOBVARH_UNIT_ENABLE)
	#define GLOBVARH_UNIT_ENABLE				0		///< optional unit field to a global variable
#endif

#if !defined(GLOBVARH_DESCRIPTION_ENABLE)
	#define GLOBVARH_DESCRIPTION_ENABLE			1		///< optional description field to a global variable
#endif

#define GLOBVARH_NAME_MAX_LENGTH				(20U)	///< maximal length of a variable



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

#ifndef CONFIG_MODULE_COMMUNICATION_ENABLE
typedef uint8_t CommProtocol_t;
typedef uint8_t CommProtocolBit_t;
#ifndef CommProtBit_Unknown
	#define CommProtBit_Unknown						(0)
#endif
#endif

typedef int8_t GlobVarH_ID_t;							///< type alias for counting the rows in the variable array. \sa GlobalVarList

///< GlobalVariable type
/// \note	This enum has a "name list" (GlobalVarTypesNames), must be in the same order with it!
/// \sa	GlobalVarTypesNames
typedef enum
{
	GlobVarH_Type_Unknown = 0,
	GlobVarH_Type_Bool,
	GlobVarH_Type_Uint8,
	GlobVarH_Type_Int8,
	GlobVarH_Type_Uint16,
	GlobVarH_Type_Int16,
	GlobVarH_Type_Uint32,
	GlobVarH_Type_Int32,
	GlobVarH_Type_Float,
	GlobVarH_Type_Bits,
	GlobVarH_Type_String,
	GlobVarH_Type_Enumerator,
	GlobVarH_Type_Count
} GlobVarH_Type_t;

///< Process result enumerator
typedef enum
{
	GlobVarH_Process_Unknown = 0,
	GlobVarH_Process_GlobalVariableNameNotFind,
	GlobVarH_Process_SourceNotEnabled,
	GlobVarH_Process_IsReadOnly,
	GlobVarH_Process_FailType,
	GlobVarH_Process_FailParam,
	GlobVarH_Process_FailParamTooLongString,
	GlobVarH_Process_FailParamIsNotNumber,
	GlobVarH_Process_FailParamIsNotHexNumber,
	GlobVarH_Process_FailParamIsNotHexStart,
	GlobVarH_Process_FailParamIsNotBinary,
	GlobVarH_Process_FailParamTooLongBinary,
	GlobVarH_Process_InvalidValue_TooMuch,
	GlobVarH_Process_InvalidValue_TooSmall,
	GlobVarH_Process_InvalidValue_NotBool,
	GlobVarH_Process_InvalidValue_NotEnumString,
	GlobVarH_Process_Settings_EmptyEnumList,
	GlobVarH_Process_Ok_Answered,
	GlobVarH_Process_Ok_SetSuccessful_SendOk
} GlobVarH_ProcessResult_t;

///< GlobalVariable Set-Get type enumerator: what to do with the variable (get/set/help)
typedef enum
{
	GlobVarH_SetGet_Unknown = 0,
	GlobVarH_SetGet_Get,
	GlobVarH_SetGet_Set,
	GlobVarH_SetGet_Help,
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
	GlobVarH_SetGet_Trace
#endif
} GlobVarH_SetGetType_t;

typedef uint32_t (*GetFunctionPointer)(void);			///< Get function pointer
typedef bool (*SetFunctionPointer)(uint32_t param);		///< Set function pointer
typedef void (*GeneralFunctionPointer)(void);			///< General function pointer

///< Structure for set-get global variables
typedef struct
{
	const char * const name;				///< Name of global variable [string]
	const GlobVarH_Type_t type;				///< Type of global variable

	void * const varPointer;				///< Pointer of variable
	const bool isReadOnly;					///< Is read only?

	// NOTE: callback functions can not be used for string variable!
	const GeneralFunctionPointer getFunctionPointer;	///< callback function for getting the value of the variable
	const GeneralFunctionPointer setFunctionPointer;	///< callback function for setting the value of the variable

	const uint32_t maxValue;				///< Max value
	const uint32_t minValue;				///< Min value

	const CommProtocolBit_t sourceEnable;	///< Enabled sources (for set-get)	// TODO: fordítási opció legyen

	// TODO: Optimize these: isHex (bool), isReadOnly (bool)
	const bool isHex;						///< Set-get in hexadecimal format? The variable is interpretable in hex format!

	const char * const * enumList;			///< Enum list, if it is enumerator

#if GLOBVARH_UNIT_ENABLE == 1
	const char * const unit;				///< units [string], example: [cm]
#endif
#if GLOBVARH_DESCRIPTION_ENABLE == 1
	const char * const description;			///< descriptions of global variable [string]
#endif

} GlobVarH_VarRecord_t;

///< Trace log record
typedef struct
{
	uint32_t tick;							///< Trace - current tick
	uint32_t data;							///< Trace - data (value), must not exceed 32 bits!
	uint32_t varID;							///< Trace - GlobalVar ID to be traced
} GlobVarH_TraceLogRecord_t;

typedef struct
{
	const GlobVarH_VarRecord_t *items;		///< pointer to a 1st item of a variable list (GlobVarH_TraceLogRecord_t[])
	const uint8_t num;						///< number of items in the list
	uint32_t traceVarEnabled;				/**<
											 * Used as bit field to enable/disable variable tracing.
											 * e.g.: bit 1. corresponds to the 1st item in GlobVarH_VarList[] -> NOTE 32th variable can not be traced!
											 */
} GlobVarH_VarListInfo_t;
/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern uint32_t GlobVarH_TemporaryValue;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void GlobVarH_CheckGlobalVarArray(const GlobVarH_VarListInfo_t *varList);
GlobVarH_ProcessResult_t GlobVarH_ProcessVariableCommand(const GlobVarH_VarListInfo_t *varList, const char *varName, const char *param, GlobVarH_SetGetType_t setGetType, CommProtocol_t source);
void GlobVarH_WriteResults(GlobVarH_ProcessResult_t result);

void GlobVarH_ListAllVariableParameters(void);
void GlobVarH_PrintAllVariableValues(void);

void GlobVarH_EnableTrace(GlobVarH_ID_t id, bool isEnable);
void GlobVarH_RunTrace(void);
void GlobVarH_PrintTraceBuffer(void);

void GlobVarH_UnitTest(void);



#endif /* GLOBALVARHANDLER_H_ */
