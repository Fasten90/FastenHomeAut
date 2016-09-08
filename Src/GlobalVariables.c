/*
 * GlobalVariables.c
 *
 *  Created on: Sep 7, 2016
 *      Author: Vizi Gábor
 */


#include "GlobalVarHandler.h"
#include "GlobalVariables.h"




/// Global variables

// Example variables
uint8_t testVar = 17;
char deviceName[20] = "Discovery";
uint8_t distance = 0;

GlobalVarCommand_t GlobalVarList[] =
{
		{
			.name = "testvar",
			.varPointer = &testVar,
			.type = Type_Uint8,
			.maxValue = 20,
			.description = "Test integer"
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


// TODO: Megvizsgálni a tagok számát
/*
#if ( (sizeof(GlobalVarList)/sizeof(GlobalVarList[0])) > 255 )
#error "uint8_t globalVarSize"
#endif
*/


const uint8_t GlobalVarMaxCommandNum = sizeof(GlobalVarList)/sizeof(GlobalVarList[0]);


