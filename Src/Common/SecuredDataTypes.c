/*
 *		SecuredDataTypes.c
 *		Created on:		2017-08-24
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Secure data types and handlers
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-08-24
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "include.h"
#include "SecuredDataTypes.h"


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
 * \brief	Get secured bool value
 */
bool Get_SecuredBool(SecuredBool_t *data)
{
	// Check pointer
	if (data == NULL)
		return false;

	// Check validity
	if (data->isEn == ~data->negIsEn)
	{
		// Valid
		return data->isEn;
	}
	else
	{
		return false;
	}
}



/**
 * \brief	Get secured bool value
 */
void Set_SecuredBool(SecuredBool_t *data, bool value)
{
	// Check pointer
	if (data == NULL)
		return;

	// Set value
	data->isEn = value;
	data->negIsEn = ~value;
}



/**
 * \brief	Get secured bool value
 */
uint32_t Get_SecuredUint32(SecuredUint32_t *data)
{
	// Check pointer
	if (data == NULL)
		return false;

	// Check validity
	if (data->data == ~data->negData)
	{
		// Valid
		return data->data;
	}
	else
	{
		return 0;
	}
}



/**
 * \brief	Get secured bool value
 */
void Set_SecuredUint32(SecuredUint32_t *data, uint32_t value)
{
	// Check pointer
	if (data == NULL)
		return;

	// Set value
	data->data = value;
	data->negData = ~value;
}



// TODO: Write UnitTest
