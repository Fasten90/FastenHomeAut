/*
 *    VirtualEEPROM.h
 *    Created on:   2019-06-10
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef VIRTUALEEPROM_H_
#define VIRTUALEEPROM_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
/* Be careful, this VirtualEEPROM module use the EEPROM types */
#include "EEPROM.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define VIRTUAL_EEPROM_FILE_NAME            "VirtualEEPROM.txt"



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void VirtualEEPROM_Init(void);

EEPROM_Result_t VirtualEEPROM_Write(const uint16_t address, const uint8_t * buffer, const uint16_t size);
EEPROM_Result_t VirtualEEPROM_Read(const uint16_t address, uint8_t * buffer, const uint16_t size);

uint32_t VirtualEEPROM_UnitTest(void);



#endif /* VIRTUALEEPROM_H_ */
