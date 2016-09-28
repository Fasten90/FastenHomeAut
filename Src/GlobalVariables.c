/*
 * GlobalVariables.c
 *
 *  Created on: Sep 7, 2016
 *      Author: Vizi Gábor
 */


#include "GlobalVarHandler.h"
#include "GlobalVariables.h"

#include "Globals.h"


/// Global variables

// Example variables
uint8_t testVar = 17;
char deviceName[20] = "Discovery";
uint8_t distance = 0;
bool myTestBool = false;
int16_t testInt16;
uint32_t testUint32;
int32_t testInt32;
bool cannotAccess = false;
float floatTest = 12.34f;
uint8_t enumValue = 2;

const char *enumExampleList[] =
{
	"example1enumstring",
	"example2enumstring",
	"example3enumstring",
	"example4enumstring",
	NULL
};

const GlobalVarCommand_t GlobalVarList[] =
{
		{
			.name = "testvar",
			.varPointer = &testVar,
			.type = Type_Uint8,
			.maxValue = 20,
			.description = "Test integer",
			.sourceEnable = Source_DebugSerial | Source_Ethernet
		},
		{
			.name = "devicename",
			.varPointer = deviceName,
			.type = Type_String,
			.maxValue = 20,
			.description = "Device name"
		},
		{
			.name = "distance",
			.varPointer = &distance,
			.type = Type_Uint8,
			.unit = "cm",
			.description = "distance from hell"
		},
		{
			.name = "bool",
			.varPointer = &myTestBool,
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
			.varPointer = &cannotAccess,
			.type = Type_Bool,
			.sourceEnable = Source_Disable
		},
		{
			.name = "version",
			.varPointer = &Global_Version,
			.type = Type_String,
			.isReadOnly = true
		},
		{
			.name = "floattest",
			.varPointer = &floatTest,
			.type = Type_Float,
		},
		{
			.name = "enumtest",
			.varPointer = &enumValue,
			.type = Type_Enumerator,
			.maxValue = 4,
			.enumList = enumExampleList
		}


		// ADD new global variable here
		/*
			const VarType_t type;

			void const *varPointer;
			const bool isReadOnly;

			const void const *maxPointer;
			const void const *minPointer;
			const char const *unit;

			const uint8_t stringMaxLength;

			const CommandSource_t sourceEnable;
		 */
};


const VarID_t GlobalVarMaxCommandNum = sizeof(GlobalVarList)/sizeof(GlobalVarList[0]);


