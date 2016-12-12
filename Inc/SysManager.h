/*******************************************************************************
 * Purpose:
 ******************************************************************************/

#ifndef SYSMANAGER_H_
#define SYSMANAGER_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "homeautmessage.h"
#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/
#define HOMEAUTMESSAGE_DEVICE_CONNECT_MAX_NUM		( 5 )

#define HOMEAUTMESSAGE_RECEIVE_QUEUE_LENGTH			( 5 )
#define HOMEAUTMESSAGE_RECEIVE_ITEM_SIZE			( sizeof(HOMEAUTMESSAGE_InformationType) )


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

/// HomeAut Device state type (connection)
typedef enum
{
	State_Unknown = 0,
	State_Connected,
	State_Disconnected,
	State_Server,
	State_Last_NotUsed_NumOfHomeAutDeviceStateType
} HomeAutDeviceStateType;


/// HomeAut Device connection type: IP and status
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
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/
void SYSMANAGER_Task ( void );




#endif /* SYSMANAGER_H_ */
