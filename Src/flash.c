/*******************************************************************************
 * Purpose:
 ******************************************************************************/

/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "flash.h"
#include "include.h"


#ifdef CONFIG_MODULE_FLASH_ENABLE
/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/
/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;

/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/
extern void Error_Handler(void);

/*------------------------------------------------------------------------------
 *  Local functions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief
 * @details
 * @param[in]
 * @param[out]
 * @return
 ******************************************************************************/
void FLASH_Init( void )
{
	
	
	HAL_SPI_MspInit(&SpiHandle);
	

	/*##-1- Configure the SPI peripheral #######################################*/
	/* Set the SPI parameters */
	SpiHandle.Instance               = FLASH_SPIx;

	// MODE 0: CPOL 0, CPHA 0 = CPOL LOW, CPHASE 1. edge
	
	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	//SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
	SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
	SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
	SpiHandle.Init.CRCPolynomial     = 7;
	SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	SpiHandle.Init.Mode = SPI_MODE_MASTER;
	SpiHandle.Init.NSS               = SPI_NSS_SOFT;
	SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLED;
	#ifdef CONFIG_USE_PANEL_NODESMALL
	SpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLED;
	SpiHandle.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
	#endif
	//#ifdef CONFIG_USE_PANEL_NODEMEDIUM	// nincs egyéb struktúra elem



	if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}



	
}



void FLASH_Test ( void )
{
	
	uint8_t Buffer[32];
	

	
///*	
	
	// Write Enable
	// Need before every block erase

	Buffer[0] = FLASH_COMMAND_WRITE_ENABLE;
	
	FLASH_CS_ACTIVE();
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)Buffer, 1, 5000);
	FLASH_CS_INACTIVE();
	
	HAL_Delay(1000);
	
	
	

	// Block Erase
	
	Buffer[0] = FLASH_COMMAND_BLOCK_ERASE;
	Buffer[1] = 0;
	Buffer[2] = 0;
	Buffer[3] = 0;
	
	FLASH_CS_ACTIVE();
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)Buffer, 4, 5000);
	
	
	// Read status register
	//Buffer[0] = FLASH_COMMAND_READ_STATUS_REGISTER;
	//HAL_SPI_Transmit(&SpiHandle, (uint8_t*)Buffer, 1, 5000);
	//HAL_SPI_Receive(&SpiHandle, (uint8_t*)Buffer, 1, 5000);
	
	FLASH_CS_INACTIVE();	
	
	//uprintf("statusreg: %b\r\n",Buffer[0]);
	
	
	HAL_Delay(1000);
	
//*/




//
/*	
	// Read status register
	
	Buffer[0] = FLASH_COMMAND_READ_STATUS_REGISTER;

	FLASH_CS_ACTIVE();
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)Buffer, 1, 5000);
	HAL_SPI_Receive(&SpiHandle, (uint8_t*)Buffer, 1, 5000);
	//HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*)Buffer, (uint8_t*)Buffer, 4, 5000);
	FLASH_CS_INACTIVE();
	
	uprintf("statusreg: %b",Buffer[0]);
//
*/
	
	
	//while(1);
	



///*
	// Write Enable
	// Need before every page program !!

	Buffer[0] = FLASH_COMMAND_WRITE_ENABLE;
	
	FLASH_CS_ACTIVE();
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)Buffer, 1, 5000);
	FLASH_CS_INACTIVE();
	
	HAL_Delay(1000);
	
//*/


///*
// Page Program

	// 1. FLASH_COMMAND_PAGE_PROGRAM
	// 2. 24 bit address
	// 3. 256 byte data
	
	Buffer[0] = FLASH_COMMAND_PAGE_PROGRAM;
	Buffer[1] = 0;
	Buffer[2] = 0;
	Buffer[3] = 0;
	
	FLASH_CS_ACTIVE();
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)Buffer, 4, 5000);
	
	Buffer[0] = 77;
	Buffer[1] = 11;
	Buffer[2] = 22;
	Buffer[3] = 33;
	Buffer[4] = 44;
	Buffer[5] = 55;
	Buffer[6] = 66;
	Buffer[7] = 00;
	
	

	for (uint8_t i = 0; i < 256/8; i++)
	{
		HAL_SPI_Transmit(&SpiHandle, (uint8_t*)Buffer, 8, 5000);	
	}


	FLASH_CS_INACTIVE();
	
	HAL_Delay(1000);
//*/	


///*
// Read Data 03h
//#define FLASH_COMMAND_READ_DATA		( 0x03 )

	Buffer[0] = FLASH_COMMAND_READ_DATA;
	Buffer[1] = 0;
	Buffer[2] = 0;
	Buffer[3] = 0;
	
	FLASH_CS_ACTIVE();
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)Buffer, 4, 5000);
	HAL_SPI_Receive(&SpiHandle, (uint8_t*)Buffer, 32, 5000);
	//HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*)Buffer, (uint8_t*)Buffer, 36, 5000);
	
	FLASH_CS_INACTIVE();
	
	//uprintf("flash: %b %b %b %b",Buffer[0], Buffer[1], Buffer[2], Buffer[3]);
	uprintf("flash: \r\n");
	for (uint8_t i = 0; i < 32; i++)
	{
		uprintf("%d - %d\r\n", i, Buffer[i]);
	}
	
//*/	
	
}






/**
  * @brief SPI MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	if(hspi->Instance == FLASH_SPIx)
	{     
		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* Enable GPIO TX/RX clock */
		/*
		SPIx_SCK_GPIO_CLK_ENABLE();
		SPIx_MISO_GPIO_CLK_ENABLE();
		SPIx_MOSI_GPIO_CLK_ENABLE
		*/
		FLASH_SPIx_PINS_CLK_ENABLES();
		
		/* Enable SPI clock */
		SPIx_CLK_ENABLE(); 

		/*##-2- Configure peripheral GPIO ##########################################*/  
		/* SPI SCK GPIO pin configuration  */
		GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = SPIx_SCK_AF;
		HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

		/* SPI MISO GPIO pin configuration  */
		GPIO_InitStruct.Pin = SPIx_MISO_PIN;
		GPIO_InitStruct.Alternate = SPIx_MISO_AF;
		HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

		/* SPI MOSI GPIO pin configuration  */
		GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
		GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
		HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);
		
		
		// FLASH_SPI_CS
		GPIO_InitStruct.Pin       = FLASH_SPI_CS_GPIO_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		//GPIO_InitStruct.Alternate = SPIx_SCK_AF;
		HAL_GPIO_Init(FLASH_SPI_CS_GPIO_PORT, &GPIO_InitStruct);
		
		FLASH_CS_INACTIVE();
		
		
		// FLASH_SPI_WP
		GPIO_InitStruct.Pin       = FLASH_SPI_WP_GPIO_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		//GPIO_InitStruct.Alternate = SPIx_SCK_AF;
		HAL_GPIO_Init(FLASH_SPI_WP_GPIO_PORT, &GPIO_InitStruct);
		
		HAL_GPIO_WritePin(FLASH_SPI_WP_GPIO_PORT,FLASH_SPI_WP_GPIO_PIN,GPIO_PIN_SET);
		
		
		// FLASH_SPI_HOLD
		GPIO_InitStruct.Pin       = FLASH_SPI_HOLD_GPIO_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		//GPIO_InitStruct.Alternate = SPIx_SCK_AF;
		HAL_GPIO_Init(FLASH_SPI_HOLD_GPIO_PORT, &GPIO_InitStruct);	
		
		HAL_GPIO_WritePin(FLASH_SPI_HOLD_GPIO_PORT,FLASH_SPI_HOLD_GPIO_PIN,GPIO_PIN_SET);
		
		
	}
	
}

/**
  * @brief SPI MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO configuration to its default state
  * @param hspi: SPI handle pointer
  * @retval None
  */
/*
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  if(hspi->Instance == SPIx)
  {   
    //##-1- Reset peripherals ##################################################
    SPIx_FORCE_RESET();
    SPIx_RELEASE_RESET();
    
    //##-2- Disable peripherals and GPIO Clocks ################################
    // Configure SPI SCK as alternate function 
    HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
    // Configure SPI MISO as alternate function
    HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
    // Configure SPI MOSI as alternate function
    HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);
  }
}
*/



uint8_t FLASH_Read(uint32_t address, uint8_t *Buffer, uint8_t size, uint16_t timeout)
{
	uint8_t CommandBuffer[4];
	
	CommandBuffer[0] = FLASH_COMMAND_READ_DATA;
	CommandBuffer[1] = ( address >> 16 ) & 0xFF;
	CommandBuffer[2] = ( address >> 8 ) & 0xFF;
	CommandBuffer[3] = ( address >> 0 ) & 0xFF;
	
	
	FLASH_CS_ACTIVE();
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)CommandBuffer, 4, timeout);
	HAL_SPI_Receive(&SpiHandle, (uint8_t*)Buffer, size, timeout);
	//HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*)Buffer, (uint8_t*)Buffer, 36, 5000);
	
	FLASH_CS_INACTIVE();
	
	return RETURN_SUCCESS;
}



uint8_t FLASH_Write(uint32_t address, uint8_t *Buffer, uint8_t size, uint16_t timeout)
{
	
	// Page Program

	// 1. FLASH_COMMAND_PAGE_PROGRAM
	// 2. 24 bit address
	// 3. 256 byte data
	
	
	uint8_t CommandBuffer[4];
	
	
	// Write Enable
	// Need before every page program !!
	CommandBuffer[0] = FLASH_COMMAND_WRITE_ENABLE;
	
	FLASH_CS_ACTIVE();
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)CommandBuffer, 1, timeout);
	FLASH_CS_INACTIVE();
	
	// wait ... ?
	HAL_Delay(1);
	

	CommandBuffer[0] = FLASH_COMMAND_PAGE_PROGRAM;
	CommandBuffer[1] = ( address >> 16 ) & 0xFF;
	CommandBuffer[2] = ( address >> 8 ) & 0xFF;
	CommandBuffer[3] = ( address >> 0 ) & 0xFF;


	FLASH_CS_ACTIVE();
	// Command:
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)CommandBuffer, 4, timeout);
	// Datas
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)Buffer, size, timeout);	
	FLASH_CS_INACTIVE();

	return RETURN_SUCCESS;
}


// Block erase 64k
// max 2sec
uint8_t FLASH_BlockErase (uint32_t address, uint16_t timeout)
{

	uint8_t CommandBuffer[4];
	
	
	// Write Enable
	// Need before every page program / ersae !!
	CommandBuffer[0] = FLASH_COMMAND_WRITE_ENABLE;
	
	FLASH_CS_ACTIVE();
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)CommandBuffer, 1, timeout);
	FLASH_CS_INACTIVE();
	
	// wait ... ?
	HAL_Delay(1);
	
	
	// Block Erase
	CommandBuffer[0] = FLASH_COMMAND_BLOCK_ERASE;
	CommandBuffer[1] = ( address >> 16 ) & 0xFF;
	CommandBuffer[2] = ( address >> 8 ) & 0xFF;
	CommandBuffer[3] = ( address >> 0 ) & 0xFF;
	
	FLASH_CS_ACTIVE();
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)CommandBuffer, 4, 5000);
	FLASH_CS_INACTIVE();	
	
	// Wait for finish	
	HAL_Delay(2000);

	return RETURN_SUCCESS;
	
}


// Sector erase: 4k
// max: 300 msec
uint8_t FLASH_SectorErase (uint32_t address, uint16_t timeout)
{
	// TODO: mask !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	

	uint8_t CommandBuffer[4];
	

	// Write Enable
	// Need before every page program !!
	CommandBuffer[0] = FLASH_COMMAND_WRITE_ENABLE;
	
	FLASH_CS_ACTIVE();
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)CommandBuffer, 1, timeout);
	FLASH_CS_INACTIVE();
	
	// wait ... ?
	HAL_Delay(1);
	
	
	// Sector Erase
	CommandBuffer[0] = FLASH_COMMAND_SECTOR_ERASE;
	CommandBuffer[1] = ( address >> 16 ) & 0xFF;
	CommandBuffer[2] = ( address >> 8 ) & 0xFF;
	CommandBuffer[3] = ( address >> 0 ) & 0xFF;
	
	FLASH_CS_ACTIVE();
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*)CommandBuffer, 4, 5000);
	FLASH_CS_INACTIVE();	
	
	
	// Wait for finish
	HAL_Delay(300);
	
	
	
	return RETURN_SUCCESS;
	
}



void FLASH_Test_WithEraseWriteRead ( void )
{
	
	// TEST CODES

	uint8_t Buffer[] = "Szeretlek Sari!";
	uint8_t size = sizeof(Buffer);
	
	FLASH_SectorErase(0x10000,5000);
	
	FLASH_Write(0x10000,Buffer,size,5000);
	
	Buffer[0] = 0;
	
	FLASH_Read(0x10000,Buffer,size,5000);
	
	uprintf("Flashben volt: %s",Buffer);
	
	/////////////////////////////////////////////
	
	/*
	// GOOD TEST CODES
	
	uint8_t Buffer[] = { 0xAA,0xBB,0xCC,0xDD };
	
	FLASH_SectorErase(0x00,5000);
	
	HAL_Delay(100);
	
	FLASH_Write(0x00,Buffer,4,5000);
	
	HAL_Delay(100);
	
	for (uint8_t i=0; i < 4; i++)
	{
		Buffer[i] = 0;
	}
	
	FLASH_Read(0x00,Buffer,4,5000);
	
	uprintf("flash 0x%b%b%b%b",Buffer[0],Buffer[1],Buffer[2],Buffer[3]);
	*/
	
}


#endif	// #ifdef CONFIG_MODULE_FLASH_ENABLE
