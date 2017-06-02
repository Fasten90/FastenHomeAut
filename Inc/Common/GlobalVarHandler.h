/*
 *		GlobalVarHandler.h
 *
 * 		Created on:		2016.09.05
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Global Variable Handler: Handle set-get variable from uart or other
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef GLOBALVARHANDLER_H_
#define GLOBALVARHANDLER_H_


#include "Communication.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define GLOBALVARHANDLER_UNIT_ENABLE
#define GLOBALVARHANDLER_DESCRIPTION_ENABLE

#define GLOBALVARHANDLER_NAME_MAX_LENGTH		(20)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef uint8_t VarID_t;


/// GlobalVariable type
/// \note	This enum has an "name list" (GlobalVarTypesNames), Must be in the same order with it!
typedef enum
{
	Type_Unknown = 0,
	Type_Bool,
	Type_Uint8,
	Type_Int8,
	Type_Uint16,
	Type_Int16,
	Type_Uint32,
	Type_Int32,
	Type_Float,
	Type_Bits,
	Type_String,
	Type_Enumerator,
	Type_Count
} VarType_t;



/// Process result enumerator
typedef enum
{
	Process_Unknown = 0,
	Process_GlobalVariableNameNotFind,
	Process_SourceNotEnabled,
	Process_IsReadOnly,
	Process_FailType,
	Process_FailParam,
	Process_FailParamTooLongString,
	Process_FailParamIsNotNumber,
	Process_FailParamIsNotHexNumber,
	Process_FailParamIsNotHexStart,
	Process_FailParamIsNotBinary,
	Process_FailParamTooLongBinary,
	Process_InvalidValue_TooMuch,
	Process_InvalidValue_TooSmall,
	Process_InvalidValue_NotBool,
	Process_InvalidValue_NotEnumString,
	Process_Settings_EmptyEnumList,
	Process_Ok_Answered,
	Process_Ok_SetSuccessful_SendOk
} ProcessResult_t;


/// GlobalVariable Set-Get type enumerator
typedef enum
{
	SetGet_Unknown = 0,
	SetGet_Get,
	SetGet_Set,
	SetGet_Help,
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
	SetGet_Trace
#endif
} SetGetType_t;


/// GlobalVarCommand structure for set-get global variables
typedef struct
{

	const char * const name;				///< Name of global variable [string]
	const VarType_t type;					///< Type of global variable

	void * const varPointer;				///< Pointer of variable
	const bool isReadOnly;					///< Is read only?

	const uint32_t maxValue;				///< Max value
	const uint32_t minValue;				///< Min value

	const CommProtocol_t sourceEnable;		///< Enabled sources (for set-get)

	// TODO: Optimize these:
	const bool isHex;						///< Set-get in hexadecimal?

	const bool isFunction;					///< It is function?

	const char * const *enumList;			///< Enum list, if it is enumerator

#ifdef GLOBALVARHANDLER_UNIT_ENABLE
	const char * const unit;				///< units [string], example: [cm]
#endif
#ifdef GLOBALVARHANDLER_DESCRIPTION_ENABLE
	const char * const description;			///< descriptions of global variable [string]
#endif

} GlobalVarCommand_t;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

bool GlobalVarHandler_CheckCommandStructAreValid(void);
void GlobalVarHandler_ProcessCommand(
		const char *commandName, const char *param,
		SetGetType_t setGetType, CommProtocol_t source);

void GlobalVarHandler_ListAllVariableParameters(void);
void GlobalVarHandler_PrintAllVariableValues(void);

void GlobalVarHandler_EnableTrace(VarID_t id, bool isEnable);
void GlobalVarHandler_RunTrace(void);
void GlobalVarHandler_PrintTraceBuffer(void);

void GlobalVarHandler_UnitTest(void);



#endif /* GLOBALVARHANDLER_H_ */
