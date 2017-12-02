/*
 *		Network.h
 *		Created on:		2017-01-14
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Network general functions
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-01-14
 */

#ifndef NETWORK_H_
#define NETWORK_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< Ethernet - IPv4 struct
typedef struct
{
	uint8_t IP[4];
} Network_IP_t;


///< Ethernet - port typedef
typedef uint16_t Network_Port_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

uint8_t Network_PrintIp(char *str, Network_IP_t *ip);
uint8_t Network_PrintIpOnDebug(char *str, Network_IP_t *ip);

bool Network_ConvertIpAddressStringToIP(char *message, Network_IP_t *address);



#endif /* NETWORK_H_ */
