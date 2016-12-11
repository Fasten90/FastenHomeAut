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


#include "include.h"


/// DEFINES

#define GLOBALVARHANDLER_UNIT_ENABLE
#define GLOBALVARHANDLER_DESCRIPTION_ENABLE

#define GLOBALVARHANDLER_NAME_MAX_LENGTH		(20)



/// TYPEDEFS
/// ENUMS

typedef uint8_t VarID_t;


// Note: This enum has an "name list" (GlobalVarTypesNames), Must be in the same order with it!
typedef enum
{
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
	Type_Enumerator,
	Type_Count
} VarType_t;


typedef enum
{
	Process_UnknownError = 0,
	Process_GlobalVariableNameNotFind,
	Process_SourceNotEnabled,
	Process_IsReadOnly,
	Process_TooLongString,
	Process_FailType,
	Process_FailParam,
	Process_FailParamIsNotNumber,
	Process_FailParamIsNotHexNumber,
	Process_FailParamIsNotHexStart,
	Process_InvalidValue_TooMuch,
	Process_InvalidValue_TooSmall,
	Process_InvalidValue_NotBool,
	Process_InvalidValue_NotEnumString,
	Process_Settings_EmptyEnumList,
	Process_Ok_Answered,
	Process_Ok_SetSuccessful_SendOk
} ProcessResult_t;


typedef enum
{
	SetGet_Unknown = 0,
	SetGet_Get,
	SetGet_Set,
	SetGet_Help
} SetGetType_t;



/// STRUCTS


///< GlobalVarCommand structure for set-get global variables
typedef struct
{

	const char * const name;				///< Name of global variable [string]
	const VarType_t type;					///< Type of global variable

	void * const varPointer;				///< Pointer of variable
	const bool isReadOnly;					///< Is read only?

	const uint32_t maxValue;				///< Max value
	const uint32_t minValue;				///< Min value

	const CommProtocol_t sourceEnable;		///< Enabled sources (for set-get)

	const bool isHex;						///< Set-get in hexadecimal?

	const char * const *enumList;			///< Enum list, if it is enumerator

#ifdef GLOBALVARHANDLER_UNIT_ENABLE
	const char * const unit;				///< units, example: [cm]
#endif
#ifdef GLOBALVARHANDLER_DESCRIPTION_ENABLE
	const char * const description;			///< descriptions of global variable
#endif

} GlobalVarCommand_t;




/// FUNCTION PROTOTYPES
bool GlobalVarHandler_CheckCommandStructAreValid(void);
void GlobalVarHandler_ProcessCommand(
		const char *commandName, const char *param,
		SetGetType_t setGetType, CommProtocol_t source,
		char *resultBuffer, uint8_t resultBufferLength);

void GlobalVarHandler_ListAllVariableParameters(void);
void GlobalVarHandler_PrintAllVariableValues(void);



#endif /* GLOBALVARHANDLER_H_ */
