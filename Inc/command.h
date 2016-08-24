/*******************************************************************************
 * Purpose:
 ******************************************************************************/

#ifndef COMMAND_H_
#define COMMAND_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"
//#include "monitor.h"

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/




#define NUM_OF(x) (sizeof(x)/sizeof(x[0]))
	
#define MONITOR_MAX_COMMAND_NUM				(uint8_t)( NUM_OF(CommandList))



/*------------------------------------------------------------------------------
*  Global function declarations
*----------------------------------------------------------------------------*/
 
extern uint8_t COMMAND_GetCommandNum( void );



// COMMANDS

uint32_t CommandFunction_cls	( uint32_t argc, char** argv );
uint32_t CommandFunction_help	( uint32_t argc, char** argv );


uint32_t CommandFunction_test	( uint32_t argc, char** argv );
uint32_t CommandFunction_start	( uint32_t argc, char** argv );
uint32_t CommandFunction_stop	( uint32_t argc, char** argv );
uint32_t CommandFunction_set	( uint32_t argc, char** argv );
uint32_t CommandFunction_get	( uint32_t argc, char** argv );


uint32_t CommandFunction_reset	( uint32_t argc, char** argv );
uint32_t CommandFunction_led	( uint32_t argc, char** argv );
uint32_t CommandFunction_buzzer	( uint32_t argc, char** argv );
uint32_t CommandFunction_accelerometer	( uint32_t argc, char** argv );
uint32_t CommandFunction_gyroscope	( uint32_t argc, char** argv );
uint32_t CommandFunction_remotecontrol	( uint32_t argc, char** argv );
uint32_t CommandFunction_proximity	( uint32_t argc, char** argv );
uint32_t CommandFunction_log 	( uint32_t argc, char** argv );
uint32_t CommandFunction_exit	( uint32_t argc, char** argv );
uint32_t CommandFunction_read	( uint32_t argc, char** argv );


uint32_t CommandFunction_dl		( uint32_t argc, char** argv );
uint32_t CommandFunction_go		( uint32_t argc, char** argv );



uint32_t CommandFunction_mr		( uint32_t argc, char** argv );
uint32_t CommandFunction_mw		( uint32_t argc, char** argv );
uint32_t CommandFunction_btn	( uint32_t argc, char** argv );

uint32_t CommandFunction_tizenegy	( uint32_t argc, char** argv );
uint32_t CommandFunction_temp	( uint32_t argc, char** argv );
uint32_t CommandFunction_lcd	( uint32_t argc, char** argv );
uint32_t CommandFunction_stop	( uint32_t argc, char** argv );
uint32_t CommandFunction_romr	( uint32_t argc, char** argv );
uint32_t CommandFunction_romw	( uint32_t argc, char** argv );
uint32_t CommandFunction_romsr	( uint32_t argc, char** argv );
uint32_t CommandFunction_romwe	( uint32_t argc, char** argv );
uint32_t CommandFunction_rominit	( uint32_t argc, char** argv );

uint32_t CommandFunction_rfm12b	( uint32_t argc, char** argv );
uint32_t CommandFunction_keypad	( uint32_t argc, char** argv );
uint32_t CommandFunction_standby	( uint32_t argc, char** argv );
uint32_t CommandFunction_rtc	( uint32_t argc, char** argv );


uint32_t CommandFunction_flashdel	( uint32_t argc, char** argv );
uint32_t CommandFunction_flashread	( uint32_t argc, char** argv );
uint32_t CommandFunction_flashwrite	( uint32_t argc, char** argv );


uint32_t CommandFunction_ESP8266	( uint32_t argc, char** argv );
uint32_t CommandFunction_raspberrypi 	( uint32_t argc, char** argv );
	
	
/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/
 
 
// Command Function
typedef uint32_t ( *FunctionPointer )( uint32_t argc, char** argv );


// Command struct
typedef struct {
	char * name;
	FunctionPointer *CommandFunctionPointer;
	char * description;
} CommandStruct;





/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const CommandStruct CommandList[];
 

//extern CommandStruct CommandList[];
//extern CommandStruct CommandList[MONITOR_MAX_COMMAND_NUM];



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local function declarations
 *----------------------------------------------------------------------------*/






#endif /* TEMPLATE_H_ */
