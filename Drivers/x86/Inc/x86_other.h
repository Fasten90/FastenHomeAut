/*
 *      x86_other.h
 *      Created on:     2019-05-05
 *      Author:         Vizi Gábor
 *      E-mail:         vizi.gabor90@gmail.com
 *      Function:       -
 *      Target:         x86
 *      Version:        -
 *      Last modified:  2019-05-05
 *      Note:           Copied from stm32f0xx.h
 */


/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup x86
  * @{
  */
    
#ifndef __X86_OTHER_H
#define __X86_OTHER_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
   
/** @addtogroup Library_configuration_section
  * @{
  */
  
   
/*  Tip: To avoid modifying this file each time you need to switch between these
        devices, you can define the device in your toolchain compiler preprocessor.
  */
#if !defined  (USE_HAL_DRIVER)
/**
 * @brief Comment the line below if you will not use the peripherals drivers.
   In this case, these drivers will not be included and the application code will 
   be based on direct access to peripherals registers 
   */
  /*#define USE_HAL_DRIVER */
#endif /* USE_HAL_DRIVER */


/**
  * @}
  */

/** @addtogroup Exported_types
  * @{
  */ 
typedef enum 
{
  RESET = 0, 
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum 
{
  DISABLE = 0, 
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

#if 0
/* Was uncomment because the same "ERROR" definition at "wingdi.h" */
typedef enum 
{
  ERROR = 0,
  SUCCESS = !ERROR
} ErrorStatus;
#endif

/**
  * @}
  */


/** @addtogroup Exported_macros
  * @{
  */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))


/**
  * @}
  */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __X86_OTHER_H */
/**
  * @}
  */

/**
  * @}
  */
