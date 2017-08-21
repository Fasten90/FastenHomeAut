/*
 *		LinkedList.c
 *
 *		Created on:		2017. aug. 1.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. aug. 1.
 */




/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "MEM.h"
#include "DebugUart.h"
#include "UnitTest.h"
#include "LinkedList.h"




/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

ListItem MyList_StoreBuffer[LINKEDLIST_STORE_LENGTH];

ListItem *MyList_first;
ListItem *MyList_last;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void LinkedList_PrintListItem(ListItem * item);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Initialize LinkedList
 */
void LinkedList_Init(void)
{
	memset(MyList_StoreBuffer, 0, (size_t)(sizeof(MyList_StoreBuffer)/sizeof(MyList_StoreBuffer[0])));

	MyList_first = NULL;
	MyList_last = NULL;
}



void LinkedList_AddFirstItem(ListItem_Value_t value)
{
	MyList_StoreBuffer[0].val = value;
	MyList_StoreBuffer[0].next = NULL;
	MyList_StoreBuffer[0].prev = NULL;
	MyList_StoreBuffer[0].isBusy = true;

	MyList_first = &MyList_StoreBuffer[0];
	MyList_last = &MyList_StoreBuffer[0];
}



bool LinkedList_AddItem(ListItem_Value_t value)
{
	bool result = false;

	if (MyList_first == NULL)
	{
		// There is no list
		LinkedList_AddFirstItem(value);
		result = true;
	}
	else
	{
		// There is list
		uint8_t i;
		for (i = 0; i < LINKEDLIST_STORE_LENGTH; i++)
		{
			if (MyList_StoreBuffer[i].isBusy == false)
			{
				// Empty
				MyList_StoreBuffer[i].val = value;
				MyList_StoreBuffer[i].next = NULL;
				MyList_StoreBuffer[i].isBusy = true;

				MyList_StoreBuffer[i].prev = MyList_last;
				MyList_last->next = &MyList_StoreBuffer[i];
				MyList_last = &MyList_StoreBuffer[i];

				result = true;
				// Added item
				break;
			}
		}
	}

	return result;
}



// TODO: Implement Add item after an item (pointer)

// TODO: Implement Add item before an item (pointer)



void LinkedList_RemoveItem(ListItem *item)
{
	if (item == NULL)
	{
		return;
	}

	if (item->next)
	{
		if (item->prev)
		{
			// There is item before and after this item
			item->next->prev = item->prev;
			item->prev->next = item->next;
		}
		else
		{
			// This is the first
			item->next->prev = NULL;
			MyList_first = item->next;
		}
	}
	else
	{
		if (item->prev)
		{
			// This is the last
			item->prev->next = NULL;
			MyList_last = item->prev;
		}
		else
		{
			// This item is alone
			MyList_first = NULL;
			MyList_last = NULL;
		}
	}


	item->isBusy = false;
	item->next = NULL;
	item->prev = NULL;
}



void LinkedList_TravelList(void)
{
	ListItem *item = MyList_first;

	while (item)
	{
		// Print
		LinkedList_PrintListItem(item);
		// Step to next
		item = item->next;
	}
}



static void LinkedList_PrintListItem(ListItem * item)
{
	if (item != NULL)
	{
		uprintf("Item: %u\r\n", item->val);
	}
}



#ifdef MODULE_LINKEDLIST_UNITTEST_ENABLE
void LinkedList_UnitTest(void)
{

	UnitTest_Start("LinkedList", __FILE__);

	LinkedList_Init();


	LinkedList_AddFirstItem(1);

	UNITTEST_ASSERT(MyList_first == MyList_last, "LinkedList pointer errors");
	UNITTEST_ASSERT(MyList_first->val == 1, "First item value error");

	// Add new item
	LinkedList_AddItem(2);
	UNITTEST_ASSERT(MyList_first != MyList_last, "LinkedList pointer error");
	UNITTEST_ASSERT(MyList_last->val == 2, "AddItem error");
	UNITTEST_ASSERT(MyList_first->next == MyList_last, "AddItem next pointer error");
	UNITTEST_ASSERT(MyList_last->prev == MyList_first, "AddItem prev pointer error");
	UNITTEST_ASSERT(MyList_first->prev == NULL, "AddItem prev pointer error");
	UNITTEST_ASSERT(MyList_last->next == NULL, "AddItem next pointer error");

	// For delete middle item...
	ListItem *middleItem = MyList_last;


	// Add 2. new item
	LinkedList_AddItem(3);
	UNITTEST_ASSERT(MyList_first != MyList_last, "LinkedList pointer error");
	UNITTEST_ASSERT(MyList_last->val == 3, "AddItem error");
	UNITTEST_ASSERT(MyList_first->next->next == MyList_last, "AddItem next pointer error");
	UNITTEST_ASSERT(MyList_last->prev->prev == MyList_first, "AddItem prev pointer error");
	UNITTEST_ASSERT(MyList_first->prev == NULL, "AddItem prev pointer error");
	UNITTEST_ASSERT(MyList_last->next == NULL, "AddItem next pointer error");


	// Test TODO: Need user checking
	LinkedList_TravelList();


	// Delete middle item
	LinkedList_RemoveItem(middleItem);
	UNITTEST_ASSERT(MyList_first != MyList_last, "LinkedList pointer error");
	UNITTEST_ASSERT(MyList_first->next == MyList_last, "AddItem next pointer error");
	UNITTEST_ASSERT(MyList_last->prev == MyList_first, "AddItem prev pointer error");
	UNITTEST_ASSERT(MyList_first->prev == NULL, "AddItem prev pointer error");
	UNITTEST_ASSERT(MyList_last->next == NULL, "AddItem next pointer error");


	// Delete 2. item (will be only one)
	LinkedList_RemoveItem(MyList_last);
	UNITTEST_ASSERT(MyList_first == MyList_last, "LinkedList pointer error");
	UNITTEST_ASSERT(MyList_first->next == NULL, "AddItem next pointer error");
	UNITTEST_ASSERT(MyList_first->prev == NULL, "AddItem prev pointer error");


	// Delete last item
	LinkedList_RemoveItem(MyList_last);
	UNITTEST_ASSERT(MyList_first == NULL, "LinkedList pointer error");
	UNITTEST_ASSERT(MyList_last == NULL, "LinkedList pointer error");


	// Finish
	UnitTest_End();

}
#endif
