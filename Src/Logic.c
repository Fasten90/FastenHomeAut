/*
 *		Logic.c
 *
 *		Created on:		2017. máj. 23.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. máj. 23.
 */




/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "Logic.h"


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

static uint8_t Logic_SystemTimeConfigState = 0;


/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void Logic_SystemTimeStepConfig(void);
static void Logic_SystemTimeStepValue(void);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * \brief	Button event handler
 */
void Logic_ButtonEventHandler(ButtonType_t button, ButtonPressType_t type)
{
#if BUTTON_NUM == 1

	// One button mode
	if (button == PressedButton_User)
	{
		if (type == ButtonPress_Long)
		{
			DebugUart_SendLine("Button pressed a long time");
			Logic_SystemTimeStepConfig();
		}
		else if (type == ButtonPress_Short)
		{
			DebugUart_SendLine("Button pressed a short time");
			Logic_SystemTimeStepValue();
		}
	}

#elif BUTTOn_NUM > 1
#warning "Implement this!"
#endif
}



static void Logic_SystemTimeStepConfig(void)
{
	if (Logic_SystemTimeConfigState < 2)
		Logic_SystemTimeConfigState++;
	else
		Logic_SystemTimeConfigState = 0;
}



void Logic_SystemTimeStepValue(void)
{
	switch (Logic_SystemTimeConfigState)
	{
		case 0:
			// Unknown
			break;

		case 1:
			// Hour
			DateTime_Steps(&DateTime_SystemTime, 60*60);
			TaskHandler_RequestTaskScheduling(Task_SystemTime);
			break;

		case 2:
			// Minute
			DateTime_Steps(&DateTime_SystemTime, 60);
			TaskHandler_RequestTaskScheduling(Task_SystemTime);
			break;

		default:
			break;
	}

}

