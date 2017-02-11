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
	
#define COMMANDHANDLER_MAX_COMMAND_NUM				(uint8_t)( NUM_OF(CommandList))


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/




/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const CommandStruct CommandList[];

extern const CommandID_t CommandHandler_CommandNum;



/*------------------------------------------------------------------------------
 *		Global function declarations - Commands
 *----------------------------------------------------------------------------*/
 

CommandResult_t CommandFunction_cls(uint32_t argc, char** argv);
CommandResult_t CommandFunction_help(uint32_t argc, char** argv);

CommandResult_t CommandFunction_version(uint32_t argc, char** argv);
CommandResult_t CommandFunction_welcome(uint32_t argc, char** argv);

CommandResult_t CommandFunction_test(uint32_t argc, char** argv);

CommandResult_t CommandFunction_set(uint32_t argc, char** argv);
CommandResult_t CommandFunction_get(uint32_t argc, char** argv);
CommandResult_t CommandFunction_GlobalVariableHelp(uint32_t argc, char** argv);
CommandResult_t CommandFunction_GlobalVariableList(uint32_t argc, char** argv);
CommandResult_t CommandFunction_GlobalVariableValueList(uint32_t argc, char** argv);

CommandResult_t CommandFunction_reset(uint32_t argc, char** argv);
CommandResult_t CommandFunction_led(uint32_t argc, char** argv);

CommandResult_t CommandFunction_unittest(uint32_t argc, char** argv);
CommandResult_t CommandFunction_moduletest(uint32_t argc, char** argv);

CommandResult_t CommandFunction_dac(uint32_t argc, char** argv);
CommandResult_t CommandFunction_io(uint32_t argc, char** argv);
CommandResult_t CommandFunction_adc(uint32_t argc, char** argv);
CommandResult_t CommandFunction_adcread(uint32_t argc, char** argv);
CommandResult_t CommandFunction_PWM(uint32_t argc, char** argv);

CommandResult_t CommandFunction_ESP8266(uint32_t argc, char** argv);

CommandResult_t CommandFunction_RTC(uint32_t argc, char** argv);

CommandResult_t CommandFunction_raspberrypi(uint32_t argc, char** argv);

CommandResult_t CommandFunction_flashdel(uint32_t argc, char** argv);
CommandResult_t CommandFunction_flashread(uint32_t argc, char** argv);
CommandResult_t CommandFunction_flashwrite(uint32_t argc, char** argv);
CommandResult_t CommandFunction_temp(uint32_t argc, char** argv);

/*
CommandResult_t CommandFunction_go(uint32_t argc, char** argv);
CommandResult_t CommandFunction_mr(uint32_t argc, char** argv);
CommandResult_t CommandFunction_mw(uint32_t argc, char** argv);
*/



#endif /* COMMANDLIST_H_ */
