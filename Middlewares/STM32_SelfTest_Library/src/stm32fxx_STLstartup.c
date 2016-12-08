/**
  ******************************************************************************
  * @file    stm32fxx_STLstartup.c 
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    14-Dec-2015
  * @brief   This file contains sequence of self-test routines to be executed just
  *          after initialization of the application before main execution starts
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32fxx_STLparam.h"
#define ALLOC_GLOBALS
#include "stm32fxx_STLlib.h"

/** @addtogroup STM32FxxSelfTestLib_src
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  IWDG_HandleTypeDef IwdgHandle = {0};
  WWDG_HandleTypeDef WwdgHandle = {0};
  CRC_HandleTypeDef CrcHandle = {0};

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/**
  * @brief  This routine is executed in case of failure is detected by one of
  *    self-test routines. The routine is empty and it has to be filled by end
  *    user to keep application safe while define proper recovery operation
  * @param  : None
  * @retval : None
  */
void FailSafePOR(void)
{
  /* disable any checking services at SystTick interrupt */
  TickCounter = TickCounterInv = 0u;
  
  #ifdef STL_VERBOSE_POR
    printf(" >>>>>>>>>> POR FailSafe Mode <<<<<<<<<<\n\r");
  #endif  /* STL_VERBOSE_POR */
  
  #if defined STL_EVAL_MODE
    /* LED_ERR On for debug purposes */
    BSP_LED_On(LED_ERR);
    #if defined STL_EVAL_LCD
      BSP_LCD_DisplayStringAtLine(6, (uint8_t *)(">>> Fail Safe Mode <<<"));
    #endif /* STL_EVAL_LCD */
  #endif  /* STL_EVAL_MODE */
    
  while(1)
  {
    #ifndef NO_RESET_AT_FAIL_MODE
      /* Generate system reset */
      HAL_NVIC_SystemReset();
    #endif /* NO_RESET_AT_FAIL_MODE */
  }
}

#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
#pragma optimize = none
#endif

#ifdef __CC_ARM             /* KEIL Compiler */
/******************************************************************************/
/**
  * @brief  Switch between startup and main code
  * @param  : None
  * @retval : None
  */
  void $Sub$$main(void)
  {
    if ( CRC_FLAG != 0xAAu )
    {
      STL_StartUp();		/* trick to call StartUp before main entry */
    }
    CRC_FLAG = 0u;
    $Super$$main(); 
  }
#endif /* __CC_ARM */
	
/******************************************************************************/
/**
  * @brief  Contains the very first test routines executed right after
  *   the reset
  * @param  : None
  *   Flash interface initialized, Systick timer ON (2ms timebase)
  * @retval : None
  */
void STL_StartUp(void)
{
  uint32_t crc_result;
  uint32_t index;
  ClockStatus clk_sts;
	
  /* block run time tests performed at SysTick interrupt */
  TickCounter= TickCounterInv= 0;
  
  /* Reset of all peripherals, Initializes the Flash interface and the Systick */
  HAL_Init();
  
  #ifdef STL_VERBOSE_POR
    USART_Configuration();
    printf("\n\n\r *******  Self Test Library Init  *******\n\r");
  #endif

  #if defined STL_EVAL_MODE
    /* init LEDs on evaluation board  to indicate phases of startup */
    BSP_LED_Init(LED_VLM);
    BSP_LED_Init(LED_NVM);
    BSP_LED_Init(LED_ERR);
  #endif

  /*--------------------------------------------------------------------------*/
  /*------------------- CPU registers and flags self test --------------------*/
  /*--------------------------------------------------------------------------*/

  /* Initialization of counters for control flow monitoring */
  init_control_flow();

  control_flow_call(CPU_TEST_CALLER);
  /* WARNING: all registers destroyed when exiting this function (including
  preserved registers R4 to R11) while excluding stack pointer R13) */
  if (STL_StartUpCPUTest() != CPUTEST_SUCCESS)
  {
    #ifdef STL_VERBOSE_POR
      printf("Start-up CPU Test Failure\n\r");
    #endif /* STL_VERBOSE_POR */
    
    FailSafePOR();
  }
  else  /* CPU Test OK */
  {
    control_flow_resume(CPU_TEST_CALLER);
    
    #ifdef STL_VERBOSE_POR
      printf(" Start-up CPU Test OK\n\r");
    #endif /* STL_VERBOSE_POR */
  }
  

  /*--------------------------------------------------------------------------*/
  /*--------------------- WDOGs functionality self test ----------------------*/
  /*--------------------------------------------------------------------------*/

  control_flow_call(WDG_TEST_CALLER);

  /* two phases IWDG & WWDG test, system reset is performed here */
  STL_WDGSelfTest();
  
  control_flow_resume(WDG_TEST_CALLER);
  
  
  /*--------------------------------------------------------------------------*/
  /*--------------------- Switch ON PLL for maximum speed --------------------*/
  /*--------------------------------------------------------------------------*/
  #ifdef STL_VERBOSE_POR
    /* finish communication flow prior system clock change */
    while (UartHandle.State == HAL_UART_STATE_BUSY_TX)
    { }
  #endif /* STL_VERBOSE_POR */
  
  /* No Control flow check here (not safety critical) */
  /* Switch on the PLL to speed-up Flash and RAM tests */
  StartUpClock_Config();

  #ifdef STL_VERBOSE_POR
    /* Re-init communication channel with modified clock setting */
    HAL_UART_DeInit(&UartHandle);
    HAL_UART_Init(&UartHandle);
  #endif /* STL_VERBOSE_POR */
  
  /*--------------------------------------------------------------------------*/
  /*--------------------- Invariable memory CRC check ------------------------*/
  /*--------------------------------------------------------------------------*/
  
  #ifdef STL_EVAL_MODE
    /* init LED pins in push pull slow output mode */
    BSP_LED_Init(LED_VLM);
    BSP_LED_Init(LED_NVM);
    BSP_LED_Init(LED_ERR);
    
    /* LED_NVM On for debug purposes */
    BSP_LED_On(LED_NVM);
  #endif /* STL_EVAL_MODE */

  control_flow_call(CRC32_TEST_CALLER);
  /* Compute the 32-bit crc of the whole Flash by CRC unit except the checksum
     pattern stored at top of FLASH */
  
  __CRC_CLK_ENABLE();
  
  CrcHandle.Instance = CRC;
  #ifdef  CRC_UNIT_CONFIGURABLE
    CrcHandle.Init.DefaultPolynomialUse    = DEFAULT_POLYNOMIAL_ENABLE;
    CrcHandle.Init.DefaultInitValueUse     = DEFAULT_INIT_VALUE_ENABLE;
    CrcHandle.Init.InputDataInversionMode  = CRC_INPUTDATA_INVERSION_NONE;
    CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLED;
    CrcHandle.InputDataFormat              = CRC_INPUTDATA_FORMAT_WORDS;
  #endif
  HAL_CRC_Init(&CrcHandle);
  
  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
    /* ==============================================================================*/
    /* MISRA violation of rule 11.4, 17.4 - pointer arithmetic is used for
       CRC calculation */
    #pragma diag_suppress=Pm088,Pm141
  #endif  /* __IAR_SYSTEMS_ICC__ */

/* the next lines replaces the standard HAL function call
   crc_result = HAL_CRC_Calculate(&CrcHandle, (uint32_t *)ROM_START, (uint32_t)ROM_SIZEinWORDS); 
  due to bug at IAR linker - check sum computation can't support both big & little endian  */
     
  for(index = 0; index < (uint32_t)ROM_SIZEinWORDS; index++)
  {
    CRC->DR = __REV(*((uint32_t *)ROM_START + index));
  }
  crc_result = CRC->DR;

  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
    #pragma diag_default=Pm088,Pm141
    /* ==============================================================================*/
  #endif  /* __IAR_SYSTEMS_ICC__ */
          
  control_flow_resume(CRC32_TEST_CALLER);

  /* Store pattern for regular 32-bit crc computation */
  control_flow_call(CRC_TEST_CALLER);
  /* ==============================================================================*/
  /* MISRA violation of rule 10.1, 11.3 and 11.4: integral casting and pointer arithmetic 
     is used here to manage the crc computation and Check Class B var integrity */
  
  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
    #pragma diag_suppress=Pm129,Pm140,Pm141

    if(crc_result != *(uint32_t *)(&REF_CRC32))

    #pragma diag_default=Pm129,Pm140,Pm141
  #endif  /* __IAR_SYSTEMS_ICC__ */
  /* ==============================================================================*/
      
  #ifdef __CC_ARM             /* KEIL Compiler */
  /* Computed 32-bit crc check is temporary stored at crc_result. This value must be copied
     into __Check_Sum address placed at the end of Flash area (see file startup_stm32fxx.s)
     Condition here can be reversed for debuging, the CRC check sum result is stored as
     a reference into RAM wherefrom it is always compared at run time */

    if(crc_result != *(uint32_t *)(&REF_CRC32))
  #endif  /* __CC_ARM */   
    {
      #ifdef STL_VERBOSE_POR
        printf("FLASH 32-bit CRC Error at Start-up\n\r");
      #endif  /* STL_VERBOSE_POR */
      FailSafePOR();
    }
    else
    { /* Test OK */
      #ifdef STL_VERBOSE_POR
        printf(" Start-up FLASH 32-bit CRC OK\n\r");
      #endif  /* STL_VERBOSE_POR */

      /* If else statement is not executed, it will be detected by control flow monitoring */
      control_flow_resume(CRC_TEST_CALLER);  
    }
    
  HAL_CRC_DeInit(&CrcHandle);
  #ifdef STL_EVAL_MODE
    /* LED_NVM Off for debug purposes */
    BSP_LED_Off(LED_NVM);
  #endif /* STL_EVAL_MODE */

  /*--------------------------------------------------------------------------*/
  /*   Verify Control flow before RAM init (which clears Ctrl flow counters)  */
  /*--------------------------------------------------------------------------*/
  if (control_flow_check_point(CHECKPOINT1) == ERROR)
  {
     #ifdef STL_VERBOSE_POR
       printf("Control Flow Error Checkpoint 1\n\r");
     #endif  /* STL_VERBOSE_POR */
     FailSafePOR();
  }
  else
  {
   #ifdef STL_VERBOSE_POR
    printf(" Control Flow Checkpoint 1 OK \n\r");
   #endif  /* STL_VERBOSE_POR */
  }
  
  /*--------------------------------------------------------------------------*/
  /* --------------------- Variable memory functional test -------------------*/
  /*--------------------------------------------------------------------------*/
  #ifdef STL_EVAL_MODE
    /* LED_VLM On for debug purposes */
    BSP_LED_On(LED_VLM);
  #endif /* STL_EVAL_MODE */
  
  /* no stack operation can be performed during the test */  
	__disable_irq();
  
  /* WARNING: Stack is zero-initialized when exiting from this routine */
  if (STL_FullRamMarchC(RAM_START, RAM_END, BCKGRND) != SUCCESS)
  {
    #ifdef STL_VERBOSE_POR
      /* restore interrupt capability */
      __enable_irq();
      /* restore destroyed content of HAL structure for UART */
      UartHandle.Instance = USARTx;
      UartHandle.ErrorCode = HAL_UART_ERROR_NONE;
      UartHandle.State= HAL_UART_STATE_READY;

      printf("RAM Test Failure\n\r");
    #endif  /* STL_VERBOSE_POR */
    FailSafePOR();
  }
  #ifdef STL_VERBOSE_POR
    /* restore interrupt capability */
    __enable_irq();
    /* restore destroyed content of HAL structure for UART */
    UartHandle.Instance = USARTx;
    UartHandle.ErrorCode = HAL_UART_ERROR_NONE;
    UartHandle.State= HAL_UART_STATE_READY;
    
    printf(" Full RAM Test OK\n\r");
  #endif /* STL_VERBOSE_POR */

  /* restore interrupt capability */
  __enable_irq();
  
  /* Initialization of counters for control flow monitoring 
     (destroyed during RAM test) */
  init_control_flow();
  
  #ifdef STL_EVAL_MODE
    /* restore destroyed C compiler constant areas */
    #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
      __iar_data_init3();

    /* LED_VLM Off for debug purposes - can be applied after C-data refresh at SRAM exclusevely */
    BSP_LED_Off(LED_VLM);
    #endif /* __IAR_SYSTEMS_ICC__ */
      
  #endif /* STL_EVAL_MODE */


  /*--------------------------------------------------------------------------*/
  /*------------- Store reference 32-bit CRC in RAM after RAM test -----------*/
  /*--------------------------------------------------------------------------*/
  control_flow_call(CRC_STORE_CALLER);
  
  /* restore destroyed content of HAL structure for CRC */
  CrcHandle.Instance = CRC;
  CrcHandle.State= HAL_CRC_STATE_READY;
  #ifdef  CRC_UNIT_CONFIGURABLE
    CrcHandle.Init.DefaultPolynomialUse    = DEFAULT_POLYNOMIAL_ENABLE;
    CrcHandle.Init.DefaultInitValueUse     = DEFAULT_INIT_VALUE_ENABLE;
    CrcHandle.Init.InputDataInversionMode  = CRC_INPUTDATA_INVERSION_NONE;
    CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLED;
    CrcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
  #endif
  
  /* read and store content of CRC calculation result */
  RefCrc32 = HAL_CRC_Accumulate(&CrcHandle, 0u, 0u);  
  RefCrc32Inv = ~RefCrc32;
  
  control_flow_resume(CRC_STORE_CALLER);  

  /*--------------------------------------------------------------------------*/
  /*----------------------- Clock Frequency Self Test ------------------------*/
  /*--------------------------------------------------------------------------*/
  #ifdef STL_EVAL_MODE
    #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
      /* LED_NVM On for debug purposes - can be applied after C-data refresh at SRAM exclusevely */
      BSP_LED_On(LED_NVM);
    #endif /* __IAR_SYSTEMS_ICC__ */
  #endif /* STL_EVAL_MODE */
    
  #ifdef STL_VERBOSE_POR
    /* finish communication flow prior system clock change */
    while (UartHandle.State == HAL_UART_STATE_BUSY_TX)
    { }
  #endif /* STL_VERBOSE_POR */
  
  control_flow_call(CLOCK_TEST_CALLER);
  
  /* test LSI & HSE clock systems */
  clk_sts = STL_ClockStartUpTest();
  
  #ifdef STL_VERBOSE_POR
    /* Re-init communication channel with modified clock setting */
    UartHandle.Instance        = USARTx;
    HAL_UART_DeInit(&UartHandle);
    
    UartHandle.Init.BaudRate   = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX;
    HAL_UART_Init(&UartHandle);
    /* print out the test result */
    switch(clk_sts)
    {
      case LSI_START_FAIL:
        printf("LSI start-up failure \n\r");
        break;
  
      case HSE_START_FAIL:
        printf("HSE start-up failure \n\r");
        break;
  
      case HSI_HSE_SWITCH_FAIL:
      case HSE_HSI_SWITCH_FAIL:
      case PLL_OFF_FAIL:
        printf("Clock switch failure \n\r");
        break;
  
      case XCROSS_CONFIG_FAIL:
        printf("Clock Xcross measurement failure \n\r");
        break;
  
      case EXT_SOURCE_FAIL:
        printf("HSE clock found out of range\n\r");
        break;
        
      case FREQ_OK:
        printf(" Clock frequency OK \n\r");
        break;
        
      default:
        printf("Abnormal exit from clock test\n\r");
    }
  #endif /* STL_VERBOSE_POR */
  
  if(clk_sts != FREQ_OK)
  {
      FailSafePOR();
  }
    
  control_flow_resume(CLOCK_TEST_CALLER);  
  
  #ifdef STL_EVAL_MODE
    #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
      /* LED_NVM Off for debug purposes - can be applied after C-data refresh at SRAM exclusevely */
      BSP_LED_Off(LED_NVM);
    #endif /* __IAR_SYSTEMS_ICC__ */
  #endif /* STL_EVAL_MODE */

  /*--------------------------------------------------------------------------*/
  /* --------------- Initialize stack overflow pattern ---------------------- */
  /*--------------------------------------------------------------------------*/

  control_flow_call(STACK_OVERFLOW_TEST);
  
  aStackOverFlowPtrn[0] = 0xAAAAAAAAuL;
  aStackOverFlowPtrn[1] = 0xBBBBBBBBuL;
  aStackOverFlowPtrn[2] = 0xCCCCCCCCuL;
  aStackOverFlowPtrn[3] = 0xDDDDDDDDuL;
  
  control_flow_resume(STACK_OVERFLOW_TEST);  

  /*--------------------------------------------------------------------------*/
  /* -----  Verify Control flow before Starting main program execution ------ */
  /*--------------------------------------------------------------------------*/
   
  if (control_flow_check_point(CHECKPOINT2) == ERROR)
  {
     #ifdef STL_VERBOSE_POR
       printf("Control Flow Error Checkpoint 2\n\r");
     #endif  /* STL_VERBOSE_POR */
     FailSafePOR();
   }
   #ifdef STL_VERBOSE_POR
    printf(" Control Flow Checkpoint 2 OK \n\r");
  
    /* finish communication flow prior system clock change */
    while (UartHandle.State == HAL_UART_STATE_BUSY_TX)
    { }
   #endif  /* STL_VERBOSE_POR */
     
   /* startup test completed succesfully - restart the application */
   GotoCompilerStartUp();
}

/******************************************************************************/
/**
  * @brief  Verifies the watchdog by forcing watchdog resets
  * @param  : None
  * @retval : None
  */
void STL_WDGSelfTest(void)
{
  /* ==============================================================================*/
  /* MISRA violation of rule 12.4 - side effect of && and || operators ignored */
  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
    #pragma diag_suppress=Pm026
  #endif /* __IAR_SYSTEMS_ICC__ */

  #ifdef STL_VERBOSE_POR  
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)  != RESET) printf("Pin reset \r\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)  != RESET) printf("POR reset \r\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)  != RESET) printf("SW reset \r\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) printf("IWDG reset \r\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET) printf("WWDG reset \r\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET) printf("LP reset \r\n");
  #endif /* STL_VERBOSE_POR */

  /* start watchdogs test if one of the 4 conditions below is valid */
  if ( (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)\
   ||  (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)\
   ||  (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET)\
   || ((__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET) && (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) == RESET) && (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) == RESET)))
  {
    #ifdef STL_VERBOSE_POR
      printf("... Power-on or software reset, testing IWDG ... \r\n");
    #endif  /* STL_VERBOSE_POR */

    #if defined(STL_EVAL_MODE)
      /* IWDG at debug mode */
      __DBGMCU_CLK_ENABLE();
      __HAL_FREEZE_IWDG_DBGMCU();
    #endif  /* STL_EVAL_MODE */

    /* Clear all flags before resuming test */
    __HAL_RCC_CLEAR_FLAG();

    /* Setup IWDG to minimum period */
    __HAL_RESET_HANDLE_STATE(&IwdgHandle);
    IwdgHandle.Instance = IWDG;
    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_4;
    IwdgHandle.Init.Reload = 1;
    #ifdef IWDG_FEATURES_BY_WINDOW_OPTION
      IwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;
    #endif /* IWDG_FEATURES_BY_WINDOW_OPTION */
    /* Initialization */
    HAL_IWDG_Init(&IwdgHandle);
    /* initialization becomes accepted when IWDG starts (LSI is enabled by HW) */
    HAL_IWDG_Start(&IwdgHandle);

    /* Wait for an independant watchdog reset */
    while(1)
    { }
  }
  else  /* Watchdog test or software reset triggered by application failure */
  {
    /* If WWDG only was set, re-start the complete test (indicates a reset triggered by safety routines */
    if ((__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)  != RESET) && (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) == RESET) && (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET))
    {
      __HAL_RCC_CLEAR_FLAG();
      #ifdef STL_VERBOSE_POR
        printf("... WWDG reset, re-start WDG test ... \r\n");
      #endif  /* STL_VERBOSE_POR */
      NVIC_SystemReset();
    }
    else  /* If IWDG only was set, continue the test with WWDG test*/
    {
      if ((__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)  != RESET) && (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) && (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) == RESET))
      { /* If IWDG only was set, test WWDG*/
        #ifdef STL_VERBOSE_POR
          printf("... IWDG reset from test or application, testing WWDG\r\n");
        #endif  /* STL_VERBOSE_POR */
          
        #if defined(STL_EVAL_MODE)
          /* WWDG at debug mode */
          __DBGMCU_CLK_ENABLE();
          __HAL_FREEZE_WWDG_DBGMCU();
        #endif  /* STL_EVAL_MODE */
          
         /* Setup WWDG to minimum period */
        __HAL_RESET_HANDLE_STATE(&WwdgHandle);
        __WWDG_CLK_ENABLE();
        WwdgHandle.Instance = WWDG;
        WwdgHandle.Init.Prescaler = WWDG_PRESCALER_1;
        WwdgHandle.Init.Counter = 64;
        WwdgHandle.Init.Window = 63;
        HAL_WWDG_Init(&WwdgHandle);
        
        /* Enable WWDG clock */
        HAL_WWDG_Start(&WwdgHandle);

        while(1)
        { }
      }
      else  /* If both flags IWDG & WWDG flags are set, means that watchdog test is completed */
      {
        if ((__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)  != RESET) && (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) && (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET))
        {
          __HAL_RCC_CLEAR_FLAG();
          #ifdef STL_VERBOSE_POR
            printf("... WWDG reset, WDG test completed ... \r\n");
          #endif  /* STL_VERBOSE_POR */
        }
        else  /* Unexpected Flag configuration, re-start WDG test */
        {
          __HAL_RCC_CLEAR_FLAG();
          #ifdef STL_VERBOSE_POR
            printf("...Unexpected Flag configuration, re-start WDG test... \r\n");
          #endif  /* STL_VERBOSE_POR */
        NVIC_SystemReset();
        } /* End of Unexpected Flag configuration */
      } /* End of normal test sequence */
    } /* End of partial WDG test (IWDG test done) */
  } /* End of part where 1 or 2 Watchdog flags are set */

  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
    #pragma diag_default=Pm026
  #endif /* __IAR_SYSTEMS_ICC__ */
  /* ==============================================================================*/
}

/******************************************************************************/
/**
  * @brief  Verifies the consistency and value of control flow counters
  * @param  : chck  value of the positive counter
  * @retval : ErrorStatus (SUCCESS, ERROR)
  */
ErrorStatus control_flow_check_point(uint32_t chck)
{
  ErrorStatus Result= SUCCESS;
  
  if ((CtrlFlowCnt != (chck)) || ((CtrlFlowCnt ^ CtrlFlowCntInv) != 0xFFFFFFFFuL))
  {
    Result= ERROR;
  }
  return(Result);
}

/**
  * @}
  */

/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/
