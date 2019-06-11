/*
 *    VirtualEEPROM.c
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
#include "VirtualEEPROM.h"

#include "EEPROM.h"

#ifdef CONFIG_MODULE_VIRTUAL_EEPROM_ENABLE

#include "MemHandler.h"
#include "UnitTest.h"

#include <stdio.h>

/* For file exists checking */

#include <sys/types.h>
#include <sys/stat.h>

/* unistd not available at MSVC and TCC*/
/* #include <unistd.h> */


/* <byte:2>\r\n */
#define VIRTUAL_EEPROM_STORE_SIZE_PER_BYTE      (4)

#define VIRTUAL_EEPROM_INIT_DATA                ((uint8_t)(0x00))



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

static uint8_t file_content[EEPROM_SIZE] = { 0 };

static FILE * file = NULL;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void VirtualEEPROM_WriteToFile(void);
static void VirtualEEPROM_ReadFromFile(void);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


static uint8_t file_exist (char * filename)
{
  struct stat   buffer;
  return (stat (filename, &buffer) == 0);
}



/**
 * @brief  VirtualEEPROM initialization
 */
void VirtualEEPROM_Init(void)
{
    if (file_exist(VIRTUAL_EEPROM_FILE_NAME))
    {
        /* File exists */
        file = fopen(VIRTUAL_EEPROM_FILE_NAME, "r");

        /* Read all content */
        VirtualEEPROM_ReadFromFile();

        fclose(file);

        printf("Virtual EEPROM initialized with exists file\n");
    }
    else
    {
        /* File doesn't exist */
        file = fopen(VIRTUAL_EEPROM_FILE_NAME, "w");

        /* Create empty */
        memset(file_content, VIRTUAL_EEPROM_INIT_DATA, EEPROM_SIZE);
        VirtualEEPROM_WriteToFile();

        fclose(file);

        printf("Virtual EEPROM initialized with empty file\n");
    }
}


/* TODO: fclose()  */



/* TODO: Change the result type? */
EEPROM_Result_t VirtualEEPROM_Write(const uint16_t address, const uint8_t * buffer, const uint16_t size)
{
    /* Not need to check the parameters */

    uint16_t index = address - EEPROM_ADDRESS_START;

    memcpy(file_content+index, buffer, size);

    /* NOTE: Be careful: it will be slow! */
    file = fopen(VIRTUAL_EEPROM_FILE_NAME, "w");

    VirtualEEPROM_WriteToFile();

    fclose(file);

    return EEPROM_RESULT_OK;
}



/* TODO: Change the result type? */
EEPROM_Result_t VirtualEEPROM_Read(const uint16_t address, uint8_t * buffer, const uint16_t size)
{
    /* Not need to check the parameters */
    uint16_t index = address - EEPROM_ADDRESS_START;

    /* Copy necessary content */
    memcpy(buffer, file_content+index, size);

    return EEPROM_RESULT_OK;
}



static void VirtualEEPROM_WriteToFile(void)
{
    /* Write the file */
    uint16_t i;
    char file_content_string[EEPROM_SIZE * VIRTUAL_EEPROM_STORE_SIZE_PER_BYTE] = { 0 };
    char * str = &file_content_string[0];

    /* Prepare content */
    for (i = 0; i < EEPROM_SIZE; i++)
    {
        uint8_t data = file_content[i];
        /* NOTE: Be careful! If format change, shall be sync with VIRTUAL_EEPROM_STORE_SIZE_PER_BYTE */
        /* Format style: One byte - on line */
        str += sprintf(str, "%02X\n", data);
    }

    /* Write to the file */
    fprintf(file, file_content_string);
}



static void VirtualEEPROM_ReadFromFile(void)
{
    /* Write the file */
    uint16_t i;

    /* Read content */
    for (i = 0; i < EEPROM_SIZE; i++)
    {
        char str[VIRTUAL_EEPROM_STORE_SIZE_PER_BYTE];

        /* Read one line */
        fgets(str, VIRTUAL_EEPROM_STORE_SIZE_PER_BYTE, file);

        uint32_t data = 0x00;
        /* NOTE: Be careful! If format change, shall be sync with VIRTUAL_EEPROM_STORE_SIZE_PER_BYTE */
        /* Format style: One byte - on line */
        sscanf(str, "%02X\r\n", &data);
        file_content[i] = (uint8_t)data;
    }
}



#ifdef CONFIG_MODULE_VIRTUAL_EEPROM_UNITTEST_ENABLE
uint32_t VirtualEEPROM_UnitTest(void)
{
    UnitTest_Start("VirtualEEPROM", __FILE__);


    /* Init - Write - Read */
    VirtualEEPROM_Init();

    uint8_t buffer[10] = { 0 };
    const uint8_t bufferTest[10] = { 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB };
    const uint16_t sizeTest = 10;
    VirtualEEPROM_Write(EEPROM_ADDRESS_START, bufferTest, sizeTest);

    VirtualEEPROM_Read(EEPROM_ADDRESS_START, buffer, sizeTest);

    /* Test */
    UNITTEST_ASSERT(memcmp(buffer, bufferTest, sizeTest) == 0, "VirtualEEPROM - Read");


    /* Read with re-init */
    memset(buffer, 0, sizeTest);
    VirtualEEPROM_Init();

    VirtualEEPROM_Read(EEPROM_ADDRESS_START, buffer, sizeTest);

    UNITTEST_ASSERT(memcmp(buffer, bufferTest, sizeTest) == 0, "VirtualEEPROM - Read");



    /* Test - 2. */
    VirtualEEPROM_Init();
    memset(buffer, 0, sizeTest);
    VirtualEEPROM_Write(EEPROM_ADDRESS_START+10, bufferTest, sizeTest);

    VirtualEEPROM_Init();


    VirtualEEPROM_Read(EEPROM_ADDRESS_START, buffer, sizeTest);
    UNITTEST_ASSERT(memcmp(buffer, bufferTest, sizeTest) == 0, "VirtualEEPROM - Read");


    VirtualEEPROM_Read(EEPROM_ADDRESS_START+10, buffer, sizeTest);
    UNITTEST_ASSERT(memcmp(buffer, bufferTest, sizeTest) == 0, "VirtualEEPROM - Read");



    return UnitTest_End();
}
#endif

#endif /* CONFIG_MODULE_VIRTUAL_EEPROM_ENABLE */
