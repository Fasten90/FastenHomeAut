/*
 *		LinkedList.h
 *		Created on:		2017-08-01
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Linked list handler
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-08-01
 */

#ifndef COMMON_LINKEDLIST_H_
#define COMMON_LINKEDLIST_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define LINKEDLIST_STORE_LENGTH		(10U)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


typedef uint32_t	ListItem_Value_t;

///< ListItem for LinkedList
typedef struct ListItem
{
	ListItem_Value_t val;
	struct ListItem *prev;
	struct ListItem *next;
	bool isBusy;
} ListItem;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void LinkedList_UnitTest(void);



#endif /* COMMON_LINKEDLIST_H_ */
