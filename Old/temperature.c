/*
 *		temperature.c
 *
 *		Created on:		2015
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 8.
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "temperature.h"
#include "compiler.h"

#ifdef MODUL_TEMPERATURE_ENABLE

/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

float GLOBAL_TemperatureValue_LM75 = 0.0;

///< I2C handler declaration
I2C_HandleTypeDef I2cHandle;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

extern void Error_Handler(void);



/*------------------------------------------------------------------------------
 *  Local functions
 *----------------------------------------------------------------------------*/

float TEMPERATURE_ConvertLM75AdTemperatureValue(int16_t measuredValue);



/*------------------------------------------------------------------------------
 *  Global functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Initialize Termperature IC
 */
void TEMPERATURE_I2C_Init(void)
{
	
	HAL_I2C_MspInit(&I2cHandle);

	/*##-1- Configure the I2C peripheral #######################################*/
	#if CONFIG_USE_PANEL_NODESMALL
	// STM32F0
	
	I2cHandle.Instance             = I2Cx;

	I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	I2cHandle.Init.Timing          = I2Cx_TIMING;
	I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	I2cHandle.Init.OwnAddress1     = 0;
	I2cHandle.Init.OwnAddress2     = 0;
	I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;
	
	#elif CONFIG_USE_PANEL_NODEMEDIUM
	
	// STM32L1 :
	
	I2cHandle.Instance             = I2Cx;

	I2cHandle.Init.ClockSpeed      = 100000;		// 100 kHz
	I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_2;
	I2cHandle.Init.OwnAddress1     = 0x00;
	I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2cHandle.Init.OwnAddress2     = 0x00;
	I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

	#else
	
	#warning "Temperature I2C error."
	
	#endif
	
	
	
	if (HAL_I2C_Init(&I2cHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	/* Enable the Analog I2C Filter */
	#ifdef CONFIG_USE_PANEL_NODESMALL
	HAL_I2CEx_AnalogFilter_Config(&I2cHandle,I2C_ANALOGFILTER_ENABLED);
	#endif
	



}



ReturnType TEMPERATURE_GetTemperature(void)
{


	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
	{
	} 
	
	uint8_t TEMPERATURE_SendBuffer[1];
	TEMPERATURE_SendBuffer[0] = TEMPERATURE_I2C_REGISTER_TEMP;
	
	uint8_t TEMPERATURE_ReceiveBuffer[2];
	
	
	HAL_StatusTypeDef status;
	
	
	// Send pointer data
	status = HAL_I2C_Master_Transmit(&I2cHandle, ( ( TEMPERATURE_I2C_TEMP_ADDRESS << 1 ) | 0 ),TEMPERATURE_SendBuffer,1,5000);
	
		
	if ( status ==	HAL_OK )
	{
		//uprintf("I2C send OK\r\n");
	}
	else if (status == HAL_TIMEOUT )
	{
		uprintf("TIMEOUT on sending\r\n");
	}
	else if (status == HAL_BUSY )
	{
		uprintf("HAL_BUSY on sending\r\n");
	}	
	else if (status == HAL_ERROR )
	{
		uprintf("HAL_ERROR in sending\r\n");
	}	
	else
	{
		uprintf("Other problem\r\n");
	}	
	
	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
	{
	} 
	
	
	status = HAL_I2C_Master_Receive(&I2cHandle, ((TEMPERATURE_I2C_TEMP_ADDRESS << 1 ) | 1 ),TEMPERATURE_ReceiveBuffer,2,5000);
	
	if ( status ==	HAL_OK )
	{
		//uprintf("I2C received: 0x%b%b\r\n",TEMPERATURE_ReceiveBuffer[0],TEMPERATURE_ReceiveBuffer[1]);
	}
	else if (status == HAL_TIMEOUT )
	{
		uprintf("TIMEOUT\r\n");
	}
	else if (status == HAL_ERROR )
	{
		uprintf("ERROR\r\n");
	}
		else if (status == HAL_BUSY )
	{
		uprintf("BUSY\r\n");
	}
	else
	{
		uprintf("other problem\r\n");
	}
	

	
	// Convert value
	float temperature;
	int16_t receivedValue = 0;
	receivedValue = TEMPERATURE_ReceiveBuffer[0] <<8 | TEMPERATURE_ReceiveBuffer[1];
	temperature = TEMPERATURE_ConvertLM75AdTemperatureValue(receivedValue);
	
	// Save to Global variable
	GLOBAL_TemperatureValue_LM75 = temperature;
	
	// DebugPrint:
	//uprintf("Temperature: %f\r\n",GLOBAL_TemperatureValue_LM75);
	
	// Return Ok
	return Return_Ok;
	
}



float TEMPERATURE_ConvertLM75AdTemperatureValue(int16_t measuredValue)
{
	
	float calculatedValue;
	// fenti 11 biten ábrázolja az értéket
	measuredValue = measuredValue >> 5;
	
	
	// measuredValue / 8;
	calculatedValue = measuredValue * 0.125;
	
	return calculatedValue;

	
}



void TEMPERATURE_I2C_Init_Test(void)
{
	
	HAL_I2C_MspInit(&I2cHandle);

	/*##-1- Configure the I2C peripheral #######################################*/
	#ifdef CONFIG_USE_PANEL_NODESMALL
	I2cHandle.Instance             = I2Cx;

	I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	I2cHandle.Init.Timing          = I2Cx_TIMING;
	I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	I2cHandle.Init.OwnAddress1     = 0;
	I2cHandle.Init.OwnAddress2     = 0;
	I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;
	#endif
	
	
	#ifdef CONFIG_USE_PANEL_NODEMEDIUM
	
	//L1 example:
	I2cHandle.Instance             = I2Cx;

	//I2cHandle.Init.ClockSpeed      = 100000;		// 100 kHz?
	I2cHandle.Init.ClockSpeed      = 10000;		// 100 kHz?
	I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_2;
	//I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
	I2cHandle.Init.OwnAddress1     = 0x00;
	I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2cHandle.Init.OwnAddress2     = 0x00;
	I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

	#endif
	
	
	/*##-1- Configure the I2C peripheral ######################################*/
	/*
	I2cHandle.Instance             = I2Cx;

	I2cHandle.Init.Timing          = I2C_TIMING;
	I2cHandle.Init.OwnAddress1     = I2C_ADDRESS;
	I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_10BIT;
	I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	I2cHandle.Init.OwnAddress2     = 0xFF;
	I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;  
	*/
	
	if (HAL_I2C_Init(&I2cHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	/* Enable the Analog I2C Filter */
	#ifdef CONFIG_USE_PANEL_NODESMALL
	HAL_I2CEx_AnalogFilter_Config(&I2cHandle,I2C_ANALOGFILTER_ENABLED);
	#endif
	
	
//#define I2C_2
#define HAL_I2C_MY_1
	
	
	
	
	#ifdef I2C_2
	

	uint8_t TEMPERATURE_SendBuffer[1];
	TEMPERATURE_SendBuffer[0] = TEMPERATURE_I2C_REGISTER_TEMP;
	
	uint8_t TEMPERATURE_ReceiveBuffer[2];
	
	

	HAL_Delay(1000);
	


	/* The board sends the message and expects to receive it back */

	/*##-2- Start the transmission process #####################################*/  
	/* While the I2C in reception process, user can transmit data through 
	"aTxBuffer" buffer */
	/* Timeout is set to 10S */
	while(HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)TEMPERATURE_I2C_TEMP_ADDRESS, (uint8_t*)TEMPERATURE_SendBuffer, 1, 10000)!= HAL_OK)
	{
	/* Error_Handler() function is called when Timout error occurs.
	When Acknowledge failure ocucurs (Slave don't acknowledge it's address)
	Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{
			Error_Handler();
		}
	}


	

	/*##-3- Put I2C peripheral in reception process ############################*/ 
	/* Timeout is set to 10S */ 
	while(HAL_I2C_Master_Receive(&I2cHandle, (uint16_t)TEMPERATURE_I2C_TEMP_ADDRESS, (uint8_t *)TEMPERATURE_ReceiveBuffer, 2, 10000) != HAL_OK)
	{
	/* Error_Handler() function is called when Timout error occurs.
	When Acknowledge failure ocucurs (Slave don't acknowledge it's address)
	Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{
			Error_Handler();
		}
	}

	uprintf("I2C: %b %b",TEMPERATURE_ReceiveBuffer[0],TEMPERATURE_ReceiveBuffer[1]);
	
	
	#endif
	
	
	
	#ifdef HAL_I2C_MY_1
	
	
	while(1)
	{
		while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
		{
		} 
		
		uint8_t TEMPERATURE_SendBuffer[1];
		TEMPERATURE_SendBuffer[0] = TEMPERATURE_I2C_REGISTER_TEMP;
		
		uint8_t TEMPERATURE_ReceiveBuffer[2];
		
		

		//HAL_I2C_Master_Transmit
		HAL_StatusTypeDef status;
		
		// It is OK: (address + read)
		//status = HAL_I2C_Master_Transmit(&I2cHandle, ( ( TEMPERATURE_I2C_TEMP_ADDRESS << 1 ) | 1 ),TEMPERATURE_SendBuffer,1,5000);
		
		
		status = HAL_I2C_Master_Transmit(&I2cHandle, ( ( TEMPERATURE_I2C_TEMP_ADDRESS << 1 ) | 0 ),TEMPERATURE_SendBuffer,1,5000);
		
		//HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
		//HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
		//status = HAL_I2C_Mem_Read(&I2cHandle, TEMPERATURE_I2C_TEMP_ADDRESS, 00, I2C_MEMADD_SIZE_8BIT, TEMPERATURE_ReceiveBuffer, 2, 5000);
		// Send pointer data
			
		if ( status ==	HAL_OK )
		{
			uprintf("I2C send OK\r\n");
		}
		else if (status == HAL_TIMEOUT )
		{
			uprintf("TIMEOUT on sending\r\n");
		}
		else if (status == HAL_BUSY )
		{
			uprintf("HAL_BUSY on sending\r\n");
		}	
		else if (status == HAL_ERROR )
		{
			uprintf("HAL_ERROR in sending\r\n");
		}	
		else
		{
			uprintf("Other problem\r\n");
		}	
		
		while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
		{
		} 
		
		
		//HAL_StatusTypeDef status;
		status = HAL_I2C_Master_Receive(&I2cHandle, ((TEMPERATURE_I2C_TEMP_ADDRESS << 1 ) | 1 ),TEMPERATURE_ReceiveBuffer,2,5000);
		
		if ( status ==	HAL_OK )
		{
			uprintf("I2C received: 0x%b%b\r\n",TEMPERATURE_ReceiveBuffer[0],TEMPERATURE_ReceiveBuffer[1]);
		}
		else if (status == HAL_TIMEOUT )
		{
			uprintf("TIMEOUT\r\n");
		}
		else if (status == HAL_ERROR )
		{
			uprintf("ERROR\r\n");
		}
			else if (status == HAL_BUSY )
		{
			uprintf("BUSY\r\n");
		}
		else
		{
			uprintf("other problem\r\n");
		}
		
		vTaskDelay(3000);
	} // End of while(1)
	
	#endif
	

	#if 0
	/* The board sends the message to EEPROM then reads it back */
	/*##-2- Start writing process ##############################################*/
	/* Initialize Remaining Bytes Value to TX Buffer Size */
	Remaining_Bytes = TXBUFFERSIZE;
	/* Initialize Memory address to 0 since EEPROM write will start from address 0 */
	Memory_Address = 0;
	/* Since page size is 4 bytes, the write procedure will be done in a loop */
	while (Remaining_Bytes > 0)
	{
	/* Write 4 bytes */
	if (HAL_I2C_Mem_Write_DMA(&I2cHandle, (uint16_t)I2C_ADDRESS, Memory_Address, I2C_MEMADD_SIZE_16BIT, (uint8_t *)(aTxBuffer + Memory_Address), 4) != HAL_OK)
	{
	  /* Writing process Error */
	  Error_Handler();
	}

	/* Wait for the end of the transfer */
	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
	{
	}

	/* Check if the EEPROM is ready for a new operation */
	while (HAL_I2C_IsDeviceReady(&I2cHandle, I2C_ADDRESS, 20, 300) == HAL_TIMEOUT);

	/* Wait for the end of the transfer */
	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
	{
	}

	/* Update Remaining bytes and Memory Address values */
	Remaining_Bytes -= 4;
	Memory_Address += 4;
	}
	
	/*##-3- Start reading process ##############################################*/
	if (HAL_I2C_Mem_Read_DMA(&I2cHandle, (uint16_t)I2C_ADDRESS, 0, I2C_MEMADD_SIZE_16BIT, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
	{
	/* Reading process Error */
	Error_Handler();
	}

	/* Wait for the end of the transfer */
	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
	{
	}

	/*##-4- Compare the sent and received buffers ##############################*/
	if (Buffercmp((uint8_t *)aTxBuffer, (uint8_t *)aRxBuffer, RXBUFFERSIZE))
	{
	/* Processing Error */
	Error_Handler();
	}
	#endif

}
 
 
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
#ifdef USE_TEMPERATURE_I2C_DMA_AND_IT
	static DMA_HandleTypeDef hdma_tx = { 0 };
	static DMA_HandleTypeDef hdma_rx = { 0 };
#endif

	GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	I2Cx_SCL_GPIO_CLK_ENABLE();
	I2Cx_SDA_GPIO_CLK_ENABLE();
	/* Enable I2C1 clock */
	I2Cx_CLK_ENABLE();
	
	#ifdef USE_TEMPERATURE_I2C_DMA_AND_IT
	/* Enable DMA2 clock */
	DMAx_CLK_ENABLE();
	#endif

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* I2C TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = I2Cx_SCL_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = I2Cx_SCL_AF;

	HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);

	/* I2C RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = I2Cx_SDA_PIN;
	GPIO_InitStruct.Alternate = I2Cx_SDA_AF;

	HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);


#ifdef USE_TEMPERATURE_I2C_DMA_AND_IT
	/*##-3- Configure the DMA ##################################################*/
	// Configure the DMA handler for Transmission process
	hdma_tx.Instance                 = I2Cx_TX_DMA_INSTANCE;

	hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_tx.Init.Mode                = DMA_NORMAL;
	hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;

	HAL_DMA_Init(&hdma_tx);

	// Associate the initialized DMA handle to the the I2C handle
	__HAL_LINKDMA(hi2c, hdmatx, hdma_tx);

	// Configure the DMA handler for Transmission process
	hdma_rx.Instance                 = I2Cx_RX_DMA_INSTANCE;

	hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_rx.Init.Mode                = DMA_NORMAL;
	hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;

	HAL_DMA_Init(&hdma_rx);

	// Associate the initialized DMA handle to the the I2C handle
	__HAL_LINKDMA(hi2c, hdmarx, hdma_rx);

	//##-4- Configure the NVIC for DMA #########################################
	// NVIC configuration for DMA transfer complete interrupt (I2C1_TX)
	HAL_NVIC_SetPriority(I2Cx_DMA_TX_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(I2Cx_DMA_TX_IRQn);

	// NVIC configuration for DMA transfer complete interrupt (I2C1_RX)
	HAL_NVIC_SetPriority(I2Cx_DMA_RX_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(I2Cx_DMA_RX_IRQn);
#endif

}

/**
  * @brief I2C MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
#ifdef USE_TEMPERATURE_I2C_DMA_AND_IT
	static DMA_HandleTypeDef hdma_tx;
	static DMA_HandleTypeDef hdma_rx;
#endif

	/*##-1- Reset peripherals ##################################################*/
	I2Cx_FORCE_RESET();
	I2Cx_RELEASE_RESET();

	/*##-2- Disable peripherals and GPIO Clocks ################################*/
	/* Configure I2C Tx as alternate function  */
	HAL_GPIO_DeInit(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN);
	/* Configure I2C Rx as alternate function  */
	HAL_GPIO_DeInit(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN);

#ifdef USE_TEMPERATURE_I2C_DMA_AND_IT
	/*##-3- Disable the DMA ####################################################*/
	/* De-Initialize the DMA associated to transmission process */
	HAL_DMA_DeInit(&hdma_tx);
	/* De-Initialize the DMA associated to reception process */
	HAL_DMA_DeInit(&hdma_rx);

	/*##-4- Disable the NVIC for DMA ###########################################*/
	HAL_NVIC_DisableIRQ(I2Cx_DMA_TX_IRQn);
	HAL_NVIC_DisableIRQ(I2Cx_DMA_RX_IRQn);
#endif
}




/**
  * @brief  Tx Transfer completed callback.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report end of DMA Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
	/* Turn LED1 on: Transfer in transmission process is correct */
	//BSP_LED_On(LED1);
	LED_BLUE_TOGGLE();
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
	/* Turn LED2 on: Transfer in reception process is correct */
	//BSP_LED_On(LED2);
	LED_GREEN_TOGGLE();
}

/**
  * @brief  I2C error callbacks.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
	/* Turn LED3 on: Transfer error in reception/transmission process */
	//BSP_LED_On(LED3);
	LED_RED_ON();
}



#endif // #ifdef CONFIG_MODULE_TEMPERATURE_ENABLE


