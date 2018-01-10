/*
 *		Queue.c
 *		Created on:		2018-01-06
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2018-01-06
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "include.h"
#include "Queue.h"

#if (QUEUE_STATISTICS_ENABLED == 1)
#include "SysTime.h"
#endif

#ifdef MODULE_QUEUE_UNITTEST_ENABLE
#include "UnitTest.h"
#include "StringHelper.h"
#endif



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static bool Queue_AddQueueElement(void * pElement, void * pData, size_t dataSize, QueueDataType_t dataType, bool isUsed);
static void * Queue_SearchEmptySpace(QueueListInfo_t * queue, size_t dataSize);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

/*
 * 1. one full empty queue
 *
 * p
 * |
 * ˇ
 * +--------------+
 * |              |
 * +--------------+
 *
 * 2. insert a element
 *
 * pData   pEmpty
 * |       |
 * ˇ       ˇ
 * +-------+------+
 * |       |      |
 * +-------+------+
 *
 */

// TODO: Végiggondolni: queue element struktúra buffer is legyen, és RAM buffer is külön? Inkább egy buffer
// TODO: láncolt adat? Még nem érdemes megcsinálni. Előnye: const és ram adat is mehet egymás után



/**
 * \brief  Queue initialization
 */
void Queue_Init(QueueListInfo_t * queue)
{
	if (queue == NULL)
		return;

	// Buffersize should be larger than QueueElement_t
	ASSERT(queue->bufferSize > sizeof(QueueElement_t));
	// Buffer pointer should be not NULL
	ASSERT(queue->pBuffer != NULL);

	// Clear all queue
	memset(queue->pBuffer, 0x00, queue->bufferSize);

	queue->first = queue->pBuffer;
	queue->end = queue->pBuffer;
	queue->elementNum = 0;	// one empty buffer


	// Make one "empty" queue element
	size_t dataSize = queue->bufferSize - sizeof(QueueElement_t);
	void * pDataPointer = (void *)((uint32_t)queue->pBuffer + sizeof(QueueElement_t));
	QueueDataType_t dataType = QueuedataType_Ram;
	// Add full empty queue
	Queue_AddQueueElement(queue->first, pDataPointer, dataSize, dataType, QUEUE_NOTUSED);
	queue->first->nextQueueData = NULL;
	queue->first->prevQueueData = NULL;
	queue->elementNum++;
}



/**
 * \brief	Add / Set queue element
 * \note	Not check empty space and others. Be careful when you use it!
 */
static bool Queue_AddQueueElement(void * pElement, void * pData, size_t dataSize, QueueDataType_t dataType, bool isUsed)
{
	// Check parameters
	if ((pElement == NULL)/* || (dataSize == 0) || (pData == NULL)*/)
		return false;

	QueueElement_t * pQueueElement = (QueueElement_t *)pElement;
	pQueueElement->dataPointer = pData;
	pQueueElement->dataSize = dataSize;
	pQueueElement->dataType = dataType;
	pQueueElement->isUsed = isUsed;

#if (QUEUE_STATISTICS_ENABLED == 1)
	SysTime_GetDateTime(&pQueueElement->putTime);
	memset(&pQueueElement->getTime, 0, sizeof(pQueueElement->getTime));
#endif

#if (QUEUE_GUARD_ENABLED == 1)
	pQueueElement->headerGuardValue = QUEUE_GUARD_VALUE;
	pQueueElement->tailGuardValue = QUEUE_GUARD_VALUE;
#endif

	/*
	 * These datas are filled in parent function
	 * nextQueueData;
	 * prevQueueData
	 */
	pQueueElement->nextQueueData = NULL;
	pQueueElement->prevQueueData = NULL;

	return true;
}



/**
 * \brief	Get a QueueElement with enough empty space
 */
static void * Queue_SearchEmptySpace(QueueListInfo_t * queue, size_t dataSize)
{
	void * pQueueElement = NULL;

	// If bufferSize == 0, it is not RAM data (e.g. in ROM data), only queuelementsize need
	if (queue->end->isUsed == QUEUE_NOTUSED)
	{
		// Last element is not used, We hope it is good for use
		if (queue->end->dataSize >= dataSize)
		{
			pQueueElement = (void *)queue->end;
		}
	}

	return pQueueElement;
}



// TODO: Adding Queue pointer?
bool Queue_GetFirstElement(QueueListInfo_t * queue, void * buffer, size_t * dataSize, size_t bufferSize, bool needDelete);
bool Queue_GetFirstElement(QueueListInfo_t * queue, void * buffer, size_t * dataSize, size_t bufferSize, bool needDelete)
{
	(void)queue;
	(void)buffer;
	(void)dataSize;
	(void)bufferSize;
	(void)needDelete;

#if (QUEUE_GUARD_ENABLED == 1)
	// TODO: Check guard values
#endif

#if (QUEUE_STATISTICS_ENABLED == 1)
	// TODO: GetTime...
#endif

	return false;
}



// TODO: Implement these functions
bool Queue_GetLastElement(QueueListInfo_t * queue, void * pData, size_t * dataSize, size_t bufferSize, bool needDelete);
bool Queue_PutFirstElement(QueueListInfo_t * queue, void * pData, size_t dataSize);



// TODO: Enum tpye for return? WrongParam, NoSpace, ...
bool Queue_PutLastElement(QueueListInfo_t * queue, void * pData, size_t dataSize, QueueDataType_t dataTpye);
bool Queue_PutLastElement(QueueListInfo_t * queue, void * pData, size_t dataSize, QueueDataType_t dataType)
{
	// Check parameters
	if ((queue == NULL) || (pData == NULL))
		return false;

	bool isOk = false;

	// TODO: buffer in RAM or ROM

	// Check empty space (in const data, we not need data buffer)
	size_t requiredSize = (dataType == QueueDataType_Const) ? (0) : (dataSize);
	void * pQueueData = Queue_SearchEmptySpace(queue, requiredSize);
	if (pQueueData != NULL)
	{
		// Has enough space
		QueueElement_t * pElement = (QueueElement_t *)pQueueData;

		// Put data to last QueueElement
		QueueElement_t * pPrevOfLast = pElement->prevQueueData;

		// Copy data if need
		void * pNewElementData = pData;
		if (dataType == QueuedataType_Ram)
		{
			pNewElementData = (void *)((uint32_t)pElement + sizeof(QueueElement_t));
			memcpy(pNewElementData, pData, dataSize);
		}

		Queue_AddQueueElement(pElement, pNewElementData, dataSize, dataType, QUEUE_ISUSED);
		pElement->nextQueueData = NULL;
		pElement->prevQueueData = pPrevOfLast;
		//queue->elementNum++;	// Because element was overwrote, this code part is not true


		// TODO: This is amateur solution for the searching empty space
		void * pEndOfData = (void *)((uint32_t)pElement + sizeof(QueueElement_t)  + ((dataType == QueuedataType_Ram) ? (dataSize) : (0)));
		// Make Stack aligned safe
		pEndOfData = (void *)MEM_MAKE_ALIGNED_ADDRESS(pEndOfData);

		// Create empty queue element, or this is the last element
		// Make one "empty" queue element
		if (((uint32_t)pEndOfData + sizeof(QueueElement_t)) <= ((uint32_t)queue->pBuffer + queue->bufferSize))
		{
			// Has enough space to create last QueueElement

			// Make one "empty" queue element
			void * pEmptyQueueElement = pEndOfData;
			size_t emptyDataSize = ((uint32_t)queue->pBuffer + queue->bufferSize) - ((uint32_t)pEmptyQueueElement + sizeof(QueueElement_t));

			void * pEmptyDataPointer = NULL;
			if (emptyDataSize > 0)
				pEmptyDataPointer = (void *)((uint32_t)pEmptyQueueElement + sizeof(QueueElement_t));

			QueueDataType_t emptyDataType = QueuedataType_Ram;
			// Add empty queue
			Queue_AddQueueElement(pEmptyQueueElement, pEmptyDataPointer, emptyDataSize, emptyDataType, QUEUE_NOTUSED);
			((QueueElement_t *)pEmptyQueueElement)->nextQueueData = NULL;
			((QueueElement_t *)pEmptyQueueElement)->prevQueueData = pElement;
			queue->elementNum++;

			pElement->nextQueueData = pEmptyQueueElement;

			queue->end = pEmptyQueueElement;
		}
		else
		{
			// There is no last empty queue element
			queue->end = pElement;
		}

		isOk = true;
	}
	// else: There is no enough empty space

	return isOk;
}



// TODO: Implement it
void Queue_Defragmentation(void);



#ifdef MODULE_QUEUE_UNITTEST_ENABLE
void Queue_UnitTest(void)
{
	#define TEST_BUFFER_SIZE		(100U)
	uint8_t testBuffer[TEST_BUFFER_SIZE];
	QueueListInfo_t testQueue =
	{
		.pBuffer = testBuffer,
		.bufferSize = TEST_BUFFER_SIZE,
		.elementNum = 0,
		.first = NULL,
		.end = NULL
	};


	UnitTest_Start("Queue", __FILE__);


	// Initialize the queue
	Queue_Init(&testQueue);

	// One full empty queue element should be created
	UNITTEST_ASSERT(testQueue.elementNum == 1, "Init elementNum error");
	UNITTEST_ASSERT(testQueue.first == testQueue.end, "Init first-end pointer error");
	UNITTEST_ASSERT(testQueue.first == (QueueElement_t *)testBuffer, "Init pointer error");


	// Add an element
	UNITTEST_ASSERT(Queue_PutLastElement(&testQueue, "12345", 5, QueuedataType_Ram),
			"PutLastElement");

	// Empty + new added queue element
	UNITTEST_ASSERT((testQueue.elementNum == 2),
			"PutLastElement ElementNum error");
	UNITTEST_ASSERT((testQueue.first != testQueue.end),
			"PutLastElement First-End pointer error");
	UNITTEST_ASSERT((testQueue.first->dataPointer == (testBuffer + sizeof(QueueElement_t))),
			"PutLastElement dataPointer error");
	UNITTEST_ASSERT(!StrCmp((char *)(testQueue.first->dataPointer), "12345"),
			"PutLastElement data not contained");
	UNITTEST_ASSERT((testQueue.first->dataSize == 5),
			"PutLastElement data size error");
	UNITTEST_ASSERT((testQueue.end == (QueueElement_t *)(MEM_MAKE_ALIGNED_ADDRESS((testBuffer + sizeof(QueueElement_t) + 5)))),
			"PutLastElement end pointer error");


	UnitTest_End();
}
#endif


