/*
 * GlobalVarHandler.h
 *
 *  Created on: Sep 5, 2016
 *      Author: Vizi G�bor
 */

#ifndef GLOBALVARHANDLER_H_
#define GLOBALVARHANDLER_H_


#include "include.h"


/// DEFINES

#define GLOBALVARHANDLER_UNIT_ENABLE
#define GLOBALVARHANDLER_DESCRIPTION_ENABLE


/// TYPEDEFS
/// ENUMS

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
	Process_CommandNotFound,
	Process_SourceNotEnabled,
	Process_IsReadOnly,
	Process_TooLongString,
	Process_FailType,
	Process_FailParam,
	Process_FailParamIsNotNumber,
	Process_FailParamIsNotHexNumber,
	Process_InvalidValue_TooMuch,
	Process_InvalidValue_TooSmall,
	Process_InvalidValue_NotBool,
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


typedef enum
{
	Source_All = 0,
	Source_DebugSerial = (1 << 0),
	Source_Ethernet = (1 << 1),
	Source_Disable = (1 << 2)
} CommandSource_t;


/// STRUCTS

typedef struct
{

	const char const *name;
	const VarType_t type;

	void const *varPointer;
	const bool isReadOnly;

	const uint32_t maxValue;	// TODO: union pl. a stringMaxLength-tel
	const uint32_t minValue;

	//const uint8_t stringMaxLength;	// TODO: �sszevonva a maxValue-val

	const CommandSource_t sourceEnable;

	const bool isHex;		// TODO: esetleg lekezelj�k k�l�n bemenetk�nt a 0x00et...

	const char *enumList;	// TODO: enumer�torhoz

#ifdef GLOBALVARHANDLER_UNIT_ENABLE
	const char const *unit;
#endif
#ifdef GLOBALVARHANDLER_DESCRIPTION_ENABLE
	const char const *description;
#endif

} GlobalVarCommand_t;




/// FUNCTION PROTOTYPES
bool GlobalVarHandler_CheckCommandStructAreValid(void);
void GlobalVarHandler_ProcessCommand(
		const char *commandName, const char *param,
		SetGetType_t setGetType, CommandSource_t source,
		char *resultBuffer, uint8_t resultBufferLength);

void GlobalVarHandler_ListAllVariables(void);




#endif /* GLOBALVARHANDLER_H_ */
