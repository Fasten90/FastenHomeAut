/*
 *    SmallApps.c
 *    Created on:   2019-03-22
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "SmallApps.h"
#include "DebugUart.h"
#include "StringHelper.h"


#ifdef CONFIG_FUNCTION_PERIODICAL_SENDING

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

static char PeriodicalSending_Message[50] = { 0 };



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

void Logic_SetPeriodicalMessageSendg(char * msg)
{
    StrCpyMax(PeriodicalSending_Message, msg, 50);
}



void Logic_PeriodicalSending(void)
{
    DebugUart_SendLine(PeriodicalSending_Message);
}

#endif /* CONFIG_FUNCTION_PERIODICAL_SENDING */
