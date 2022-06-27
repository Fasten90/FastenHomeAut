/*
 *    GpsHandler.h
 *    Created on:   2019-06-07
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef GPSHANDLER_H_
#define GPSHANDLER_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "DateTime.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define LOG_MESSAGE_COORD_MAX_LENGTH    (32)

#define GSM_SIMCOM_SIM868               (1)
#define GSM_QUECTEL_MC60                (2)

#define GSM_MODE                        GSM_SIMCOM_SIM868



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

///< Coordinate
typedef struct
{
    float Lat;
    float Lon;
} Coord_t;


///< Coordinate log: coordinate with other important values
typedef struct
{
    Coord_t coord;
    DateTime_t dateTime;
    float speed;              ///< Speed in kmph
    uint32_t tick;
} CoordinateLog_t;


///< Counter type
typedef uint8_t CoordLogID_t;


///< Statistics
typedef struct
{
    uint32_t receivedCoordNum;
    uint32_t receivedGoodCoordNum;
} CoordinateStatistics_t;


///< CoordinateLogMessage struct
typedef struct
{
    uint32_t ID;
    uint32_t sysTick;
    DateTime_t dateTime;

    uint8_t coordRequestedNum;                    ///< Requested coordinates
    uint8_t coordCollectedNum;                    ///< Collected coordinates
    CoordinateLog_t Coordinate[LOG_MESSAGE_COORD_MAX_LENGTH];

    uint8_t maxSpeed;                             ///< Maximal speed

} GPS_CoordinateLogMsg_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void GpsHandler_ProcessMessage(const char * msg);

bool GpsHandler_GetLastCoordinateLog(CoordinateLog_t * coordLog);
bool GpsHandler_GetCoordinate(CoordinateLog_t * coordLog, CoordLogID_t id);
bool GpsHandler_GetCoordinateByTime(CoordinateLog_t * coordLog, DateTime_t * fromDateTime, DateTime_t * toDateTime);
void GpsHandler_ClearBuffer(void);

bool GPS_GetLastValidCoordinatesWithSysTick(uint32_t startSysTick, uint32_t endSysTick, uint8_t requestCoordinateNum, GPS_CoordinateLogMsg_t* logMessage);

uint32_t GpsHandler_UnitTest(void);



#endif /* GPSHANDLER_H_ */
