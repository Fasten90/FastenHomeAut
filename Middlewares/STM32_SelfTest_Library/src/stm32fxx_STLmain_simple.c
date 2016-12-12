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

