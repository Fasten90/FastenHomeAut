/*
 *    EEPROM.h
 *    Created on:   2019-06-10
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef EEPROM_H_
#define EEPROM_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define EEPROM_ADDRESS_START        ((uint16_t)0x0000U)
#define EEPROM_SIZE                 ((uint16_t)0x800U)
#define EEPROM_ADDRESS_END          ((uint16_t)(EEPROM_ADDRESS_START + EEPROM_SIZE))



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
    EEPROM_RESULT_OK,
    EEPROM_RESULT_ERROR,
    EEPROM_RESULT_INVALID_ADDRESS,
} EEPROM_Result_t;


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void EEPROM_Init(void);

EEPROM_Result_t EEPROM_Write(const uint16_t address, const uint8_t * buffer, const uint16_t size);
EEPROM_Result_t EEPROM_Read(const uint16_t address, uint8_t * buffer, const uint16_t size);



#endif /* EEPROM_H_ */
