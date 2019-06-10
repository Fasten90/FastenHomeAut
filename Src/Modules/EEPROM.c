/*
 *    EEPROM.c
 *    Created on:   2019-06-10
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "compiler.h"
#include "MemHandler.h"
#include "EEPROM.h"



#ifdef CONFIG_MODULE_EEPROM_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief  EEPROM initialization
 */
void EEPROM_Init(void)
{

}



/**
 * @brief   Write EEPROM
 * @param   address EEPROM address, where shall write datas
 * @param   buffer  Datas, which will be written to the EEPROM
 * @param   size    size of data in byte
 */
EEPROM_Result_t EEPROM_Write(const uint16_t address, const uint8_t * buffer, const uint16_t size)
{
    /* Check parameters */
    ASSERT(buffer != NULL);
    if (!MEM_IN_FLASH_OR_RAM(buffer, size))
    {
        return EEPROM_RESULT_ERROR;
    }

    if ((address < EEPROM_ADDRESS_START) || ((address + size) > EEPROM_ADDRESS_END))
    {
        return EEPROM_RESULT_INVALID_ADDRESS;
    }


    /* TODO: Implement */
    return EEPROM_RESULT_OK;
}



/**
 * @brief   Read EEPROM
 * @param   address EEPROM address, where shall read datas
 * @param   buffer  Datas, which will be written from the EEPROM
 * @param   size    size of data in byte
 */
EEPROM_Result_t EEPROM_Read(const uint16_t address, uint8_t * buffer, const uint16_t size)
{
    /* Check parameters */
    ASSERT(buffer != NULL);
    if (!MEM_IN_FLASH_OR_RAM(buffer, size))
    {
        return EEPROM_RESULT_ERROR;
    }

    if ((address < EEPROM_ADDRESS_START) || ((address + size) > EEPROM_ADDRESS_END))
    {
        return EEPROM_RESULT_INVALID_ADDRESS;
    }


    /* TODO: Implement */
    return EEPROM_RESULT_OK;
}



#endif /* CONFIG_MODULE_EEPROM_ENABLE */
