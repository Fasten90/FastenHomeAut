/*
 *    x86_hal_dma.h
 *    Created on:   2019-05-05
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       x86
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef X86_HAL_DMA_H_
#define X86_HAL_DMA_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "x86_hal_def.h"

/** @addtogroup STM32F0xx_HAL_Driver
  * @{
  */

/** @addtogroup DMA
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup DMA_Exported_Types DMA Exported Types
  * @{
  */

/**
  * @brief  DMA Configuration Structure definition
  */
typedef struct
{
  uint32_t Direction;                 /*!< Specifies if the data will be transferred from memory to peripheral,
                                           from memory to memory or from peripheral to memory.
                                           This parameter can be a value of @ref DMA_Data_transfer_direction */

  uint32_t PeriphInc;                 /*!< Specifies whether the Peripheral address register should be incremented or not.
                                           This parameter can be a value of @ref DMA_Peripheral_incremented_mode */

  uint32_t MemInc;                    /*!< Specifies whether the memory address register should be incremented or not.
                                           This parameter can be a value of @ref DMA_Memory_incremented_mode */

  uint32_t PeriphDataAlignment;       /*!< Specifies the Peripheral data width.
                                           This parameter can be a value of @ref DMA_Peripheral_data_size */

  uint32_t MemDataAlignment;          /*!< Specifies the Memory data width.
                                           This parameter can be a value of @ref DMA_Memory_data_size */

  uint32_t Mode;                      /*!< Specifies the operation mode of the DMAy Channelx.
                                           This parameter can be a value of @ref DMA_mode
                                           @note The circular buffer mode cannot be used if the memory-to-memory
                                                 data transfer is configured on the selected Channel */

  uint32_t Priority;                   /*!< Specifies the software priority for the DMAy Channelx.
                                            This parameter can be a value of @ref DMA_Priority_level */
} DMA_InitTypeDef;

/**
  * @brief  HAL DMA State structures definition
  */
typedef enum
{
  HAL_DMA_STATE_RESET             = 0x00U,  /*!< DMA not yet initialized or disabled */
  HAL_DMA_STATE_READY             = 0x01U,  /*!< DMA initialized and ready for use   */
  HAL_DMA_STATE_BUSY              = 0x02U,  /*!< DMA process is ongoing              */
  HAL_DMA_STATE_TIMEOUT           = 0x03U   /*!< DMA timeout state                   */
}HAL_DMA_StateTypeDef;

/**
  * @brief  HAL DMA Error Code structure definition
  */
typedef enum
{
  HAL_DMA_FULL_TRANSFER      = 0x00U,    /*!< Full transfer     */
  HAL_DMA_HALF_TRANSFER      = 0x01U     /*!< Half Transfer     */
}HAL_DMA_LevelCompleteTypeDef;

/**
  * @brief  HAL DMA Callback ID structure definition
  */
typedef enum
{
  HAL_DMA_XFER_CPLT_CB_ID          = 0x00U,    /*!< Full transfer     */
  HAL_DMA_XFER_HALFCPLT_CB_ID      = 0x01U,    /*!< Half transfer     */
  HAL_DMA_XFER_ERROR_CB_ID         = 0x02U,    /*!< Error             */
  HAL_DMA_XFER_ABORT_CB_ID         = 0x03U,    /*!< Abort             */
  HAL_DMA_XFER_ALL_CB_ID           = 0x04U     /*!< All               */

}HAL_DMA_CallbackIDTypeDef;

/**
  * @brief  DMA handle Structure definition
  */
typedef struct __DMA_HandleTypeDef
{
  DMA_Channel_TypeDef   *Instance;                                                    /*!< Register base address                  */

  DMA_InitTypeDef       Init;                                                         /*!< DMA communication parameters           */

  HAL_LockTypeDef       Lock;                                                         /*!< DMA locking object                     */

  __IO HAL_DMA_StateTypeDef  State;                                                   /*!< DMA transfer state                     */

  void                  *Parent;                                                      /*!< Parent object state                    */

  void                  (* XferCpltCallback)( struct __DMA_HandleTypeDef * hdma);     /*!< DMA transfer complete callback         */

  void                  (* XferHalfCpltCallback)( struct __DMA_HandleTypeDef * hdma); /*!< DMA Half transfer complete callback    */

  void                  (* XferErrorCallback)( struct __DMA_HandleTypeDef * hdma);    /*!< DMA transfer error callback            */

  void                  (* XferAbortCallback)( struct __DMA_HandleTypeDef * hdma);    /*!< DMA transfer abort callback            */

  __IO uint32_t         ErrorCode;                                                    /*!< DMA Error code                         */

  DMA_TypeDef          *DmaBaseAddress;                                               /*!< DMA Channel Base Address                */

  uint32_t              ChannelIndex;                                                 /*!< DMA Channel Index                       */
} DMA_HandleTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup DMA_Exported_Constants DMA Exported Constants
  * @{
  */

/** @defgroup DMA_Error_Code DMA Error Code
  * @{
  */
#define HAL_DMA_ERROR_NONE          (0x00000000U)    /*!< No error             */
#define HAL_DMA_ERROR_TE            (0x00000001U)    /*!< Transfer error       */
#define HAL_DMA_ERROR_NO_XFER       (0x00000004U)    /*!< no ongoin transfer   */
#define HAL_DMA_ERROR_TIMEOUT       (0x00000020U)    /*!< Timeout error        */
#define HAL_DMA_ERROR_NOT_SUPPORTED (0x00000100U)    /*!< Not supported mode */
/**
  * @}
  */

/** @defgroup DMA_Data_transfer_direction DMA Data transfer direction
  * @{
  */
#define DMA_PERIPH_TO_MEMORY         (0x00000000U)        /*!< Peripheral to memory direction */
#define DMA_MEMORY_TO_PERIPH         ((uint32_t)DMA_CCR_DIR)       /*!< Memory to peripheral direction */
#define DMA_MEMORY_TO_MEMORY         ((uint32_t)(DMA_CCR_MEM2MEM)) /*!< Memory to memory direction     */

/**
  * @}
  */

/** @defgroup DMA_Peripheral_incremented_mode DMA Peripheral incremented mode
  * @{
  */
#define DMA_PINC_ENABLE        ((uint32_t)DMA_CCR_PINC)  /*!< Peripheral increment mode Enable */
#define DMA_PINC_DISABLE       (0x00000000U)    /*!< Peripheral increment mode Disable */
/**
  * @}
  */

/** @defgroup DMA_Memory_incremented_mode DMA Memory incremented mode
  * @{
  */
#define DMA_MINC_ENABLE         ((uint32_t)DMA_CCR_MINC)  /*!< Memory increment mode Enable  */
#define DMA_MINC_DISABLE        (0x00000000U)    /*!< Memory increment mode Disable */
/**
  * @}
  */

/** @defgroup DMA_Peripheral_data_size DMA Peripheral data size
  * @{
  */
#define DMA_PDATAALIGN_BYTE          (0x00000000U)       /*!< Peripheral data alignment : Byte     */
#define DMA_PDATAALIGN_HALFWORD      ((uint32_t)DMA_CCR_PSIZE_0)  /*!< Peripheral data alignment : HalfWord */
#define DMA_PDATAALIGN_WORD          ((uint32_t)DMA_CCR_PSIZE_1)  /*!< Peripheral data alignment : Word     */
/**
  * @}
  */

/** @defgroup DMA_Memory_data_size DMA Memory data size
  * @{
  */
#define DMA_MDATAALIGN_BYTE          (0x00000000U)       /*!< Memory data alignment : Byte     */
#define DMA_MDATAALIGN_HALFWORD      ((uint32_t)DMA_CCR_MSIZE_0)  /*!< Memory data alignment : HalfWord */
#define DMA_MDATAALIGN_WORD          ((uint32_t)DMA_CCR_MSIZE_1)  /*!< Memory data alignment : Word     */
/**
  * @}
  */

/** @defgroup DMA_mode DMA mode
  * @{
  */
#define DMA_NORMAL         (0x00000000U)      /*!< Normal Mode                  */
#define DMA_CIRCULAR       ((uint32_t)DMA_CCR_CIRC)    /*!< Circular Mode                */
/**
  * @}
  */

/** @defgroup DMA_Priority_level DMA Priority level
  * @{
  */
#define DMA_PRIORITY_LOW             (0x00000000U)    /*!< Priority level : Low       */
#define DMA_PRIORITY_MEDIUM          ((uint32_t)DMA_CCR_PL_0)  /*!< Priority level : Medium    */
#define DMA_PRIORITY_HIGH            ((uint32_t)DMA_CCR_PL_1)  /*!< Priority level : High      */
#define DMA_PRIORITY_VERY_HIGH       ((uint32_t)DMA_CCR_PL)    /*!< Priority level : Very_High */
/**
  * @}
  */


/** @defgroup DMA_interrupt_enable_definitions DMA interrupt enable definitions
  * @{
  */
#define DMA_IT_TC                         ((uint32_t)DMA_CCR_TCIE)
#define DMA_IT_HT                         ((uint32_t)DMA_CCR_HTIE)
#define DMA_IT_TE                         ((uint32_t)DMA_CCR_TEIE)
/**
  * @}
  */

/** @defgroup DMA_flag_definitions DMA flag definitions
  * @{
  */

#define DMA_FLAG_GL1                      (0x00000001U) /*!< Channel 1 global interrupt flag  */
#define DMA_FLAG_TC1                      (0x00000002U) /*!< Channel 1 transfer complete flag */
#define DMA_FLAG_HT1                      (0x00000004U) /*!< Channel 1 half transfer flag     */
#define DMA_FLAG_TE1                      (0x00000008U) /*!< Channel 1 transfer error flag    */
#define DMA_FLAG_GL2                      (0x00000010U) /*!< Channel 2 global interrupt flag  */
#define DMA_FLAG_TC2                      (0x00000020U) /*!< Channel 2 transfer complete flag */
#define DMA_FLAG_HT2                      (0x00000040U) /*!< Channel 2 half transfer flag     */
#define DMA_FLAG_TE2                      (0x00000080U) /*!< Channel 2 transfer error flag    */
#define DMA_FLAG_GL3                      (0x00000100U) /*!< Channel 3 global interrupt flag  */
#define DMA_FLAG_TC3                      (0x00000200U) /*!< Channel 3 transfer complete flag */
#define DMA_FLAG_HT3                      (0x00000400U) /*!< Channel 3 half transfer flag     */
#define DMA_FLAG_TE3                      (0x00000800U) /*!< Channel 3 transfer error flag    */
#define DMA_FLAG_GL4                      (0x00001000U) /*!< Channel 4 global interrupt flag  */
#define DMA_FLAG_TC4                      (0x00002000U) /*!< Channel 4 transfer complete flag */
#define DMA_FLAG_HT4                      (0x00004000U) /*!< Channel 4 half transfer flag     */
#define DMA_FLAG_TE4                      (0x00008000U) /*!< Channel 4 transfer error flag    */
#define DMA_FLAG_GL5                      (0x00010000U) /*!< Channel 5 global interrupt flag  */
#define DMA_FLAG_TC5                      (0x00020000U) /*!< Channel 5 transfer complete flag */
#define DMA_FLAG_HT5                      (0x00040000U) /*!< Channel 5 half transfer flag     */
#define DMA_FLAG_TE5                      (0x00080000U) /*!< Channel 5 transfer error flag    */
#define DMA_FLAG_GL6                      (0x00100000U) /*!< Channel 6 global interrupt flag  */
#define DMA_FLAG_TC6                      (0x00200000U) /*!< Channel 6 transfer complete flag */
#define DMA_FLAG_HT6                      (0x00400000U) /*!< Channel 6 half transfer flag     */
#define DMA_FLAG_TE6                      (0x00800000U) /*!< Channel 6 transfer error flag    */
#define DMA_FLAG_GL7                      (0x01000000U) /*!< Channel 7 global interrupt flag  */
#define DMA_FLAG_TC7                      (0x02000000U) /*!< Channel 7 transfer complete flag */
#define DMA_FLAG_HT7                      (0x04000000U) /*!< Channel 7 half transfer flag     */
#define DMA_FLAG_TE7                      (0x08000000U) /*!< Channel 7 transfer error flag    */

/**
  * @}
  */

#if defined(SYSCFG_CFGR1_DMA_RMP)
/** @defgroup HAL_DMA_remapping HAL DMA remapping
  *        Elements values convention: 0xYYYYYYYY
  *           - YYYYYYYY  : Position in the SYSCFG register CFGR1
  * @{
  */
#define DMA_REMAP_ADC_DMA_CH2         ((uint32_t)SYSCFG_CFGR1_ADC_DMA_RMP) /*!< ADC DMA remap
                                                                         0: No remap (ADC DMA requests mapped on DMA channel 1
                                                                         1: Remap (ADC DMA requests mapped on DMA channel 2 */
#define DMA_REMAP_USART1_TX_DMA_CH4   ((uint32_t)SYSCFG_CFGR1_USART1TX_DMA_RMP) /*!< USART1 TX DMA remap
                                                                         0: No remap (USART1_TX DMA request mapped on DMA channel 2
                                                                         1: Remap (USART1_TX DMA request mapped on DMA channel 4 */
#define DMA_REMAP_USART1_RX_DMA_CH5   ((uint32_t)SYSCFG_CFGR1_USART1RX_DMA_RMP) /*!< USART1 RX DMA remap
                                                                         0: No remap (USART1_RX DMA request mapped on DMA channel 3
                                                                         1: Remap (USART1_RX DMA request mapped on DMA channel 5 */
#define DMA_REMAP_TIM16_DMA_CH4       ((uint32_t)SYSCFG_CFGR1_TIM16_DMA_RMP) /*!< TIM16 DMA request remap
                                                                         0: No remap (TIM16_CH1 and TIM16_UP DMA requests mapped on DMA channel 3)
                                                                         1: Remap (TIM16_CH1 and TIM16_UP DMA requests mapped on DMA channel 4) */
#define DMA_REMAP_TIM17_DMA_CH2       ((uint32_t)SYSCFG_CFGR1_TIM17_DMA_RMP) /*!< TIM17 DMA request remap
                                                                         0: No remap (TIM17_CH1 and TIM17_UP DMA requests mapped on DMA channel 1
                                                                         1: Remap (TIM17_CH1 and TIM17_UP DMA requests mapped on DMA channel 2) */
#if defined (STM32F070xB)
#define DMA_REMAP_USART3_DMA_CH32     ((uint32_t)SYSCFG_CFGR1_USART3_DMA_RMP) /*!< USART3 DMA request remapping bit. Available on STM32F070xB devices only.
                                                                         0: Disabled, need to remap before use
                                                                         1: Remap (USART3_RX and USART3_TX DMA requests mapped on DMA channel 3 and 2 respectively) */

#endif

#if defined (STM32F071xB) || defined (STM32F072xB) || defined (STM32F078xx)
#define DMA_REMAP_TIM16_DMA_CH6       ((uint32_t)SYSCFG_CFGR1_TIM16_DMA_RMP2) /*!< TIM16 alternate DMA request remapping bit. Available on STM32F07x devices only
                                                                         0: No alternate remap (TIM16 DMA requestsmapped according to TIM16_DMA_RMP bit)
                                                                         1: Alternate remap (TIM16_CH1 and TIM16_UP DMA requests mapped on DMA channel 6) */
#define DMA_REMAP_TIM17_DMA_CH7       ((uint32_t)SYSCFG_CFGR1_TIM17_DMA_RMP2) /*!< TIM17 alternate DMA request remapping bit. Available on STM32F07x devices only
                                                                         0: No alternate remap (TIM17 DMA requestsmapped according to TIM17_DMA_RMP bit)
                                                                         1: Alternate remap (TIM17_CH1 and TIM17_UP DMA requests mapped on DMA channel 7) */
#define DMA_REMAP_SPI2_DMA_CH67       ((uint32_t)SYSCFG_CFGR1_SPI2_DMA_RMP) /*!< SPI2 DMA request remapping bit. Available on STM32F07x devices only.
                                                                         0: No remap (SPI2_RX and SPI2_TX DMA requests mapped on DMA channel 4 and 5 respectively)
                                                                         1: Remap (SPI2_RX and SPI2_TX DMA requests mapped on DMA channel 6 and 7 respectively) */
#define DMA_REMAP_USART2_DMA_CH67     ((uint32_t)SYSCFG_CFGR1_USART2_DMA_RMP) /*!< USART2 DMA request remapping bit. Available on STM32F07x devices only.
                                                                         0: No remap (USART2_RX and USART2_TX DMA requests mapped on DMA channel 5 and 4 respectively)
                                                                         1: 1: Remap (USART2_RX and USART2_TX DMA requests mapped on DMA channel 6 and 7 respectively) */
#define DMA_REMAP_USART3_DMA_CH32     ((uint32_t)SYSCFG_CFGR1_USART3_DMA_RMP) /*!< USART3 DMA request remapping bit. Available on STM32F07x devices only.
                                                                         0: No remap (USART3_RX and USART3_TX DMA requests mapped on DMA channel 6 and 7 respectively)
                                                                         1: Remap (USART3_RX and USART3_TX DMA requests mapped on DMA channel 3 and 2 respectively) */
#define DMA_REMAP_I2C1_DMA_CH76       ((uint32_t)SYSCFG_CFGR1_I2C1_DMA_RMP) /*!< I2C1 DMA request remapping bit. Available on STM32F07x devices only.
                                                                         0: No remap (I2C1_RX and I2C1_TX DMA requests mapped on DMA channel 3 and 2 respectively)
                                                                         1: Remap (I2C1_RX and I2C1_TX DMA requests mapped on DMA channel 7 and 6 respectively) */
#define DMA_REMAP_TIM1_DMA_CH6        ((uint32_t)SYSCFG_CFGR1_TIM1_DMA_RMP) /*!< TIM1 DMA request remapping bit. Available on STM32F07x devices only.
                                                                         0: No remap (TIM1_CH1, TIM1_CH2 and TIM1_CH3 DMA requests mapped on DMA channel 2, 3 and 4 respectively)
                                                                         1: Remap (TIM1_CH1, TIM1_CH2 and TIM1_CH3 DMA requests mapped on DMA channel 6 */
#define DMA_REMAP_TIM2_DMA_CH7        ((uint32_t)SYSCFG_CFGR1_TIM2_DMA_RMP) /*!< TIM2 DMA request remapping bit. Available on STM32F07x devices only.
                                                                         0: No remap (TIM2_CH2 and TIM2_CH4 DMA requests mapped on DMA channel 3 and 4 respectively)
                                                                         1: Remap (TIM2_CH2 and TIM2_CH4 DMA requests mapped on DMA channel 7 */
#define DMA_REMAP_TIM3_DMA_CH6        ((uint32_t)SYSCFG_CFGR1_TIM3_DMA_RMP) /*!< TIM3 DMA request remapping bit. Available on STM32F07x devices only.
                                                                         0: No remap (TIM3_CH1 and TIM3_TRIG DMA requests mapped on DMA channel 4)
                                                                         1: Remap (TIM3_CH1 and TIM3_TRIG DMA requests mapped on DMA channel 6) */
#endif

/**
  * @}
  */

#endif /* SYSCFG_CFGR1_DMA_RMP */
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup DMA_Exported_Macros DMA Exported Macros
  * @{
  */

/** @brief  Reset DMA handle state
  * @param  __HANDLE__: DMA handle.
  * @retval None
  */
#define __HAL_DMA_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_DMA_STATE_RESET)

/**
  * @brief  Enable the specified DMA Channel.
  * @param  __HANDLE__: DMA handle
  * @retval None
  */
#define __HAL_DMA_ENABLE(__HANDLE__)        ((__HANDLE__)->Instance->CCR |= DMA_CCR_EN)

/**
  * @brief  Disable the specified DMA Channel.
  * @param  __HANDLE__: DMA handle
  * @retval None
  */
#define __HAL_DMA_DISABLE(__HANDLE__)       ((__HANDLE__)->Instance->CCR &= ~DMA_CCR_EN)


/* Interrupt & Flag management */

/**
  * @brief  Enables the specified DMA Channel interrupts.
  * @param  __HANDLE__: DMA handle
  * @param __INTERRUPT__: specifies the DMA interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg DMA_IT_TC:  Transfer complete interrupt mask
  *            @arg DMA_IT_HT:  Half transfer complete interrupt mask
  *            @arg DMA_IT_TE:  Transfer error interrupt mask
  * @retval None
  */
#define __HAL_DMA_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->CCR |= (__INTERRUPT__))

/**
  * @brief  Disables the specified DMA Channel interrupts.
  * @param  __HANDLE__: DMA handle
  * @param __INTERRUPT__: specifies the DMA interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg DMA_IT_TC:  Transfer complete interrupt mask
  *            @arg DMA_IT_HT:  Half transfer complete interrupt mask
  *            @arg DMA_IT_TE:  Transfer error interrupt mask
  * @retval None
  */
#define __HAL_DMA_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((__HANDLE__)->Instance->CCR &= ~(__INTERRUPT__))

/**
  * @brief  Checks whether the specified DMA Channel interrupt is enabled or disabled.
  * @param  __HANDLE__: DMA handle
  * @param  __INTERRUPT__: specifies the DMA interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg DMA_IT_TC:  Transfer complete interrupt mask
  *            @arg DMA_IT_HT:  Half transfer complete interrupt mask
  *            @arg DMA_IT_TE:  Transfer error interrupt mask
  * @retval The state of DMA_IT (SET or RESET).
  */
#define __HAL_DMA_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)  (((__HANDLE__)->Instance->CCR & (__INTERRUPT__)))

/**
  * @brief  Returns the number of remaining data units in the current DMAy Channelx transfer.
  * @param  __HANDLE__: DMA handle
  *
  * @retval The number of remaining data units in the current DMA Channel transfer.
  */
#define __HAL_DMA_GET_COUNTER(__HANDLE__) ((__HANDLE__)->Instance->CNDTR)

#if defined(SYSCFG_CFGR1_DMA_RMP)
/** @brief  DMA remapping enable/disable macros
  * @param __DMA_REMAP__: This parameter can be a value of @ref HAL_DMA_remapping
  */
#define __HAL_DMA_REMAP_CHANNEL_ENABLE(__DMA_REMAP__)   do {assert_param(IS_DMA_REMAP((__DMA_REMAP__)));                  \
                                                           SYSCFG->CFGR1 |= (__DMA_REMAP__);                              \
                                                         }while(0)
#define __HAL_DMA_REMAP_CHANNEL_DISABLE(__DMA_REMAP__)  do {assert_param(IS_DMA_REMAP((__DMA_REMAP__)));                  \
                                                           SYSCFG->CFGR1 &= ~(__DMA_REMAP__);                             \
                                                         }while(0)
#endif /* SYSCFG_CFGR1_DMA_RMP */

/**
  * @}
  */

/* Include DMA HAL Extension module */
#include "x86_hal_dma_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup DMA_Exported_Functions
  * @{
  */

/** @addtogroup DMA_Exported_Functions_Group1
  * @{
  */
/* Initialization and de-initialization functions *****************************/
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_DeInit (DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/** @addtogroup DMA_Exported_Functions_Group2
  * @{
  */
/* Input and Output operation functions *****************************************************/
HAL_StatusTypeDef HAL_DMA_Start (DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *hdma, uint32_t CompleteLevel, uint32_t Timeout);
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID, void (* pCallback)( DMA_HandleTypeDef * _hdma));
HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID);

/**
  * @}
  */

/** @addtogroup DMA_Exported_Functions_Group3
  * @{
  */
/* Peripheral State and Error functions ***************************************/
HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma);
uint32_t             HAL_DMA_GetError(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup DMA_Private_Macros
  * @{
  */
#define IS_DMA_DIRECTION(DIRECTION) (((DIRECTION) == DMA_PERIPH_TO_MEMORY ) || \
                                     ((DIRECTION) == DMA_MEMORY_TO_PERIPH)  || \
                                     ((DIRECTION) == DMA_MEMORY_TO_MEMORY))
#define IS_DMA_PERIPHERAL_INC_STATE(STATE) (((STATE) == DMA_PINC_ENABLE) || \
                                            ((STATE) == DMA_PINC_DISABLE))

#define IS_DMA_MEMORY_INC_STATE(STATE) (((STATE) == DMA_MINC_ENABLE)  || \
                                        ((STATE) == DMA_MINC_DISABLE))

#define IS_DMA_PERIPHERAL_DATA_SIZE(SIZE) (((SIZE) == DMA_PDATAALIGN_BYTE)     || \
                                           ((SIZE) == DMA_PDATAALIGN_HALFWORD) || \
                                           ((SIZE) == DMA_PDATAALIGN_WORD))

#define IS_DMA_MEMORY_DATA_SIZE(SIZE) (((SIZE) == DMA_MDATAALIGN_BYTE)     || \
                                       ((SIZE) == DMA_MDATAALIGN_HALFWORD) || \
                                       ((SIZE) == DMA_MDATAALIGN_WORD ))

#define IS_DMA_MODE(MODE) (((MODE) == DMA_NORMAL )  || \
                           ((MODE) == DMA_CIRCULAR))
#define IS_DMA_PRIORITY(PRIORITY) (((PRIORITY) == DMA_PRIORITY_LOW )   || \
                                   ((PRIORITY) == DMA_PRIORITY_MEDIUM) || \
                                   ((PRIORITY) == DMA_PRIORITY_HIGH)   || \
                                   ((PRIORITY) == DMA_PRIORITY_VERY_HIGH))
#define IS_DMA_BUFFER_SIZE(SIZE) (((SIZE) >= 0x1U) && ((SIZE) < 0x10000U))

#if defined(SYSCFG_CFGR1_DMA_RMP)

#if defined (STM32F071xB) || defined (STM32F072xB) || defined (STM32F078xx)
#define IS_DMA_REMAP(RMP) (((RMP) == DMA_REMAP_ADC_DMA_CH2)          || \
                              ((RMP) == DMA_REMAP_USART1_TX_DMA_CH4) || \
                              ((RMP) == DMA_REMAP_USART1_RX_DMA_CH5) || \
                              ((RMP) == DMA_REMAP_TIM16_DMA_CH4)     || \
                              ((RMP) == DMA_REMAP_TIM17_DMA_CH2)     || \
                              ((RMP) == DMA_REMAP_TIM16_DMA_CH6)     || \
                              ((RMP) == DMA_REMAP_TIM17_DMA_CH7)     || \
                              ((RMP) == DMA_REMAP_SPI2_DMA_CH67)     || \
                              ((RMP) == DMA_REMAP_USART2_DMA_CH67)   || \
                              ((RMP) == DMA_REMAP_USART3_DMA_CH32)   || \
                              ((RMP) == DMA_REMAP_I2C1_DMA_CH76)     || \
                              ((RMP) == DMA_REMAP_TIM1_DMA_CH6)      || \
                              ((RMP) == DMA_REMAP_TIM2_DMA_CH7)      || \
                              ((RMP) == DMA_REMAP_TIM3_DMA_CH6))
#elif defined (STM32F070xB)
#define IS_DMA_REMAP(RMP)     (((RMP) == DMA_REMAP_USART3_DMA_CH32)  || \
                              ((RMP) == DMA_REMAP_ADC_DMA_CH2)       || \
                              ((RMP) == DMA_REMAP_USART1_TX_DMA_CH4) || \
                              ((RMP) == DMA_REMAP_USART1_RX_DMA_CH5) || \
                              ((RMP) == DMA_REMAP_TIM16_DMA_CH4)     || \
                              ((RMP) == DMA_REMAP_TIM17_DMA_CH2))
#else
#define IS_DMA_REMAP(RMP)     (((RMP) == DMA_REMAP_ADC_DMA_CH2)      || \
                              ((RMP) == DMA_REMAP_USART1_TX_DMA_CH4) || \
                              ((RMP) == DMA_REMAP_USART1_RX_DMA_CH5) || \
                              ((RMP) == DMA_REMAP_TIM16_DMA_CH4)     || \
                              ((RMP) == DMA_REMAP_TIM17_DMA_CH2))
#endif

#endif /* SYSCFG_CFGR1_DMA_RMP */


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

#endif /* X86_HAL_DMA_H_ */
