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

#include "UnitTest.h"

#ifdef CONFIG_MODULE_VIRTUAL_EEPROM_ENABLE
#include "VirtualEEPROM.h"
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



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief  EEPROM initialization
 */
void EEPROM_Init(void)
{

#ifdef CONFIG_MODULE_VIRTUAL_EEPROM_ENABLE
    VirtualEEPROM_Init();
#endif
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
    if (!MEM_IN_FLASH_OR_RAM(buffer, size) || (buffer == NULL))
    {
        /* Wrong data */
        return EEPROM_RESULT_ERROR;
    }

#if (EEPROM_ADDRESS_START_DEF != 0)
    if ((address < EEPROM_ADDRESS_START) || ((address + size) > EEPROM_ADDRESS_END))
#else
    /* Note: Some compiler will report the always true situation */
    if ((address + size) > EEPROM_ADDRESS_END)
#endif
    {
        return EEPROM_RESULT_INVALID_ADDRESS;
    }
    if (size > EEPROM_SIZE)
    {
        return EEPROM_RESULT_INVALID_DATA_SIZE;
    }

#ifdef CONFIG_MODULE_VIRTUAL_EEPROM_ENABLE
    return VirtualEEPROM_Write(address, buffer, size);
#else
    /* TODO: Implement */
    return EEPROM_RESULT_OK;
#endif
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
    if (!MEM_IN_FLASH_OR_RAM(buffer, size) || (buffer == NULL))
    {
        return EEPROM_RESULT_ERROR;
    }

#if (EEPROM_ADDRESS_START_DEF != 0)
    if ((address < EEPROM_ADDRESS_START) || ((address + size) > EEPROM_ADDRESS_END))
#else
    /* Note: Some compiler will report the always true situation */
    if ((address + size) > EEPROM_ADDRESS_END)
#endif
    {
        return EEPROM_RESULT_INVALID_ADDRESS;
    }
    if (size > EEPROM_SIZE)
    {
        return EEPROM_RESULT_INVALID_DATA_SIZE;
    }

#ifdef CONFIG_MODULE_VIRTUAL_EEPROM_ENABLE
    return VirtualEEPROM_Read(address, buffer, size);
#else
    /* TODO: Implement */
    return EEPROM_RESULT_OK;
#endif
}



#ifdef CONFIG_MODULE_EEPROM_TEST
uint32_t EEPROM_ModuleTest(void)
{
    uint8_t eepromTestBuffer1[10] = { 0 };
    const uint8_t eepromTestBuffer2[10] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };
    const uint16_t eepromTestBufferSize = 10;
    uint16_t size = 0;
    uint16_t address = 0;
    EEPROM_Result_t result = EEPROM_RESULT_OK;

    UnitTest_Start("EEPROM", __FILE__);


    /* Test: EEPROM_Write */

    /* EEPROM_Write - Too high address */
    address = EEPROM_ADDRESS_END + 1;
    result = EEPROM_Write(address, eepromTestBuffer2, eepromTestBufferSize);
    UNITTEST_ASSERT(result == EEPROM_RESULT_INVALID_ADDRESS, "EEPROM - Invalid address");
    /* TODO: test: has changed the EEPROM content? */

    /* EEPROM_Write - Size error */
    /* TODO: The result will EEPROM_RESULT_INVALID_DATA_SIZE because the end address at after end of eeprom */
    address = EEPROM_ADDRESS_START;  /* Normal address */
    size = EEPROM_SIZE + 1; /* Too high size */
    result = EEPROM_Write(address, eepromTestBuffer2, size);
    UNITTEST_ASSERT(result == EEPROM_RESULT_INVALID_ADDRESS, "EEPROM - Invalid size");
    /* TODO: test: has changed the EEPROM content? */

    /* EEPROM_Write - error */
    address = EEPROM_ADDRESS_START;
    result = EEPROM_Write(address, NULL, eepromTestBufferSize);
    UNITTEST_ASSERT(result == EEPROM_RESULT_ERROR, "EEPROM - Error");
    /* TODO: test: has changed the EEPROM content? */


    /* Test: EEPROM_Read */

    /* EEPROM_Read - Too high address */
    address = EEPROM_ADDRESS_END + 1;
    result = EEPROM_Read(address, eepromTestBuffer1, eepromTestBufferSize);
    UNITTEST_ASSERT(result == EEPROM_RESULT_INVALID_ADDRESS, "EEPROM - Invalid address");
    /* TODO: test: has changed the EEPROM content? */

    /* EEPROM_Read - Size error */
    /* TODO: The result will EEPROM_RESULT_INVALID_DATA_SIZE because the end address at after end of eeprom */
    address = EEPROM_ADDRESS_START;
    size = EEPROM_SIZE + 1;
    result = EEPROM_Read(address, eepromTestBuffer1, size);
    UNITTEST_ASSERT(result == EEPROM_RESULT_INVALID_ADDRESS, "EEPROM - Invalid size");
    /* TODO: test: has changed the EEPROM content? */

    /* EEPROM_Read - error */
    result = EEPROM_Read(address, NULL, eepromTestBufferSize);
    UNITTEST_ASSERT(result == EEPROM_RESULT_ERROR, "EEPROM - Error");
    /* TODO: test: has changed the EEPROM content? */


    return UnitTest_End();
}
#endif



#endif /* CONFIG_MODULE_EEPROM_ENABLE */
