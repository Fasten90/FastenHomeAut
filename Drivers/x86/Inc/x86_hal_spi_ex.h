/*
 *      x86_hal_spi_ex.h
 *      Created on:     2019-05-05
 *      Author:         Vizi Gábor
 *      E-mail:         vizi.gabor90@gmail.com
 *      Function:       -
 *      Target:         x86
 *      Version:        -
 *      Last modified:  2019-05-05
 *      Note:           This file was copied from stm32f0xx_hal_spi_ex.h [STMicroelectronics file]
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F0xx_HAL_SPI_EX_H
#define __STM32F0xx_HAL_SPI_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "x86_hal_def.h"

/** @addtogroup STM32F0xx_HAL_Driver
  * @{
  */

/** @addtogroup SPIEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @addtogroup SPIEx_Exported_Functions
  * @{
  */

/* Initialization and de-initialization functions  ****************************/
/* IO operation functions *****************************************************/
/** @addtogroup SPIEx_Exported_Functions_Group1
  * @{
  */
HAL_StatusTypeDef HAL_SPIEx_FlushRxFifo(SPI_HandleTypeDef *hspi);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F0xx_HAL_SPI_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
