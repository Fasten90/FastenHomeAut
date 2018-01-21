/*
 *		SysManager.h
 *
 *		Created on:		2015
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 8.
 */

#ifndef SYSMANAGER_H_
#define SYSMANAGER_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "homeautmessage.h"
#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/
#define HOMEAUTMESSAGE_DEVICE_CONNECT_MAX_NUM		( 5 )

#define HOMEAUTMESSAGE_RECEIVE_QUEUE_LENGTH			( 5 )
#define HOMEAUTMESSAGE_RECEIVE_ITEM_SIZE			( sizeof(HOMEAUTMESSAGE_InformationType) )



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< HomeAut Device state type (connection)
typedef enum
{
	State_Unknown = 0,
	State_Connected,
	State_Disconnected,
	State_Server,
	State_Last_NotUsed_NumOfHomeAutDeviceStateType
} HomeAutDeviceStateType;


///< HomeAut Device connection type: IP and status
typedef struct
{
	uint8_t IP_addressDec;				///< IP address
	HomeAutDeviceStateType State;		///< State
} HomeAutDeviceDataType;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern HomeAutDeviceDataType HomeAutDevices[];
extern volatile uint8_t SYSMANAGER_ReceivedAnValidHomeAutMessageAndNeedProcess;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void SysManager_init(void);
void SYSMANAGER_Task(void);



#endif /* SYSMANAGER_H_ */
