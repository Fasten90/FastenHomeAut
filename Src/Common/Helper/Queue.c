/*
 *    Queue.c
 *    Created on:   2018-01-06
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Dynamic queue for giving/sending.
 *                  - It is useful for unknown message sending, if length range is large (e.g. message length: from 1 to 100)
 *                  - Header structure has a large size (~44 byte, if STATISTICS and GUARD configs are turned on),
 *                    therefore this module is not recommended to use if only send small packages
 *                    (small size array-array is better: ARRAY[MSG_MAX_LENGTH][MSG_MAX_COUNT])
 *                  - Now it can be used for FIFO queue/list
 *                  - Will extended to LIFO queue/list using, but now it is not available
 *    Target:       STM32Fx
 */


 /*
 *
 *
 * ---------------------------------------
 *            How does it work?
 * ---------------------------------------
 *
 * Start:
 * 1. state: one full empty queue
 *
 * p
 * |
 * ˇ
 * +--------------+
 * |              |
 * +--------------+
 *
 * 2. state: insert a element
 *
 * pData   pEmpty
 * |       |
 * ˇ       ˇ
 * +-------+------+
 * |       |      |
 * +-------+------+
 *
 */


/* TODO: Think on: queue element structure shall be buffer and RAM buffer indenpendently? Or only one buffer */
/* TODO: Linked elements? Do not do it now yet. Pos: const and ram data could follow each other */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "compiler.h"
#include "Queue.h"

#if (QUEUE_STATISTICS_ENABLED == 1)
#include "SysTime.h"
#endif

#ifdef CONFIG_MODULE_QUEUE_UNITTEST_ENABLE
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



/**
 * @brief  Queue initialization
 */
void Queue_Init(QueueListInfo_t * queue)
{
    if (queue == NULL)
        return;

    /* Buffersize should be larger than QueueElement_t */
    ASSERT(queue->bufferSize > sizeof(QueueElement_t));
    /* Buffer pointer should be not NULL */
    ASSERT(queue->pBuffer != NULL);

    /* Clear all queue */
    memset(queue->pBuffer, 0x00, queue->bufferSize);

    queue->first = queue->pBuffer;
    queue->end = queue->pBuffer;
    queue->elementNum = 0;    /* one empty buffer */


    /* Make one "empty" queue element */
    size_t dataSize = queue->bufferSize - sizeof(QueueElement_t);
    void * pDataPointer = (void *)((Address_t)queue->pBuffer + sizeof(QueueElement_t));
    QueueDataType_t dataType = QueuedataType_Ram;
    /* Add full empty queue */
    Queue_AddQueueElement(queue->first, pDataPointer, dataSize, dataType, QUEUE_NOTUSED);
    queue->first->nextQueueData = NULL;
    queue->first->prevQueueData = NULL;
    queue->elementNum++;
}



/**
 * @brief   Add / Set queue element
 * @note    Not check empty space and others. Be careful when you use it!
 */
static bool Queue_AddQueueElement(void * pElement, void * pData, size_t dataSize, QueueDataType_t dataType, bool isUsed)
{
    /* Check parameters */
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
 * @brief    Get a QueueElement with enough empty space
 */
static void * Queue_SearchEmptySpace(QueueListInfo_t * queue, size_t dataSize)
{
    void * pQueueElement = NULL;

    /* If bufferSize == 0, it is not RAM data (e.g. in ROM data), only queuelementsize need */
    if (queue->end->isUsed == QUEUE_NOTUSED)
    {
        /* Last element is not used, We hope it is good for use */
        if (queue->end->dataSize >= dataSize)
        {
            pQueueElement = (void *)queue->end;
        }
    }

    return pQueueElement;
}



bool Queue_DropFirstElement(QueueListInfo_t * queue, bool needDelete);
bool Queue_DropFirstElement(QueueListInfo_t * queue, bool needDelete)
{
    /* TODO: Implement */

    UNUSED_ARGUMENT(queue);
    UNUSED_ARGUMENT(needDelete);

    return false;
}



bool Queue_GetFirstElement(QueueListInfo_t * queue, void * buffer, size_t * dataSize, size_t bufferSize, bool needDelete);
bool Queue_GetFirstElement(QueueListInfo_t * queue, void * buffer, size_t * dataSize, size_t bufferSize, bool needDelete)
{
    bool result = false;

    /* Check parameters */
    if ((queue == NULL) || (buffer == NULL) || (dataSize == NULL) || (bufferSize == 0))
        return false;

    /* TODO: "CheckQueueListInfo" function? instead of queue->first... checking + guard checking */

    if ((queue->first != NULL) && (queue->first->isUsed == true))
    {
#if (QUEUE_GUARD_ENABLED == 1)
        /* Check guard values */
        if ((queue->first->headerGuardValue == QUEUE_GUARD_VALUE)
            && (queue->first->tailGuardValue == QUEUE_GUARD_VALUE))
        {
#endif
            /* Check data copy information */
            QueueElement_t *element = queue->first;
            size_t copyLength = element->dataSize;

            if (element->dataSize > bufferSize)
            {
                /* Too large data in queue, need cut */
                copyLength = bufferSize;
            }

            /* Copy data to parameterized buffer */
            memcpy(buffer, element->dataPointer, copyLength);
            *dataSize = copyLength;

            /* Change to empty element if can be delete */
            if (needDelete)
            {
                /* Set to the last element */
                QueueElement_t *previousEndElement = queue->end;
                queue->first = element->nextQueueData;
                queue->first->prevQueueData = NULL;
                queue->end = element;

                /* Make one "empty" queue element */
                QueueDataType_t dataType = QueuedataType_Ram;
                Queue_AddQueueElement(element, element->dataPointer, element->dataSize,
                        dataType, QUEUE_NOTUSED);

                element->prevQueueData = previousEndElement;
                element->nextQueueData = NULL;
                /* queue->end->prevQueueData has been set at above */

                /* queue->elementNum not need to increment, because the element num is not changed */
            }

            /* TODO: Defragment */

#if (QUEUE_STATISTICS_ENABLED == 1)
            /* GetTime... */
            SysTime_GetDateTime(&element->getTime);
#endif

            /* Successful Get element */
            result = true;

#if (QUEUE_GUARD_ENABLED == 1)
        }
        else
        {
            /* Guard fail */
            result = false;
        }
#endif
    }
    /* else: result = false */

    return result;
}



bool Queue_GetLastElement(QueueListInfo_t * queue, void * pData, size_t * dataSize, size_t bufferSize, bool needDelete);
bool Queue_GetLastElement(QueueListInfo_t * queue, void * pData, size_t * dataSize, size_t bufferSize, bool needDelete)
{
    /* TODO: Implement! */
    UNUSED_ARGUMENT(queue);
    UNUSED_ARGUMENT(pData);
    UNUSED_ARGUMENT(dataSize);
    UNUSED_ARGUMENT(bufferSize);
    UNUSED_ARGUMENT(needDelete);

    return false;
}



bool Queue_PutFirstElement(QueueListInfo_t * queue, void * pData, size_t dataSize, QueueDataType_t dataTpye);
bool Queue_PutFirstElement(QueueListInfo_t * queue, void * pData, size_t dataSize, QueueDataType_t dataTpye)
{
    /* TODO: Implement! */
    UNUSED_ARGUMENT(queue);
    UNUSED_ARGUMENT(pData);
    UNUSED_ARGUMENT(dataSize);
    UNUSED_ARGUMENT(dataTpye);

    return false;
}



/* TODO: Enum tpye for return? WrongParam, NoSpace, ... */
bool Queue_PutLastElement(QueueListInfo_t * queue, void * pData, size_t dataSize, QueueDataType_t dataType);
bool Queue_PutLastElement(QueueListInfo_t * queue, void * pData, size_t dataSize, QueueDataType_t dataType)
{
    /* Check parameters */
    if ((queue == NULL) || (pData == NULL))
        return false;

    bool isOk = false;

    /* TODO: buffer in RAM or ROM */

    /* Check empty space (in const data, we not need data buffer) */
    size_t requiredSize = (dataType == QueueDataType_Const) ? (0) : (dataSize);
    void * pQueueData = Queue_SearchEmptySpace(queue, requiredSize);
    if (pQueueData != NULL)
    {
        /* Has enough space */
        QueueElement_t * pElement = (QueueElement_t *)pQueueData;

        /* Put data to last QueueElement */
        QueueElement_t * pPrevOfLast = pElement->prevQueueData;

        /* Copy data if need */
        void * pNewElementData = pData;
        if (dataType == QueuedataType_Ram)
        {
            pNewElementData = (void *)((Address_t)pElement + sizeof(QueueElement_t));
            memcpy(pNewElementData, pData, dataSize);
        }

        Queue_AddQueueElement(pElement, pNewElementData, dataSize, dataType, QUEUE_ISUSED);
        pElement->nextQueueData = NULL;
        pElement->prevQueueData = pPrevOfLast;
        /* ecause element was overwrote, not need increment ElementNum */


        /* TODO: This is amateur solution for the searching empty space */
        void * pEndOfData = (void *)((Address_t)pElement + sizeof(QueueElement_t)  + ((dataType == QueuedataType_Ram) ? (dataSize) : (0)));
        /* Make Stack aligned safe */
        pEndOfData = (void *)MEM_MAKE_ALIGNED_ADDRESS(pEndOfData);

        /* Create empty queue element, or this is the last element */
        /* Make one "empty" queue element */
        if (((Address_t)pEndOfData + sizeof(QueueElement_t)) <= ((Address_t)queue->pBuffer + queue->bufferSize))
        {
            /* Has enough space to create last QueueElement */

            /* Make one "empty" queue element */
            void * pEmptyQueueElement = pEndOfData;
            size_t emptyDataSize = ((Address_t)queue->pBuffer + queue->bufferSize) - ((Address_t)pEmptyQueueElement + sizeof(QueueElement_t));

            void * pEmptyDataPointer = NULL;
            if (emptyDataSize > 0)
                pEmptyDataPointer = (void *)((Address_t)pEmptyQueueElement + sizeof(QueueElement_t));

            QueueDataType_t emptyDataType = QueuedataType_Ram;
            /* Add empty queue */
            Queue_AddQueueElement(pEmptyQueueElement, pEmptyDataPointer, emptyDataSize, emptyDataType, QUEUE_NOTUSED);
            ((QueueElement_t *)pEmptyQueueElement)->nextQueueData = NULL;
            ((QueueElement_t *)pEmptyQueueElement)->prevQueueData = pElement;
            queue->elementNum++;

            pElement->nextQueueData = pEmptyQueueElement;

            queue->end = pEmptyQueueElement;
        }
        else
        {
            /* There is no last empty queue element */
            queue->end = pElement;
        }

        isOk = true;
    }
    /* else: There is no enough empty space */

    return isOk;
}



void Queue_Defragmentation(void);
void Queue_Defragmentation(void)
{
    /* TODO: Implement it */
    return;
}



#ifdef CONFIG_MODULE_QUEUE_UNITTEST_ENABLE
uint32_t Queue_UnitTest(void)
{
    #define TEST_BUFFER_SIZE        (400U)
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


    /* Initialize the queue */
    Queue_Init(&testQueue);

    /* One full empty queue element should be created */
    UNITTEST_ASSERT(testQueue.elementNum == 1, "Init elementNum error");
    UNITTEST_ASSERT(testQueue.first == testQueue.end, "Init first-end pointer error");
    UNITTEST_ASSERT(testQueue.first == (QueueElement_t *)testBuffer, "Init pointer error");


    /* Add an element */
    UNITTEST_ASSERT(Queue_PutLastElement(&testQueue, "12345", 5, QueuedataType_Ram),
            "PutLastElement");

    /* Empty + new added queue element */
    UNITTEST_ASSERT((testQueue.elementNum == 2),
            "PutLastElement ElementNum error");
    UNITTEST_ASSERT((testQueue.first != testQueue.end),
            "PutLastElement First-End pointer error");
    UNITTEST_ASSERT((testQueue.first->dataPointer == (testBuffer + sizeof(QueueElement_t))),
            "PutLastElement dataPointer error");
    UNITTEST_ASSERT(!StrCmpWithLength((char *)(testQueue.first->dataPointer), "12345", 5),
            "PutLastElement data not contained");
    UNITTEST_ASSERT((testQueue.first->dataSize == 5),
            "PutLastElement data size error");
    UNITTEST_ASSERT((testQueue.end == (QueueElement_t *)(MEM_MAKE_ALIGNED_ADDRESS((testBuffer + sizeof(QueueElement_t) + 5)))),
            "PutLastElement end pointer error");

    /* FIXME: There is a 64 bit bug here */
    #warning 64-bit system bug here

    /* Add an element */
    UNITTEST_ASSERT(Queue_PutLastElement(&testQueue, "4321", 4, QueuedataType_Ram),
            "PutLastElement");

    /* Empty + new added queue element */
    UNITTEST_ASSERT((testQueue.elementNum == 3),
            "PutLastElement ElementNum error");
    UNITTEST_ASSERT((testQueue.first->nextQueueData->nextQueueData == testQueue.end),
            "PutLastElement First->Next - End pointer error");
    UNITTEST_ASSERT((testQueue.first->nextQueueData == testQueue.end->prevQueueData),
            "PutLastElement First->Next - End pointer error");
    /*UNITTEST_ASSERT((testQueue.first->dataPointer == (testBuffer + sizeof(QueueElement_t))),
            "PutLastElement dataPointer error");*/
    UNITTEST_ASSERT(!StrCmpWithLength((char *)(testQueue.first->nextQueueData->dataPointer), "4321", 4),
            "PutLastElement data not contained");
    UNITTEST_ASSERT((testQueue.first->nextQueueData->dataSize == 4),
            "PutLastElement data size error");
    /*UNITTEST_ASSERT((testQueue.end == (QueueElement_t *)(MEM_MAKE_ALIGNED_ADDRESS((testBuffer + sizeof(QueueElement_t) + 4)))),
            "PutLastElement end pointer error");*/

    /* Empty block */
    UNITTEST_ASSERT((testQueue.end->isUsed == QUEUE_NOTUSED),
                "PutLastElement end empty queue element error");


    /* Final checks */
    UNITTEST_ASSERT((testQueue.first->prevQueueData == NULL),
            "PutLastElement First->prev not NULL error");
    UNITTEST_ASSERT((testQueue.end->nextQueueData == NULL),
            "PutLastElement End->next not NULL error");


    /* Test: Get data */
    QueueElement_t * testFirstElement = testQueue.first;
    QueueElement_t * testSecondElement = testQueue.first->nextQueueData;    /* It is suppose the 2 put queue element before these code part */
    uint16_t elementNum = testQueue.elementNum;

    #define TEST_GETBUFFER_SIZE        (50U)
    uint8_t testGetBuffer[TEST_GETBUFFER_SIZE];
    size_t getDataSize = 0;

    /* Get - without delete */
    UNITTEST_ASSERT(Queue_GetFirstElement(&testQueue, testGetBuffer, &getDataSize, TEST_GETBUFFER_SIZE, false),
            "GetFirstElement error");
    UNITTEST_ASSERT((testQueue.elementNum==elementNum),
            "GetFirstElement - ElementNum error");
    UNITTEST_ASSERT((getDataSize == 5),
            "GetFirstElement - DataSize error");
    UNITTEST_ASSERT(!StrCmpWithLength((char *)testGetBuffer, "12345", 5),
            "GetFirstElement - Data error");
    UNITTEST_ASSERT((testQueue.first==testFirstElement),
            "GetFirstElement - First pointer error");
    UNITTEST_ASSERT((testQueue.first->nextQueueData==testSecondElement),
            "GetFirstElement - Second pointer error");


    /* Get - with delete */
    UNITTEST_ASSERT(Queue_GetFirstElement(&testQueue, testGetBuffer, &getDataSize, TEST_GETBUFFER_SIZE, true),
            "GetFirstElement error");
    UNITTEST_ASSERT((testQueue.elementNum==elementNum),
            "GetFirstElement - ElementNum error");
    UNITTEST_ASSERT((getDataSize == 5),
            "GetFirstElement - DataSize error");
    UNITTEST_ASSERT(!StrCmpWithLength((char *)testGetBuffer, "12345", 5),
            "GetFirstElement - Data error");
    /* Different from above test: (pointers) */
    UNITTEST_ASSERT((testQueue.first==testSecondElement),
            "GetFirstElement - First pointer error");
    UNITTEST_ASSERT((testQueue.end==testFirstElement),
            "GetFirstElement - End pointer error");
    UNITTEST_ASSERT((testQueue.end->isUsed==false),
                "GetFirstElement - End pointer not emptyerror");


    /* TODO: Test: GetFirstElement with small buffer (bufferSize < getDataSize) */


    /* TODO: Add test: Put Const data */



    /* Final checks */
    UNITTEST_ASSERT((testQueue.first->prevQueueData == NULL),
            "PutLastElement First->prev not NULL error");
    UNITTEST_ASSERT((testQueue.end->nextQueueData == NULL),
            "PutLastElement End->next not NULL error");


    return UnitTest_End();
}
#endif


