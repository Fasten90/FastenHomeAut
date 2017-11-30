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
#include "GlobVarList.h"
#include "Globals.h"
#include "ADC.h"



#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



///< Global variables list
const GlobVarH_VarRecord_t GlobVarH_VarList[] =
{
		{
			.name = "version",
			.varPointer = (void * const)&Global_Version,
			.type = GlobVarH_Type_String,
			.isReadOnly = true,
		},
		{
			.name = "devicename",
			.varPointer = Global_DeviceName,
			.type = GlobVarH_Type_String,
			.maxValue = 20,
			.description = "Device name",
		},
		{
			.name = "boardname",
			.varPointer = (void * const)Global_BoardName,
			.type = GlobVarH_Type_String,
			.isReadOnly = true,
		},
		{
			.name = "boardmcu",
			.varPointer = (void * const)Global_BoardMCU,
			.type = GlobVarH_Type_String,
			.isReadOnly = true,
		},
		{
			.name = "tick",
			.varPointer = (void * const)&GlobVarH_TemporaryValue,	// It is need for function pointer
			.getFunctionPointer = (GeneralFunctionPointer)HAL_GetTick,
			.type = GlobVarH_Type_Uint32,
			.isReadOnly = true,
#if GLOBVARH_DESCRIPTION_ENABLE == 1
			.description = "Tick counter",
#endif
		},
#ifdef CONFIG_MODULE_ADC_ENABLE
		{
			.name = "vsource",
			.varPointer = (void * const)&ADC_ConvertedValues[0],
			.type = GlobVarH_Type_Float,
			.isReadOnly = true,
#if GLOBVARH_UNIT_ENABLE == 1
			.unit = "V",
#endif
#if GLOBVARH_DESCRIPTION_ENABLE == 1
			.description = "Vsource",
#endif
		}
#endif


		// XXX: Add new global variable here
		/*
			const char * const name;				///< Name of global variable [string]
			const GlobVarH_Type_t type;				///< Type of global variable

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


///< GlobVarH_VarList length
const GlobVarH_ID_t GlobVarH_MaxCommandNum = NUM_OF(GlobVarH_VarList);


///< Global variable info struct
GlobVarH_VarListInfo_t GlobVarH_Variables =
{
	.items = &GlobVarH_VarList[0],
	.num = NUM_OF(GlobVarH_VarList),
	.traceVarEnabled = 0,
};



#endif	// #ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
