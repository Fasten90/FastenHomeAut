/**
  ******************************************************************************
  * @file    stm32fxx_STLclassBvar.h 
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    14-Dec-2015
  * @brief   Contains all safety critical variables; they must have
  *          predefined addresses and inverse redundant storage
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STL_CLASS_B_VAR_H
#define __STL_CLASS_B_VAR_H

#include "stm32fxx_STLparam.h"

/* This avoids having mutiply defined global variables */
#ifdef ALLOC_GLOBALS
#define EXTERN
#else
#define EXTERN extern
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
  /* RAM location for temporary storage of original values at run time RAM transparent test */
  EXTERN __no_init uint32_t aRunTimeRamBuf[RT_RAM_BLOCKSIZE + 2] @ "RUN_TIME_RAM_BUF";

  /* pointers for run time RAM transparent test */
  EXTERN __no_init uint32_t *pRunTimeRamChk @ "RUN_TIME_RAM_PNT";
  EXTERN __no_init uint32_t *pRunTimeRamChkInv @ "RUN_TIME_RAM_PNT";
  EXTERN __no_init uint32_t aGAP_FOR_RAM_TEST_OVERLAY[2] @ "RUN_TIME_RAM_PNT";

  /* Class B non inverted variables area */
  EXTERN __no_init uint32_t CtrlFlowCnt @ "CLASS_B_RAM";      /* Counter for verifying correct program execution at start */
  EXTERN __no_init uint32_t ISRCtrlFlowCnt @ "CLASS_B_RAM";   /* Counter for verifying correct program execution in interrupt */
  EXTERN __no_init uint32_t PeriodValue @ "CLASS_B_RAM";      /* LSI period measurement at TIM5 IRQHandler */
  EXTERN __no_init uint32_t TickCounter @ "CLASS_B_RAM";      /* Sofware time base used in main program (incremented in SysTick timer ISR */
  EXTERN __no_init __IO uint32_t TimeBaseFlag @ "CLASS_B_RAM";/* Indicates to the main routine a 100ms tick */
  EXTERN __no_init __IO uint32_t LSIPeriodFlag @ "CLASS_B_RAM";/* Indicates to the main routine a 100ms tick */
  EXTERN __no_init uint32_t LastCtrlFlowCnt @ "CLASS_B_RAM";  /* Stores the Control flow counter from one main loop to the other */
  EXTERN __no_init uint32_t *pRunCrc32Chk @ "CLASS_B_RAM";    /* Pointer to FLASH for crc32 run-time tests */
  EXTERN __no_init uint32_t RefCrc32 @ "CLASS_B_RAM";         /* Reference 32-bit CRC for run-time tests */
    
  /* Class B inverted variables area */
  EXTERN __no_init uint32_t CtrlFlowCntInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init uint32_t ISRCtrlFlowCntInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init uint32_t PeriodValueInv @ "CLASS_B_RAM_REV";  
  EXTERN __no_init uint32_t TickCounterInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init __IO uint32_t TimeBaseFlagInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init __IO uint32_t LSIPeriodFlagInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init uint32_t LastCtrlFlowCntInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init uint32_t *pRunCrc32ChkInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init uint32_t RefCrc32Inv @ "CLASS_B_RAM_REV";

  /* Magic pattern for Stack overflow in this array */
  EXTERN __no_init __IO uint32_t aStackOverFlowPtrn[4] @ "STACK_BOTTOM";
#endif  /* __IAR_SYSTEMS_ICC__ */


#ifdef __CC_ARM   /* KEIL Compiler */
  /* RAM location for temporary storage of original values at run time RAM transparent test */
  EXTERN uint32_t aRunTimeRamBuf[RT_RAM_BLOCKSIZE + 2] __attribute__((section("RUN_TIME_RAM_BUF")));

  /* RAM pointer for run-time tests */
  EXTERN uint32_t *pRunTimeRamChk        __attribute__((section("RUN_TIME_RAM_PNT")));
  EXTERN uint32_t *pRunTimeRamChkInv     __attribute__((section("RUN_TIME_RAM_PNT")));
  EXTERN uint32_t aGAP_FOR_RAM_TEST_OVERLAY[2] __attribute__((section("RUN_TIME_RAM_PNT")));

  /* Note: the zero_init forces the linker to place variables in the bss section */
  /* This allows the UNINIT directive (in scatter file) to work. On the contrary */
  /* all Class B variables pairs should be initialized properly by user before using them */

  /* Counter for verifying correct program execution at start */
  EXTERN uint32_t CtrlFlowCnt             __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t CtrlFlowCntInv          __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Counter for verifying correct program execution in interrupt */
  EXTERN uint32_t ISRCtrlFlowCnt          __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t ISRCtrlFlowCntInv       __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* LSI period measurement at TIM5 IRQHandler */
  EXTERN uint32_t PeriodValue           __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t PeriodValueInv        __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Sofware time base used in main program (incremented in SysTick timer ISR */
  EXTERN uint32_t TickCounter             __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t TickCounterInv          __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Indicates to the main routine a 100ms tick */
  EXTERN __IO uint32_t TimeBaseFlag       __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN __IO uint32_t TimeBaseFlagInv    __attribute__((section("CLASS_B_RAM_REV"), zero_init));
  
  /* Indicates to the main routine a 100ms tick */
  EXTERN __IO uint32_t LSIPeriodFlag      __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN __IO uint32_t LSIPeriodFlagInv   __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Stores the Control flow counter from one main loop to the other */
  EXTERN uint32_t LastCtrlFlowCnt         __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t LastCtrlFlowCntInv      __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Pointer to FLASH for crc32 run-time tests */
  EXTERN uint32_t *pRunCrc32Chk           __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t *pRunCrc32ChkInv        __attribute__((section("CLASS_B_RAM_REV"), zero_init));

/* Reference 32-bit CRC for run-time tests */
  EXTERN uint32_t RefCrc32                __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t RefCrc32Inv             __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Magic pattern for Stack overflow in this array */
  EXTERN __IO uint32_t aStackOverFlowPtrn[4]   __attribute__((section("STACK_BOTTOM"), zero_init));
#endif  /* __CC_ARM */


#ifdef __GNUC__
  /* RAM location for temporary storage of original values at run time RAM transparent test */
  EXTERN uint32_t aRunTimeRamBuf[RT_RAM_BLOCKSIZE + 2] __attribute__((section("RUN_TIME_RAM_BUF")));

  /* RAM pointer for run-time tests */
  EXTERN uint32_t *pRunTimeRamChk        __attribute__((section("RUN_TIME_RAM_PNT")));
  EXTERN uint32_t *pRunTimeRamChkInv     __attribute__((section("RUN_TIME_RAM_PNT")));
  EXTERN uint32_t aGAP_FOR_RAM_TEST_OVERLAY[2] __attribute__((section("RUN_TIME_RAM_PNT")));

  /* Note: the zero_init forces the linker to place variables in the bss section */
  /* This allows the UNINIT directive (in scatter file) to work. On the contrary */
  /* all Class B variables pairs should be initialized properly by user before using them */

  /* Counter for verifying correct program execution at start */
  EXTERN uint32_t CtrlFlowCnt             __attribute__((section("CLASS_B_RAM")));
  EXTERN uint32_t CtrlFlowCntInv          __attribute__((section("CLASS_B_RAM_REV")));

  /* Counter for verifying correct program execution in interrupt */
  EXTERN uint32_t ISRCtrlFlowCnt          __attribute__((section("CLASS_B_RAM")));
  EXTERN uint32_t ISRCtrlFlowCntInv       __attribute__((section("CLASS_B_RAM_REV")));

  /* LSI period measurement at TIM5 IRQHandler */
  EXTERN uint32_t PeriodValue           __attribute__((section("CLASS_B_RAM")));
  EXTERN uint32_t PeriodValueInv        __attribute__((section("CLASS_B_RAM_REV")));

  /* Sofware time base used in main program (incremented in SysTick timer ISR */
  EXTERN uint32_t TickCounter             __attribute__((section("CLASS_B_RAM")));
  EXTERN uint32_t TickCounterInv          __attribute__((section("CLASS_B_RAM_REV")));

  /* Indicates to the main routine a 100ms tick */
  EXTERN __IO uint32_t TimeBaseFlag       __attribute__((section("CLASS_B_RAM")));
  EXTERN __IO uint32_t TimeBaseFlagInv    __attribute__((section("CLASS_B_RAM_REV")));

  /* Indicates to the main routine a 100ms tick */
  EXTERN __IO uint32_t LSIPeriodFlag      __attribute__((section("CLASS_B_RAM")));
  EXTERN __IO uint32_t LSIPeriodFlagInv   __attribute__((section("CLASS_B_RAM_REV")));

  /* Stores the Control flow counter from one main loop to the other */
  EXTERN uint32_t LastCtrlFlowCnt         __attribute__((section("CLASS_B_RAM")));
  EXTERN uint32_t LastCtrlFlowCntInv      __attribute__((section("CLASS_B_RAM_REV")));

  /* Pointer to FLASH for crc32 run-time tests */
  EXTERN uint32_t *pRunCrc32Chk           __attribute__((section("CLASS_B_RAM")));
  EXTERN uint32_t *pRunCrc32ChkInv        __attribute__((section("CLASS_B_RAM_REV")));

/* Reference 32-bit CRC for run-time tests */
  EXTERN uint32_t RefCrc32                __attribute__((section("CLASS_B_RAM")));
  EXTERN uint32_t RefCrc32Inv             __attribute__((section("CLASS_B_RAM_REV")));

  /* Magic pattern for Stack overflow in this array */
  EXTERN __IO uint32_t aStackOverFlowPtrn[4]   __attribute__((section("STACK_BOTTOM")));
#endif


#endif /* __STL_CLASS_B_VAR_H */

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
