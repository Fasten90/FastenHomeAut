/*
 *    LinkedList.h
 *    Created on:   2017-08-01
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Linked list handler
 *    Target:       STM32Fx
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define LINKEDLIST_STORE_LENGTH        (10U)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


typedef uint32_t    ListItem_Value_t;

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

uint32_t LinkedList_UnitTest(void);



#endif /* LINKEDLIST_H_ */
