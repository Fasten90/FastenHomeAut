/*
 * stm32fxx_STLmain_simple.c
 *
 *  Created on: Dec 12, 2016
 *      Author: vizi.gabor
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32fxx_STLparam.h"
#include "stm32fxx_STLlib.h"


uint8_t STL_FlashResult = 0;



const __attribute__((section(".flashendcrc"))) uint32_t __Check_Sum = 0x877e923;
//const __attribute__((at(FLASH_ADDR_FLASH_END_VARIABLE))) uint32_t __Check_Sum = 0x00001234;


/**
 * \brief	Initialize SelfTests (Changed: only Flash CRC test)
 * \note	Copied from "stm32fxx_STLmain.c" file
 */
void STL_InitRunTimeChecks(void)
{
	__CRC_CLK_ENABLE();

	STL_FlashCrc32Init();

	/* read and store content of CRC calculation result */
	RefCrc32 = HAL_CRC_Accumulate(&CrcHandle, 0u, 0u);
	RefCrc32Inv = ~RefCrc32;
}



/**
 * \brief	Run periodic test (Changed: Only CRC)
 * \note	Copied from "stm32fxx_STLmain.c" file
 */
void STL_DoRunTimeChecks(void)
{
	uint32_t rom_test;

	/*----------------------------------------------------------------------*/
	/*------------------ Invariable memory CRC check -----------------------*/
	/*----------------------------------------------------------------------*/
	control_flow_call(FLASH_TEST_CALLER);

	rom_test = STL_crc32Run();  /* Requires the control flow check to be modified */
	switch ( rom_test )
	{
		case TEST_RUNNING:
			control_flow_resume(FLASH_TEST_CALLER);
			STL_FlashResult = 0;
			break;

		case TEST_OK:
		  #ifdef STL_VERBOSE
			/* avoid any long string output here, '*' just marks the flash test completed ok */
			putchar((int)'*');
		  #endif /* STL_VERBOSE */
		  #if defined STL_EVAL_MODE
			/* Toggle LED_NVM for debug purpose */
			BSP_LED_Toggle(LED_NVM);
		  #endif  /* STL_EVAL_MODE */
		  #if defined(STL_EVAL_LCD)
			++MyFLASHCounter;
		  #endif /* STL_EVAL_LCD */
		  CtrlFlowCntInv -= FLASH_TEST_CALLER;
		  STL_FlashResult = 1;
		  break;

		case TEST_FAILURE:
		case CLASS_B_DATA_FAIL:
		default:
			#ifdef STL_VERBOSE
			printf(" Run-time FLASH CRC Error\n\r");
			#endif  /* STL_VERBOSE */
			//FailSafePOR();
			STL_FlashResult = 2;
			break;
	}

}



#ifdef CONFIG_MODULE_STL_SELFTEST_ENABLE
/******************************************************************************/
/**
  * @brief Configure TIM5 to measure LSI period
  * @param  : None
  * @retval : ErrorStatus = (ERROR, SUCCESS)
  */
ErrorStatus STL_InitClock_Xcross_Measurement(void)
{
  ErrorStatus result = SUCCESS;
  TIM_HandleTypeDef  tim_capture_handle;
  TIM_IC_InitTypeDef tim_input_config;

  /*## Enable peripherals and GPIO Clocks ####################################*/
  /* TIMx Peripheral clock enable */
  __TIM5_CLK_ENABLE();

  /*## Configure the NVIC for TIMx ###########################################*/
  HAL_NVIC_SetPriority(TIM5_IRQn, 4u, 0u);

  /* Enable the TIM5 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM5_IRQn);

  /* TIM5 configuration: Input Capture mode ---------------------
  The LSI oscillator is connected to TIM5 CH4.
  The Rising edge is used as active edge, ICC input divided by 8
  The TIM5 CCR4 is used to compute the frequency value.
  ------------------------------------------------------------ */
  tim_capture_handle.Instance = TIM5;
  tim_capture_handle.Init.Prescaler         = 0u;
  tim_capture_handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  tim_capture_handle.Init.Period            = 0xFFFFFFFFul;
  tim_capture_handle.Init.ClockDivision     = 0u;
  tim_capture_handle.Init.RepetitionCounter = 0u;
  /* define internal HAL driver status here as handle structure is defined locally */
  __HAL_RESET_HANDLE_STATE(&tim_capture_handle);
  if(HAL_TIM_IC_Init(&tim_capture_handle) != HAL_OK)
  {
    /* Initialization Error */
    result = ERROR;
  }
  /* Connect internally the TIM5_CH4 Input Capture to the LSI clock output */
  HAL_TIMEx_RemapConfig(&tim_capture_handle, TIM_TIM5_LSI);

  /* Configure the TIM5 Input Capture of channel 4 */
  tim_input_config.ICPolarity  = TIM_ICPOLARITY_RISING;
  tim_input_config.ICSelection = TIM_ICSELECTION_DIRECTTI;
  tim_input_config.ICPrescaler = TIM_ICPSC_DIV8;
  tim_input_config.ICFilter    = 0u;
  if(HAL_TIM_IC_ConfigChannel(&tim_capture_handle, &tim_input_config, TIM_CHANNEL_4) != HAL_OK)
  {
    /* Initialization Error */
    result = ERROR;
  }

  /* Reset the flags */
  tim_capture_handle.Instance->SR = 0u;
  LSIPeriodFlag = 0u;

  /* Start the TIM Input Capture measurement in interrupt mode */
  if(HAL_TIM_IC_Start_IT(&tim_capture_handle, TIM_CHANNEL_4) != HAL_OK)
  {
    /* Initialization Error */
    result = ERROR;
  }
  return(result);
}
#endif	// CONFIG_MODULE_STL_SELFTEST_ENABLE

