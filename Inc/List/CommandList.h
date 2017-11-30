/*
 *		CommandList.h
 *		Created on:		2016-01-01
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		General commands's list
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016-09-28
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

#ifndef CONFIG_MODULE_COMMUNICATION_ENABLE
#ifndef CommProt_DebugUart
	#define CommProt_DebugUart			(0)
#endif
#endif



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const CmdH_Command_t CmdH_CommandList[];

extern const CmdH_CommandID_t CmdH_CommandNum;



#endif /* COMMANDLIST_H_ */
