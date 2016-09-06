/*
 * GlobalVarHandler.h
 *
 *  Created on: Sep 5, 2016
 *      Author: Vizi Gábor
 */

#ifndef GLOBALVARHANDLER_H_
#define GLOBALVARHANDLER_H_


#include "include.h"


/// TYPEDEFS
/// ENUMS

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
	Type_String,
	Type_Enumerator
} VarType_t;


typedef enum
{
	Process_UnknownError = 0,
	Process_CommandNotFound,
	Process_SourceNotEnabled,
	Process_IsReadOnly,
	Process_TooLongString,
	Process_FailParam,
	Process_FailParamIsNotNumber,
	Process_InvalidValue_TooMuch,
	Process_InvalidValue_TooSmall,
	Process_Ok_Answered,
	Process_Ok_SetSuccessful_SendOk
} ProcessResult_t;


typedef enum
{
	SetGet_Unknown = 0,
	SetGet_Get,
	SetGet_Set
} SetGetType_t;


typedef enum
{
	Source_All = 0,
	Source_DebugSerial = 1 << 0,
	Source_Ethernet = 1 << 1,
} CommandSource_t;


/// STRUCTS

typedef struct
{

	const char const *name;
	const VarType_t type;

	void const *varPointer;
	const bool isReadOnly;

	uint32_t maxValue;
	uint32_t minValue;
	bool isHex;	// TODO: esetleg lekezeljük külön bemenetként a 0x00et...
	const char const *unit;
	const char const *description;

	const uint8_t stringMaxLength;

	const CommandSource_t sourceEnable;

} GlobalVarCommand_t;




/// FUNCTION PROTOTYPES
bool GlobalVarHandler_CheckCommandStructAreValid(void);
void GlobalVarHandler_CheckReceivedCommand(
		const char *commandName, const char *param,
		SetGetType_t setGetType, CommandSource_t source,
		char *resultBuffer, uint8_t resultBufferLength);






#endif /* GLOBALVARHANDLER_H_ */
