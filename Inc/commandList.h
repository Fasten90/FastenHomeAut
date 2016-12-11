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
	
#define MONITOR_MAX_COMMAND_NUM				(uint8_t)( NUM_OF(CommandList))


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/




/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const CommandStruct CommandList[];

extern const CommandID_t MONITOR_CommandNum;



/*------------------------------------------------------------------------------
*  Global function declarations
*----------------------------------------------------------------------------*/
 



// COMMANDS
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

CommandResult_t CommandFunction_flashdel(uint32_t argc, char** argv);
CommandResult_t CommandFunction_flashread(uint32_t argc, char** argv);
CommandResult_t CommandFunction_flashwrite(uint32_t argc, char** argv);
CommandResult_t CommandFunction_temp(uint32_t argc, char** argv);

CommandResult_t CommandFunction_raspberrypi(uint32_t argc, char** argv);

CommandResult_t CommandFunction_unittest(uint32_t argc, char** argv);
CommandResult_t CommandFunction_moduletest(uint32_t argc, char** argv);

CommandResult_t CommandFunction_dac(uint32_t argc, char** argv);
CommandResult_t CommandFunction_io(uint32_t argc, char** argv);
CommandResult_t CommandFunction_adc(uint32_t argc, char** argv);

/*
CommandResult_t CommandFunction_buzzer	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_accelerometer	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_gyroscope	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_remotecontrol	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_proximity	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_log 	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_exit	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_read	( uint32_t argc, char** argv );

CommandResult_t CommandFunction_start	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_stop	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_dl		( uint32_t argc, char** argv );
CommandResult_t CommandFunction_go		( uint32_t argc, char** argv );

CommandResult_t CommandFunction_mr		( uint32_t argc, char** argv );
CommandResult_t CommandFunction_mw		( uint32_t argc, char** argv );
CommandResult_t CommandFunction_btn	( uint32_t argc, char** argv );

CommandResult_t CommandFunction_lcd	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_stop	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_romr	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_romw	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_romsr	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_romwe	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_rominit	( uint32_t argc, char** argv );

CommandResult_t CommandFunction_rfm12b	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_keypad	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_standby	( uint32_t argc, char** argv );
CommandResult_t CommandFunction_rtc	( uint32_t argc, char** argv );

CommandResult_t CommandFunction_ESP8266	( uint32_t argc, char** argv );
*/



#endif /* COMMANDLIST_H_ */
