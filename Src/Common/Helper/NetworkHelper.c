/*
 *    Network.c
 *    Created on:   2017-01-14
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Network general functions
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "DebugUart.h"
#include "StringHelper.h"
#include "NetworkHelper.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief       Print IP to string
 * @return      string length
 *              !! Provide fix length buffer !!
 *              "123.123.123.123" --> 15+1 = 16
 */
size_t Network_PrintIp(char *str, Network_IP_t *ip)
{
    return usnprintf(str, 16,
            "%d.%d.%d.%d",
            ip->IP[0],
            ip->IP[1],
            ip->IP[2],
            ip->IP[3]);
}



/**
 * @brief       Print IP to string
 * @return      string length
 */
size_t Network_PrintIpOnDebug(char *str, Network_IP_t *ip)
{
    return uprintf("%s IP address: %d.%d.%d.%d\r\n",
            str,
            ip->IP[0],
            ip->IP[1],
            ip->IP[2],
            ip->IP[3]);
}



/**
 * @brief       Convert string IP address to Network_IP_t
 * @retval      true, if ok
 * @retval      false, if false
 */
bool Network_ConvertIpAddressStringToIP(char *message, Network_IP_t *address)
{
    /* Process String like "192.168.0.1" */

    char *separated[4];

    if (STRING_Splitter(message, ".",  separated, 4) == 4)
    {
        /* Successful separated */
        /* Convert IP string to number */
        uint8_t i;
        for (i = 0; i < 4; i++)
        {
            uint32_t convertValue;
            if (StringToUnsignedDecimalNum(separated[i], &convertValue))
            {
                /* Successful convert to number */
                if (convertValue <= 255)
                {
                    address->IP[i] = (uint8_t)convertValue;
                }
                else
                {
                    /* Error, Too large value */
                    return false;
                }
            }
            else
            {
                /* Failed convert to number */
                return false;
            }
        }

        /* Successful convert */
        return true;
    }
    else
    {
        /* Failed separate string */
        return false;
    }
}


