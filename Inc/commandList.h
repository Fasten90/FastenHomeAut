/*
 *		command.h
 *
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Command list
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef COMMANDLIST_H_
#define COMMANDLIST_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "CommandHandler.h"

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


#define NUM_OF(x) (sizeof(x)/sizeof(x[0]))
	
#define COMMANDHANDLER_MAX_COMMAND_NUM				(uint8_t)(NUM_OF(CommandList))


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/




/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const CommandStruct CommandList[];

extern const CommandID_t CommandHandler_CommandNum;



#endif /* COMMANDLIST_H_ */
