/*
 *		GlobalVariables.c
 *
 *		Created on:		2016.09.07
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Global Variable Handler: Handle set-get variable from uart or other
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2017.07.10
 */

#include "GlobalVarHandler.h"
#include "GlobalVariables.h"

#include "Globals.h"
#include "ADC.h"



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
uint32_t testBits = 0;


///< enumExample string list
const char * const testEnumExampleList[] =
{
	"exampleenumstring0",
	"exampleenumstring1",
	"exampleenumstring2",
	"exampleenumstring3",
	NULL
};
#endif



///< Global variables list
const GlobalVarCommand_t GlobalVarList[] =
{
#ifdef MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE
	{
		.name = "testbool",
		.varPointer = &testBool,
		.type = Type_Bool
	},
	{
		.name = "testuint8",
		.varPointer = &testUint8,
		.type = Type_Uint8,
		.unit = "cm",
		.description = "test uint8 variable"
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
		.name = "testcannotaccess",
		.varPointer = &testCannotAccess,
		.type = Type_Bool,
		.sourceEnable = CommProt_Disable
	},
	{
		.name = "testfloat",
		.varPointer = &testFloat,
		.type = Type_Float,
	},
	{
		.name = "testenum",
		.varPointer = &testEnumValue,
		.type = Type_Enumerator,
		.maxValue = 4,
		.enumList = testEnumExampleList
	},
	{
		.name = "testbit",
		.varPointer = &testBits,
		.type = Type_Bits,
		.maxValue = 15,
		.minValue = 8
	},
#endif	// #ifdef MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE
	// Normal Variables:
	{
		.name = "version",
		.varPointer = (void * const)Global_Version,
		.type = Type_String,
		.isReadOnly = true
	},
	{
		.name = "devicename",
		.varPointer = Global_DeviceName,
		.type = Type_String,
		.maxValue = 20,
		.description = "Device name"
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
		.name = "tick",
		.varPointer = (void * const)&GlobarVarHandler_TemporaryValue,
		.getFunctionPointer = (GeneralFunctionPointer)HAL_GetTick,
		.type = Type_Uint32,
		.isReadOnly = true,
		.isFunction = true,
		.description = "Tick counter"
	},
#ifdef CONFIG_MODULE_ADC_ENABLE
	{
		.name = "vsource",
		.varPointer = (void * const)&ADC_ConvertedValues[0],
		.type = Type_Float,
		.isReadOnly = true,
		.unit = "V",
		.description = "Vsource"
	}
#endif


	// XXX: Add new global variable here
	/*

	const char * const name;				///< Name of global variable [string]
	const VarType_t type;					///< Type of global variable

	void * const varPointer;				///< Pointer of variable
	const bool isReadOnly;					///< Is read only?

	const bool isFunction;					///< It is function?
	const GeneralFunctionPointer getFunctionPointer;	///< Function
	const GeneralFunctionPointer setFunctionPointer;	///< Function

	const uint32_t maxValue;				///< Max value
	const uint32_t minValue;				///< Min value

	const CommProtocol_t sourceEnable;		///< Enabled sources (for set-get)

	const bool isHex;						///< Set-get in hexadecimal?

	const char * const * enumList;			///< Enum list, if it is enumerator

	const char * const unit;				///< units [string], example: [cm]

	const char * const description;			///< descriptions of global variable [string]
	 */
};



///< GlobalVarList length
const VarID_t GlobalVar_MaxCommandNum = sizeof(GlobalVarList)/sizeof(GlobalVarList[0]);



#endif	// #ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
