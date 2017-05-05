/*
 *		GlobalVariables.c
 *
 *		Created on:		2016.09.07
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Global Variable Handler: Handle set-get variable from uart or other
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#include "GlobalVarHandler.h"
#include "GlobalVariables.h"

#include "Globals.h"


#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE

/// Global variables

#ifdef MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE
// UnitTest variables
uint8_t testUint8 = 0;
int16_t testInt16 = 0;
uint32_t testUint32 = 0;
int32_t testInt32 = 0;
bool testBool = false;
bool testCannotAccess = false;
float testFloat = 0.0f;
uint8_t testEnumValue = 0;
#endif


char deviceName[20] = "Discovery";


///< enumExample string list
const char * const enumExampleList[] =
{
	"example1enumstring",
	"example2enumstring",
	"example3enumstring",
	"example4enumstring",
	NULL
};



///< Global variables list
const GlobalVarCommand_t GlobalVarList[] =
{
	{
		.name = "devicename",
		.varPointer = deviceName,
		.type = Type_String,
		.maxValue = 20,
		.description = "Device name"
	},
	{
		.name = "testUint8",
		.varPointer = &testUint8,
		.type = Type_Uint8,
		.unit = "cm",
		.description = "distance from hell"
	},
	{
		.name = "bool",
		.varPointer = &testBool,
		.type = Type_Bool
	},
	{
		.name = "testint16",
		.varPointer = &testInt16,
		.type = Type_Int16,
	},
	{
		.name = "testint32",
		.varPointer = &testInt32,
		.type = Type_Int32,
	},
	{
		.name = "testuint32",
		.varPointer = &testUint32,
		.type = Type_Uint32,
		.isHex = true,
	},
	{
		.name = "cannotaccess",
		.varPointer = &testCannotAccess,
		.type = Type_Bool,
		.sourceEnable = CommProt_Disable
	},
	{
		.name = "version",
		.varPointer = (void * const)Global_Version,
		.type = Type_String,
		.isReadOnly = true
	},
	{
		.name = "floattest",
		.varPointer = &testFloat,
		.type = Type_Float,
	},
	{
		.name = "enumtest",
		.varPointer = &testEnumValue,
		.type = Type_Enumerator,
		.maxValue = 4,
		.enumList = enumExampleList
	},
	{
		.name = "boardname",
		.varPointer = (void * const)Global_BoardName,
		.type = Type_String,
		.isReadOnly = true
	},
	{
		.name = "boardmcu",
		.varPointer = (void * const)Global_BoardMCU,
		.type = Type_String,
		.isReadOnly = true
	},
	{
		.name = "bittest",
		.varPointer = &testUint32,
		.type = Type_Bits,
		.maxValue = 15,
		.minValue = 8
	}


	// XXX: ADD new global variable here
	/*
		const VarType_t type;

		void * const varPointer;
		const bool isReadOnly;

		const void * const maxPointer;
		const void * const minPointer;
		const char * const unit;

		const uint8_t stringMaxLength;

		const CommandSource_t sourceEnable;
	 */
};



///< GlobalVarList length
const VarID_t GlobalVarMaxCommandNum = sizeof(GlobalVarList)/sizeof(GlobalVarList[0]);


#endif	// #ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
