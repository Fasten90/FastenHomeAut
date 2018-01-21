/*
 *		ConvertTable.h
 *		Created on:		2017-09-17
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017-09-17
 */

#ifndef CONVERTTABLE_H_
#define CONVERTTABLE_H_




/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< Convert table record (ADC - converted value pair)
typedef struct
{
	uint32_t adcValue;						///< ADC value
	float convertedValue;					///< Converted value
} ConvertTableRecord_t;


///< Convert table record list (records + num)
typedef struct
{
	const ConvertTableRecord_t *recordList;	///< List (ADC-Converted value)
	const uint16_t size;					///< Size
} ConverTable_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

bool ConvertTable_Init(const ConverTable_t * table);
float ConvertTable_GetValue(const ConverTable_t * table, uint32_t value);
void ConvertTable_UnitTest(void);



#endif /* CONVERTTABLE_H_ */
