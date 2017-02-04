/*******************************************************************************
 * Purpose:
 ******************************************************************************/

#ifndef FLASH_S25FL_H_
#define FLASH_S25FL_H_


/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/
#include "include.h"


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/
 
#ifdef CONFIG_MODULE_FLASH_ENABLE


 // S25FL204K

// Write Enable (06h)
 #define FLASH_COMMAND_WRITE_ENABLE				((uint8_t) 0x06 )

// Read Data 03h
#define FLASH_COMMAND_READ_DATA					((uint8_t) 0x03 )

// Fast Read Data 0Bh
#define FLASH_COMMAND_FAST_READ_DATA			((uint8_t) 0x03 )


// Page Program 02h
#define FLASH_COMMAND_PAGE_PROGRAM				((uint8_t) 0x02 )

// Read Status Register
#define FLASH_COMMAND_READ_STATUS_REGISTER		((uint8_t) 0x05 )

// Write Status Register
#define FLASH_COMMAND_WRITE_STATUS_REGISTER		((uint8_t) 0x01 )

//Block Erase
//(64 kB) D8h A23-A16 A15-A8 A7-A0
#define FLASH_COMMAND_BLOCK_ERASE				((uint8_t) 0xD8 ) 

//Sector Erase
//(4 kB) 20h A23-A16 A15-A8 A7-A0
#define FLASH_COMMAND_SECTOR_ERASE				((uint8_t) 0x20 )

//Chip Erase C7h/60h
#define FLASH_COMMAND_CHIP_ERASE				((uint8_t) 0xC7 )

//Power-down B9h
#define FLASH_COMMAND_POWER_DOWN				((uint8_t) 0xB9 )


 /*
 Command
Name Byte1 Code Byte2 Byte3 Byte4 Byte5 Byte6 N-bytes
Write Enable 06h
write Disable 04h
Read Status
Register 05h (S7-S0) (1) (Note 2)
Write Status
Register 01h S7-S0
Read Data 03h A23-A16 A15-A8 A7-A0 (D7-D0) (Next byte) continuous
Fast Read 0Bh A23-A16 A15-A8 A7-A0 dummy (D7-D0) (Next byte)
continuous
Fast Read Dual
Output 3Bh A23-A16 A15-A8 A7-A0 dummy
I/O= (D6, D4,
D2, D0)
O= (D7, D5,
D3, D1)
(One byte per 4
clocks,
continuous)
Page Program 02h A23-A16 A15-A8 A7-A0 (D7-D0) (Next byte) Up to 256
bytes
Block Erase
(64 kB) D8h A23-A16 A15-A8 A7-A0
Sector Erase
(4 kB) 20h A23-A16 A15-A8 A7-A0
Chip Erase C7h/60h
Power-down B9h
Release
Power-down /
Device ID
ABh dummy dummy dummy (ID7-ID0) (4)
Manufacturer /
Device ID (3) 90h dummy dummy 00h (M7-M0) (ID7-ID0)
JEDEC ID 9Fh (M7-M0)
manufacturer
(ID15-ID8)
Memory Type
(ID7-ID0)
Capacity
 */
 

#define FLASH_CS_ACTIVE()			HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_PORT,FLASH_SPI_CS_GPIO_PIN,GPIO_PIN_RESET);
#define FLASH_CS_INACTIVE()			HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_PORT,FLASH_SPI_CS_GPIO_PIN,GPIO_PIN_SET);


// STATUS REGISTER

// WIP: Write in Progress
#define FLASH_STATUS_REGISTER_WRITEINPROGRESS_BIT	( 1 << 0)
// WEL: Write enable Latch
#define FLASH_STATUS_REGISTER_WRITEENABLELATCH_BIT	( 1 << 1)

// Block protect bits - BP0
#define FLASH_STATUS_REGISTER_BP0_BIT	( 1 << 2)
// Block protect bits - BP1
#define FLASH_STATUS_REGISTER_BP1_BIT	( 1 << 3)
// Block protect bits - BP2
#define FLASH_STATUS_REGISTER_BP2_BIT	( 1 << 4)
// Block protect bits - BP3
#define FLASH_STATUS_REGISTER_BP3_BIT	( 1 << 5)

// REV: Reserved Bits
#define FLASH_STATUS_REGISTER_RESERVED_BIT	( 1 << 6)
// SRP: The Status Register Proctect
#define FLASH_STATUS_REGISTER_STATUSREGISTERPROTECTED_BIT	( 1 << 7)




/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local function declarations
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/
void FLASH_Init( void );
void FLASH_Test ( void );
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi);

uint8_t FLASH_Read(uint32_t address, uint8_t *Buffer, uint8_t size, uint16_t timeout);
uint8_t FLASH_Write(uint32_t address, uint8_t *Buffer, uint8_t size, uint16_t timeout);

uint8_t FLASH_BlockErase (uint32_t address, uint16_t timeout);
uint8_t FLASH_SectorErase (uint32_t address, uint16_t timeout);

void FLASH_Test_WithEraseWriteRead ( void );


#endif	// #ifdef CONFIG_MODULE_FLASH_ENABLE


#endif /* FLASH_S25FL_H_ */
