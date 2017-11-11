/*
 *		GlobalVariables.c
 *		Created on:		2016-09-07
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Global Variable Handler: Handle set-get variable from uart or other
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2017-07-10
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "GlobalVarHandler.h"
#include "Globals.h"
#include "ADC.h"
#include "GlobalVariables.h"



#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


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
const GlobVarH_VarRecord_t GlobalVarList[] =
{
#ifdef MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE
		{
			.name = "testbool",
			.varPointer = &testBool,
			.type = GlobVarH_Type_Bool
		},
		{
			.name = "testuint8",
			.varPointer = &testUint8,
			.type = GlobVarH_Type_Uint8,
			.unit = "cm",
			.description = "test uint8 variable"
		},
		{
			.name = "testint16",
			.varPointer = &testInt16,
			.type = GlobVarH_Type_Int16,
		},
		{
			.name = "testint32",
			.varPointer = &testInt32,
			.type = GlobVarH_Type_Int32,
		},
		{
			.name = "testuint32",
			.varPointer = &testUint32,
			.type = GlobVarH_Type_Uint32,
			.isHex = true,
		},
		{
			.name = "testcannotaccess",
			.varPointer = &testCannotAccess,
			.type = GlobVarH_Type_Bool,
			.sourceEnable = CommProtBit_Disable
		},
		{
			.name = "testfloat",
			.varPointer = &testFloat,
			.type = GlobVarH_Type_Float,
		},
		{
			.name = "testenum",
			.varPointer = &testEnumValue,
			.type = GlobVarH_Type_Enumerator,
			.maxValue = 4,
			.enumList = testEnumExampleList
		},
		{
			.name = "testbit",
			.varPointer = &testBits,
			.type = GlobVarH_Type_Bits,
			.maxValue = 15,
			.minValue = 8
		},
#endif	// #ifdef MODULE_GLOBALVARHANDLER_UNITTEST_ENABLE
		{
			.name = "version",
			.varPointer = (void * const)&Global_Version,
			.type = GlobVarH_Type_String,
			.isReadOnly = true
		},
		{
			.name = "devicename",
			.varPointer = Global_DeviceName,
			.type = GlobVarH_Type_String,
			.maxValue = 20,
			.description = "Device name"
		},
		{
			.name = "boardname",
			.varPointer = (void * const)Global_BoardName,
			.type = GlobVarH_Type_String,
			.isReadOnly = true
		},
		{
			.name = "boardmcu",
			.varPointer = (void * const)Global_BoardMCU,
			.type = GlobVarH_Type_String,
			.isReadOnly = true
		},
		{
			.name = "tick",
			.varPointer = (void * const)&GlobVarH_TemporaryValue,	// It is need for function pointer
			.getFunctionPointer = (GeneralFunctionPointer)HAL_GetTick,
			.type = GlobVarH_Type_Uint32,
			.isReadOnly = true,
			.isFunction = true,
			.description = "Tick counter"
		},
#ifdef CONFIG_MODULE_ADC_ENABLE
		{
			.name = "vsource",
			.varPointer = (void * const)&ADC_ConvertedValues[0],
			.type = GlobVarH_Type_Float,
			.isReadOnly = true,
			.unit = "V",
			.description = "Vsource"
		}
#endif


		// XXX: Add new global variable here
		/*
			const char * const name;				///< Name of global variable [string]
			const VarGlobVarH_Type_t type;					///< Type of global variable

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
const GlobVarH_ID_t GlobalVar_MaxCommandNum = sizeof(GlobalVarList)/sizeof(GlobalVarList[0]);



#endif	// #ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
