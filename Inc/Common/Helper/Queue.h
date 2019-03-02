/*
 *    queue.h
 *    Created on:   2018-01-06
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef COMMON_HELPER_QUEUE_H_
#define COMMON_HELPER_QUEUE_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "GenericTypeDefs.h"
#include "SysTime.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

// Settings
#define QUEUE_STATISTICS_ENABLED        (1)
#define QUEUE_GUARD_ENABLED                (1)
#define QUEUE_ELEMENT_MAX_NUM            (1000)

#define QUEUE_GUARD_VALUE                ((QueueGuard_t)0xEFEFEFEFEFU)

#define QUEUE_ISUSED                    ((bool)true)
#define QUEUE_NOTUSED                    ((bool)false)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
    QueueDataType_Unknown,
    QueueDataType_Const,
    QueuedataType_Ram,
} QueueDataType_t;


typedef uint32_t QueueGuard_t;


typedef struct QueueElement_t
{
#if (QUEUE_GUARD_ENABLED == 1)
    QueueGuard_t headerGuardValue;            ///< Guard value for security
#endif

    bool isUsed;                            ///< It is used? (true = yes, it is used. false = not used, empty element)

    void * dataPointer;                        ///< Data pointer
    size_t dataSize;                        ///< Element data size (in byte)

    QueueDataType_t dataType;                ///< Actual element type (const or not)

    struct QueueElement_t * prevQueueData;    ///< Previous queue element
    struct QueueElement_t * nextQueueData;    ///< Next queue element

#if (QUEUE_STATISTICS_ENABLED == 1)
    DateTime_t putTime;
    DateTime_t getTime;
#endif

#if (QUEUE_GUARD_ENABLED == 1)
    QueueGuard_t tailGuardValue;            ///< Guard value for security
#endif
} QueueElement_t;


typedef struct
{
    QueueElement_t * first;        ///< First item
    QueueElement_t * end;        ///< Last item
    uint16_t elementNum;        ///< Queue actual element num

    void * pBuffer;                ///< Pointer of buffer
    size_t bufferSize;            ///< Buffer size (in byte)
} QueueListInfo_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Queue_Init(QueueListInfo_t * queue);
uint32_t Queue_UnitTest(void);



#endif /* COMMON_HELPER_QUEUE_H_ */
