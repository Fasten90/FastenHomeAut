/*
 *    CommonSPI.c
 *    Created on:   2017-03-16
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Common SPI
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "compiler.h"
#include "CommonSPI.h"



#ifdef CONFIG_MODULE_COMMONSPI_ENABLE

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

/*
 * TODO: This module is not finished!!
 */


/**
 * @brief    Common SPI initialization for general SPI message sending (IT mode)
 */
void CommonSPI_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    SPIx_SCK_GPIO_CLK_ENABLE();
    SPIx_MISO_GPIO_CLK_ENABLE();
    SPIx_MOSI_GPIO_CLK_ENABLE();
    /* Enable SPI clock */
    SPIx_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
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

    /*##-3- Configure the NVIC for SPI #########################################*/
    /* NVIC for SPI */
    HAL_NVIC_SetPriority(SPIx_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(SPIx_IRQn);




      /*##-1- Configure the SPI peripheral #######################################*/
      /* Set the SPI parameters */
      SpiHandle.Instance               = SPIx;
      SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
      SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
      SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
      SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
      SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
      SpiHandle.Init.CRCPolynomial     = 7;
      SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
      SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
      SpiHandle.Init.NSS               = SPI_NSS_SOFT;
      SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;

    #ifdef MASTER_BOARD
      SpiHandle.Init.Mode = SPI_MODE_MASTER;
    #else
      SpiHandle.Init.Mode = SPI_MODE_SLAVE;
    #endif /* MASTER_BOARD */

      if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
      {
        /* Initialization Error */
        Error_Handler();
      }

    #ifdef MASTER_BOARD
      /* Configure USER Button */
      BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

      /* Wait for USER Button press before starting the Communication */
      while (BSP_PB_GetState(BUTTON_KEY) != 1)
      {
        BSP_LED_Toggle(LED3);
        HAL_Delay(40);
      }

        BSP_LED_Off(LED3);
    #endif /* MASTER_BOARD */

      /*##-2- Start the Full Duplex Communication process ########################*/
      /* While the SPI in TransmitReceive process, user can transmit data through
         "aTxBuffer" buffer & receive data through "aRxBuffer" */
      if(HAL_SPI_TransmitReceive_IT(&SpiHandle, (uint8_t*)aTxBuffer, (uint8_t *)aRxBuffer, BUFFERSIZE) != HAL_OK)
      {
        /* Transfer error in transmission process */
        Error_Handler();
      }

      /*##-3- Wait for the end of the transfer ###################################*/
      /*  Before starting a new communication transfer, you need to check the current
          state of the peripheral; if it’s busy you need to wait for the end of current
          transfer before starting a new one.
          For simplicity reasons, this example is just waiting till the end of the
          transfer, but application may perform other tasks while transfer operation
          is ongoing. */
      while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY)
      {
      }
}



#endif    /* CONFIG_MODULE_COMMONSPI_ENABLE */
