/*
 *    GpsHandler.c
 *    Created on:   2019-06-07
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */


/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "GpsHandler.h"
#include "StringHelper.h"
#include "DateTime.h"
#include "Debug.h"
#include "DebugList.h"

#include "UnitTest.h"

#ifdef CONFIG_MODULE_GPS_ENABLE



/*------------------------------------------------------------------------------
 *  Macros
 *----------------------------------------------------------------------------*/

#define GPSHANDLER_MESSAGE_CGNSINF_MAX_LENGTH         (100U)
#define GPSHANDLER_MESSAGE_CGNSINF_PARAMETER_NUM      (21U)
#define GPSHANDLER_MESSAGE_NMEA_MAX_LENGTH            (110U)
#define GPSHANDLER_MESSAGE_NMEA_PARAMETER_NUM         (12U)

#define GPSHANDLER_COORD_BUFFER_LENGTH                (100U)


#define GPSHANDLER_DEBUG_ENABLE                       (1)

#if (GPSHANDLER_DEBUG_ENABLE == 1)
    #define GPSHANDLER_DEBUG_PRINT(_str)              Debug_Print(Debug_GPS, _str)
    #define GPSHANDLER_DEBUG_PRINTF(...)              Debug_Printf(Debug_GPS, __VA_ARGS__)
#else
    /* Debug not used */
    #define GPSHANDLER_DEBUG_PRINT(_str)
    #define GPSHANDLER_DEBUG_PRINTF(...)
#endif



/*------------------------------------------------------------------------------
 *  Typedefs
 *----------------------------------------------------------------------------*/

/* For NMEA GPRMC - coordinate message separate to valid value */
typedef struct
{
    Time_t time;
    bool isValid;
    float latitude;
    bool isNorth;
    float longitude;
    bool isEast;
    float speed;        ///< km/h
    Date_t date;
} GPS_NmeaRmcInfo_t;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

/* Do not write these Gps datas from interrupt */
static CoordinateLog_t GpsHandler_CoordLogBuffer[GPSHANDLER_COORD_BUFFER_LENGTH] = { 0 };
static CoordLogID_t GpsHandler_CoordLogActualCnt = 0;
static bool GpsHandler_IsReceivedCoordLog = false;
static CoordinateStatistics_t GpsHandler_Statistics = { 0 };



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void GpsHandler_ProcessCgnsinfMessage(const char * msg);

static void GpsHandler_ProcessNmeaMessage(char * msg);
static bool GpsHandler_ProcessNmeaGprmcMessage(char * separated[], GPS_NmeaRmcInfo_t * rmcData);
static bool GpsHandler_Coord2Float(float * floatValue, char * coordinateString, uint8_t len);
static float GpsHandler_ConvertKnotToKmph(float floatValue);
static bool GpsHandler_NmeaGprmcCheckAndConvert(GPS_NmeaRmcInfo_t * rmcData, CoordinateLog_t * coordLog);

static bool GpsHandler_CheckCoordRanges(Coord_t * coord);
static void GpsHandler_SaveCoordinate(CoordinateLog_t * coord);

static bool GPS_FindCoordinateFromBuffer(CoordinateLog_t * coordLog, uint32_t startSysTick, uint32_t endSysTick);

#if (GPSHANDLER_DEBUG_ENABLE == 1)
    static void DebugPrintCoordinate(CoordinateLog_t * coordLog);
#else
    #define DebugPrintCoordinate()
#endif

static bool GpsHandler_ConvertCgnsinfDateTime(char * str, DateTime_t * dateTime);

static bool GpsMessageHandler_ConvertTime(const char * str, Time_t * time);
static bool GpsMessageHandler_ConvertDate(const char * str, Date_t * date);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

/**
 * @brief    Process received GPS message
 */
void GpsHandler_ProcessMessage(const char * msg)
{
#if (GSM_MODE == GSM_SIMCOM_SIM868)
    GpsHandler_ProcessCgnsinfMessage(msg);
#elif (GSM_MODE == GSM_QUECTEL_MC60)
    GpsHandler_ProcessNmeaMessage(msg);
#else
    #error GPS_MODE defined incorrectly
#endif
}



/**
 * @brief    Handle CGNSINF message
 *           Process SIM868 GPS message
 *           Will save the message/GPS coordinate, if the message correct
 */
/* TODO: Return value with success + input parameter with struct */
static void GpsHandler_ProcessCgnsinfMessage(const char * msg)
{
    /*
     *  1  GNSS run status -- 0-1 {1}
     *  2  Fix status -- 0-1 {1}
     *  3  UTC date & Time yyyyMMddhhmmss.sss
     *       yyyy: [1980,2039]
     *       MM : [1,12]
     *       dd: [1,31]
     *       hh: [0,23]
     *       mm: [0,59]
     *       ss.sss:[0.000,60.999] {18}
     *  4  Latitude ±dd.dddddd [-90.000000,90.000000] {10}
     *  5  Longitude ±ddd.dddddd [-180.000000,180.000000] {11}
     *  6  MSL Altitude meters {8}
     *  7  Speed Over Ground Km/hour [0,999.99] {6}
     *  8  Course Over Ground degrees [0,360.00] {6}
     *  9  Fix Mode -- 0,1,2[1] {1}
     *  10 Reserved1 {0}
     *  11 HDOP -- [0,99.9] {4}
     *  12 PDOP -- [0,99.9] {4}
     *  13 VDOP -- [0,99.9] {4}
     *  14 Reserved2 {0}
     *  15 GPS Satellites in View -- [0,99] {2}
     *  16 GNSS Satellites Used -- [0,99] {2}
     *  17 GLONASS Satellites in View -- [0,99] {2}
     *  18 Reserved3 {0}
     *  19 C/N0 max dBHz [0,55] {2}
     *  20 HPA[2] meters [0,9999.9] {6}
     *  21 VPA[2] meters [0,9999.9] {6}
     *
     *  Total： {94} chars
     *
     *  E.g.
     *  "1,0,19800106001106.000,,,,0.00,0.0,0,,,,,,0,0,,,,,"
     *  "1,1,20171116212122.000,47.492245,19.017659,687.576,1.22,184.7,1,,1.3,1.6,1.0,,5,5,,,44,,"
     */

    char procMsg[GPSHANDLER_MESSAGE_CGNSINF_MAX_LENGTH];
    char * separated[GPSHANDLER_MESSAGE_CGNSINF_PARAMETER_NUM] = { NULL };

    StrCpyMax(procMsg, msg, GPSHANDLER_MESSAGE_CGNSINF_MAX_LENGTH);

    /* Split by ',' - 21 parameter --> 20 ',' */
    if (STRING_Splitter(procMsg, ",", separated, GPSHANDLER_MESSAGE_CGNSINF_PARAMETER_NUM) == (GPSHANDLER_MESSAGE_CGNSINF_PARAMETER_NUM - 1))
    {
        CoordinateLog_t coordLog = { 0 };

        /* Successful split */
        GPSHANDLER_DEBUG_PRINT("Successful split GPS message");

        /* Process parameters */

        /* 3. time - yyyyMMddhhmmss.sss */
        /* First, check length */
        if (!GpsHandler_ConvertCgnsinfDateTime(separated[2], &coordLog.dateTime))
        {
            GPSHANDLER_DEBUG_PRINT("Failed process DateTime");
            return;
        }

        /* Check DateTime */
        if (!DateTime_CheckDateTime(&coordLog.dateTime))
        {
            GPSHANDLER_DEBUG_PRINT("Failed process: Invalid DateTime");
            return;
        }


        /* Note: The two coordinates not need to convert (it is a correct degree, see: 47.492245, 19.017659) */

        /* 4. Coord -Lat */
        if (!StringToFloat(separated[3], &coordLog.coord.Lat))
        {
            /* Check Lat value below */
            GPSHANDLER_DEBUG_PRINT("Failed process GPS: Lat");
            return;
        }

        /* 5. Coord - Lon */
        if (!StringToFloat(separated[4], &coordLog.coord.Lon))
        {
            /* Check Lon value below */
            GPSHANDLER_DEBUG_PRINT("Failed process GPS: Lon");
            return;
        }

        /* Check coordinate values */
        if (GpsHandler_CheckCoordRanges(&coordLog.coord))
        {
            /* Valid, save received coordinate to buffer */
            GpsHandler_SaveCoordinate(&coordLog);
            GpsHandler_Statistics.receivedGoodCoordNum++;

            /* GPSHANDLER_DEBUG_PRINTF("Received coordinate: Lat: %f, Lon: %f\r\n", lat, lon); */
            DebugPrintCoordinate(&coordLog);
        }
        else
        {
            GPSHANDLER_DEBUG_PRINT("Coordinate has invalid value");
        }
    }
    else
    {
        /* Fail... */
        GPSHANDLER_DEBUG_PRINT("Failed split GPS message");
    }

    GpsHandler_Statistics.receivedCoordNum++;
}



/**
 * @brief Convert "yyyyMMddhhmmss.sss" DateTime message
 */
static bool GpsHandler_ConvertCgnsinfDateTime(char * str, DateTime_t * dateTime)
{
    size_t dateLength = StringLength(str);
    uint32_t val = 0; /* Not need init */
    bool isOk = true;

    if (dateLength != (sizeof("yyyyMMddhhmmss.sss") - 1))
    {
        return false;
    }

    isOk &= StringToUnsignedDecimalNumWithLength(&str[0], &val, 4);
    dateTime->date.year = (uint8_t)(val - 2000);    /*  Converted to valid year (20xx --> xx ; e.g 2017 --> 17) */
    isOk &= StringToUnsignedDecimalNumWithLength(&str[4], &val, 2);
    dateTime->date.month = (uint8_t)val;
    isOk &= StringToUnsignedDecimalNumWithLength(&str[6], &val, 2);
    dateTime->date.day = (uint8_t)val;
    isOk &= StringToUnsignedDecimalNumWithLength(&str[8], &val, 2);
    dateTime->time.hour = (uint8_t)val;
    isOk &= StringToUnsignedDecimalNumWithLength(&str[10], &val, 2);
    dateTime->time.minute = (uint8_t)val;
    isOk &= StringToUnsignedDecimalNumWithLength(&str[12], &val, 2);
    dateTime->time.second = (uint8_t)val;

    return isOk;
}



/**
 * @brief        Process GPRMC message
 */
static void GpsHandler_ProcessNmeaMessage(char * msg)
{

    /*
    Recommended minimum specific GPS/Transit data

    eg1. $GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
    eg2. $GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68


               225446       Time of fix 22:54:46 UTC
               A            Navigation receiver warning A = OK, V = warning
               4916.45,N    Latitude 49 deg. 16.45 min North
               12311.12,W   Longitude 123 deg. 11.12 min West
               000.5        Speed over ground, Knots
               054.7        Course Made Good, True
               191194       Date of fix  19 November 1994
               020.3,E      Magnetic variation 20.3 deg East
               *68          mandatory checksum


    eg3. $GPRMC,220516,A,5133.82,N,00042.24,W,173.8,231.8,130694,004.2,W*70
                  1    2    3    4    5     6    7    8      9     10  11 12


          1   220516     Time Stamp
          2   A          validity - A-ok, V-invalid
          3   5133.82    current Latitude
          4   N          North/South
          5   00042.24   current Longitude
          6   W          East/West
          7   173.8      Speed in knots
          8   231.8      True course
          9   130694     Date Stamp
          10  004.2      Variation
          11  W          East/West
          12  *70        checksum


    eg4. $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
    1    = UTC of position fix
    2    = Data status (V=navigation receiver warning)
    3    = Latitude of fix
    4    = N or S
    5    = Longitude of fix
    6    = E or W
    7    = Speed over ground in knots
    8    = Track made good in degrees True
    9    = UT date
    10   = Magnetic variation degrees (Easterly var. subtracts from true course)
    11   = E or W
    12   = Checksum
    */

    /* TODO: Check max length of message ? */
    char procMsg[GPSHANDLER_MESSAGE_NMEA_MAX_LENGTH];

    /* TODO: Do not copy */
    StrCpyMax(procMsg, msg, GPSHANDLER_MESSAGE_NMEA_MAX_LENGTH);

    GPSHANDLER_DEBUG_PRINTF("Received NMEA msg: %s\r\n", procMsg);

    /* Check the header */
    char * headerPos = (char *)STRING_FindString((const char *)procMsg, "$GNRMC,");
    /* TODO: \r\n cut: */
    if (headerPos != NULL)
    {
        headerPos += sizeof("$GNRMC,") - 1;
        char * separated[GPSHANDLER_MESSAGE_NMEA_PARAMETER_NUM] = { NULL };

        uint8_t splitLength = STRING_Splitter(headerPos, ",", separated, GPSHANDLER_MESSAGE_NMEA_PARAMETER_NUM);
        if (splitLength == GPSHANDLER_MESSAGE_NMEA_PARAMETER_NUM)
        {
            /* Successful splitting */

            /* GPRMC message */
            /* TODO: Get in argument */
            GPS_NmeaRmcInfo_t rmcData = { 0 };

            if (GpsHandler_ProcessNmeaGprmcMessage(separated, &rmcData))
            {
                /* Good coordinate */
                CoordinateLog_t coordLog = { 0 };

                /* Check and validate */
                if (GpsHandler_NmeaGprmcCheckAndConvert(&rmcData, &coordLog))
                {
                    /* OK, save it */
                    GPSHANDLER_DEBUG_PRINT("GPS coordinate is good");

                    /* Valid, save received coordinate to buffer */
                    /* TODO: Better SaveCoordinate-t?? */
                    GpsHandler_SaveCoordinate(&coordLog);
                    GpsHandler_Statistics.receivedGoodCoordNum++;

                    /* DebugPrintf("Received coordinate: Lat: %f, Lon: %f\r\n", coordLog.coord.Lat, coordLog.coord.Lon); */
                    DebugPrintCoordinate(&coordLog);
                }
                else
                {
                    /* Wrong coordinate */
                    GPSHANDLER_DEBUG_PRINT("GPS message is not valid");
                }
            }
            else
            {
                /* Wrong coordinate */
                GPSHANDLER_DEBUG_PRINT("GPS message process failed");
            }


            /* TODO: Set date-time if it was correct? */
        }
        else
        {
            /* Failed splitting */
            GPSHANDLER_DEBUG_PRINT("Failed split GPS message");
        }
    }
    else
    {
        /* Wrong GPS message, not find header */
        GPSHANDLER_DEBUG_PRINT("Wrong GPS message, not found header");
    }
}



/**
 * @brief    GPS - NMEA GPRMC message processing
 */
static bool GpsHandler_ProcessNmeaGprmcMessage(char * separated[], GPS_NmeaRmcInfo_t * rmcData)
{
    /* Temporary values */
    float floatValue = 0.0f;
    DateTime_t convertDateTime;

    /* Check it is:GPRMC message? */
    /* Recommended minimum specific GPS/Transit data */
    /*
    eg4. $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh

    eg3. $GPRMC,220516,A,5133.82,N,00042.24,W,173.8,231.8,130694,004.2,W*70
          1    2    3    4    5     6    7    8      9     10  11 12
    1   220516     Time Stamp
    2   A          validity - A-ok, V-invalid
    3   5133.82    current Latitude
    4   N          North/South
    5   00042.24   current Longitude
    6   W          East/West
    7   173.8      Speed in knots
    8   231.8      True course
    9   130694     Date Stamp
    10  004.2      Variation
    11  W          East/West
    12  *70        checksum
    */

    /* 1   220516     Time Stamp */
    if (GpsMessageHandler_ConvertTime(separated[0], &convertDateTime.time))
    {
        /* TODO: Add parameter the rmcData ? */
        rmcData->time.hour = convertDateTime.time.hour;
        rmcData->time.minute = convertDateTime.time.minute;
        rmcData->time.second = convertDateTime.time.second;
    }
    else
    {
        /* TODO: Misra */
        return false;
    }


    /* 2   A          validity - A-ok, V-invalid */
    if (separated[1][0] == 'A')
    {
        rmcData->isValid = true;
    }
    else
    {
        rmcData->isValid = false;
    }


    /* 3   5133.82    current Latitude */
    if (GpsHandler_Coord2Float(&floatValue,  separated[2], 2))
    {
        rmcData->latitude = floatValue;
    }
    else
    {
        /* Wrong Latitude */
        /* TODO: Misra */
        return false;
    }


    /*    4   N          North/South */
    if (separated[3][0] == 'N')
    {
        rmcData->isNorth = true;
    }
    else if (separated[3][0] == 'S')
    {
        rmcData->isNorth = false;
    }
    else
    {
        /* Wrong North/South */
        /* TODO: Misra */
        return false;
    }


    /* 5   00042.24   current Longitude */
    if (GpsHandler_Coord2Float(&floatValue,  separated[4], 3))
    {
        rmcData->longitude = floatValue;
    }
    else
    {
        /* Wrong Longitude */
        /* TODO: Misra */
        return false;
    }


    /* 6   W          East/West */
    if (separated[5][0] == 'E')
    {
        rmcData->isEast = true;
    }
    else if (separated[5][0] == 'W')
    {
        rmcData->isEast = false;
    }
    else
    {
        /* Wrong North/South */
        /* TODO: Misra */
        return false;
    }


    /* 7   173.8      Speed in knots */
    if (StringToFloat(separated[6], &floatValue))
    {
        /* 1 knot = 1.85200 kilometers per hour */
        /* Convert knot -> km/h */
        rmcData->speed = GpsHandler_ConvertKnotToKmph(floatValue);
        /* Now in km/h */
    }
    else
    {
        /* TODO: Misra */
        return false;
    }


    /*  8   231.8      True course */
    /* TODO: Does not need? */


    /* 9   130694     Date Stamp */
    if (GpsMessageHandler_ConvertDate(separated[8], &convertDateTime.date))
    {
        /* TODO: Add parameter the rmcData ? */
        rmcData->date.year = convertDateTime.date.year;
        rmcData->date.month = convertDateTime.date.month;
        rmcData->date.day = convertDateTime.date.day;
    }
    else
    {
        /* TODO: Misra */
        return false;
    }


    /* 10  004.2      Variation */
    /* TODO: Does not need? */

    /* 11  W          East/West */
    /* TODO: Does not need? */
    /* Note: There was East/West at above too */

    /* 12  *70        checksum */
    /* TODO: Check? */

    return true;
}



/**
 * @brief   Convert HHmmSS GPS time format to Time_t object (NMEA)
 * @return  Is okay
 */
static bool GpsMessageHandler_ConvertTime(const char * str, Time_t * time)
{
    /* Not need to check the parameters */
    uint32_t val = 0; /* Not need to init */
    bool isOk = true;

    isOk &= StringToUnsignedDecimalNumWithLength(&str[0], &val, 2);
    time->hour = (uint8_t)val;
    isOk &= StringToUnsignedDecimalNumWithLength(&str[2], &val, 2);
    time->minute = (uint8_t)val;
    isOk &= StringToUnsignedDecimalNumWithLength(&str[4], &val, 2);
    time->second = (uint8_t)val;

    return isOk;
}



/**
 * @brief   Convert DDMMYY GPS date format to Date_t object (NMEA)
 * @return  Is okay
 */
static bool GpsMessageHandler_ConvertDate(const char * str, Date_t * date)
{
    /* Not need to check the parameters */
    uint32_t val = 0; /* Not need to init */
    bool isOk = true;

    isOk &= StringToUnsignedDecimalNumWithLength(&str[0], &val, 2);
    date->day = (uint8_t)val;
    isOk &= StringToUnsignedDecimalNumWithLength(&str[2], &val, 2);
    date->month = (uint8_t)val;
    isOk &= StringToUnsignedDecimalNumWithLength(&str[4], &val, 2);
    date->year = (uint8_t)val;

    return isOk;
}



/**
 * @brief        Coordinate (degree+minute : xxyy.yy) converting to float coordinate TODO: Explain
 * @param        coordinateString        coordinate string which shall be converted
 * @param        len                     degree (not minute) length (2-3)
 * @return        bool                   true, if successful
 *                                       false, if failed
 */
static bool GpsHandler_Coord2Float(float * floatValue, char * coordinateString, uint8_t len)
{
    /* TODO: Rename len --> only degree length */
    uint32_t degrees = 0; /* Not need to init, STRING_String2Dec() will reinit this */
    float degree_minutes = 0.0f; /* Not need to init, STRING_String2Float() will reinit this */

    /* First 2-3 number = degree (xx) */
    if (StringToUnsignedDecimalNumWithLength(coordinateString, &degrees, len) != true)
    {
        return false;
    }

    /* Degree minutes: yy.yy */
    /* TODO: Check - this was null terminated or not? */
    if (StringToFloat(&coordinateString[len], &degree_minutes) != true)
    {
        return false;
    }

    /* Convert */
    *floatValue = degrees + (degree_minutes / 60.0f);

    return true;
}



/**
 * @brief        Convert Knot to Km/h (Kmph)
 */
static float GpsHandler_ConvertKnotToKmph(float floatValue)
{
    /*  1 knot = 1.85200 kilometers per hour */
    return floatValue * 1.85200f;
}



static bool GpsHandler_NmeaGprmcCheckAndConvert(GPS_NmeaRmcInfo_t * rmcData, CoordinateLog_t * coordLog)
{
    /* East / West */
    if (rmcData->isEast == true)
    {
        /* East - Longitude + */
        coordLog->coord.Lon = rmcData->longitude;
    }
    else
    {
        /* West - Longitude will '-' */
        coordLog->coord.Lon = (-1) * rmcData->longitude;
    }


    /* North / South */
    if (rmcData->isNorth == true)
    {
        /* North - Latitude + */
        coordLog->coord.Lat = rmcData->latitude;
    }
    else
    {
        /* South - Latitude will '-' */
        coordLog->coord.Lat = (-1) * rmcData->latitude;
    }


    /* Check coordinate values */
    if (!GpsHandler_CheckCoordRanges(&coordLog->coord))
    {
        GPSHANDLER_DEBUG_PRINT("GPS coordinate range error!");
        return false;
    }


    /* TODO: Better copy */
    /* DateTime */
    coordLog->dateTime.date.year = rmcData->date.year;
    coordLog->dateTime.date.month = rmcData->date.month;
    coordLog->dateTime.date.day = rmcData->date.day;

    coordLog->dateTime.time.hour = rmcData->time.hour;
    coordLog->dateTime.time.minute = rmcData->time.minute;
    coordLog->dateTime.time.second = rmcData->time.second;

    if (!DateTime_CheckDateTime(&coordLog->dateTime))
    {
        GPSHANDLER_DEBUG_PRINT("GPS DateTime invalid!");
        return false;
    }


    /* Speed */
    coordLog->speed = rmcData->speed;


    /* SysTick */
    /* TODO: xTaskGetTickCount() not always be here */
    coordLog->tick = HAL_GetTick();

    return true;
}



/**
 * @brief     Check coordinate values
 * @retval    true    if good
 * @retval    false    if wrong
 */
static bool GpsHandler_CheckCoordRanges(Coord_t * coord)
{
    bool isGood = false;

    /* Check parameter */
    if (coord == NULL)
    {
        return false;
    }

    /* Check range */
    if ((coord->Lat >= -90.00000f) && (coord->Lat <= 90.00000f)
            && (coord->Lon >= -180.00000f) && (coord->Lon <= 180.00000f))
    {
        isGood = true;
    }

    return isGood;
}



/**
 * @brief    Save CoordLog
 */
static void GpsHandler_SaveCoordinate(CoordinateLog_t * coord)
{
    /* Create coordlog */
    coord->tick = HAL_GetTick();

    /* Save coordlog */
    memcpy(&GpsHandler_CoordLogBuffer[GpsHandler_CoordLogActualCnt], coord, sizeof(CoordinateLog_t));

    /* Received a new coordinate */
    GpsHandler_IsReceivedCoordLog = true;

    /* Increment Cnt */
    GpsHandler_CoordLogActualCnt++;
    if (GpsHandler_CoordLogActualCnt >= GPSHANDLER_COORD_BUFFER_LENGTH)
    {
        GpsHandler_CoordLogActualCnt = 0;
    }

    /* Save DateTime */
    /* TODO ? */
}



/**
 * @brief    Get Last coordinate
 */
bool GpsHandler_GetLastCoordinateLog(CoordinateLog_t * coordLog)
{
    bool result = false;

    /* Check parameters */
    if (coordLog == NULL)
    {
        return false;
    }

    if (GpsHandler_IsReceivedCoordLog)
    {
        CoordLogID_t id = GpsHandler_CoordLogActualCnt ;
        if (GpsHandler_CoordLogActualCnt == 0)
        {
            id = GPSHANDLER_COORD_BUFFER_LENGTH;
        }

        memcpy(coordLog, &GpsHandler_CoordLogBuffer[id], sizeof(CoordinateLog_t));
        result = true;
    }

    return result;
}



/**
 * @brief    Get fix id-s coordinate
 */
bool GpsHandler_GetCoordinate(CoordinateLog_t * coordLog, CoordLogID_t id)
{
    bool result = false;

    /* Check parameters */
    if ((coordLog == NULL) || (id >= GPSHANDLER_COORD_BUFFER_LENGTH))
    {
        return false;
    }

    if (GpsHandler_IsReceivedCoordLog)
    {
        if (id > GpsHandler_Statistics.receivedGoodCoordNum)
        {
            /* Can get */
            memcpy(coordLog, &GpsHandler_CoordLogBuffer[id], sizeof(CoordinateLog_t));
            result = true;
        }
        /* else - Not received... invalid coordinate */
    }

    return result;
}



/**
 * @brief    Get coordinate from interval
 */
bool GpsHandler_GetCoordinateByTime(CoordinateLog_t * coordLog, DateTime_t * fromDateTime, DateTime_t * toDateTime)
{
    CoordLogID_t maxId = GPSHANDLER_COORD_BUFFER_LENGTH;
    CoordLogID_t i;
    bool result = false;

    /* Check parameters */
    if ((coordLog == NULL) || (fromDateTime == NULL) || (toDateTime == NULL))
    {
        return 0;
    }

    /* Check max ID */
    if (GpsHandler_Statistics.receivedGoodCoordNum < GPSHANDLER_COORD_BUFFER_LENGTH)
    {
        maxId = GPSHANDLER_COORD_BUFFER_LENGTH;
    }

    /* Search */
    /* TODO: Optimization: from old times or from new times? */
    for (i = 0; i < maxId; i++)
    {
        if (DateTime_CompareDateTime(&GpsHandler_CoordLogBuffer[i].dateTime, fromDateTime) == DateTimeCompare_FirstNewSecondOld)
        {
            /* This GPS time is "after" fromDateTime */
            if (DateTime_CompareDateTime(&GpsHandler_CoordLogBuffer[i].dateTime, toDateTime) == DateTimeCompare_FirstOldSecondNew)
            {
                /* This GPS time is "under" (before) toDateTime */
                /* Good GPS coordinate */
                memcpy(coordLog, &GpsHandler_CoordLogBuffer[i], sizeof(CoordinateLog_t));
                result = true;
                break;
            }
            /* else: After */
        }
        /* else: Before (not after) */
    }

    return result;
}



/**
 * @brief    Get coordinate from systick interval
 */
static bool GPS_FindCoordinateFromBuffer(CoordinateLog_t * coordLog, uint32_t startSysTick, uint32_t endSysTick)
{
    CoordLogID_t maxId = GPSHANDLER_COORD_BUFFER_LENGTH;
    CoordLogID_t i;
    bool result = false;

    /* Check parameters */
    if (coordLog == NULL)
    {
        return false;
    }

    /* Check max ID */
    if (GpsHandler_Statistics.receivedGoodCoordNum < GPSHANDLER_COORD_BUFFER_LENGTH)
    {
        maxId = GPSHANDLER_COORD_BUFFER_LENGTH;
    }

    /* Search */
    for (i = 0; i < maxId; i++)
    {
        if ((GpsHandler_CoordLogBuffer[i].tick > startSysTick) && (GpsHandler_CoordLogBuffer[i].tick < endSysTick))
        {
            /* In interval */
            /* Good GPS coordinate */
            memcpy(coordLog, &GpsHandler_CoordLogBuffer[i], sizeof(CoordinateLog_t));
            result = true;
            break;
        }
        /* else: Not in interval */
    }

    return result;
}



/**
 * @brief    Clear GPS buffer and statistics
 */
void GpsHandler_ClearBuffer(void)
{
    memset(GpsHandler_CoordLogBuffer, '\0', sizeof(GpsHandler_CoordLogBuffer));
    memset(&GpsHandler_Statistics, '\0', sizeof(GpsHandler_Statistics));
    GpsHandler_CoordLogActualCnt = 0;
    GpsHandler_IsReceivedCoordLog = false;
}



/**
 * @brief        Get last coordinates in the parameterized interval
 * @param        startSysTick               Start time for preferred interval
 * @param        endSysTick                 Finish time for preferred interval
 * @param        requestCoordinateNum       Collect coordinate limits
 */
bool GPS_GetLastValidCoordinatesWithSysTick(uint32_t startSysTick, uint32_t endSysTick, uint8_t requestCoordinateNum, GPS_CoordinateLogMsg_t * logMessage)
{

    /* Check parameters */
    if ((logMessage == NULL) || (requestCoordinateNum == 0) || (endSysTick < startSysTick) || (requestCoordinateNum >= LOG_MESSAGE_COORD_MAX_LENGTH))
    {
        return false;
    }


    uint32_t sysTickSize = (endSysTick - startSysTick) / requestCoordinateNum;

    /* Init log */
    logMessage->coordRequestedNum = requestCoordinateNum;
    logMessage->coordCollectedNum = 0;
    memset(&logMessage->Coordinate[0], 0, sizeof(CoordinateLog_t));
    logMessage->maxSpeed = 0; /* Max speed init with 0*/


    /* Collect coordinates */
    for (uint8_t i = 0; i < requestCoordinateNum; i++)
    {
        bool success = false;

        /* Find coordinate from calculated small interval */
        success = GPS_FindCoordinateFromBuffer(&logMessage->Coordinate[i],
                                               startSysTick,
                                               startSysTick + sysTickSize);

        /* TODO: Extended find */

        if (success)
        {
            /* Successful */
            logMessage->coordCollectedNum++;
        }
        else
        {
            /* Wrong, set it null coord */
            memset(&logMessage->Coordinate[i], 0, sizeof(CoordinateLog_t));
        }

        /* StartSysTick ++ */
        startSysTick += sysTickSize;
    }

    return (logMessage->coordCollectedNum != 0) ? true : false;
}



#if (GPSHANDLER_DEBUG_ENABLE == 1)
static void DebugPrintCoordinate(CoordinateLog_t * coordLog)
{
    char dateTimeString[DATETIME_STRING_MAX_LENGTH] = { 0 }; /* Not need to init, because we will fill it */

    DateTime_PrintDateTimeToString(dateTimeString, &coordLog->dateTime);

    /* Lat, Lon, Speed [kmph], DateTime */
    GPSHANDLER_DEBUG_PRINTF("Coord: %f, %f, %3.2f, %s\r\n",
            coordLog->coord.Lat,
            coordLog->coord.Lon,
            coordLog->speed,
            dateTimeString);
}
#endif



#ifdef CONFIG_MODULE_GPSHANDLER_UNITTEST_ENABLE
/**
 * @brief    GpsHandler module Unit Test
 */
uint32_t GpsHandler_UnitTest(void)
{
    UnitTest_Start("GpsHandler", __FILE__);

    GpsHandler_ClearBuffer();

    /* Test: GpsHandler_ProcessCgnsinfMessage() */

    /* "1,0,19800106001106.000,,,,0.00,0.0,0,,,,,,0,0,,,,," */
    /* Invalid DateTime, no coordinate... */
    GpsHandler_ProcessCgnsinfMessage("1,0,19800106001106.000,,,,0.00,0.0,0,,,,,,0,0,,,,,");

    /* Expected results: */
    UNITTEST_ASSERT((GpsHandler_CoordLogActualCnt == 0), "GpsHandlerProcess error");
    UNITTEST_ASSERT((GpsHandler_IsReceivedCoordLog == false), "GpsHandlerProcess error");


    /* "1,1,20171116212122.000,47.492245,19.017659,687.576,1.22,184.7,1,,1.3,1.6,1.0,,5,5,,,44,," */
    /* Good DateTime, good coordinate */
    GpsHandler_ProcessCgnsinfMessage("1,1,20171116212122.000,47.492245,19.017659,687.576,1.22,184.7,1,,1.3,1.6,1.0,,5,5,,,44,,");

    UNITTEST_ASSERT((GpsHandler_CoordLogActualCnt == 1), "GpsHandlerProcess error");
    UNITTEST_ASSERT((GpsHandler_IsReceivedCoordLog == true), "GpsHandlerProcess error");

    Coord_t expectedCoord = { .Lat = 47.492245f, .Lon = 19.017659f };
    DateTime_t expectedDateTime = { .date.year = 17, .date.month = 11, .date.day = 16, .time.hour = 21, .time.minute = 21, .time.second = 22 };
    UNITTEST_ASSERT(
            (GpsHandler_CoordLogBuffer[0].coord.Lat >= expectedCoord.Lat - 0.00001f)
            && (GpsHandler_CoordLogBuffer[0].coord.Lat <= expectedCoord.Lat + 0.00001f)
            && (GpsHandler_CoordLogBuffer[0].coord.Lon >= expectedCoord.Lon - 0.00001f)
            && (GpsHandler_CoordLogBuffer[0].coord.Lon <= expectedCoord.Lon + 0.00001f)
            , "GpsHandlerProcess error - Coordinate");
    UNITTEST_ASSERT(!memcmp(&GpsHandler_CoordLogBuffer[0].dateTime, &expectedDateTime, sizeof(DateTime_t)), "GpsHandlerProcess error - DateTime");


    /* Test GpsHandler_GetCoordinateByTime() */
    /* With datetime request coordinates - shall get what we saved at above */
    CoordinateLog_t getCoordLog = { 0 };
    DateTime_t fromDateTime = { .date.year = 17, .date.month = 11, .date.day = 16, .time.hour = 21, .time.minute = 21, .time.second = 21 };
    DateTime_t toDateTime = { .date.year = 17, .date.month = 11, .date.day = 16, .time.hour = 21, .time.minute = 21, .time.second = 23 };
    UNITTEST_ASSERT(GpsHandler_GetCoordinateByTime(&getCoordLog, &fromDateTime, &toDateTime), "GpsHandler - GetByDateTime error");
    UNITTEST_ASSERT(
            (getCoordLog.coord.Lat >= expectedCoord.Lat - 0.00001f)
            && (getCoordLog.coord.Lat <= expectedCoord.Lat + 0.00001f)
            && (getCoordLog.coord.Lon >= expectedCoord.Lon - 0.00001f)
            && (getCoordLog.coord.Lon <= expectedCoord.Lon + 0.00001f)
            , "GpsHandlerProcess error - Get Coordinate by DateTime");
    UNITTEST_ASSERT(!memcmp(&GpsHandler_CoordLogBuffer[0].dateTime, &expectedDateTime, sizeof(DateTime_t)), "GpsHandlerProcess error - DateTime");


    /* If there is no correct datetime coordinate, will be the result empty? */
    DateTime_t fromDateTime2 = { .date.year = 17, .date.month = 11, .date.day = 16, .time.hour = 21, .time.minute = 21, .time.second = 20 };
    DateTime_t toDateTime2 = { .date.year = 17, .date.month = 11, .date.day = 16, .time.hour = 21, .time.minute = 21, .time.second = 21 };
    UNITTEST_ASSERT(GpsHandler_GetCoordinateByTime(&getCoordLog, &fromDateTime2, &toDateTime2) == false, "GpsHandler - GetByDateTime error");


    /* +CGNSINF: 1,1,20171205191016.000,47.492158,19.017985,174.498,0.00,30.0,1,,1.0,1.4,0.9,,13,9,,,38,, */

    /* +CGNSINF: 1,1,20171205192122.000,47.492132,19.018004,176.125,3.67,177.6,1,,1.0,1.4,0.9,,13,9,,,38,, */

    /* TODO: More beautiful test */
    /* TODO: 1. fil buffer (will overwrite the 0. element?) */
    /* TODO: 2. At more coordinates adding will be the found coordinates correct? */


    GpsHandler_ClearBuffer();

    /* NMEA GPRMC test */
    /* "$GNRMC,034036.000,A,3150.8612,N,11711.9045,E,2.74,178.00,240516,,,A*7C" */
    GpsHandler_ProcessNmeaMessage("$GNRMC,034036.000,A,3150.8612,N,11711.9045,E,2.74,178.00,240516,,,A*7C");

    Coord_t expectedCoord2 = { .Lat = 31.8476868f, .Lon = 117.19841f };
    DateTime_t expectedDateTime2 = { .date.year = 16, .date.month = 5, .date.day = 24, .time.hour = 3, .time.minute = 40, .time.second = 36 };
    UNITTEST_ASSERT(
            (GpsHandler_CoordLogBuffer[0].coord.Lat >= expectedCoord2.Lat - 0.00001f)
            && (GpsHandler_CoordLogBuffer[0].coord.Lat <= expectedCoord2.Lat + 0.00001f)
            && (GpsHandler_CoordLogBuffer[0].coord.Lon >= expectedCoord2.Lon - 0.00001f)
            && (GpsHandler_CoordLogBuffer[0].coord.Lon <= expectedCoord2.Lon + 0.00001f)
            , "GpsHandlerProcess error - Coordinate");
    UNITTEST_ASSERT(!memcmp(&GpsHandler_CoordLogBuffer[0].dateTime, &expectedDateTime2, sizeof(DateTime_t)), "GpsHandlerProcess error - DateTime");


    GpsHandler_ClearBuffer();


    /* +QGNSSRD: $GNRMC,184928.000,A,4729.5263,N,01901.0777,E,0.40,200.73,161217,,,A*7A */
    GpsHandler_ProcessNmeaMessage("+QGNSSRD: $GNRMC,184928.000,A,4729.5263,N,01901.0777,E,0.40,200.73,161217,,,A*7A");
    Coord_t expectedCoord3 = { .Lat = 47.4921036f, .Lon = 19.0179615f };
    DateTime_t expectedDateTime3 = { .date.year = 17, .date.month = 12, .date.day = 16, .time.hour = 18, .time.minute = 49, .time.second = 28 };
    UNITTEST_ASSERT(
            (GpsHandler_CoordLogBuffer[0].coord.Lat >= expectedCoord3.Lat - 0.00001f)
            && (GpsHandler_CoordLogBuffer[0].coord.Lat <= expectedCoord3.Lat + 0.00001f)
            && (GpsHandler_CoordLogBuffer[0].coord.Lon >= expectedCoord3.Lon - 0.00001f)
            && (GpsHandler_CoordLogBuffer[0].coord.Lon <= expectedCoord3.Lon + 0.00001f)
            , "GpsHandlerProcess error - Coordinate");
    UNITTEST_ASSERT(!memcmp(&GpsHandler_CoordLogBuffer[0].dateTime, &expectedDateTime3, sizeof(DateTime_t)), "GpsHandlerProcess error - DateTime");


    /* TODO: More NMEA UnitTest */


    /* Finish */
    GpsHandler_ClearBuffer();
    return UnitTest_End();
}
#endif    /* CONFIG_MODULE_GPSHANDLER_UNITTEST_ENABLE */


#endif /* CONFIG_MODULE_GPS_ENABLE */
